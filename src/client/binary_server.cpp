/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote server implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/client/binary_server.h>
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

  typedef std::map<IntegerID, std::function<void (PublishResult)>> SubscriptionCallbackMap;

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
       std::copy(&Buffer[Pos], &Buffer[Pos + size], data);
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

    void OnData(std::vector<char> data)
    {
      Data = std::move(data);
      doneEvent.notify_all();
    }

    T WaitForData(std::chrono::milliseconds msec)
    {
      doneEvent.wait_for(lock, msec);
      T result;
      BufferInputChannel bufferInput(Data);
      IStreamBinary in(bufferInput);
      in >> result;
      return result;
    }

  private:
    std::vector<char> Data;
    std::mutex m;
    std::unique_lock<std::mutex> lock;
    std::condition_variable doneEvent;
  };

  class CallbackThread
  {
    public:
      CallbackThread() : StopRequest(false)
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
          if (Debug)  { std::cout << "binary_client| CallbackThread : waiting for nest post" << std::endl; }
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
      std::mutex Mutex;
      std::condition_variable Condition;
      std::atomic<bool> StopRequest;
      std::queue<std::function<void()>> Queue;
      bool Debug = false;
  };

  class BinaryServer
    : public Services
    , public EndpointServices
    , public ViewServices
    , public SubscriptionServices
    , public AttributeServices
    , public NodeManagementServices
    , public std::enable_shared_from_this<BinaryServer>
  {
  private:
    typedef std::function<void(std::vector<char>)> ResponseCallback;
    typedef std::map<uint32_t, ResponseCallback> CallbackMap;

  public:
    BinaryServer(std::shared_ptr<IOChannel> channel, const SecureConnectionParams& params, bool debug)
      : Channel(channel)
      , Stream(channel)
      , Params(params)
      , SequenceNumber(1)
      , RequestNumber(1)
      , RequestHandle(0)
      , Debug(debug)
      , CallbackService()

    {
      //Initialize the worker thread for subscriptions
      callback_thread = std::thread([&](){ CallbackService.Run(); });

      const Acknowledge& ack = HelloServer(params);
      const OpenSecureChannelResponse& response = OpenChannel();
      ChannelSecurityToken = response.ChannelSecurityToken;
      ReceiveThread = std::move(std::thread([this](){
        try
        {
          while(!Finished)
            Receive();
        }
        catch (const std::exception& exc)
        {
          if (Debug)  { std::cerr << "binary_client| CallbackThread :"; }
          std::cerr << exc.what() << std::endl;
        }
      }));
    }

    ~BinaryServer()
    {
      Finished = true;

      if (Debug) std::cout << "binary_client| Stopping callback thread." << std::endl;
      CallbackService.Stop();
      if (Debug) std::cout << "binary_client| Joining service thread." << std::endl;
      callback_thread.join(); //Not sure it is necessary

      CloseSecureChannel();
      Channel->Stop();
      if (Debug) std::cout << "binary_client| Joining receive thread." << std::endl;
      ReceiveThread.join();


    }

    virtual CreateSessionResponse CreateSession(const RemoteSessionParameters& parameters)
    {
      if (Debug)  { std::cout << "binary_client| CreateSession -->" << std::endl; }
      CreateSessionRequest request;
      request.Header = CreateRequestHeader();

      request.Parameters.ClientDescription.URI = parameters.ClientDescription.URI;
      request.Parameters.ClientDescription.ProductURI = parameters.ClientDescription.ProductURI;
      request.Parameters.ClientDescription.Name = parameters.ClientDescription.Name;
      request.Parameters.ClientDescription.Type = parameters.ClientDescription.Type;
      request.Parameters.ClientDescription.GatewayServerURI = parameters.ClientDescription.GatewayServerURI;
      request.Parameters.ClientDescription.DiscoveryProfileURI = parameters.ClientDescription.DiscoveryProfileURI;
      request.Parameters.ClientDescription.DiscoveryURLs = parameters.ClientDescription.DiscoveryURLs;

      request.Parameters.ServerURI = parameters.ServerURI;
      request.Parameters.EndpointURL = parameters.EndpointURL; // TODO make just endpoint.URL;
      request.Parameters.SessionName = parameters.SessionName;
      request.Parameters.ClientNonce = std::vector<uint8_t>(32,0);
      request.Parameters.ClientCertificate = parameters.ClientCertificate;
      request.Parameters.RequestedSessionTimeout = parameters.Timeout;
      request.Parameters.MaxResponseMessageSize = 65536;
      CreateSessionResponse response = Send<CreateSessionResponse>(request);
      AuthenticationToken = response.Session.AuthenticationToken;
      if (Debug)  { std::cout << "binary_client| CreateSession <--" << std::endl; }
      return response;
    }

    virtual ActivateSessionResponse ActivateSession()
    {
      if (Debug)  { std::cout << "binary_client| ActivateSession -->" << std::endl; }
      ActivateSessionRequest request;
      request.Parameters.LocaleIDs.push_back("en");
      ActivateSessionResponse response = Send<ActivateSessionResponse>(request);
      if (Debug)  { std::cout << "binary_client| ActivateSession <--" << std::endl; }
      return response;
    }

    virtual CloseSessionResponse CloseSession()
    {
      if (Debug)  { std::cout << "binary_client| CloseSession -->" << std::endl; }
      CloseSessionRequest request;
      CloseSessionResponse response = Send<CloseSessionResponse>(request);
      if (Debug)  { std::cout << "binary_client| CloseSession <--" << std::endl; }
      return response;
    }

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

    virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      if (Debug)  { std::cout << "binary_client| GetEndpoints -->" << std::endl; }
      OpcUa::GetEndpointsRequest request;
      request.Header = CreateRequestHeader();
      request.Filter.EndpointURL = filter.EndpointURL;
      request.Filter.LocaleIDs = filter.LocaleIDs;
      request.Filter.ProfileUries = filter.ProfileUries;
      const GetEndpointsResponse response = Send<GetEndpointsResponse>(request);
      if (Debug)  { std::cout << "binary_client| GetEndpoints <--" << std::endl; }
      return response.Endpoints;
    }

    virtual void RegisterServer(const ServerParameters& parameters)
    {
    }

    // ViewServices
    virtual std::shared_ptr<ViewServices> Views() override
    {
      return shared_from_this();
    }

    virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      if (Debug)  { std::cout << "binary_client| TranslateBrowsePathsToNodeIds -->" << std::endl; }
      TranslateBrowsePathsToNodeIDsRequest request;
      request.Header = CreateRequestHeader();
      request.Parameters = params;
      const TranslateBrowsePathsToNodeIDsResponse response = Send<TranslateBrowsePathsToNodeIDsResponse>(request);
      if (Debug)  { std::cout << "binary_client| TranslateBrowsePathsToNodeIds <--" << std::endl; }
      return response.Result.Paths;
    }


    virtual std::vector<BrowseResult> Browse(const OpcUa::NodesQuery& query) const
    {
      if (Debug)  { std::cout << "binary_client| Browse -->" << std::endl; }
      BrowseRequest request;
      request.Header = CreateRequestHeader();
      request.Query = query;
      const BrowseResponse response = Send<BrowseResponse>(request);
      for ( BrowseResult result : response.Results )
      {
        if (! result.ContinuationPoint.empty())
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
      if ( ContinuationPoints.empty() )
      {
        if (Debug)  { std::cout << "No Continuation point, no need to send browse next request" << std::endl; }
        return std::vector<BrowseResult>();
      }
      if (Debug)  { std::cout << "binary_client| BrowseNext -->" << std::endl; }
      BrowseNextRequest request;
      request.ReleaseContinuationPoints = ContinuationPoints.empty() ? true: false;
      request.ContinuationPoints = ContinuationPoints;
      const BrowseNextResponse response = Send<BrowseNextResponse>(request);
      if (Debug)  { std::cout << "binary_client| BrowseNext <--" << std::endl; }
      return response.Results;
    }

  private:
    //FIXME: this method should be removed, better add realease option to BrowseNext
    void Release() const
    {
      ContinuationPoints.clear();
      BrowseNext();
    }

  public:
    virtual std::shared_ptr<NodeManagementServices> NodeManagement() override
    {
      return shared_from_this();
    }

  public:
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

    virtual std::shared_ptr<AttributeServices> Attributes() override
    {
      return shared_from_this();
    }

  public:
    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      if (Debug)  { std::cout << "binary_client| Read -->" << std::endl; }
      ReadRequest request;
      request.Parameters = params;
      const ReadResponse response = Send<ReadResponse>(request);
      if (Debug)  { std::cout << "binary_client| Read <--" << std::endl; }
      return response.Result.Results;
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<WriteValue>& values)
    {
      if (Debug)  { std::cout << "binary_client| Write -->" << std::endl; }
      WriteRequest request;
      request.Parameters.NodesToWrite = values;
      const WriteResponse response = Send<WriteResponse>(request);
      if (Debug)  { std::cout << "binary_client| Write <--" << std::endl; }
      return response.Result.StatusCodes;
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions() override
    {
      return shared_from_this();
    }

    virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback)
    {
      if (Debug)  { std::cout << "binary_client| CreateSubscription -->" << std::endl; }
      const CreateSubscriptionResponse response = Send<CreateSubscriptionResponse>(request);
      if (Debug) std::cout << "BinaryClient | got CreateSubscriptionResponse" << std::endl;
      PublishCallbacks[response.Data.ID] = callback;// TODO Pass calback to the Publish method.
      if (Debug)  { std::cout << "binary_client| CreateSubscription <--" << std::endl; }
      return response.Data;
    }

    virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
    {
      if (Debug)  { std::cout << "binary_client| DeleteSubscriptions -->" << std::endl; }
      DeleteSubscriptionRequest request;
      request.SubscriptionsIds = subscriptions;
      const DeleteSubscriptionResponse response = Send<DeleteSubscriptionResponse>(request);
      if (Debug)  { std::cout << "binary_client| DeleteSubscriptions <--" << std::endl; }
      return response.Results;
    }

    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      if (Debug)  { std::cout << "binary_client| CreateMonitoredItems -->" << std::endl; }
      CreateMonitoredItemsRequest request;
      request.Parameters = parameters;
      const CreateMonitoredItemsResponse response = Send<CreateMonitoredItemsResponse>(request);
      if (Debug)  { std::cout << "binary_client| CreateMonitoredItems <--" << std::endl; }
      return response.Data;
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
      if (Debug) {std::cout << "binary_client| Publish -->" << std::endl << "request with " << originalrequest.Parameters.Acknowledgements.size() << " acks" << std::endl;}
      PublishRequest request(originalrequest); //Should parameter not be const?
      request.Header = CreateRequestHeader();
      request.Header.Timeout = 0; //We do not want the request to timeout!

      ResponseCallback responseCallback = [this](std::vector<char> buffer){
        if (Debug) {std::cout << "BinaryClient | Got Publish Response, from server"  << std::endl;}
        BufferInputChannel bufferInput(buffer);
        IStreamBinary in(bufferInput);
        PublishResponse response;
        in >> response;
        CallbackService.post([this, response]() 
            { 
              if (Debug) {std::cout << "BinaryClient | Calling callback for publishresponse "  << std::endl;}
              this->PublishCallbacks[response.Result.SubscriptionID](response.Result);
            });
      };
      std::unique_lock<std::mutex> lock(Mutex);
      Callbacks.insert(std::make_pair(request.Header.RequestHandle, responseCallback));
      lock.unlock();
      Send(request);
      if (Debug) {std::cout << "binary_client| Publish  <--" << std::endl;}
    }

