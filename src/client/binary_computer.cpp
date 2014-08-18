/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote server implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/uri_facade.h>
#include <opc/ua/client/binary_server.h>

#include <opc/ua/protocol/channel.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/client/binary_server.h>
#include <opc/ua/server.h>

#include <atomic>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;


  class ResponseProcessor
  {
  public:
    DEFINE_CLASS_POINTERS(ResponseProcessor);

  public:
    virtual void OnCreateSession(const CreateSessionResponse& response) = 0;
    virtual void OnActivateSession(const ActivateSessionResponse& response) = 0;
    virtual void OnCloseSession(const CloseSessionResponse& response) = 0;
    virtual void OnGetEndpoints(const GetEndpointsResponse& response) = 0;
    virtual void OnFindServers(const FindServersResponse& response) = 0;
    virtual void OnBrowse(const BrowseResponse& response) = 0;
    virtual void OnRead(const ReadResponse& response) = 0;
    virtual void OnWrite(const WriteResponse& response) = 0;
    virtual void OnTranslateBrowsePathsToNodeIDs(const TranslateBrowsePathsToNodeIDsResponse& response) = 0;
    virtual void OnCreateSubscription(const CreateSubscriptionResponse& response) = 0;
    virtual void OnDeleteSubscription(const DeleteSubscriptionResponse& response) = 0;
    virtual void OnCreateMonitoredItems(const CreateMonitoredItemsResponse& response) = 0;
    virtual void OnDeleteMonitoredItems(const DeleteMonitoredItemsResponse& response) = 0;
    virtual void OnPublish(const PublishResponse& response) = 0;
    virtual void OnSetPublishingMode(const SetPublishingModeResponse& response) = 0;
    virtual void OnAddNodes(const AddNodesResponse& response) = 0;
    virtual void OnAddReferenciesNodes(const AddReferencesResponse& response) = 0;

    virtual ~ResponseProcessor(){}

    ResponseProcessor(){}
    ResponseProcessor(const ResponseProcessor&) = delete;
    ResponseProcessor(ResponseProcessor&&) = delete;
    ResponseProcessor& operator=(const ResponseProcessor&) = delete;
  };

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

  private:
    const std::vector<char>& Buffer;
    std::size_t Pos;
  };


  class BufferOutputChannel : public OpcUa::OutputChannel
  {
   public:
     virtual void Send(const char* message, std::size_t size) override
     {
       std::copy(message, message + size, std::back_inserter(Buffer));
     }

     const std::vector<char>& GetBuffer() const
     {
       return Buffer;
     }

  private:
    std::vector<char> Buffer;
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
  public:
    BinaryServer(std::shared_ptr<IOChannel> channel, const Remote::SecureConnectionParams& params, bool debug)
      : Stream(channel)
      , RequestHandle(0)
      , Params(params)
      , Buffer(8192)
      , SequenceNumber(1)
      , RequestNumber(1)
      , Debug(debug)
    {
      const Acknowledge& ack = HelloServer(params);
      const OpenSecureChannelResponse& response = OpenChannel();
      ChannelSecurityToken = response.ChannelSecurityToken;
    }

    ~BinaryServer()
    {
      CloseSecureChannel();
    }

    virtual void CreateSession(const Remote::SessionParameters& parameters)
    {
      CreateSessionRequest request;
      request.Header.RequestHandle = GetRequestHandle();
      request.Header.Timeout = 10000;

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
      Send(request);

      const CreateSessionResponse response = Receive<CreateSessionResponse>();
      AuthenticationToken = response.Session.AuthenticationToken;
    }

    virtual void ActivateSession()
    {
      ActivateSessionRequest activate;
      activate.Header.SessionAuthenticationToken = AuthenticationToken;
      activate.Header.RequestHandle = GetRequestHandle();
      activate.Header.Timeout = 10000;
      activate.Parameters.LocaleIDs.push_back("en");
      Send(activate);

      const ActivateSessionResponse response = Receive<ActivateSessionResponse>();
    }

    virtual void CloseSession()
    {
      CloseSessionRequest closeSession;
      closeSession.Header.SessionAuthenticationToken = AuthenticationToken;
      closeSession.Header.RequestHandle = GetRequestHandle();
      closeSession.Header.Timeout = 10000;
      Send(closeSession);

      const CloseSessionResponse closeResponse = Receive<CloseSessionResponse>();
    }

    virtual std::shared_ptr<Remote::EndpointServices> Endpoints() override
    {
      return shared_from_this();
    }

    virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
    {
      OpcUa::FindServersRequest request;
      request.Parameters = params;
      Send(request);

      const FindServersResponse response = Receive<OpcUa::FindServersResponse>();
      return response.Data.Descriptions;
    }

    virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      OpcUa::GetEndpointsRequest request;
      request.Header.RequestHandle = GetRequestHandle();
      request.Filter.EndpointURL = filter.EndpointURL;
      request.Filter.LocaleIDs = filter.LocaleIDs;
      request.Filter.ProfileUries = filter.ProfileUries;
      Send(request);

      const GetEndpointsResponse response = Receive<OpcUa::GetEndpointsResponse>();
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
      TranslateBrowsePathsToNodeIDsRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters = params;
      Send(request);

      const TranslateBrowsePathsToNodeIDsResponse response = Receive<TranslateBrowsePathsToNodeIDsResponse>();
      return response.Result.Paths;
    }


    virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
    {
      BrowseRequest browse;
      browse.Header.SessionAuthenticationToken = AuthenticationToken;
      browse.Query = query;
      Send(browse);

      BrowseResponse response = Receive<BrowseResponse>();

      if (!response.Results.empty())
      {
        const BrowseResult& result = *response.Results.begin();
        ContinuationPoint = result.ContinuationPoint;
        return result.Referencies;
      }

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
      BrowseNextRequest browseNext;
      browseNext.Header.SessionAuthenticationToken = AuthenticationToken;
      browseNext.ReleaseContinuationPoints= false;
      browseNext.ContinuationPoints.push_back(ContinuationPoint);
      Send(browseNext);

      const BrowseNextResponse response = Receive<BrowseNextResponse>();
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
      AddNodesRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters.NodesToAdd = items;
      Send(request);

      const AddNodesResponse response = Receive<AddNodesResponse>();
      return response.results;
    }

    virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
    {
      AddReferencesRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters.ReferencesToAdd = items;
      Send(request);

      const AddReferencesResponse response = Receive<AddReferencesResponse>();
      return response.Results;
    }

    virtual std::shared_ptr<Remote::AttributeServices> Attributes() override
    {
      return shared_from_this();
    }

  public:
    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      ReadRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters = params;
      Send(request);

      const ReadResponse response = Receive<ReadResponse>();
      return response.Result.Results;
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<WriteValue>& values)
    {
      WriteRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters.NodesToWrite = values;
      Send(request);

      const WriteResponse response = Receive<WriteResponse>();
      return response.Result.StatusCodes;
    }

    virtual std::shared_ptr<Remote::SubscriptionServices> Subscriptions() override
    {
      return shared_from_this();
    }

    virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters, std::function<void (PublishResult)> callback)
    {
      Callback = callback;
      CreateSubscriptionRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters = parameters;
      Send(request);

      const CreateSubscriptionResponse response = Receive<CreateSubscriptionResponse>();
      return response.Data;
    }

    virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
    {
      DeleteSubscriptionRequest request;
      request.SubscriptionsIds = subscriptions;
      Send(request);

      const DeleteSubscriptionResponse response = Receive<DeleteSubscriptionResponse>();
      return response.Results;
    }

    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      CreateMonitoredItemsRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters = parameters;
      Send(request);

      const CreateMonitoredItemsResponse response = Receive<CreateMonitoredItemsResponse>();
      return response.Data;
    }

    virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters params)
    {
      DeleteMonitoredItemsRequest request;
      request.Header.SessionAuthenticationToken = AuthenticationToken;
      request.Parameters = params;
      Send(request);

      const DeleteMonitoredItemsResponse response = Receive<DeleteMonitoredItemsResponse>();
      return response.Results;
    }


    virtual std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIds)
    {
      return std::vector<PublishResult>();
    }

    virtual void Publish(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
    {
      PublishRequest request;
      request.Parameters.Acknowledgements = acknowledgements;
      Send(request);
    }

private:
    template <typename Request>
    void Send(Request request) const
    {
      request.Header.RequestHandle = GetRequestHandle();
      request.Header.Timeout = 10000;
      // TODO add support for breaking message into multiple chunks
      SecureHeader hdr(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelSecurityToken.SecureChannelID);
      const SymmetricAlgorithmHeader algorithmHeader = CreateAlgorithmHeader();
      hdr.AddSize(RawSize(algorithmHeader));

      const SequenceHeader sequence = CreateSequenceHeader();
      hdr.AddSize(RawSize(sequence));
      hdr.AddSize(RawSize(request));

      Stream << hdr << algorithmHeader << sequence << request << flush;
    }

    template<typename ResponseType>
    ResponseType Receive() const
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
      Buffer.resize(dataSize);
      BufferInputChannel bufferInput(Buffer);
      Binary::RawBuffer raw(&Buffer[0], dataSize);
      Stream >> raw;

      IStreamBinary in(bufferInput);
      ResponseType response;
      in >> response;
      return response;
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


    unsigned GetRequestHandle() const
    {
      return ++RequestHandle;
    }

  private:
    Remote::SecureConnectionParams Params;
    mutable std::vector<char> Buffer;

    SecurityToken ChannelSecurityToken;
    mutable uint32_t SequenceNumber;
    mutable uint32_t RequestNumber;
    mutable IOStreamBinary Stream;
    NodeID AuthenticationToken;
    mutable std::atomic<uint32_t> RequestHandle;
    std::function<void (PublishResult)> Callback;
    mutable std::vector<uint8_t> ContinuationPoint;
    std::map<uint32_t, std::function<void()>> RequestsCallbacks;
    const bool Debug = false;
  };

} // namespace


OpcUa::Remote::Server::SharedPtr OpcUa::Remote::CreateBinaryServer(OpcUa::IOChannel::SharedPtr channel, const OpcUa::Remote::SecureConnectionParams& params, bool debug)
{
  return OpcUa::Remote::Server::SharedPtr(new BinaryServer(channel, params, debug));
}
