/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa binary protocol connection processor.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "opc_tcp_processor.h"

#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/opcua_protocol.h>

#include <opc/common/uri_facade.h>
#include <opc/ua/connection_listener.h>
#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/input_from_buffer.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/status_codes.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/tcp_server.h>

#include <chrono>
#include <iostream>
#include <list>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <queue>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using namespace OpcUa::UaServer;

  void PrintBlob(const std::vector<char>& buf)
  {
    unsigned pos = 0;
    std::cout << "length: " << buf.size() << std::endl;
    for (const auto it : buf)
    {
      if (pos)
        printf((pos % 16 == 0) ? "\n" : " ");

      printf("%02x", (unsigned)it & 0x000000FF);

      if (it > ' ')
        std::cout << "(" << it << ")";
      else
        std::cout << "   ";

      ++pos;
    }

    std::cout << std::endl << std::flush;
  }



  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    OpcTcp(OpcUa::Remote::Server::SharedPtr computer, bool debug)
      : Server(computer)
      , Debug(debug)
    {
    }

    virtual void Process(OpcUa::IOChannel::SharedPtr clientChannel)
    {
      if (!clientChannel)
      {
        if (Debug) std::cerr << "opc_tcp_processor| Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      if (Debug) std::clog << "opc_tcp_processor| Hello client!" << std::endl;

      std::auto_ptr<OpcTcpMessages> messageProcessor(new OpcTcpMessages(Server, *clientChannel, Debug));

      for(;;)
      {
        double period = messageProcessor->GetNextSleepPeriod();
        int res = clientChannel->WaitForData(period); //double to float cast
        if (res < 0)
        {
          return;
        }
        else if (res == 1)
        {
          ProcessData(*clientChannel, *messageProcessor);
        }
        else
        {
          //SendPublishResponse(*clientChannel);
        }
      }
    }

    virtual void StopProcessing(OpcUa::IOChannel::SharedPtr clientChannel)
    {
    }

  private:
    void ProcessData(OpcUa::IOChannel& clientChannel, OpcTcpMessages& messageProcessor)
    {
      using namespace OpcUa::Binary;

      IStreamBinary iStream(clientChannel);
      ProcessChunk(iStream, messageProcessor);
    }

    // TODO implement collecting full message from chunks before processing.
    void ProcessChunk(IStreamBinary& iStream, OpcTcpMessages& messageProcessor)
    {
      if (Debug) std::cout << "opc_tcp_processor| Processing new chunk." << std::endl;
      Header hdr;
      // Receive message header.
      iStream >> hdr;

      // Receive full message.
      std::vector<char> buffer(hdr.MessageSize());
      OpcUa::Binary::RawBuffer buf(&buffer[0], buffer.size());
      iStream >> buf;
      if (Debug)
      {
        std::clog << "opc_tcp_processor| Received message." << std::endl;
        PrintBlob(buffer);
      }

      // restrict server size code only with current message.
      OpcUa::InputFromBuffer messageChannel(&buffer[0], buffer.size());
      IStreamBinary messageStream(messageChannel);
      messageProcessor.ProcessMessage(hdr.Type, messageStream);

      if (messageChannel.GetRemainSize())
      {
        std::cerr << "opc_tcp_processor| ERROR!!! Message from client has been processed partially." << std::endl;
      }
    }

  private:
    OpcUa::Remote::Server::SharedPtr Server;
    bool Debug;
  };

  class OpcUaProtocol : public OpcUa::UaServer::OpcUaProtocol
  {
  public:
    DEFINE_CLASS_POINTERS(OpcUaProtocol);

  public:
    OpcUaProtocol(OpcUa::UaServer::TcpServer::SharedPtr tcpServer, bool debug)
      : Debug(debug)
      , TcpAddon(tcpServer)
    {
    }

    virtual void StartEndpoints(const std::vector<EndpointDescription>& endpoints, OpcUa::Remote::Server::SharedPtr server) override
    {
      for (const EndpointDescription endpoint : endpoints)
      {
        const Common::Uri uri(endpoint.EndpointURL);
        if (uri.Scheme() == "opc.tcp")
        {
          std::shared_ptr<IncomingConnectionProcessor> processor(new OpcTcp(server, Debug));
          TcpParameters tcpParams;
          tcpParams.Port = uri.Port();
          if (Debug) std::clog << "opc_tcp_processor| Starting listen port " << tcpParams.Port << std::endl;
          TcpAddon->Listen(tcpParams, processor);
          Ports.push_back(tcpParams);
        }
      }
    }

    virtual void StopEndpoints() override
    {
      for (const TcpParameters& params : Ports)
      {
        TcpAddon->StopListen(params);
      }
      TcpAddon.reset();
    }

  private:
    OpcUa::UaServer::TcpServer::SharedPtr TcpAddon;
    std::vector<TcpParameters> Ports;
    bool Debug;
  };

}

