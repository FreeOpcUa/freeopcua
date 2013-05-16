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

#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/status_codes.h>

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <sstream>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using namespace OpcUa::Server;


  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    OpcTcp(std::shared_ptr<OpcUa::Remote::Computer> computer, bool debug)
      : Computer(computer)
      , Debug(debug)
      , ChannelID(1)
      , TokenID(2)
    {
    }

    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      std::unique_lock<std::mutex> lock(ProcessMutex);
      if (!clientChannel)
      {
        if (Debug) std::cerr << "Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      if (Debug) std::clog << "Hello client!" << std::endl;
      OpcUa::Binary::IOStream stream(clientChannel);
      while(ProcessChunk(stream));
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }

  private:
    bool ProcessChunk(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      Header hdr;
      stream >> hdr;
      switch (hdr.Type)
      {
        case MT_HELLO:
        {
          if (Debug) std::clog << "Accepted hello message." << std::endl;
          HelloClient(stream);
          break;
        }


        case MT_SECURE_OPEN:
        {
          if (Debug) std::clog << "Opening securechannel." << std::endl;
          OpenChannel(stream);
          break;
        }

        case MT_SECURE_CLOSE:
        {
          if (Debug) std::clog << "Closing secure channel." << std::endl;
          CloseChannel(stream);
          return false;
        }

        case MT_SECURE_MESSAGE:
        {
          ProcessMessage(stream, hdr.MessageSize());
          break;
        }

        case MT_ACKNOWLEDGE:
        {
          if (Debug) std::clog << "Received acknowledge from client. He mustn't do that.." << std::endl;
          throw std::logic_error("Thank to client about acknowledge.");
        }
        case MT_ERROR:
        {
          if (Debug) std::clog << "There is an error happend in the client!" << std::endl;
          throw std::logic_error("It is very nice get to know server about error in the client.");
        }
        default:
        {
          if (Debug) std::clog << "Unknown message received!" << std::endl;
          throw std::logic_error("Invalid message type received.");
        }
      }
      return true;
    }

    void HelloClient(OpcUa::Binary::IOStream& stream)
    {
      using namespace OpcUa::Binary;

      if (Debug) std::clog << "Reading hello message." << std::endl;
      Hello hello;
      stream >> hello;

      Acknowledge ack;
      ack.ReceiveBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.SendBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxMessageSize = OPCUA_DEFAULT_BUFFER_SIZE;
      ack.MaxChunkCount = 1;

      Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      if (Debug) std::clog << "Sending answer to client." << std::endl;
      stream << ackHeader << ack << flush; 
    }

    void OpenChannel(IOStream& stream)
    {
      uint32_t channelID = 0;
      stream >> channelID;
      AsymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      if (algorithmHeader.SecurityPolicyURI != "http://opcfoundation.org/UA/SecurityPolicy#None")
      {
        throw std::logic_error(std::string("Client want to create secure channel with invalid policy '") + algorithmHeader.SecurityPolicyURI + std::string("'"));
      }

      SequenceHeader sequence;
      stream >> sequence;

      OpenSecureChannelRequest request;
      stream >> request;

      if (request.RequestType != STR_ISSUE)
      {
        throw std::logic_error("Client have to create secure channel!");
      }

      if (request.SecurityMode != MSM_NONE)
      {
        throw std::logic_error("Unsupported security mode.");
      }

      OpenSecureChannelResponse response;
      FillResponseHeader(request.Header, response.Header);
      response.ChannelSecurityToken.SecureChannelID = ChannelID;
      response.ChannelSecurityToken.TokenID = TokenID;
      response.ChannelSecurityToken.CreatedAt = OpcUa::CurrentDateTime();
      response.ChannelSecurityToken.RevisedLifetime = 3600;

      SecureHeader responseHeader(MT_SECURE_OPEN, CHT_SINGLE, ChannelID);
      responseHeader.AddSize(RawSize(algorithmHeader));
      responseHeader.AddSize(RawSize(sequence));
      responseHeader.AddSize(RawSize(response));
      stream << responseHeader << algorithmHeader << sequence << response << flush;
    }

    void CloseChannel(IOStream& stream)
    {
      uint32_t channelID = 0;
      stream >> channelID;
     
      SymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      SequenceHeader sequence;
      stream >> sequence;

      CloseSecureChannelRequest request;
      stream >> request;
    }

    void ProcessMessage(IOStream& stream, std::size_t messageSize)
    {
      uint32_t channelID = 0;
      stream >> channelID;
     
      SymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      SequenceHeader sequence;
      stream >> sequence;

      NodeID typeID;
      stream >> typeID;

      RequestHeader requestHeader;
      stream >> requestHeader;

      const std::size_t receivedSize =
        RawSize(channelID) +
        RawSize(algorithmHeader) +
        RawSize(sequence) +
        RawSize(typeID) +
        RawSize(requestHeader);

      const std::size_t restSize = messageSize - receivedSize;

      const OpcUa::MessageID message = GetMessageID(typeID);
      switch (message)
      {
        case OpcUa::GET_ENDPOINTS_REQUEST:
        {
          if (Debug) std::clog << "Processing get endpoints request." << std::endl;
          EndpointsFilter filter;
          stream >> filter;

          GetEndpointsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Endpoints = Computer->Endpoints()->GetEndpoints(filter);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::BROWSE_REQUEST:
        {
          if (Debug) std::clog << "Processing browse request." << std::endl;
          NodesQuery query;
          stream >> query;

          BrowseResponse response;
          FillResponseHeader(requestHeader, response.Header);

          for (auto node : query.NodesToBrowse)
          {
            BrowseResult result;
            OpcUa::Remote::BrowseParameters browseParams;
            browseParams.Description = node;
            browseParams.MaxReferenciesCount = query.MaxReferenciesPerNode;

            result.Referencies = Computer->Views()->Browse(browseParams);
            response.Results.push_back(result);
          }

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::READ_REQUEST:
        {
          if (Debug) std::clog << "Processing read request." << std::endl;
          ReadParameters params;
          stream >> params;

          ReadResponse response;
          FillResponseHeader(requestHeader, response.Header);
          std::vector<DataValue> values;
          if (std::shared_ptr<OpcUa::Remote::AttributeServices> service = Computer->Attributes())
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
          stream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }

        case CREATE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "Processing create session request." << std::endl;
          SessionParameters params;
          stream >> params;

          CreateSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          response.Session.SessionID = SessionID;
          response.Session.AuthenticationToken = SessionID;
          response.Session.RevisedSessionTimeout = params.RequestedSessionTimeout;
          response.Session.MaxRequestMessageSize = 65536;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }
        case ACTIVATE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "Processing activate session request." << std::endl;
          UpdatedSessionParameters params;
          stream >> params;

          ActivateSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CLOSE_SESSION_REQUEST:
        {
          if (Debug) std::clog << "Processing close session request." << std::endl;
          bool deleteSubscriptions = false;
          stream >> deleteSubscriptions;

          CloseSessionResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CREATE_SUBSCRIPTION_REQUEST:
        {
          if (Debug) std::clog << "Processing create subscription request." << std::endl;
          SubscriptionParameters params;
          stream >> params;

          CreateSubscriptionResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Data.ID = 2;
          response.Data.RevisedLifetimeCount = params.RequestedLifetimeCount;
          response.Data.RevisedPublishingInterval = params.RequestedPublishingInterval;
          response.Data.RevizedMaxKeepAliveCount = params.RequestedMaxKeepAliveCount;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case CREATE_MONITORED_ITEMS_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Create Monitored Items' request." << std::endl;
          std::vector<char> data(restSize);
          RawBuffer buffer(&data[0], restSize);
          stream >> buffer;

          CreateMonitoredItemsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to Create Monitored Items Request." << std::endl;
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Translate Browse Paths To Node IDs' request." << std::endl;
          std::vector<char> data(restSize);
          RawBuffer buffer(&data[0], restSize);
          stream >> buffer;

          TranslateBrowsePathsToNodeIDsResponse response;
          FillResponseHeader(requestHeader, response.Header);
          BrowsePathResult path;
          path.Status = StatusCode::BadNotReadable;
          response.Result.Paths.push_back(path);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to 'Translate Browse Paths To Node IDs' request." << std::endl;
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case PUBLISH_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Publish' request." << std::endl;
          std::vector<char> data(restSize);
          RawBuffer buffer(&data[0], restSize);
          stream >> buffer;
/*
          PublishResponse response;
          FillResponseHeader(requestHeader, response.Header);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to 'Publish' request." << std::endl;
          stream << secureHeader << algorithmHeader << sequence << response << flush;
*/
          return;
        }

        case SET_PUBLISHING_MODE_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Set Publishing Mode' request." << std::endl;
          PublishingModeParameters params;
          stream >> params;

          SetPublishingModeResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Result.Statuses.resize(params.SubscriptionIDs.size(), StatusCode::Good);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to 'Set Publishing Mode' request." << std::endl;
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }


        default:
        {
          std::stringstream ss;
          ss << "ERROR: Unknown message with id '" << message << "' was recieved.";
          throw std::logic_error(ss.str());
        }
      }
    }

  private:
    void FillResponseHeader(const RequestHeader& requestHeader, ResponseHeader& responseHeader)
    {
       responseHeader.InnerDiagnostics.push_back(DiagnosticInfo());
       responseHeader.Timestamp = CurrentDateTime();
       responseHeader.RequestHandle = requestHeader.RequestHandle;
    }

  private:
    std::mutex ProcessMutex;
    std::shared_ptr<OpcUa::Remote::Computer> Computer;
    bool Debug;
    uint32_t ChannelID;
    uint32_t TokenID;
    NodeID SessionID;
    NodeID AuthenticationToken;
  };

}

namespace OpcUa
{
  namespace Internal
  {

    std::unique_ptr<IncomingConnectionProcessor> CreateOpcTcpProcessor(std::shared_ptr<OpcUa::Remote::Computer> computer, bool debug)
    {
      return std::unique_ptr<IncomingConnectionProcessor>(new OpcTcp(computer, debug));
    }

  }
}