private:
    template <typename Response, typename Request>
    Response Send(Request request) const
    {
      request.Header = CreateRequestHeader();

      RequestCallback<Response> requestCallback;
      ResponseCallback responseCallback = [&requestCallback](std::vector<char> buffer){
        requestCallback.OnData(std::move(buffer));
      };
      std::unique_lock<std::mutex> lock(Mutex);
      Callbacks.insert(std::make_pair(request.Header.RequestHandle, responseCallback));
      lock.unlock();

      Send(request);

      return requestCallback.WaitForData(std::chrono::milliseconds(request.Header.Timeout));
    }

    template <typename Request>
    void Send(Request request) const
    {
      // TODO add support for breaking message into multiple chunks
      SecureHeader hdr(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelSecurityToken.SecureChannelID);
      const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
      hdr.AddSize(RawSize(algorithmHeader));

      const SequenceHeader sequence = CreateSequenceHeader();
      hdr.AddSize(RawSize(sequence));
      hdr.AddSize(RawSize(request));

      Stream << hdr << algorithmHeader << sequence << request << flush;
    }

    void Receive() const
    {
      Binary::SecureHeader responseHeader;
      Stream >> responseHeader;

      Binary::SymmetricAlgorithmHeader responseAlgo;
      Stream >> responseAlgo;

      Binary::SequenceHeader responseSequence;
      Stream >> responseSequence; // TODO Check for request Number

      const std::size_t expectedHeaderSize = RawSize(responseHeader) + RawSize(responseAlgo) + RawSize(responseSequence);
      if (expectedHeaderSize >= responseHeader.Size)
      {
        std::stringstream stream;
        stream << "Size of received message " << responseHeader.Size << " bytes is invalid. Expected size " << expectedHeaderSize << " bytes";
        throw std::runtime_error(stream.str());
      }

      const std::size_t dataSize = responseHeader.Size - expectedHeaderSize;
      std::vector<char> buffer(dataSize);
      BufferInputChannel bufferInput(buffer);
      Binary::RawBuffer raw(&buffer[0], dataSize);
      Stream >> raw;

      IStreamBinary in(bufferInput);
      NodeID id;
      in >> id;
      ResponseHeader header;
      in >> header;
      if ( Debug )std::cout << "binary_client| Got response id: " << id << " and handle " << header.RequestHandle<< std::endl;

      if (id == SERVICE_FAULT) 
      {
        std::cerr << std::endl;
        std::cerr << "Receive ServiceFault from Server with StatusCode " << ToString(header.ServiceResult) << std::cout ;
        std::cerr << std::endl;
        return;
      }
      std::unique_lock<std::mutex> lock(Mutex);
      CallbackMap::const_iterator callbackIt = Callbacks.find(header.RequestHandle);
      if (callbackIt == Callbacks.end())
      {
        std::cout << "binary_client| No callback found for message with id: " << id << " and handle " << header.RequestHandle << std::endl;
        return;
      }
      callbackIt->second(std::move(buffer));
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

    OpcUa::OpenSecureChannelResponse OpenChannel()
    {
      if (Debug) {std::cout << "binary_client| OpenChannel -->" << std::endl;}
      SecureHeader hdr(MT_SECURE_OPEN, CHT_SINGLE, 0);
      AsymmetricAlgorithmHeader algorithmHeader;
      algorithmHeader.SecurityPolicyURI = Params.SecurePolicy;
      algorithmHeader.SenderCertificate = Params.SenderCertificate;
      algorithmHeader.ReceiverCertificateThumbPrint = Params.ReceiverCertificateThumbPrint;
      hdr.AddSize(RawSize(algorithmHeader));

      const SequenceHeader sequence = CreateSequenceHeader();
      hdr.AddSize(RawSize(sequence));

      OpenSecureChannelRequest openChannel;
      openChannel.ClientProtocolVersion = 0;
      openChannel.RequestType = STR_ISSUE;
      openChannel.SecurityMode = MSM_NONE;
      openChannel.ClientNonce = std::vector<uint8_t>(1, 0);
      openChannel.RequestLifeTime = 300000;
      hdr.AddSize(RawSize(openChannel));

      Stream << hdr << algorithmHeader << sequence << openChannel << flush;

      SecureHeader responseHeader;
      Stream >> responseHeader;
      AsymmetricAlgorithmHeader responseAlgo;
      Stream >> responseAlgo;

      SequenceHeader responseSequence;
      Stream >> responseSequence;
      OpenSecureChannelResponse response;
      Stream >> response;

      if (Debug) {std::cout << "binary_client| OpenChannel <--" << std::endl;}
      return response;
    }

    SymmetricAlgorithmHeader CreateAlgorithmHeader() const
    {
      SymmetricAlgorithmHeader algorithmHeader;
      algorithmHeader.TokenID = ChannelSecurityToken.TokenID;
      return algorithmHeader;
    }

    SequenceHeader CreateSequenceHeader() const
    {
      SequenceHeader sequence;
      sequence.SequenceNumber = ++SequenceNumber;
      sequence.RequestID = ++RequestNumber;
      return sequence;
    }

    void CloseSecureChannel()
    {
      try
      {
        if (Debug) {std::cout << "binary_client| CloseSecureChannel -->" << std::endl;}
        SecureHeader hdr(MT_SECURE_CLOSE, CHT_SINGLE, ChannelSecurityToken.SecureChannelID);

        const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
        hdr.AddSize(RawSize(algorithmHeader));

        const SequenceHeader sequence = CreateSequenceHeader();
        hdr.AddSize(RawSize(sequence));

        CloseSecureChannelRequest request;
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

  private:
    std::shared_ptr<IOChannel> Channel;
    mutable IOStreamBinary Stream;
    SecureConnectionParams Params;
    std::thread ReceiveThread;

    SubscriptionCallbackMap PublishCallbacks;
    SecurityToken ChannelSecurityToken;
    mutable std::atomic<uint32_t> SequenceNumber;
    mutable std::atomic<uint32_t> RequestNumber;
    NodeID AuthenticationToken;
    mutable std::atomic<uint32_t> RequestHandle;
    mutable std::vector<std::vector<uint8_t>> ContinuationPoints;
    mutable CallbackMap Callbacks;
    const bool Debug = true;
    bool Finished = false;

    std::thread callback_thread;
    CallbackThread CallbackService;
    mutable std::mutex Mutex;

  };

} // namespace


OpcUa::Services::SharedPtr OpcUa::CreateBinaryServer(OpcUa::IOChannel::SharedPtr channel, const OpcUa::SecureConnectionParams& params, bool debug)
{
  return OpcUa::Services::SharedPtr(new BinaryServer(channel, params, debug));
}

OpcUa::Services::SharedPtr OpcUa::CreateBinaryServer(const std::string& endpointUrl, bool debug)
{
  const Common::Uri serverUri(endpointUrl);
  OpcUa::IOChannel::SharedPtr channel = OpcUa::Connect(serverUri.Host(), serverUri.Port());
  OpcUa::SecureConnectionParams params;
  params.EndpointUrl = endpointUrl;
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
  return CreateBinaryServer(channel, params, debug);
}