namespace OpcUa
{
  namespace UaServer
  {

    OpcTcpMessages::OpcTcpMessages(std::shared_ptr<OpcUa::Remote::Server> computer, OpcUa::OutputChannel& outputChannel, bool debug)
      : Server(computer)
      , OutputStream(outputChannel)
      , Debug(debug)
      , ChannelID(1)
      , TokenID(2)
    {
      SessionID = NumericNodeID(5, 0);
      std::cout << "opc_tcp_processor| Debug is " << Debug << std::endl;
      std::cout << "opc_tcp_processor| SessionID is " << Debug << std::endl;
    }

    void OpcTcpMessages::ProcessMessage(MessageType msgType, IStreamBinary& iStream)
    {
      switch (msgType)
      {
        case MT_HELLO:
        {
          if (Debug) std::clog << "opc_tcp_processor| Accepted hello message." << std::endl;
          HelloClient(iStream, OutputStream);
          break;
        }


        case MT_SECURE_OPEN:
        {
          if (Debug) std::clog << "opc_tcp_processor| Opening secure channel." << std::endl;
          OpenChannel(iStream, OutputStream);
          break;
        }

        case MT_SECURE_CLOSE:
        {
          if (Debug) std::clog << "opc_tcp_processor| Closing secure channel." << std::endl;
          CloseChannel(iStream);
          return;
        }

        case MT_SECURE_MESSAGE:
        {
          ProcessRequest(iStream, OutputStream);
          break;
        }

        case MT_ACKNOWLEDGE:
        {
          if (Debug) std::clog << "opc_tcp_processor| Received acknowledge from client. This should not have happend..." << std::endl;
          throw std::logic_error("Thank to client about acknowledge.");
        }
        case MT_ERROR:
        {
          if (Debug) std::clog << "opc_tcp_processor| There is an error happend in the client!" << std::endl;
          throw std::logic_error("It is very nice get to know server about error in the client.");
        }
        default:
        {
          if (Debug) std::clog << "opc_tcp_processor| Unknown message type '" << msgType << "' received!" << std::endl;
          throw std::logic_error("Invalid message type received.");
        }
      }
    }
/*
    void OpcTcpMessages::SendPublishResponse(OpcUa::OutputChannel& clientChannel)
    {
      OStreamBinary stream(clientChannel);
      for (SubscriptionBinaryData& subdata: Subscriptions)
      {
        if ( PublishRequestQueue.size() == 0)
        {
          std::cerr << "opc_tcp_processor| RequestQueueSize is empty we are waiting for pubilshrequest from server" << std::endl;
          return;
        }
        std::cout << "opc_tcp_processor| We have x publishrequest in queue "<<  PublishRequestQueue.size() << std::endl;

        std::chrono::duration<double> now =  std::chrono::system_clock::now().time_since_epoch(); //make sure it is in milliseconds
        if ((now - subdata.last_check) <= subdata.period)
        {
          if (Debug) std::cout << "opc_tcp_processor| No need to process subscription yet" << std::endl;
          continue;
        }
        subdata.last_check = now;

        std::vector<IntegerID> sub_query;
        sub_query.push_back(subdata.SubscriptionID);
        std::vector<PublishResult> res_list = Server->Subscriptions()->PopPublishResults(sub_query);

        for (const PublishResult& publishResult: res_list)
        {

          PublishRequestElement requestData = PublishRequestQueue.front();
          PublishRequestQueue.pop();

          PublishResponse response;
          FillResponseHeader(requestData.requestHeader, response.Header);
          response.Result = publishResult;


          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(requestData.algorithmHeader));
          secureHeader.AddSize(RawSize(requestData.sequence));
          secureHeader.AddSize(RawSize(response));
          if (Debug) {
            std::cout << "opc_tcp_processor| Sedning publishResponse with " << response.Result.Message.Data.size() << " PublishResults" << std::endl;
            for  ( NotificationData d: response.Result.Message.Data )
            {
              std::cout << "opc_tcp_processor|      " << d.DataChange.Notification.size() <<  " modified items" << std::endl;
            }
          }
          stream << secureHeader << requestData.algorithmHeader << requestData.sequence << response << flush;
        }
      }
    }
*/
    double OpcTcpMessages::GetNextSleepPeriod()
    {
      if ( Subscriptions.size() == 0 || PublishRequestQueue.size() == 0)
      {
        return  9999;
      }
      std::chrono::duration<double> now =  std::chrono::system_clock::now().time_since_epoch();
      std::chrono::duration<double>  next_fire = std::chrono::duration<double>(std::numeric_limits<double>::max() ) ;

      for (const SubscriptionBinaryData& data: Subscriptions)
      {
        std::chrono::duration<double> tmp =  data.last_check + data.period;
        if (Debug) std::cout << "opc_tcp_processor| Time since last check : " << (now - data.last_check).count() << " Period: " << data.period.count() << " time to next fire: " << (tmp - now).count() << std::endl;
        if (tmp < next_fire)
        {
          next_fire = tmp;
        }
      }
      auto diff = next_fire - now;
      if ( diff.count() < 0 )
      {
        if (Debug)  std::cout << "opc_tcp_processor| Event should allrady have been fired returning 0"<< std::endl;
        return 0;
      }
      return diff.count() ;
    }

