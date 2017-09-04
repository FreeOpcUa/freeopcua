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
#include <opc/ua/node.h>
#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/input_from_buffer.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/status_codes.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/services_registry.h>

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

OpcTcpMessages::OpcTcpMessages(OpcUa::Services::SharedPtr server, OpcUa::OutputChannel::SharedPtr outputChannel, const Common::Logger::SharedPtr & logger)
  : Server(server)
  , OutputChannel(outputChannel)
  // do not create a reference loop - if OutputStream is called with a
  // shared_ptr it holds a strong reference to it! So call it with a dereferenced
  // pointer
  , OutputStream(*outputChannel)
  , Logger(logger)
  , ChannelId(1)
  , TokenId(2)
  , SessionId(GenerateSessionId())
  , SequenceNb(0)
{
  LOG_INFO(Logger, "opc_tcp_processor     | log level: {}", Logger->level());
  LOG_INFO(Logger, "opc_tcp_processor     | SessionId; {}", SessionId);
}


OpcTcpMessages::~OpcTcpMessages()
{
  // This is a hack, we cannot leave subscriptions running since they have a callback to us
  try
    {
      DeleteAllSubscriptions();
    }

  catch (const std::exception & exc)
    {
      LOG_ERROR(Logger, "opc_tcp_processor     | stopping OpcTcpMessages failed: {}", exc.what());
    }
}

bool OpcTcpMessages::ProcessMessage(MessageType msgType, IStreamBinary & iStream)
{
  std::lock_guard<std::mutex> lock(ProcessMutex);

  switch (msgType)
    {
    case MT_HELLO:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | accepted hello message");

      HelloClient(iStream, OutputStream);
      break;
    }


    case MT_SECURE_OPEN:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | opening secure channel");

      OpenChannel(iStream, OutputStream);
      break;
    }

    case MT_SECURE_CLOSE:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | closing secure channel");

      CloseChannel(iStream);
      return false;
    }

    case MT_SECURE_MESSAGE:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing secure message");

      ProcessRequest(iStream, OutputStream);
      break;
    }

    case MT_ACKNOWLEDGE:
    {
      LOG_ERROR(Logger, "opc_tcp_processor     | received acknowledge from client: this should not have happend...");

      throw std::logic_error("Thank to client about acknowledge.");
    }

    case MT_ERROR:
    {
      LOG_ERROR(Logger, "opc_tcp_processor     | client signaled an error");

      throw std::logic_error("It is very nice get to know server about error in the client.");
    }

    default:
    {
      LOG_ERROR(Logger, "opc_tcp_processor     | unknown message type '{}' received", msgType);

      throw std::logic_error("unknown message type received.");
    }
    }

  return true;
}

void OpcTcpMessages::ForwardPublishResponse(const PublishResult result)
{
  std::lock_guard<std::mutex> lock(ProcessMutex);

  LOG_DEBUG(Logger, "opc_tcp_processor     | sending PublishResult to client");

  // get a shared_ptr from weak_ptr to make sure OutputChannel
  // does not get deleted before end of operation
  OpcUa::OutputChannel::SharedPtr outputChannel = OutputChannel.lock();
  // test if OutputChannel was still active when calling lock
  if (!outputChannel) {
    LOG_WARN(Logger, "opc_tcp_processor     | parent instance already deleted");
    return;
  }

  if (PublishRequestQueue.empty())
    {
      LOG_WARN(Logger, "error trying to send publish response while we do not have data from a PublishRequest");
      return;
    }

  PublishRequestElement requestData = PublishRequestQueue.front();
  PublishRequestQueue.pop();

  PublishResponse response;

  FillResponseHeader(requestData.requestHeader, response.Header);
  response.Parameters = result;

  requestData.sequence.SequenceNumber = ++SequenceNb;

  SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
  secureHeader.AddSize(RawSize(requestData.algorithmHeader));
  secureHeader.AddSize(RawSize(requestData.sequence));
  secureHeader.AddSize(RawSize(response));

  LOG_DEBUG(Logger, "opc_tcp_processor     | sending PublishResponse with: {} PublishResults", response.Parameters.NotificationMessage.NotificationData.size());

  OutputStream << secureHeader << requestData.algorithmHeader << requestData.sequence << response << flush;
}

void OpcTcpMessages::HelloClient(IStreamBinary & istream, OStreamBinary & ostream)
{
  using namespace OpcUa::Binary;

  LOG_DEBUG(Logger, "opc_tcp_processor     | reading hello message");

  Hello hello;
  istream >> hello;

  Acknowledge ack;
  ack.ReceiveBufferSize = hello.ReceiveBufferSize;
  ack.SendBufferSize = hello.SendBufferSize;
  ack.MaxMessageSize = hello.MaxMessageSize;
  ack.MaxChunkCount = 1;

  Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
  ackHeader.AddSize(RawSize(ack));

  LOG_DEBUG(Logger, "opc_tcp_processor     | sending answer");

  ostream << ackHeader << ack << flush;
}

