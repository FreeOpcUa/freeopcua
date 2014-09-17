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
#include <opc/ua/server.h>

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
      CallbackThread() 
      {

      }
      void post(std::function<void()> callback)
      {
        if (Debug)  { std::cout << "CallbackThread | start post" << std::endl; }
        std::unique_lock<std::mutex> lock(Mutex);
        Queue.push(callback);
        Condition.notify_one();
        if (Debug)  { std::cout << "CallbackThread | end post" << std::endl; }
      }

      void Run()
      {
        while (true)
        {
          if (Debug)  { std::cout << "CallbackThread | waiting for nest post" << std::endl; }
          std::unique_lock<std::mutex> lock(Mutex);
          Condition.wait(lock, [&]() { return (StopRequest == true) || ( ! Queue.empty() ) ;} );
          if (StopRequest)
          {
            return;
          }
          while ( ! Queue.empty() ) //to avoid crashing on spurious events
          {
            if (Debug)  { std::cout << "CallbackThread | condition has triggered copying callback and poping. queue size is  " << Queue.size() << std::endl; }
            std::function<void()> callbackcopy = Queue.front();
            Queue.pop();
            lock.unlock();
            if (Debug)  { std::cout << "CallbackThread | now calling callback" << std::endl; }
            callbackcopy();
            lock.lock();
          }
        }
      }

      void Stop()
      {
        StopRequest = true;
        Condition.notify_all();
      }

    private:
      std::mutex Mutex;
      std::condition_variable Condition;
      bool StopRequest = false;
      std::queue<std::function<void()>> Queue;
      bool Debug = false;
  };

  class BinaryServer
    : public Remote::Server
    , public Remote::EndpointServices
    , public Remote::ViewServices
    , public Remote::SubscriptionServices
    , public Remote::AttributeServices
    , public Remote::NodeManagementServices
    , public std::enable_shared_from_this<BinaryServer>
  {
  private:
    typedef std::function<void(std::vector<char>)> ResponseCallback;
    typedef std::map<uint32_t, ResponseCallback> CallbackMap;

  public:
    BinaryServer(std::shared_ptr<IOChannel> channel, const Remote::SecureConnectionParams& params, bool debug)
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

    virtual void CreateSession(const Remote::SessionParameters& parameters)
    {
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
    }

    virtual void ActivateSession()
    {
      ActivateSessionRequest request;
      request.Parameters.LocaleIDs.push_back("en");
      ActivateSessionResponse response = Send<ActivateSessionResponse>(request);
    }

    virtual void CloseSession()
    {
      CloseSessionRequest request;
      CloseSessionResponse response = Send<CloseSessionResponse>(request);
    }

    virtual std::shared_ptr<Remote::EndpointServices> Endpoints() override
    {
      return shared_from_this();
    }

    virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
    {
      OpcUa::FindServersRequest request;
      request.Parameters = params;
      FindServersResponse response = Send<FindServersResponse>(request);
      return response.Data.Descriptions;
    }

    virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      OpcUa::GetEndpointsRequest request;
      request.Header = CreateRequestHeader();
      request.Filter.EndpointURL = filter.EndpointURL;
      request.Filter.LocaleIDs = filter.LocaleIDs;
      request.Filter.ProfileUries = filter.ProfileUries;
      const GetEndpointsResponse response = Send<GetEndpointsResponse>(request);
      return response.Endpoints;
    }

    virtual void RegisterServer(const Remote::ServerParameters& parameters)
    {
    }

    // ViewServices
    virtual std::shared_ptr<Remote::ViewServices> Views() override
    {
      return shared_from_this();
    }

    virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      if (Debug) std::cout << "BinaryClient | sending TranslateBrowsPathToNodeIds to server" << std::endl;
      TranslateBrowsePathsToNodeIDsRequest request;
      request.Header = CreateRequestHeader();
      request.Parameters = params;
      const TranslateBrowsePathsToNodeIDsResponse response = Send<TranslateBrowsePathsToNodeIDsResponse>(request);
      if (Debug) std::cout << "BinaryClient | got TranslateBrowsPathToNodeIds repsonse " << std::endl;
      return response.Result.Paths;
    }


    virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
    {
      if (Debug) std::cout << "BinaryClient | sending Browse to server" << std::endl;
      BrowseRequest request;
      request.Header = CreateRequestHeader();
      request.Query = query;
      const BrowseResponse response = Send<BrowseResponse>(request);
      if (!response.Results.empty())
      {
        const BrowseResult& result = *response.Results.begin();
        ContinuationPoint = result.ContinuationPoint;
        return result.Referencies;
      }
      if (Debug) std::cout << "BinaryClient | got Browse repsonse " << std::endl;

      return  std::vector<ReferenceDescription>();
    }

    virtual std::vector<ReferenceDescription> BrowseNext() const
    {
      if (ContinuationPoint.empty())
      {
        return std::vector<ReferenceDescription>();
      }

      const std::vector<ReferenceDescription>& referencies = Next();
      if (referencies.empty())
      {
        Release();
      }
      return referencies;
    }

  private:
    std::vector<ReferenceDescription> Next() const
    {
      return SendBrowseNext(false);
    }

    void Release() const
    {
      SendBrowseNext(true);
    }

    std::vector<ReferenceDescription> SendBrowseNext(bool releasePoint) const
    {
      BrowseNextRequest request;
      request.ReleaseContinuationPoints= false;
      request.ContinuationPoints.push_back(ContinuationPoint);
      const BrowseNextResponse response = Send<BrowseNextResponse>(request);
      return !response.Results.empty() ? response.Results.begin()->Referencies :  std::vector<ReferenceDescription>();
    }

  public:
    virtual std::shared_ptr<Remote::NodeManagementServices> NodeManagement() override
    {
      return shared_from_this();
    }

  public:
    virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items)
    {
      if (Debug) std::cout << "BinaryClient | sending AddNodes to server" << std::endl;
      AddNodesRequest request;
      request.Parameters.NodesToAdd = items;
      const AddNodesResponse response = Send<AddNodesResponse>(request);
      if (Debug) std::cout << "BinaryClient | Got AddNodes response" << std::endl;
      return response.results;
    }

    virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
    {
      if (Debug) std::cout << "BinaryClient | sending AddReferences to server" << std::endl;
      AddReferencesRequest request;
      request.Parameters.ReferencesToAdd = items;
      const AddReferencesResponse response = Send<AddReferencesResponse>(request);
      if (Debug) std::cout << "BinaryClient | Got AddReferences response" << std::endl;
      return response.Results;
    }

    virtual std::shared_ptr<Remote::AttributeServices> Attributes() override
    {
      return shared_from_this();
    }

  public:
    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      if (Debug) std::cout << "BinaryClient | sending Read to server" << std::endl;
      ReadRequest request;
      request.Parameters = params;
      const ReadResponse response = Send<ReadResponse>(request);
      if (Debug) std::cout << "BinaryClient | got Read reponse" << std::endl;
      return response.Result.Results;
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<WriteValue>& values)
    {
      if (Debug) std::cout << "BinaryClient | sending Write to server" << std::endl;
      WriteRequest request;
      request.Parameters.NodesToWrite = values;
      const WriteResponse response = Send<WriteResponse>(request);
      if (Debug) std::cout << "BinaryClient | got Write reponse" << std::endl;
      return response.Result.StatusCodes;
    }

    virtual std::shared_ptr<Remote::SubscriptionServices> Subscriptions() override
    {
      return shared_from_this();
    }

    virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback)
    {
      if (Debug) std::cout << "BinaryClient | sending CreateSubscriptionRequest to server" << std::endl;
      const CreateSubscriptionResponse response = Send<CreateSubscriptionResponse>(request);
      if (Debug) std::cout << "BinaryClient | got CreateSubscriptionResponse" << std::endl;
      PublishCallbacks[response.Data.ID] = callback;// TODO Pass calback to the Publish method.
      return response.Data;
    }

    virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
    {
      if (Debug) std::cout << "BinaryClient | sending DeleteSubscriptionRequest to server" << std::endl;
      DeleteSubscriptionRequest request;
      request.SubscriptionsIds = subscriptions;
      const DeleteSubscriptionResponse response = Send<DeleteSubscriptionResponse>(request);
      return response.Results;
    }

    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      if (Debug) std::cout << "BinaryClient | sending CreateMonitoredItems to server" << std::endl;
      CreateMonitoredItemsRequest request;
      request.Parameters = parameters;
      const CreateMonitoredItemsResponse response = Send<CreateMonitoredItemsResponse>(request);
      if (Debug) std::cout << "BinaryClient | got CreateMonitoredItems response" << std::endl;
      return response.Data;
    }

    virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params)
    {
      if (Debug) std::cout << "BinaryClient | sending DeleteMonitoredItems to server" << std::endl;
      DeleteMonitoredItemsRequest request;
      request.Parameters = params;
      const DeleteMonitoredItemsResponse response = Send<DeleteMonitoredItemsResponse>(request);
      return response.Results;
    }

    virtual void Publish(const PublishRequest& originalrequest)
    {
      if (Debug) {std::cout << "BinaryClient | Sending publish request with " << originalrequest.Parameters.Acknowledgements.size() << " acks" << std::endl;}
      PublishRequest request(originalrequest); //Should parameter not be const?
      request.Header = CreateRequestHeader();

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
      Callbacks.insert(std::make_pair(request.Header.RequestHandle, responseCallback));
      Send(request);
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
      Callbacks.insert(std::make_pair(request.Header.RequestHandle, responseCallback));

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
      if ( Debug )std::cout << "Got data with id: " << id << " and handle " << header.RequestHandle<< std::endl;

      CallbackMap::const_iterator callbackIt = Callbacks.find(header.RequestHandle);
      if (callbackIt == Callbacks.end())
      {
        std::cout << "No callback found for message with id: " << id << " and handle " << header.RequestHandle << std::endl;
        return;
      }
      callbackIt->second(std::move(buffer));
    }

    Binary::Acknowledge HelloServer(const Remote::SecureConnectionParams& params)
    {
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
      return ack;
    }

    OpcUa::OpenSecureChannelResponse OpenChannel()
    {
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
        SecureHeader hdr(MT_SECURE_CLOSE, CHT_SINGLE, ChannelSecurityToken.SecureChannelID);

        const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
        hdr.AddSize(RawSize(algorithmHeader));

        const SequenceHeader sequence = CreateSequenceHeader();
        hdr.AddSize(RawSize(sequence));

        CloseSecureChannelRequest request;
        hdr.AddSize(RawSize(request));

        Stream << hdr << algorithmHeader << sequence << request << flush;
      }
      catch (const std::exception& exc)
      {
        std::cerr << "Closing secure channel failed with error: " << exc.what() << std::endl;
      }
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
    Remote::SecureConnectionParams Params;
    std::thread ReceiveThread;

    SubscriptionCallbackMap PublishCallbacks;
    SecurityToken ChannelSecurityToken;
    mutable uint32_t SequenceNumber;
    mutable uint32_t RequestNumber;
    NodeID AuthenticationToken;
    mutable std::atomic<uint32_t> RequestHandle;
    mutable std::vector<uint8_t> ContinuationPoint;
    mutable CallbackMap Callbacks;
    const bool Debug = true;
    bool Finished = false;

    std::thread callback_thread;
    CallbackThread CallbackService;

  };

} // namespace


OpcUa::Remote::Server::SharedPtr OpcUa::Remote::CreateBinaryServer(OpcUa::IOChannel::SharedPtr channel, const OpcUa::Remote::SecureConnectionParams& params, bool debug)
{
  return OpcUa::Remote::Server::SharedPtr(new BinaryServer(channel, params, debug));
}

OpcUa::Remote::Server::SharedPtr OpcUa::Remote::CreateBinaryServer(const std::string& endpointUrl, bool debug)
{
  const Common::Uri serverUri(endpointUrl);
  OpcUa::IOChannel::SharedPtr channel = OpcUa::Connect(serverUri.Host(), serverUri.Port());
  OpcUa::Remote::SecureConnectionParams params;
  params.EndpointUrl = endpointUrl;
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
  return CreateBinaryServer(channel, params, debug);
}