    void OpcTcpMessages::HelloClient(IStreamBinary& istream, OStreamBinary& ostream)
    {
      using namespace OpcUa::Binary;

      if (Debug) std::clog << "opc_tcp_processor| Reading hello message." << std::endl;
      Hello hello;
      istream >> hello;

      Acknowledge ack;
      ack.ReceiveBufferSize = hello.ReceiveBufferSize;
      ack.SendBufferSize = hello.SendBufferSize;
      ack.MaxMessageSize = hello.MaxMessageSize;
      ack.MaxChunkCount = 1;

      Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      if (Debug) std::clog << "opc_tcp_processor| Sending answer to client." << std::endl;
      ostream << ackHeader << ack << flush;
    }

    void OpcTcpMessages::OpenChannel(IStreamBinary& istream, OStreamBinary& ostream)
    {
      uint32_t channelID = 0;
      istream >> channelID;
      AsymmetricAlgorithmHeader algorithmHeader;
      istream >> algorithmHeader;

      if (algorithmHeader.SecurityPolicyURI != "http://opcfoundation.org/UA/SecurityPolicy#None")
      {
        throw std::logic_error(std::string("Client want to create secure channel with unsupported policy '") + algorithmHeader.SecurityPolicyURI + std::string("'"));
      }

      SequenceHeader sequence;
      istream >> sequence;

      OpenSecureChannelRequest request;
      istream >> request;

      if (request.SecurityMode != MSM_NONE)
      {
        throw std::logic_error("Unsupported security mode.");
      }

      if (request.RequestType == STR_RENEW)
      {
        //FIXME:Should check that channel has been issued first
        ++TokenID;
      }

      sequence.SequenceNumber = ++SequenceNb;

      OpenSecureChannelResponse response;
      FillResponseHeader(request.Header, response.Header);
      response.ChannelSecurityToken.SecureChannelID = ChannelID;
      response.ChannelSecurityToken.TokenID = TokenID;
      response.ChannelSecurityToken.CreatedAt = OpcUa::CurrentDateTime();
      response.ChannelSecurityToken.RevisedLifetime = request.RequestLifeTime;

      SecureHeader responseHeader(MT_SECURE_OPEN, CHT_SINGLE, ChannelID);
      responseHeader.AddSize(RawSize(algorithmHeader));
      responseHeader.AddSize(RawSize(sequence));
      responseHeader.AddSize(RawSize(response));
      ostream << responseHeader << algorithmHeader << sequence << response << flush;
    }

    void OpcTcpMessages::CloseChannel(IStreamBinary& istream)
    {
      uint32_t channelID = 0;
      istream >> channelID;

      SymmetricAlgorithmHeader algorithmHeader;
      istream >> algorithmHeader;

      SequenceHeader sequence;
      istream >> sequence;

      CloseSecureChannelRequest request;
      istream >> request;
    }

