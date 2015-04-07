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

#include "opcua_protocol.h"

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
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/services_registry.h>

#include <boost/thread/locks.hpp>
#include <chrono>
#include <iostream>
#include <list>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <queue>


namespace OpcUa
{
  namespace Server
  {

    using namespace OpcUa::Binary;

    OpcTcpMessages::OpcTcpMessages(std::shared_ptr<OpcUa::Services> computer, OpcUa::OutputChannel& outputChannel, bool debug)
      : Server(computer)
      , OutputStream(outputChannel)
      , Debug(debug)
      , ChannelId(1)
      , TokenId(2)
      , SessionId(GenerateSessionId())
      , SequenceNb(0)
    {
      std::cout << "opc_tcp_processor| Debug is " << Debug << std::endl;
      std::cout << "opc_tcp_processor| SessionId is " << Debug << std::endl;
    }


    OpcTcpMessages::~OpcTcpMessages()
    {
      // This is a hack, we cannot leave subcsriptoins running since they have a cllback to us
      try
      {
        DeleteAllSubscriptions();
      }
      catch (const std::exception& exc)
      {
        std::cerr << "Error during stopping OpcTcpMessages. " << exc.what() <<std::endl;
      }
    }

    bool OpcTcpMessages::ProcessMessage(MessageType msgType, IStreamBinary& iStream)
    {
      boost::unique_lock<boost::shared_mutex> lock(ProcessMutex);

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
          return false;
        }

        case MT_SECURE_MESSAGE:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing secure message." << std::endl;
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
      return true;
    }

