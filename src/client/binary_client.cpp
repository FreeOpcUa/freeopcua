/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote server implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/utils.h>
#include <opc/ua/client/binary_client.h>
#include <opc/ua/client/remote_connection.h>

#include <opc/common/uri_facade.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/channel.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/services/services.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <iostream>


namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  typedef std::map<uint32_t, std::function<void (PublishResult)>> SubscriptionCallbackMap;

  class BufferInputChannel : public OpcUa::InputChannel
  {
  public:
     BufferInputChannel(const std::vector<char>& buffer)
       : Buffer(buffer)
       , Pos(0)
     {
       Reset();
     }

     virtual std::size_t Receive(char* data, std::size_t size)
     {
       if (Pos >= Buffer.size())
       {
         return 0;
       }

       size = std::min(size, Buffer.size() - Pos);
       std::vector<char>::const_iterator begin = Buffer.begin() + Pos;
       std::vector<char>::const_iterator end = begin + size;
       std::copy(begin, end, data);
       Pos += size;
       return size;
     }

     void Reset()
     {
       Pos = 0;
     }

     virtual void Stop()
     {
     }

  private:
    const std::vector<char>& Buffer;
    std::size_t Pos;
  };


  template <typename T>
  class RequestCallback
  {
  public:
    RequestCallback()
      : lock(m)
    {
    }

    void OnData(std::vector<char> data, ResponseHeader h)
    {
      //PrintBlob(data);
      Data = std::move(data);
	  this->header = std::move(h);
      doneEvent.notify_all();
    }

    T WaitForData(std::chrono::milliseconds msec)
    {
	  if (doneEvent.wait_for(lock, msec) == std::cv_status::timeout)
		  throw std::runtime_error("Response timed out");

      T result;
	  result.Header = std::move(this->header);
      if ( Data.empty() )
      {
        std::cout << "Error: received empty packet from server" << std::endl;
      }
	  else
	  {
		  BufferInputChannel bufferInput(Data);
		  IStreamBinary in(bufferInput);
		  in >> result;
	  }
      return result;
    }

  private:
    std::vector<char> Data;
	ResponseHeader	  header;
    std::mutex m;
    std::unique_lock<std::mutex> lock;
    std::condition_variable doneEvent;
  };

  class CallbackThread
  {
    public:
      CallbackThread(bool debug=false) : Debug(debug), StopRequest(false)
      {

      }

      void post(std::function<void()> callback)
      {
        if (Debug)  { std::cout << "binary_client| CallbackThread :  start post" << std::endl; }
        std::unique_lock<std::mutex> lock(Mutex);
        Queue.push(callback);
        Condition.notify_one();
        if (Debug)  { std::cout << "binary_client| CallbackThread :  end post" << std::endl; }
      }

      void Run()
      {
        while (true)
        {
          if (Debug)  { std::cout << "binary_client| CallbackThread : waiting for next post" << std::endl; }
          std::unique_lock<std::mutex> lock(Mutex);
          Condition.wait(lock, [&]() { return (StopRequest == true) || ( ! Queue.empty() ) ;} );
          if (StopRequest)
          {
            if (Debug)  { std::cout << "binary_client| CallbackThread : exited." << std::endl; }
            return;
          }
          while ( ! Queue.empty() ) //to avoid crashing on spurious events
          {
            if (Debug)  { std::cout << "binary_client| CallbackThread : condition has triggered copying callback and poping. queue size is  " << Queue.size() << std::endl; }
            std::function<void()> callbackcopy = Queue.front();
            Queue.pop();
            lock.unlock();
            if (Debug)  { std::cout << "binary_client| CallbackThread : now calling callback." << std::endl; }
            callbackcopy();
            if (Debug)  { std::cout << "binary_client| CallbackThread : callback called." << std::endl; }
            lock.lock();
          }
        }
      }

      void Stop()
      {
        if (Debug)  { std::cout << "binary_client| CallbackThread : stopping." << std::endl; }
        StopRequest = true;
        Condition.notify_all();
      }

    private:
      bool Debug = false;
      std::mutex Mutex;
      std::condition_variable Condition;
      std::atomic<bool> StopRequest;
      std::queue<std::function<void()>> Queue;
  };

  class BinaryClient
    : public Services
    , public AttributeServices
    , public EndpointServices
    , public MethodServices
    , public NodeManagementServices
    , public SubscriptionServices
    , public ViewServices
    , public std::enable_shared_from_this<BinaryClient>
  {
  private:
    typedef std::function<void(std::vector<char>, ResponseHeader)> ResponseCallback;
    typedef std::map<uint32_t, ResponseCallback> CallbackMap;
		std::vector<char> messageBuffer;

  public:
    BinaryClient(std::shared_ptr<IOChannel> channel, const SecureConnectionParams& params, bool debug)
      : Channel(channel)
      , Stream(channel)
      , Params(params)
      , SequenceNumber(1)
      , RequestNumber(1)
      , RequestHandle(0)
      , Debug(debug)
      , CallbackService(debug)

    {
      //Initialize the worker thread for subscriptions
      callback_thread = std::thread([&](){ CallbackService.Run(); });

      HelloServer(params);

      ReceiveThread = std::move(std::thread([this](){
        try
        {
          while(!Finished)
            Receive();
        }
        catch (const std::exception& exc)
        {
          if (Finished) return;
          if (Debug)  { std::cerr << "binary_client| ReceiveThread : Error receiving data: "; }
          std::cerr << exc.what() << std::endl;
        }
      }));
    }

    ~BinaryClient()
    {
      Finished = true;

      if (Debug) std::cout << "binary_client| Stopping callback thread." << std::endl;
      CallbackService.Stop();
      if (Debug) std::cout << "binary_client| Joining service thread." << std::endl;
      callback_thread.join(); //Not sure it is necessary

      Channel->Stop();
      if (Debug) std::cout << "binary_client| Joining receive thread." << std::endl;
      ReceiveThread.join();
      if (Debug) std::cout << "binary_client| Receive thread stopped." << std::endl;

      if (Debug) std::cout << "binary_client| Destroyed." << std::endl;
    }

    ////////////////////////////////////////////////////////////////
    /// Session Services
    ////////////////////////////////////////////////////////////////
    virtual CreateSessionResponse CreateSession(const RemoteSessionParameters& parameters)
    {
      if (Debug)  { std::cout << "binary_client| CreateSession -->" << std::endl; }
      CreateSessionRequest request;
      request.Header = CreateRequestHeader();

      request.Parameters.ClientDescription.ApplicationUri = parameters.ClientDescription.ApplicationUri;
      request.Parameters.ClientDescription.ProductUri = parameters.ClientDescription.ProductUri;
      request.Parameters.ClientDescription.ApplicationName = parameters.ClientDescription.ApplicationName;
      request.Parameters.ClientDescription.ApplicationType = parameters.ClientDescription.ApplicationType;
      request.Parameters.ClientDescription.GatewayServerUri = parameters.ClientDescription.GatewayServerUri;
      request.Parameters.ClientDescription.DiscoveryProfileUri = parameters.ClientDescription.DiscoveryProfileUri;
      request.Parameters.ClientDescription.DiscoveryUrls = parameters.ClientDescription.DiscoveryUrls;

      request.Parameters.ServerUri = parameters.ServerURI;
      request.Parameters.EndpointUrl = parameters.EndpointUrl; // TODO make just endpoint.URL;
      request.Parameters.SessionName = parameters.SessionName;
      request.Parameters.ClientNonce = ByteString(std::vector<uint8_t>(32,0));
      request.Parameters.ClientCertificate = ByteString(parameters.ClientCertificate);
      request.Parameters.RequestedSessionTimeout = parameters.Timeout;
      request.Parameters.MaxResponseMessageSize = 65536;
      CreateSessionResponse response = Send<CreateSessionResponse>(request);
      AuthenticationToken = response.Parameters.AuthenticationToken;
      if (Debug)  { std::cout << "binary_client| CreateSession <--" << std::endl; }
      return response;
    }

    ActivateSessionResponse ActivateSession(const ActivateSessionParameters &session_parameters) override
    {
      if (Debug)  { std::cout << "binary_client| ActivateSession -->" << std::endl; }
      ActivateSessionRequest request;
      request.Parameters = session_parameters;
      request.Parameters.LocaleIds.push_back("en");
      ActivateSessionResponse response = Send<ActivateSessionResponse>(request);
      if (Debug)  { std::cout << "binary_client| ActivateSession <--" << std::endl; }
      return response;
    }

    virtual CloseSessionResponse CloseSession()
    {
      if (Debug)  { std::cout << "binary_client| CloseSession -->" << std::endl; }
      CloseSessionRequest request;
      CloseSessionResponse response = Send<CloseSessionResponse>(request);
      RemoveSelfReferences();
      if (Debug)  { std::cout << "binary_client| CloseSession <--" << std::endl; }
      return response;
    }

    virtual void AbortSession()
    {
      if (Debug)  { std::cout << "binary_client| AbortSession -->" << std::endl; }
      RemoveSelfReferences();
      if (Debug)  { std::cout << "binary_client| AbortSession <--" << std::endl; }
    }

    DeleteNodesResponse DeleteNodes(const std::vector<OpcUa::DeleteNodesItem> &nodesToDelete) override
    {
      if (Debug)  { std::cout << "binary_client| DeleteNodes -->" << std::endl; }
      DeleteNodesRequest request;
      request.NodesToDelete = nodesToDelete;
      DeleteNodesResponse response = Send<DeleteNodesResponse>(request);
      if (Debug)  { std::cout << "binary_client| DeleteNodes <--" << std::endl; }
      return response;
    }

    ////////////////////////////////////////////////////////////////
    /// Attribute Services
    ////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<AttributeServices> Attributes() override
    {
      return shared_from_this();
    }

  public:
    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      if (Debug)  {
        std::cout << "binary_client| Read -->" << std::endl;
        for ( ReadValueId attr : params.AttributesToRead )
        {
          std::cout << attr.NodeId << "  " << (uint32_t)attr.AttributeId;
        }
        std::cout << std::endl;
      }
      ReadRequest request;
      request.Parameters = params;
      const ReadResponse response = Send<ReadResponse>(request);
      if (Debug)  { std::cout << "binary_client| Read <--" << std::endl; }
      return response.Results;
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<WriteValue>& values)
    {
      if (Debug)  { std::cout << "binary_client| Write -->" << std::endl; }
      WriteRequest request;
      request.Parameters.NodesToWrite = values;
      const WriteResponse response = Send<WriteResponse>(request);
      if (Debug)  { std::cout << "binary_client| Write <--" << std::endl; }
      return response.Results;
    }

    ////////////////////////////////////////////////////////////////
    /// Endpoint Services
    ////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<EndpointServices> Endpoints() override
    {
      return shared_from_this();
    }

    virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
    {
      if (Debug)  { std::cout << "binary_client| FindServers -->" << std::endl; }
      OpcUa::FindServersRequest request;
      request.Parameters = params;
      FindServersResponse response = Send<FindServersResponse>(request);
      if (Debug)  { std::cout << "binary_client| FindServers <--" << std::endl; }
      return response.Data.Descriptions;
    }

    virtual std::vector<EndpointDescription> GetEndpoints(const GetEndpointsParameters& filter) const
    {
      if (Debug)  { std::cout << "binary_client| GetEndpoints -->" << std::endl; }
      OpcUa::GetEndpointsRequest request;
      request.Header = CreateRequestHeader();
      request.Parameters.EndpointUrl = filter.EndpointUrl;
      request.Parameters.LocaleIds = filter.LocaleIds;
      request.Parameters.ProfileUris = filter.ProfileUris;
      const GetEndpointsResponse response = Send<GetEndpointsResponse>(request);
      if (Debug)  { std::cout << "binary_client| GetEndpoints <--" << std::endl; }
      return response.Endpoints;
    }

    virtual void RegisterServer(const ServerParameters& parameters)
    {
    }

    ////////////////////////////////////////////////////////////////
    /// Method Services
    ////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<MethodServices> Method() override
    {
      return shared_from_this();
    }

    virtual std::vector<CallMethodResult> Call(const std::vector<CallMethodRequest>& methodsToCall)
    {
      if (Debug) {std::cout << "binary_clinent | Call -->" << std::endl;}
      CallRequest request;
      request.Parameters.MethodsToCall = methodsToCall;
      const CallResponse response = Send<CallResponse>(request);
      if (Debug) {std::cout << "binary_clinent | Call <--" << std::endl;}
      // Manage errors
//       if (!response.DiagnosticInfos.empty())
//       {
// For now commented out, handling of diagnostic should be probably added for all communication
//       }
      return response.Results;
    }

    ////////////////////////////////////////////////////////////////
    /// Node management Services
    ////////////////////////////////////////////////////////////////

    virtual std::shared_ptr<NodeManagementServices> NodeManagement() override
    {
      return shared_from_this();
    }

    virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items)
    {
      if (Debug)  { std::cout << "binary_client| AddNodes -->" << std::endl; }
      AddNodesRequest request;
      request.Parameters.NodesToAdd = items;
      const AddNodesResponse response = Send<AddNodesResponse>(request);
      if (Debug)  { std::cout << "binary_client| AddNodes <--" << std::endl; }
      return response.results;
    }

    virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
    {
      if (Debug)  { std::cout << "binary_client| AddReferences -->" << std::endl; }
      AddReferencesRequest request;
      request.Parameters.ReferencesToAdd = items;
      const AddReferencesResponse response = Send<AddReferencesResponse>(request);
      if (Debug)  { std::cout << "binary_client| AddReferences <--" << std::endl; }
      return response.Results;
    }

    virtual void SetMethod(const NodeId& node, std::function<std::vector<OpcUa::Variant> (NodeId context, std::vector<OpcUa::Variant> arguments)> callback)
    {
      if (Debug)  { std::cout << "binary_client| SetMethod has no effect on client!" << std::endl; }
      return;
    }

    ////////////////////////////////////////////////////////////////
    /// Subscriptions Services
    ////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<SubscriptionServices> Subscriptions() override
    {
      return shared_from_this();
    }

    virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback)
    {
      if (Debug)  { std::cout << "binary_client| CreateSubscription -->" << std::endl; }
      const CreateSubscriptionResponse response = Send<CreateSubscriptionResponse>(request);
      if (Debug) std::cout << "BinaryClient | got CreateSubscriptionResponse" << std::endl;
      PublishCallbacks[response.Data.SubscriptionId] = callback;// TODO Pass calback to the Publish method.
      if (Debug)  { std::cout << "binary_client| CreateSubscription <--" << std::endl; }
      return response.Data;
    }

    virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<uint32_t>& subscriptions)
    {
      if (Debug)  { std::cout << "binary_client| DeleteSubscriptions -->" << std::endl; }
      DeleteSubscriptionsRequest request;
      request.SubscriptionIds = subscriptions;
      const DeleteSubscriptionsResponse response = Send<DeleteSubscriptionsResponse>(request);
      if (Debug)  { std::cout << "binary_client| DeleteSubscriptions <--" << std::endl; }
      return response.Results;
    }

    virtual std::vector<MonitoredItemCreateResult> CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      if (Debug)  { std::cout << "binary_client| CreateMonitoredItems -->" << std::endl; }
      CreateMonitoredItemsRequest request;
      request.Parameters = parameters;
      const CreateMonitoredItemsResponse response = Send<CreateMonitoredItemsResponse>(request);
      if (Debug)  { std::cout << "binary_client| CreateMonitoredItems <--" << std::endl; }
      return response.Results;
    }

    virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params)
    {
      if (Debug)  { std::cout << "binary_client| DeleteMonitoredItems -->" << std::endl; }
      DeleteMonitoredItemsRequest request;
      request.Parameters = params;
      const DeleteMonitoredItemsResponse response = Send<DeleteMonitoredItemsResponse>(request);
      if (Debug)  { std::cout << "binary_client| DeleteMonitoredItems <--" << std::endl; }
      return response.Results;
    }

    virtual void Publish(const PublishRequest& originalrequest)
    {
      if (Debug) {std::cout << "binary_client| Publish -->" << "request with " << originalrequest.SubscriptionAcknowledgements.size() << " acks" << std::endl;}
      PublishRequest request(originalrequest);
      request.Header = CreateRequestHeader();
      request.Header.Timeout = 0; //We do not want the request to timeout!

      ResponseCallback responseCallback = [this](std::vector<char> buffer, ResponseHeader h){
        if (Debug) {std::cout << "BinaryClient | Got Publish Response, from server " << std::endl;}
		PublishResponse response;
		if (h.ServiceResult != OpcUa::StatusCode::Good)
		{
			response.Header = std::move(h);
		}
		else
		{
			BufferInputChannel bufferInput(buffer);
			IStreamBinary in(bufferInput);
			in >> response;
		}

        CallbackService.post([this, response]()
            {
			if (response.Header.ServiceResult == OpcUa::StatusCode::Good)
			{
				if (Debug) { std::cout << "BinaryClient | Calling callback for Subscription " << response.Parameters.SubscriptionId << std::endl; }
				SubscriptionCallbackMap::const_iterator callbackIt = this->PublishCallbacks.find(response.Parameters.SubscriptionId);
				if (callbackIt == this->PublishCallbacks.end())
				{
					std::cout << "BinaryClient | Error Unknown SubscriptionId " << response.Parameters.SubscriptionId << std::endl;
				}
				else
				{
					try { //calling client code, better put it under try/catch otherwise we crash entire client
						callbackIt->second(response.Parameters);
					}
					catch (const std::exception& ex)
					{
						std::cout << "Error calling application callback " << ex.what() << std::endl;
					}
				}
			}
			else if (response.Header.ServiceResult == OpcUa::StatusCode::BadSessionClosed)
			{
				if (Debug)
				{
					std::cout << "BinaryClient | Session is closed";
				}
			}
			else
			{
				// TODO
			}
			});
	  };
	  std::unique_lock<std::mutex> lock(Mutex);
	  Callbacks.insert(std::make_pair(request.Header.RequestHandle, responseCallback));
	  lock.unlock();
	  Send(request);
	  if (Debug) { std::cout << "binary_client| Publish  <--" << std::endl; }
	}

	virtual RepublishResponse Republish(const RepublishParameters& params)
	{
		if (Debug) { std::cout << "binary_client| Republish -->" << std::endl; }
		RepublishRequest request;
		request.Header = CreateRequestHeader();
		request.Parameters = params;

		RepublishResponse response = Send<RepublishResponse>(request);
		if (Debug) { std::cout << "binary_client| Republish  <--" << std::endl; }
		return response;
	}

	////////////////////////////////////////////////////////////////
	/// View Services
	////////////////////////////////////////////////////////////////
	virtual std::shared_ptr<ViewServices> Views() override
	{
		return shared_from_this();
	}

	virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
	{
		if (Debug)  { std::cout << "binary_client| TranslateBrowsePathsToNodeIds -->" << std::endl; }
		TranslateBrowsePathsToNodeIdsRequest request;
		request.Header = CreateRequestHeader();
		request.Parameters = params;
		const TranslateBrowsePathsToNodeIdsResponse response = Send<TranslateBrowsePathsToNodeIdsResponse>(request);
		if (Debug)  { std::cout << "binary_client| TranslateBrowsePathsToNodeIds <--" << std::endl; }
		return response.Result.Paths;
	}


	virtual std::vector<BrowseResult> Browse(const OpcUa::NodesQuery& query) const
	{
		if (Debug)  {
			std::cout << "binary_client| Browse -->";
			for (BrowseDescription desc : query.NodesToBrowse)
			{
				std::cout << desc.NodeToBrowse << "  ";
			}
			std::cout << std::endl;
		}
		BrowseRequest request;
		request.Header = CreateRequestHeader();
		request.Query = query;
		const BrowseResponse response = Send<BrowseResponse>(request);
		ContinuationPoints.clear();
		for (BrowseResult result : response.Results)
		{
			if (!result.ContinuationPoint.empty())
			{
				ContinuationPoints.push_back(result.ContinuationPoint);
			}
		}
		if (Debug)  { std::cout << "binary_client| Browse <--" << std::endl; }
		return  response.Results;
	}

	virtual std::vector<BrowseResult> BrowseNext() const
	{
		//FIXME: fix method interface so we do not need to decice arbitriraly if we need to send BrowseNext or not...
		if (ContinuationPoints.empty())
		{
			if (Debug)  { std::cout << "No Continuation point, no need to send browse next request" << std::endl; }
			return std::vector<BrowseResult>();
		}
		if (Debug)  { std::cout << "binary_client| BrowseNext -->" << std::endl; }
		BrowseNextRequest request;
		request.ReleaseContinuationPoints = ContinuationPoints.empty() ? true : false;
		request.ContinuationPoints = ContinuationPoints;
		const BrowseNextResponse response = Send<BrowseNextResponse>(request);
		ContinuationPoints.clear();
		for (auto result : response.Results)
		{
			if (!result.ContinuationPoint.empty())
			{
				ContinuationPoints.push_back(result.ContinuationPoint);
			}
		}
		if (Debug)  { std::cout << "binary_client| BrowseNext <--" << std::endl; }
		return response.Results;
	}

	std::vector<NodeId> RegisterNodes(const std::vector<NodeId>& params) const
	{
		if (Debug)
		{
			std::cout << "binary_clinet| RegisterNodes -->\n\tNodes to register:" << std::endl;
			for (auto& param : params) {
				std::cout << "\t\t" << param << std::endl;
			}
		}

		RegisterNodesRequest request;

		request.NodesToRegister = params;
		RegisterNodesResponse response = Send<RegisterNodesResponse>(request);
		if (Debug)
		{
			std::cout << "binary_client| RegisterNodes <--\n\tRegistered NodeIds:" << std::endl;
			for (auto&id : response.Result) {
				std::cout << "\t\t" << id << std::endl;
			}
		}
		return response.Result;
	}

	void UnregisterNodes(const std::vector<NodeId>& params) const
	{
		if (Debug) {
			std::cout << "binary_client| UnregisterNodes -->\n\tUnregistering nodes:" << std::endl;
			for (auto& id : params) {
				std::cout << "\t\t" << id << std::endl;
			}
		}

		UnregisterNodesRequest request;
		request.NodesToUnregister = params;
		UnregisterNodesResponse response = Send<UnregisterNodesResponse>(request);

		if (Debug) {
			std::cout << "binary_client| UnregisterNodes <--" << std::endl;
		}
	}

  private:
    //FIXME: this method should be removed, better add realease option to BrowseNext
    void Release() const
    {
      ContinuationPoints.clear();
      BrowseNext();
    }

  public:

    ////////////////////////////////////////////////////////////////
    /// SecureChannel Services
    ////////////////////////////////////////////////////////////////
    virtual OpcUa::OpenSecureChannelResponse OpenSecureChannel(const OpenSecureChannelParameters& params)
    {
      if (Debug) {std::cout << "binary_client| OpenChannel -->" << std::endl;}

      OpenSecureChannelRequest request;
      request.Parameters = params;

      OpenSecureChannelResponse response = Send<OpenSecureChannelResponse>(request);

      ChannelSecurityToken = response.ChannelSecurityToken; //Save security token, we need it

      if (Debug) {std::cout << "binary_client| OpenChannel <--" << std::endl;}
      return response;
    }

    virtual void CloseSecureChannel(uint32_t channelId)
    {
      try
      {
        if (Debug) {std::cout << "binary_client| CloseSecureChannel -->" << std::endl;}
        SecureHeader hdr(MT_SECURE_CLOSE, CHT_SINGLE, ChannelSecurityToken.SecureChannelId);

        const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
        hdr.AddSize(RawSize(algorithmHeader));

        const SequenceHeader sequence = CreateSequenceHeader();
        hdr.AddSize(RawSize(sequence));

        CloseSecureChannelRequest request;
        //request. ChannelId = channelId; FIXME: spec says it hsould be here, in practice it is not even sent?!?!
        hdr.AddSize(RawSize(request));

        Stream << hdr << algorithmHeader << sequence << request << flush;
        if (Debug) {std::cout << "binary_client| Secure channel closed." << std::endl;}
      }
      catch (const std::exception& exc)
      {
        std::cerr << "Closing secure channel failed with error: " << exc.what() << std::endl;
      }
      if (Debug) {std::cout << "binary_client| CloseSecureChannel <--" << std::endl;}
    }

