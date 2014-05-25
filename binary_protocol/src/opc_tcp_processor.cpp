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
#include <queue>
#include <list>
#include <chrono>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using namespace OpcUa::UaServer;

  typedef OpcUa::Binary::IOStream<OpcUa::IOChannel> IOStreamBinary;

  struct SubscriptionBinaryData
  {
    IntegerID SubscriptionID;
    std::chrono::duration<double> period;
    std::chrono::duration<double> last_check;
  };

  struct PublishRequestElement
  {
    SequenceHeader sequence;
    RequestHeader requestHeader;
    SymmetricAlgorithmHeader algorithmHeader;
  };


  class OpcTcp : public IncomingConnectionProcessor
  {
  public:
    OpcTcp(std::shared_ptr<OpcUa::Remote::Server> computer, bool debug)
      : Server(computer)
      , Debug(debug)
      , ChannelID(1)
      , TokenID(2)
    {
      SessionID = NumericNodeID(5, 0);
    }

    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      if (!clientChannel)
      {
        if (Debug) std::cerr << "Empty channel passed to endpoints opc binary protocol processor." << std::endl;
        return;
      }

      if (Debug) std::clog << "Hello client!" << std::endl;
      IOStreamBinary stream(clientChannel);

      while(ProcessChunk(stream));

/*
      for(;;)
      {
        double period = GetNextSleepPeriod();
        std::cout << "Sleeping for " << period << " seconds" << std::endl;
        int res = clientChannel->WaitForData(period); //double to float cast
        if (res < 0)
        {
          return;
        }
        else if (res == 1)
        {
          ProcessChunk(stream);
        }
        else
        {
          SendPublishResponse(stream);
        }
      }
*/
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }

  private:
    bool ProcessChunk(IOStreamBinary& stream)
    {
      using namespace OpcUa::Binary;

      if (Debug) std::cout << "Processing new chunk." << std::endl;
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
          if (Debug) std::clog << "Received acknowledge from client. This should not have happend..." << std::endl;
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

      //std::cout << "Release Lock ..." << std::endl;
      return true;
    }

    void HelloClient(IOStreamBinary& stream)
    {
      using namespace OpcUa::Binary;

      if (Debug) std::clog << "Reading hello message." << std::endl;
      Hello hello;
      stream >> hello;

      Acknowledge ack;
      ack.ReceiveBufferSize = hello.ReceiveBufferSize;
      ack.SendBufferSize = hello.SendBufferSize;
      ack.MaxMessageSize = hello.MaxMessageSize;
      ack.MaxChunkCount = 1;

      Header ackHeader(MT_ACKNOWLEDGE, CHT_SINGLE);
      ackHeader.AddSize(RawSize(ack));
      if (Debug) std::clog << "Sending answer to client." << std::endl;
      stream << ackHeader << ack << flush;
    }

    void OpenChannel(IOStreamBinary& stream)
    {
      uint32_t channelID = 0;
      stream >> channelID;
      AsymmetricAlgorithmHeader algorithmHeader;
      stream >> algorithmHeader;

      if (algorithmHeader.SecurityPolicyURI != "http://opcfoundation.org/UA/SecurityPolicy#None")
      {
        throw std::logic_error(std::string("Client want to create secure channel with unsupported policy '") + algorithmHeader.SecurityPolicyURI + std::string("'"));
      }

      SequenceHeader sequence;
      stream >> sequence;

      OpenSecureChannelRequest request;
      stream >> request;
      
      if (request.SecurityMode != MSM_NONE)
      {
        throw std::logic_error("Unsupported security mode.");
      }

      if (request.RequestType == STR_RENEW)
      {
        //FIXME:Should check that channel has been issued first
        ++TokenID;
      }

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
      stream << responseHeader << algorithmHeader << sequence << response << flush;
    }

    void CloseChannel(IOStreamBinary& stream)
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

    void ProcessMessage(IOStreamBinary& stream, std::size_t messageSize)
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
          response.Endpoints = Server->Endpoints()->GetEndpoints(filter);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::FIND_SERVERS_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Find Servers' request." << std::endl;
          FindServersParameters params;
          stream >> params;

          FindServersResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Data.Descriptions = Server->Endpoints()->FindServers(params);

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

          OpcUa::BrowseResult result;
          result.Referencies = Server->Views()->Browse(query);
          response.Results.push_back(result);

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case OpcUa::READ_REQUEST:
        {
          ReadParameters params;
          stream >> params;

          if (Debug)
          {
            std::clog << "Processing read request for Node:";
            for (AttributeValueID id : params.AttributesToRead) 
            {
              std::clog << " " << id.Node ;  
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
          stream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }

        case OpcUa::WRITE_REQUEST:
        {
          if (Debug) std::clog << "Processing write request." << std::endl;
          WriteParameters params;
          stream >> params;

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
          stream << secureHeader << algorithmHeader << sequence << response << flush;

          return;
        }

        case TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Translate Browse Paths To Node IDs' request." << std::endl;
          std::vector<char> data(restSize);
          TranslateBrowsePathsParameters params;
          stream >> params;

          if (Debug) 
          {
            for ( BrowsePath path : params.BrowsePaths)
            {
              std::cout << "Requested path is: " << path.StartingNode << " : " ;
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
              std::cout << "Result of browsePath is: " << (uint) res.Status << ". Target is: ";
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

          if (Debug) std::clog << "Sending response to 'Translate Browse Paths To Node IDs' request." << std::endl;
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
          EndpointsFilter epf;
          response.Session.ServerEndpoints = Server->Endpoints()->GetEndpoints(epf);


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

          if (deleteSubscriptions)
          {
            std::vector<IntegerID> subs;
            for (SubscriptionBinaryData data: Subscriptions)
            {
              subs.push_back(data.SubscriptionID);
            }
            Server->Subscriptions()->DeleteSubscriptions(subs);
          }

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

          response.Data = Server->Subscriptions()->CreateSubscription(params);
          SubscriptionBinaryData SubData;
          SubData.SubscriptionID = response.Data.ID;
          SubData.period =  std::chrono::duration<double>(response.Data.RevisedPublishingInterval/1000); //seconds
          Subscriptions.push_back(SubData);

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
          MonitoredItemsParameters params;
          stream >> params;

          CreateMonitoredItemsResponse response;

          response.Data = Server->Subscriptions()->CreateMonitoredItems(params);

          FillResponseHeader(requestHeader, response.Header);
          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to Create Monitored Items Request." << std::endl;
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case PUBLISH_REQUEST:
        {
          if (Debug) std::clog << "Processing and queuing 'Publish' request." << std::endl;
          PublishParameters params;
          stream >> params;
          PublishRequestElement data;
          data.sequence = sequence;
          data.algorithmHeader = algorithmHeader;
          data.requestHeader = requestHeader;
          PublishRequestQueue.push(data);
          Server->Subscriptions()->CreatePublishRequest(params.Acknowledgements);
          return;
        }

        case SET_PUBLISHING_MODE_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Set Publishing Mode' request." << std::endl;
          PublishingModeParameters params;
          stream >> params;
          
          //FIXME: forward request to internal server!!
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

        case ADD_NODES_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Add Nodes' request." << std::endl;
          AddNodesParameters params;
          stream >> params;
          
          std::vector<AddNodesResult> results = Server->NodeManagement()->AddNodes(params.NodesToAdd);

          AddNodesResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.results = results;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to 'Add Nodes' request." << std::endl;
          stream << secureHeader << algorithmHeader << sequence << response << flush;
          return;
        }

        case ADD_REFERENCES_REQUEST:
        {
          if (Debug) std::clog << "Processing 'Add References' request." << std::endl;
          AddReferencesParameters params;
          stream >> params;
          
          std::vector<StatusCode> results = Server->NodeManagement()->AddReferences(params.ReferencesToAdd);

          AddReferencesResponse response;
          FillResponseHeader(requestHeader, response.Header);
          response.Results = results;

          SecureHeader secureHeader(MT_SECURE_MESSAGE, CHT_SINGLE, ChannelID);
          secureHeader.AddSize(RawSize(algorithmHeader));
          secureHeader.AddSize(RawSize(sequence));
          secureHeader.AddSize(RawSize(response));

          if (Debug) std::clog << "Sending response to 'Add References' request." << std::endl;
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
       //responseHeader.InnerDiagnostics.push_back(DiagnosticInfo());
       responseHeader.Timestamp = CurrentDateTime();
       responseHeader.RequestHandle = requestHeader.RequestHandle;
    }
/*
    double GetNextSleepPeriod()
    {
      if ( Subscriptions.size() == 0 || PublishRequestQueue.size() == 0)
      {
        return  10;
      }
      std::chrono::duration<double> now =  std::chrono::system_clock::now().time_since_epoch(); 
      std::chrono::duration<double>  next_fire = std::chrono::duration<double>(std::numeric_limits<double>::max() ) ;
      
      for (const SubscriptionBinaryData& data: Subscriptions)
      {
        std::chrono::duration<double> tmp =  data.last_check + data.period;
        //std::cout << "Time since last check : " << (now - data.last_check).count() << " Period: " << data.period.count() << " time to next fire: " << (tmp - now).count() << std::endl;
        if (tmp < next_fire)
        {
          next_fire = tmp;
        }
      }
      auto diff = next_fire - now;
      if ( diff.count() < 0 ) 
      {
        //std::cout << "Event should allrady have been fired returning 0"<< std::endl;
        return 0;
      }
      return diff.count() ;
    }
*/
    void SendPublishResponse(IOStreamBinary& stream)
    {
      for (SubscriptionBinaryData& subdata: Subscriptions)
      {
        if ( PublishRequestQueue.size() == 0)
        {
          std::cerr << "RequestQueueSize is empty we cannot process more subscriptions, this is a client error" << std::endl;
          return;
        }
       
        std::chrono::duration<double> now =  std::chrono::system_clock::now().time_since_epoch(); //make sure it is in milliseconds
        if ((now - subdata.last_check) <= subdata.period)
        {
          std::cout << " No need to process subscription yet" << std::endl;
          continue;
        } 
        subdata.last_check = now;

        std::vector<IntegerID> sub_query;
        sub_query.push_back(subdata.SubscriptionID);
        std::vector<PublishResult> res_list = Server->Subscriptions()->PopPublishResults(sub_query);
        std::cout << "got " << res_list.size() << " notifications from server " << subdata.SubscriptionID << std::endl;

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
            std::cout << "Sedning publishResponse with " << response.Result.Message.Data.size() << " PublishResults" << std::endl;
            for  ( NotificationData d: response.Result.Message.Data )
            {
              std::cout << "     " << d.DataChange.Notification.size() <<  " modified items" << std::endl;
            }
          }
          stream << secureHeader << requestData.algorithmHeader << requestData.sequence << response << flush;
        }
      }
    }

  private:
    std::mutex ProcessMutex;
    std::shared_ptr<OpcUa::Remote::Server> Server;
    bool Debug;
    uint32_t ChannelID;
    uint32_t TokenID;
    NodeID SessionID;
    NodeID AuthenticationToken;
    std::list<SubscriptionBinaryData> Subscriptions; //Keep a list of subscriptions to query internal server at correct rate
    std::queue<PublishRequestElement> PublishRequestQueue; //Keep track of request data to answer them when we have data and 
  };

}

namespace OpcUa
{
  namespace Internal
  {

    std::unique_ptr<IncomingConnectionProcessor> CreateOpcTcpProcessor(std::shared_ptr<OpcUa::Remote::Server> computer, bool debug)
    {
      return std::unique_ptr<IncomingConnectionProcessor>(new OpcTcp(computer, debug));
    }

  }
}

