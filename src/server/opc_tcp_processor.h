/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa binary protocol connection processor.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/services/services.h>

#include <boost/thread/shared_mutex.hpp>
#include <chrono>
#include <list>
#include <mutex>
#include <queue>

namespace OpcUa
{
  namespace Server
  {

    class OpcTcpMessages
    {
    public:
      OpcTcpMessages(std::shared_ptr<OpcUa::Services> computer, OpcUa::OutputChannel& outputChannel, bool debug);
      ~OpcTcpMessages();

      bool ProcessMessage(Binary::MessageType msgType, Binary::IStreamBinary& iStream);

    private:
      void HelloClient(Binary::IStreamBinary& istream, Binary::OStreamBinary& ostream);
      void OpenChannel(Binary::IStreamBinary& istream, Binary::OStreamBinary& ostream);
      void CloseChannel(Binary::IStreamBinary& istream);
      void ProcessRequest(Binary::IStreamBinary& istream, Binary::OStreamBinary& ostream);
      void FillResponseHeader(const RequestHeader& requestHeader, ResponseHeader& responseHeader);
      void DeleteSubscriptions(const std::vector<IntegerId>& ids);
      void DeleteAllSubscriptions();
      void ForwardPublishResponse(const PublishResult response);

    private:
      boost::shared_mutex ProcessMutex;
      std::shared_ptr<OpcUa::Services> Server;
      OpcUa::Binary::OStreamBinary OutputStream;
      bool Debug;
      uint32_t ChannelId;
      uint32_t TokenId;
      ExpandedNodeId SessionId;
      //ExpandedNodeId AuthenticationToken;
      uint32_t SequenceNb;

      struct PublishRequestElement
      {
        Binary::SequenceHeader sequence;
        RequestHeader requestHeader;
        Binary::SymmetricAlgorithmHeader algorithmHeader;
      };

      std::list<IntegerId> Subscriptions; //Keep a list of subscriptions to query internal server at correct rate
      std::mutex PublishRequestQueueMutex;
      std::queue<PublishRequestElement> PublishRequestQueue; //Keep track of request data to answer them when we have data and
    };


  } // namespace UaServer
} // namespace OpcUa