private:
    template <typename Response, typename Request>
    Response Send(Request request) const
    {
      request.Header = CreateRequestHeader();

      RequestCallback<Response> requestCallback;
      ResponseCallback responseCallback = [&requestCallback](std::vector<char> buffer, ResponseHeader h){
        requestCallback.OnData(std::move(buffer), std::move(h));
      };
      std::unique_lock<std::mutex> lock(Mutex);
      Callbacks.insert(std::make_pair(request.Header.RequestHandle, responseCallback));
      lock.unlock();

      Send(request);

	  Response res;
	  try {
      res = requestCallback.WaitForData(std::chrono::milliseconds(request.Header.Timeout));
	  }
	  catch (std::exception &ex)
	  {
		  //Remove the callback on timeout
		  std::unique_lock<std::mutex> lock(Mutex);
		  Callbacks.erase(request.Header.RequestHandle);
		  lock.unlock();
		  throw;
	  }

	  return res;
    }

    // Prevent multiple threads from sending parts of different packets at the same time.
    mutable std::mutex send_mutex;

    template <typename Request>
    void Send(Request request) const
    {
      // TODO add support for breaking message into multiple chunks
      SecureHeader hdr(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelSecurityToken.SecureChannelId);
      const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
      hdr.AddSize(RawSize(algorithmHeader));

      const SequenceHeader sequence = CreateSequenceHeader();
      hdr.AddSize(RawSize(sequence));
      hdr.AddSize(RawSize(request));

	  std::unique_lock<std::mutex> send_lock(send_mutex);
      Stream << hdr << algorithmHeader << sequence << request << flush;
    }



    void Receive()
    {
      Binary::SecureHeader responseHeader;
      Stream >> responseHeader;

      size_t algo_size;
      if (responseHeader.Type == MessageType::MT_SECURE_OPEN )
      {
        AsymmetricAlgorithmHeader responseAlgo;
        Stream >> responseAlgo;
        algo_size = RawSize(responseAlgo);
      }
      else if (responseHeader.Type == MessageType::MT_ERROR )
      {
        StatusCode error;
        std::string msg;
        Stream >> error;
        Stream >> msg;
        std::stringstream stream;
        stream << "Received error message from server: " << ToString(error) << ", " << msg ;
        throw std::runtime_error(stream.str());
      }
      else //(responseHeader.Type == MessageType::MT_SECURE_MESSAGE )
      {
        Binary::SymmetricAlgorithmHeader responseAlgo;
        Stream >> responseAlgo;
        algo_size = RawSize(responseAlgo);
      }
			
      NodeId id;
      Binary::SequenceHeader responseSequence;
      Stream >> responseSequence; // TODO Check for request Number

      const std::size_t expectedHeaderSize = RawSize(responseHeader) + algo_size + RawSize(responseSequence);
      if (expectedHeaderSize >= responseHeader.Size)
      {
        std::stringstream stream;
        stream << "Size of received message " << responseHeader.Size << " bytes is invalid. Expected size " << expectedHeaderSize << " bytes";
        throw std::runtime_error(stream.str());
      }

      std::size_t dataSize = responseHeader.Size - expectedHeaderSize;
      if(responseHeader.Chunk == CHT_SINGLE)
      {
        parseMessage(dataSize, id);			  
        firstMsgParsed = false;

        std::unique_lock<std::mutex> lock(Mutex);
        CallbackMap::const_iterator callbackIt = Callbacks.find(header.RequestHandle);
        if (callbackIt == Callbacks.end())
        {
          std::cout << "binary_client| No callback found for message with id: " << id << " and handle " << header.RequestHandle << std::endl;
          return;
        }
        callbackIt->second(std::move(messageBuffer), std::move(header));
        messageBuffer.clear();
        Callbacks.erase(callbackIt);
			}
      else if(responseHeader.Chunk == CHT_INTERMEDIATE)
      {
        parseMessage(dataSize, id);
        firstMsgParsed = true;		  
      }	  
    }

    ResponseHeader parseMessage(std::size_t &dataSize, NodeId &id)
    {
      std::vector<char> buffer(dataSize);
      BufferInputChannel bufferInput(buffer);
      Binary::RawBuffer raw(&buffer[0], dataSize);

      Stream >> raw;

      if(!firstMsgParsed)
      {
        IStreamBinary in(bufferInput);
        in >> id;
        in >> header;

        if ( Debug )std::cout << "binary_client| Got response id: " << id << " and handle " << header.RequestHandle<< std::endl;

        if (header.ServiceResult != StatusCode::Good) {
          std::cout << "binary_client| Received a response from server with error status: " << OpcUa::ToString(header.ServiceResult) <<  std::endl;
        }

        if (id == SERVICE_FAULT)
        {
          std::cerr << std::endl;
          std::cerr << "Receive ServiceFault from Server with StatusCode " << OpcUa::ToString(header.ServiceResult) << std::endl;
          std::cerr << std::endl;
        }

        messageBuffer.insert(messageBuffer.end(), buffer.begin(), buffer.end());
        return header;
      }
      else
      {
        messageBuffer.insert(messageBuffer.end(), buffer.begin(), buffer.end());
      }
    }

    Binary::Acknowledge HelloServer(const SecureConnectionParams& params)
    {
      if (Debug) {std::cout << "binary_client| HelloServer -->" << std::endl;}
      Binary::Hello hello;
      hello.ProtocolVersion = 0;
      hello.ReceiveBufferSize = 65536;
      hello.SendBufferSize = 65536;
      hello.MaxMessageSize = 65536;
      hello.MaxChunkCount = 256;
      hello.EndpointUrl = params.EndpointUrl;

      Binary::Header hdr(Binary::MT_HELLO, Binary::CHT_SINGLE);
      hdr.AddSize(RawSize(hello));

      Stream << hdr << hello << flush;

      Header respHeader;
      Stream >> respHeader; // TODO add check for acknowledge header

      Acknowledge ack;
      Stream >> ack; // TODO check for connection parameters
      if (Debug) {std::cout << "binary_client| HelloServer <--" << std::endl;}
      return ack;
    }


    SymmetricAlgorithmHeader CreateAlgorithmHeader() const
    {
      SymmetricAlgorithmHeader algorithmHeader;
      algorithmHeader.TokenId = ChannelSecurityToken.TokenId;
      return algorithmHeader;
    }

    SequenceHeader CreateSequenceHeader() const
    {
      SequenceHeader sequence;
      sequence.SequenceNumber = ++SequenceNumber;
      sequence.RequestId = ++RequestNumber;
      return sequence;
    }

    RequestHeader CreateRequestHeader() const
    {
      RequestHeader header;
      header.SessionAuthenticationToken = AuthenticationToken;
      header.RequestHandle = GetRequestHandle();
      header.Timeout = 10000;
      return header;
    }

    unsigned GetRequestHandle() const
    {
      return ++RequestHandle;
    }

    // Binary client is self-referenced from captures of subscription callbacks
    // Remove this references to make ~BinaryClient() run possible
    void RemoveSelfReferences()
    {
      if (Debug)  { std::cout << "binary_client| Clearing cached references to server" << std::endl; }
      PublishCallbacks.clear();
    }

  private:
    std::shared_ptr<IOChannel> Channel;
    mutable IOStreamBinary Stream;
    SecureConnectionParams Params;
    std::thread ReceiveThread;

    SubscriptionCallbackMap PublishCallbacks;
    SecurityToken ChannelSecurityToken;
    mutable std::atomic<uint32_t> SequenceNumber;
    mutable std::atomic<uint32_t> RequestNumber;
    ExpandedNodeId AuthenticationToken;
    mutable std::atomic<uint32_t> RequestHandle;
    mutable std::vector<std::vector<uint8_t>> ContinuationPoints;
    mutable CallbackMap Callbacks;
    const bool Debug = true;
    bool Finished = false;

    std::thread callback_thread;
    CallbackThread CallbackService;
    mutable std::mutex Mutex;
    
		bool firstMsgParsed = false;
		ResponseHeader header;
  };

  template <>
  void BinaryClient::Send<OpenSecureChannelRequest>(OpenSecureChannelRequest request) const
  {
    SecureHeader hdr(MT_SECURE_OPEN, CHT_SINGLE, ChannelSecurityToken.SecureChannelId);
    AsymmetricAlgorithmHeader algorithmHeader;
    algorithmHeader.SecurityPolicyUri = Params.SecurePolicy;
    algorithmHeader.SenderCertificate = Params.SenderCertificate;
    algorithmHeader.ReceiverCertificateThumbPrint = Params.ReceiverCertificateThumbPrint;
    hdr.AddSize(RawSize(algorithmHeader));
    hdr.AddSize(RawSize(request));

    const SequenceHeader sequence = CreateSequenceHeader();
    hdr.AddSize(RawSize(sequence));
    Stream << hdr << algorithmHeader << sequence << request << flush;
  }

} // namespace


OpcUa::Services::SharedPtr OpcUa::CreateBinaryClient(OpcUa::IOChannel::SharedPtr channel, const OpcUa::SecureConnectionParams& params, bool debug)
{
  return std::make_shared<BinaryClient>(channel, params, debug);
}

OpcUa::Services::SharedPtr OpcUa::CreateBinaryClient(const std::string& endpointUrl, bool debug)
{
  const Common::Uri serverUri(endpointUrl);
  OpcUa::IOChannel::SharedPtr channel = OpcUa::Connect(serverUri.Host(), serverUri.Port());
  OpcUa::SecureConnectionParams params;
  params.EndpointUrl = endpointUrl;
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
  return CreateBinaryClient(channel, params, debug);
}