    void OpcTcpMessages::ProcessRequest(IStreamBinary& istream, OStreamBinary& ostream)
    {
      uint32_t channelID = 0;
      istream >> channelID;

      SymmetricAlgorithmHeader algorithmHeader;
      istream >> algorithmHeader;

      SequenceHeader sequence;
      istream >> sequence;

      NodeID typeID;
      istream >> typeID;

      RequestHeader requestHeader;
      istream >> requestHeader;

      sequence.SequenceNumber = ++SequenceNb;

      const std::size_t receivedSize =
        RawSize(channelID) +
        RawSize(algorithmHeader) +
        RawSize(sequence) +
        RawSize(typeID) +
        RawSize(requestHeader);

      const OpcUa::MessageID message = GetMessageID(typeID);
      switch (message)
      {
        case OpcUa::GET_ENDPOINTS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing get endpoints request." << std::endl;
          EndpointsFilter filter;
          istream >> filter;

          GetEndpointsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Endpoints = Server->Endpoints()->GetEndpoints(filter);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::FIND_SERVERS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Find Servers' request." << std::endl;
          FindServersParameters params;
          istream >> params;

          FindServersResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Data.Descriptions = Server->Endpoints()->FindServers(params);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::BROWSE_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing browse request." << std::endl;
          NodesQuery query;
          istream >> query;

          OpcUa::BrowseResult result;
          result.Referencies = Server->Views()->Browse(query);

          BrowseResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Results.push_back(result);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::READ_REQUEST:
        {
          ReadParameters params;
          istream >> params;

          if (Debug)
          {
            std::clog << "opc_tcp_processor| Processing read request for Node:";
            for (AttributeValueID id : params.AttributesToRead)
            {
              std::clog << "opc_tcp_processor|  " << id.Node;
            }
            std::cout << std::endl;
          }

          ReadResponse response;
          FillResponseHeader(requestHeader, response.Header);
          std::vector<DataValue> values;
          if (std::shared_ptr<OpcUa::Remote::AttributeServices> service = Server->Attributes())
          {
            values = service->Read(params);
          }
          else
          {
            for (auto attribID : params.AttributesToRead)
            {
              DataValue value;
              value.Encoding = DATA_VALUE_STATUS_CODE;
              value.Status = OpcUa::StatusCode::BadNotImplemented;
              values.push_back(value);
            }
          }
          response.Result.Results = values;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }

        case OpcUa::WRITE_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing write request." << std::endl;
          WriteParameters params;
          istream >> params;

          WriteResponse response;
          FillResponseHeader(requestHeader, response.Header);
          std::vector<DataValue> values;
          if (std::shared_ptr<OpcUa::Remote::AttributeServices> service = Server->Attributes())
          {
            response.Result.StatusCodes = service->Write(params.NodesToWrite);
          }
          else
          {
            response.Result.StatusCodes = std::vector<StatusCode>(params.NodesToWrite.size(), OpcUa::StatusCode::BadNotImplemented);
          }

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }

        case TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Translate Browse Paths To Node IDs' request." << std::endl;
          TranslateBrowsePathsParameters params;
          istream >> params;

          if (Debug)
          {
            for ( BrowsePath path : params.BrowsePaths)
            {
              std::cout << "opc_tcp_processor| Requested path is: " << path.StartingNode << " : " ;
              for ( RelativePathElement el : path.Path.Elements)
              {
                std::cout << "/" << el.TargetName ;
              }
              std::cout << std::endl;
            }
          }

          std::vector<BrowsePathResult> result = Server->Views()->TranslateBrowsePathsToNodeIds(params);

          if (Debug)
          {
            for (BrowsePathResult res: result)
            {
              std::cout << "opc_tcp_processor| Result of browsePath is: " << (uint) res.Status << ". Target is: ";
              for ( BrowsePathTarget path : res.Targets)
              {
                std::cout << path.Node ;
              }
              std::cout << std::endl;
            }
          }

          TranslateBrowsePathsToNodeIDsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Result.Paths = result;
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Translate Browse Paths To Node IDs' request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }


        case CREATE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing create session request." << std::endl;
          SessionParameters params;
          istream >> params;

          CreateSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Session.SessionID = SessionID;
          response.Session.AuthenticationToken = SessionID;
          response.Session.RevisedSessionTimeout = params.RequestedSessionTimeout;
          response.Session.MaxRequestMessageSize = 65536;
          EndpointsFilter epf;
          response.Session.ServerEndpoints = Server->Endpoints()->GetEndpoints(epf);


          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }
        case ACTIVATE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing activate session request." << std::endl;
          UpdatedSessionParameters params;
          istream >> params;

          ActivateSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CLOSE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing close session request." << std::endl;
          bool deleteSubscriptions = false;
          istream >> deleteSubscriptions;

          if (deleteSubscriptions)
          {
            std::vector<IntegerID> subs;
            for (const SubscriptionBinaryData& data: Subscriptions)
            {
              subs.push_back(data.SubscriptionID);
            }
            Server->Subscriptions()->DeleteSubscriptions(subs);
            Subscriptions.clear();
          }

          CloseSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          if (Debug) std::clog << "opc_tcp_processor| Session Closed " << std::endl;
          return;
        }

        case CREATE_SUBSCRIPTION_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing create subscription request." << std::endl;
          SubscriptionParameters params;
          istream >> params;

          CreateSubscriptionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Data = Server->Subscriptions()->CreateSubscription(params);
          SubscriptionBinaryData SubData;
          SubData.SubscriptionID = response.Data.ID;
          SubData.period =  std::chrono::duration<double>(response.Data.RevisedPublishingInterval/1000); //seconds
          Subscriptions.push_back(SubData);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case DELETE_SUBSCRIPTION_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing delete subscription request." << std::endl;
          std::vector<IntegerID> ids;
          istream >> ids;

          DeleteSubscriptions(ids); //remove from locale subscription lis

          DeleteSubscriptionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Results = Server->Subscriptions()->DeleteSubscriptions(ids);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to Delete Subscription Request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CREATE_MONITORED_ITEMS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Create Monitored Items' request." << std::endl;
          MonitoredItemsParameters params;
          istream >> params;

          CreateMonitoredItemsResponse response;

          response.Data = Server->Subscriptions()->CreateMonitoredItems(params);

          FillResponseHeader(requestHeader, response.Header);
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to Create Monitored Items Request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case DELETE_MONITORED_ITEMS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Delete Monitored Items' request." << std::endl;
          DeleteMonitoredItemsParameters params;
          istream >> params;

          DeleteMonitoredItemsResponse response;

          response.Results = Server->Subscriptions()->DeleteMonitoredItems(params);

          FillResponseHeader(requestHeader, response.Header);
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to Delete Monitored Items Request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case PUBLISH_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Publish' request." << std::endl;
          PublishParameters params;
          istream >> params;
          PublishRequestElement data;
          data.sequence = sequence;
          data.algorithmHeader = algorithmHeader;
          data.requestHeader = requestHeader;
          PublishRequestQueue.push(data);
          Server->Subscriptions()->Publish(params.Acknowledgements);

          --SequenceNb; //We do not send response, so do not increase sequence

          return;
        }

        case SET_PUBLISHING_MODE_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Set Publishing Mode' request." << std::endl;
          PublishingModeParameters params;
          istream >> params;

          //FIXME: forward request to internal server!!
          SetPublishingModeResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Result.Statuses.resize(params.SubscriptionIDs.size(), StatusCode::Good);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Set Publishing Mode' request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case ADD_NODES_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Add Nodes' request." << std::endl;
          AddNodesParameters params;
          istream >> params;

          std::vector<AddNodesResult> results = Server->NodeManagement()->AddNodes(params.NodesToAdd);

          AddNodesResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.results = results;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Add Nodes' request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case ADD_REFERENCES_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Add References' request." << std::endl;
          AddReferencesParameters params;
          istream >> params;

          std::vector<StatusCode> results = Server->NodeManagement()->AddReferences(params.ReferencesToAdd);

          AddReferencesResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Results = results;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Add References' request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        default:
        {
          std::stringstream ss;
          ss << "opc_tcp_processor| ERROR: Unknown message with id '" << message << "' was recieved.";
          throw std::logic_error(ss.str());
        }
      }
    }

    void OpcTcpMessages::FillResponseHeader(const RequestHeader& requestHeader, ResponseHeader& responseHeader)
    {
       //responseHeader.InnerDiagnostics.push_back(DiagnosticInfo());
       responseHeader.Timestamp = CurrentDateTime();
       responseHeader.RequestHandle = requestHeader.RequestHandle;
    }

    void OpcTcpMessages::DeleteSubscriptions(const std::vector<IntegerID>& ids)
    {
      for ( auto id : ids )
      {
        Subscriptions.erase(std::remove_if(Subscriptions.begin(), Subscriptions.end(),
                      [&](const SubscriptionBinaryData d) { return ( d.SubscriptionID == id) ; }), Subscriptions.end());
      }
    }

  } // namespace UaServer
} // namespace OpcUa

OpcUa::UaServer::OpcUaProtocol::UniquePtr OpcUa::UaServer::CreateOpcUaProtocol(OpcUa::UaServer::TcpServer::SharedPtr tcpServer, bool debug)
{
  return OpcUaProtocol::UniquePtr(new ::OpcUaProtocol(tcpServer, debug));
}