void OpcTcpMessages::OpenChannel(IStreamBinary & istream, OStreamBinary & ostream)
{
  uint32_t channelId = 0;
  istream >> channelId;
  AsymmetricAlgorithmHeader algorithmHeader;
  istream >> algorithmHeader;

  if (algorithmHeader.SecurityPolicyUri != "http://opcfoundation.org/UA/SecurityPolicy#None")
    {
      throw std::logic_error(std::string("Client want to create secure channel with unsupported policy '") + algorithmHeader.SecurityPolicyUri + std::string("'"));
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

void OpcTcpMessages::CloseChannel(IStreamBinary & istream)
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

void OpcTcpMessages::ProcessRequest(IStreamBinary & istream, OStreamBinary & ostream)
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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Get Endpoints' request");

      GetEndpointsParameters filter;
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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Find Servers' request");

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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Browse' request");

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

      if (Logger && Logger->should_log(spdlog::level::debug))
        {
          Logger->debug("opc_tcp_processor     | processing 'Read' request for Node:");

          for (ReadValueId id : params.AttributesToRead)
            {
              std::string name = "unknown";
                {
                  Node node(Server, id.NodeId);
                  name = node.GetBrowseName().Name;
                }
              Logger->debug("opc_tcp_processor     |   {} ({})", id.NodeId, name);
            }
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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Write' request");

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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Translate Browse Paths To Node Ids' request");

      TranslateBrowsePathsParameters params;
      istream >> params;

      if (Logger && Logger->should_log(spdlog::level::debug))
        {
          for (BrowsePath path : params.BrowsePaths)
            {
              std::stringstream result;
              result << path.StartingNode << ":";

              for (RelativePathElement el : path.Path.Elements)
                {
                  result << "/" << el.TargetName ;
                }
              Logger->debug("opc_tcp_processor     | requested path is: {}", result.str());
            }
        }

      std::vector<BrowsePathResult> result = Server->Views()->TranslateBrowsePathsToNodeIds(params);

      if (Logger && Logger->should_log(spdlog::level::debug))
        {
          for (BrowsePathResult res : result)
            {
              std::stringstream target;
              for (BrowsePathTarget path : res.Targets)
                {
                  target << path.Node ;
                }
              Logger->debug("opc_tcp_processor     | result of browsePath is: {}, target is: {}", (uint32_t)res.Status, target.str());
            }
        }

      TranslateBrowsePathsToNodeIdsResponse response;
      FillResponseHeader(requestHeader, response.Header);
      response.Result.Paths = result;
      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Translate Browse Paths To Node Ids' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }


    case CREATE_SESSION_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Create Session' request");

      CreateSessionParameters params;
      istream >> params;

      CreateSessionResponse response;
      FillResponseHeader(requestHeader, response.Header);

      response.Parameters.SessionId = SessionId;
      response.Parameters.AuthenticationToken = SessionId;
      response.Parameters.RevisedSessionTimeout = params.RequestedSessionTimeout;
      response.Parameters.MaxRequestMessageSize = 65536;
      GetEndpointsParameters epf;
      response.Parameters.ServerEndpoints = Server->Endpoints()->GetEndpoints(epf);


      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));
      ostream << secureHeader << algorithmHeader << sequence << response << flush;

      return;
    }

    case ACTIVATE_SESSION_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Activate Session' request");

      ActivateSessionParameters params;
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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Close Session' request");

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

      LOG_DEBUG(Logger, "opc_tcp_processor     | session closed");

      return;
    }

    case CREATE_SUBSCRIPTION_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Create Subscription' request");

      CreateSubscriptionRequest request;
      istream >> request.Parameters;
      request.Header = requestHeader;

      CreateSubscriptionResponse response;
      FillResponseHeader(requestHeader, response.Header);

      SharedPtr self = shared_from_this();
      response.Data = Server->Subscriptions()->CreateSubscription(request, [self](PublishResult i)
      {
        try
          {
            self->ForwardPublishResponse(i);
          }

        catch (std::exception & ex)
          {
            // TODO Disconnect client!
            LOG_WARN(self->Logger, "error forwarding PublishResult to client: {}", ex.what());
          }
      });

      Subscriptions.push_back(response.Data.SubscriptionId); //Keep a link to eventually delete subcriptions when exiting

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));
      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case MODIFY_SUBSCRIPTION_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Modify Subscription' request");

      ModifySubscriptionRequest request;
      istream >> request.Parameters;
      request.Header = requestHeader;

      ModifySubscriptionResponse response = Server->Subscriptions()->ModifySubscription(request.Parameters);
      FillResponseHeader(requestHeader, response.Header);

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Modify Subscription' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case DELETE_SUBSCRIPTION_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Delete Subscription' request");

      std::vector<uint32_t> ids;
      istream >> ids;

      DeleteSubscriptions(ids); //remove from locale subscription lis

      DeleteSubscriptionsResponse response;
      FillResponseHeader(requestHeader, response.Header);

      response.Results = Server->Subscriptions()->DeleteSubscriptions(ids);

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Delete Subscription' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case CREATE_MONITORED_ITEMS_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Create Monitored Items' request");

      MonitoredItemsParameters params;
      istream >> params;

      CreateMonitoredItemsResponse response;

      response.Results = Server->Subscriptions()->CreateMonitoredItems(params);

      FillResponseHeader(requestHeader, response.Header);
      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Create Monitored Items' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case DELETE_MONITORED_ITEMS_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Delete Monitored Items' request");

      DeleteMonitoredItemsParameters params;
      istream >> params;

      DeleteMonitoredItemsResponse response;

      response.Results = Server->Subscriptions()->DeleteMonitoredItems(params);

      FillResponseHeader(requestHeader, response.Header);
      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Delete Monitored Items' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case PUBLISH_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Publish' request");

      PublishRequest request;
      request.Header = requestHeader;
      istream >> request.SubscriptionAcknowledgements;

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
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Set Publishing Mode' request");

      PublishingModeParameters params;
      istream >> params;

      //FIXME: forward request to internal server!!
      SetPublishingModeResponse response;
      FillResponseHeader(requestHeader, response.Header);
      response.Result.Results.resize(params.SubscriptionIds.size(), StatusCode::Good);

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Set Publishing Mode' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case ADD_NODES_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Add Nodes' request");

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

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Add Nodes' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case ADD_REFERENCES_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Add References' request");

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

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Add References' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case REPUBLISH_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Republish' request");

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

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Republish' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case CALL_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Call' request");

      CallParameters params;
      istream >> params;

      CallResponse response;
      FillResponseHeader(requestHeader, response.Header);

      if (std::shared_ptr<OpcUa::MethodServices> service = Server->Method())
        {
          response.Results = service->Call(params.MethodsToCall);
        }

      else
        {
          for (auto callMethodRequest : params.MethodsToCall)
            {
              OpcUa::CallMethodResult result;
              result.Status = OpcUa::StatusCode::BadNotImplemented;
              response.Results.push_back(result);
            }
        }

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));
      ostream << secureHeader << algorithmHeader << sequence << response << flush;

      return;
    }

    case OpcUa::REGISTER_NODES_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Register Nodes' request");

      RegisterNodesRequest request;

      istream >> request.NodesToRegister;

      RegisterNodesResponse response;
      response.Result = Server->Views()->RegisterNodes(request.NodesToRegister);

      FillResponseHeader(requestHeader, response.Header);

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Register Nodes' request");

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }

    case OpcUa::UNREGISTER_NODES_REQUEST:
    {
      LOG_DEBUG(Logger, "opc_tcp_processor     | processing 'Unregister Nodes' request");

      UnregisterNodesRequest request;

      istream >> request.NodesToUnregister;

      UnregisterNodesResponse response;
      Server->Views()->UnregisterNodes(request.NodesToUnregister);

      FillResponseHeader(requestHeader, response.Header);

      SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelId);
      secureHeader.AddSize(RawSize(algorithmHeader));
      secureHeader.AddSize(RawSize(sequence));
      secureHeader.AddSize(RawSize(response));

      LOG_DEBUG(Logger, "opc_tcp_processor     | sending response to 'Unregister Nodes' request");

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

      LOG_WARN(Logger, "opc_tcp_processor     | sending 'ServiceFaultResponse' to unsupported request of id: {}", message);

      ostream << secureHeader << algorithmHeader << sequence << response << flush;
      return;
    }
    }
}

void OpcTcpMessages::FillResponseHeader(const RequestHeader & requestHeader, ResponseHeader & responseHeader)
{
  //responseHeader.InnerDiagnostics.push_back(DiagnosticInfo());
  responseHeader.Timestamp = DateTime::Current();
  responseHeader.RequestHandle = requestHeader.RequestHandle;
}

void OpcTcpMessages::DeleteAllSubscriptions()
{
  std::vector<uint32_t> subs;

  for (const uint32_t & subid : Subscriptions)
    {
      subs.push_back(subid);
    }

  Server->Subscriptions()->DeleteSubscriptions(subs);
  Subscriptions.clear();
}

void OpcTcpMessages::DeleteSubscriptions(const std::vector<uint32_t> & ids)
{
  for (auto id : ids)
    {
      Subscriptions.erase(std::remove_if(Subscriptions.begin(), Subscriptions.end(),
      [&](const uint32_t d) { return (d == id) ; }), Subscriptions.end());
    }
}

} // namespace UaServer
} // namespace OpcUa
