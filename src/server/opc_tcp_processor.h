/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa binary protocol connection processor.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/logger.h>
#include <opc/ua/protocol/binary/common.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/services/services.h>

#include <chrono>
#include <list>
#include <mutex>
#include <queue>

namespace OpcUa
{
namespace Server
{

class OpcTcpMessages: public std::enable_shared_from_this<OpcTcpMessages>
{
public:
  DEFINE_CLASS_POINTERS(OpcTcpMessages)

public:
  OpcTcpMessages(OpcUa::Services::SharedPtr server, OpcUa::OutputChannel::SharedPtr outputChannel, const Common::Logger::SharedPtr & logger);
  ~OpcTcpMessages();

  bool ProcessMessage(Binary::MessageType msgType, Binary::IStreamBinary & iStream);

private:
  void HelloClient(Binary::IStreamBinary & istream, Binary::OStreamBinary & ostream);
  void OpenChannel(Binary::IStreamBinary & istream, Binary::OStreamBinary & ostream);
  void CloseChannel(Binary::IStreamBinary & istream);
  void ProcessRequest(Binary::IStreamBinary & istream, Binary::OStreamBinary & ostream);
  void FillResponseHeader(const RequestHeader & requestHeader, ResponseHeader & responseHeader);
  void DeleteSubscriptions(const std::vector<uint32_t> & ids);
  void DeleteAllSubscriptions();
  void ForwardPublishResponse(const PublishResult response);

private:
  std::mutex ProcessMutex;
  OpcUa::Services::SharedPtr Server;
  OpcUa::OutputChannel::WeakPtr OutputChannel;
  OpcUa::Binary::OStreamBinary OutputStream;
  Common::Logger::SharedPtr Logger;
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

  std::list<uint32_t> Subscriptions; //Keep a list of subscriptions to query internal server at correct rate
  std::mutex PublishRequestQueueMutex;
  std::queue<PublishRequestElement> PublishRequestQueue; //Keep track of request data to answer them when we have data and
};


} // namespace UaServer
} // namespace OpcUa