    void OpcTcpMessages::ForwardPublishResponse(const PublishResult result)
    {
      boost::unique_lock<boost::shared_mutex> lock(ProcessMutex);

      if (Debug) std::clog << "opc_tcp_processor| Sending PublishResult to client!" << std::endl;
      if ( PublishRequestQueue.empty() )
      {
        std::cerr << "Error trying to send publish response while we do not have data from a PublishRequest" << std::endl;
        return;
      }
      PublishRequestElement requestData = PublishRequestQueue.front();
      PublishRequestQueue.pop();

      PublishResponse response;

      FillResponseHeader(requestData.requestHeader, response.Header);
      response.Result = result;
     
      requestData.sequence.SequenceNumber = ++SequenceNb;

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(requestData.algorithmHeader));
      secureHeader.AddSize(RawSize(requestData.sequence));
      secureHeader.AddSize(RawSize(response));
      if (Debug) {
        std::cout << "opc_tcp_processor| Sedning publishResponse with " << response.Result.Message.Data.size() << " PublishResults" << std::endl;
      }
      OutputStream << secureHeader << requestData.algorithmHeader << requestData.sequence << response << flush;
    };
    
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
      uint32_t channelId = 0;
      istream >> channelId;
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

      if (request.Parameters.SecurityMode != MessageSecurityMode::None)
      {
        throw std::logic_error("Unsupported security mode.");
      }

      if (request.Parameters.RequestType == SecurityTokenRequestType::Renew)
      {
        //FIXME:Should check that channel has been issued first
        ++TokenId;
      }

      sequence.SequenceNumber = ++SequenceNb;

      OpenSecureChannelResponse response;
      FillResponseHeader(request.Header, response.Header);
      response.ChannelSecurityToken.SecureChannelId = ChannelId;
      response.ChannelSecurityToken.TokenId = TokenId;
      response.ChannelSecurityToken.CreatedAt = OpcUa::DateTime::Current();
      response.ChannelSecurityToken.RevisedLifetime = request.Parameters.RequestLifeTime;

      SecureHeader responseHeader(MT_SECURE_OPEN, CHT_SINGLE, ChannelId);
      responseHeader.AddSize(RawSize(algorithmHeader));
      responseHeader.AddSize(RawSize(sequence));
      responseHeader.AddSize(RawSize(response));
      ostream << responseHeader << algorithmHeader << sequence << response << flush;
    }

    void OpcTcpMessages::CloseChannel(IStreamBinary& istream)
    {
      uint32_t channelId = 0;
      istream >> channelId;

      SymmetricAlgorithmHeader algorithmHeader;
      istream >> algorithmHeader;

      SequenceHeader sequence;
      istream >> sequence;

      CloseSecureChannelRequest request;
      istream >> request;
    }

    void OpcTcpMessages::ProcessRequest(IStreamBinary& istream, OStreamBinary& ostream)
    {
      uint32_t channelId = 0;
      istream >> channelId;

      SymmetricAlgorithmHeader algorithmHeader;
      istream >> algorithmHeader;

      SequenceHeader sequence;
      istream >> sequence;

      NodeId typeId;
      istream >> typeId;

      RequestHeader requestHeader;
      istream >> requestHeader;

      sequence.SequenceNumber = ++SequenceNb;
/*
      const std::size_t receivedSize =
        RawSize(channelId) +
        RawSize(algorithmHeader) +
        RawSize(sequence) +
        RawSize(typeId) +
        RawSize(requestHeader);
*/
      const OpcUa::MessageId message = GetMessageId(typeId);
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

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::FIND_ServerS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Find Servers' request." << std::endl;
          FindServersParameters params;
          istream >> params;

          FindServersResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Data.Descriptions = Server->Endpoints()->FindServers(params);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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

          BrowseResponse response;
          response.Results =  Server->Views()->Browse(query);

          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
            for (ReadValueId id : params.AttributesToRead)
            {
              std::clog << "opc_tcp_processor|  " << id.NodeId;
            }
            std::cout << std::endl;
          }

          ReadResponse response;
          FillResponseHeader(requestHeader, response.Header);
          std::vector<DataValue> values;
          if (std::shared_ptr<OpcUa::AttributeServices> service = Server->Attributes())
          {
            values = service->Read(params);
          }
          else
          {
            for (auto attribId : params.AttributesToRead)
            {
              DataValue value;
              value.Encoding = DATA_VALUE_STATUS_CODE;
              value.Status = OpcUa::StatusCode::BadNotImplemented;
              values.push_back(value);
            }
          }
          response.Results = values;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
          if (std::shared_ptr<OpcUa::AttributeServices> service = Server->Attributes())
          {
            response.Results = service->Write(params.NodesToWrite);
          }
          else
          {
            response.Results = std::vector<StatusCode>(params.NodesToWrite.size(), OpcUa::StatusCode::BadNotImplemented);
          }

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }

        case TRANSLATE_BROWSE_PATHS_TO_NODE_IdS_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Translate Browse Paths To Node Ids' request." << std::endl;
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
              std::cout << "opc_tcp_processor| Result of browsePath is: " << (uint32_t) res.Status << ". Target is: ";
              for ( BrowsePathTarget path : res.Targets)
              {
                std::cout << path.Node ;
              }
              std::cout << std::endl;
            }
          }

          TranslateBrowsePathsToNodeIdsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Result.Paths = result;
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Translate Browse Paths To Node Ids' request." << std::endl;
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

          response.Session.SessionId = SessionId;
          response.Session.AuthenticationToken = SessionId;
          response.Session.RevisedSessionTimeout = params.RequestedSessionTimeout;
          response.Session.MaxRequestMessageSize = 65536;
          EndpointsFilter epf;
          response.Session.ServerEndpoints = Server->Endpoints()->GetEndpoints(epf);


          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
            DeleteAllSubscriptions();
          }

          CloseSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
          CreateSubscriptionRequest request;
          istream >> request.Parameters;
          request.Header = requestHeader;

          CreateSubscriptionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Data = Server->Subscriptions()->CreateSubscription(request, [this](PublishResult i){ 
                try
                {
                  this->ForwardPublishResponse(i); 
                }
                catch (std::exception& ex)
                {
                  // TODO Disconnect client!
                  std::cerr << "Error forwarding publishResult to client: " << ex.what() << std::endl;
                }
              });

          Subscriptions.push_back(response.Data.Id); //Keep a link to eventually delete subcriptions when exiting

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case DELETE_SUBSCRIPTION_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing delete subscription request." << std::endl;
          std::vector<IntegerId> ids;
          istream >> ids;

          DeleteSubscriptions(ids); //remove from locale subscription lis

          DeleteSubscriptionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Results = Server->Subscriptions()->DeleteSubscriptions(ids);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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
          PublishRequest request;
          request.Header = requestHeader;
          istream >> request.Parameters;

          PublishRequestElement data;
          data.sequence = sequence;
          data.algorithmHeader = algorithmHeader;
          data.requestHeader = requestHeader;
          PublishRequestQueue.push(data);
          Server->Subscriptions()->Publish(request);

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
          response.Result.Statuses.resize(params.SubscriptionIds.size(), StatusCode::Good);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
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

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Add References' request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case REPUBLISH_REQUEST:
        {
          if (Debug) std::clog << "opc_tcp_processor| Processing 'Republish' request." << std::endl;
          RepublishParameters params;
          istream >> params;

          //Not implemented so we just say we do not have that notification
          RepublishResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Header.ServiceResult = StatusCode::BadMessageNotAvailable;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "opc_tcp_processor| Sending response to 'Republish' request." << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        default:
        {
          ServiceFaultResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Header.ServiceResult = StatusCode::BadNotImplemented;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::cerr << "opc_tcp_processor| Sending ServiceFaultResponse to unsupported request of id: " << message << std::endl;
          ostream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }
      }
    }

    void OpcTcpMessages::FillResponseHeader(const RequestHeader& requestHeader, ResponseHeader& responseHeader)
    {
       //responseHeader.InnerDiagnostics.push_back(DiagnosticInfo());
       responseHeader.Timestamp = DateTime::Current();
       responseHeader.RequestHandle = requestHeader.RequestHandle;
    }

    void OpcTcpMessages::DeleteAllSubscriptions()
    {
      std::vector<IntegerId> subs;
      for (const IntegerId& subid: Subscriptions)
      {
        subs.push_back(subid);
      }
      Server->Subscriptions()->DeleteSubscriptions(subs);
      Subscriptions.clear();
    }

    void OpcTcpMessages::DeleteSubscriptions(const std::vector<IntegerId>& ids)
    {
      for ( auto id : ids )
      {
        Subscriptions.erase(std::remove_if(Subscriptions.begin(), Subscriptions.end(),
                      [&](const IntegerId d) { return ( d == id) ; }), Subscriptions.end());
      }
    }

  } // namespace UaServer
} // namespace OpcUa
