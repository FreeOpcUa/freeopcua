/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <opc/ua/binary_connection.h>
#include <opc/ua/binary_messages.h>
#include <opc/ua/binary_server.h>
#include <opc/ua/binary_stream.h>

#include <chrono>
#include <thread>


using namespace testing;

TEST(OpcBinaryServer, AcceptConnections)
{
  std::unique_ptr<IncomingConnectionProcessor::Mock> clientsProcessor(new IncomingConnectionProcessor::Mock);
  EXPECT_CALL(*clientsProcessor, DoProcess(_)).Times(1);

  const unsigned short port = 33445;
  std::unique_ptr<OpcUa::Binary::Server> server = OpcUa::Binary::CreateServer(port, std::unique_ptr<OpcUa::Binary::IncomingConnectionProcessor>(clientsProcessor.release()));
  server->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::unique_ptr<OpcUa::Binary::Connection> connect = OpcUa::Binary::Connect("localhost", port);
}

namespace
{
  class EchoProcessor : public OpcUa::Binary::IncomingConnectionProcessor
  {
  public:
    virtual void Process(std::unique_ptr<OpcUa::IOChannel> clientChannel)
    {
      char data[4] = {0};
      clientChannel->Receive(data, 4);
      clientChannel->Send(data, 4);
    }
  };
}

TEST(OpcBinaryServer, CanSendAndReceiveData)
{
  std::unique_ptr<OpcUa::Binary::IncomingConnectionProcessor> clientsProcessor(new EchoProcessor());
  const unsigned short port = 4840;
  std::unique_ptr<OpcUa::Binary::Server> server = OpcUa::Binary::CreateServer(port, std::unique_ptr<OpcUa::Binary::IncomingConnectionProcessor>(clientsProcessor.release()));
  server->Start();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::unique_ptr<OpcUa::Binary::Connection> connection = OpcUa::Binary::Connect("localhost", port);

  char data[4] = {0, 1, 2, 3};
  connection->Send(data, 4);

  char dataReceived[4] = {0};
  connection->Receive(dataReceived, 4);
  connection.reset();
  server->Stop();
  server.reset();
  ASSERT_EQ(memcmp(data, dataReceived, 3), 0);
}

