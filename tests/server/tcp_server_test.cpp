/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/tcp_server.h>

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <thread>


using namespace testing;

const unsigned short TestPort = 33445;


namespace 
{

  class IncomingConnectionProcessorMock : public OpcUa::UaServer::IncomingConnectionProcessor
  {
  public:
    IncomingConnectionProcessorMock()
      : ProcessCallCount(0)
    {
    }

    virtual ~IncomingConnectionProcessorMock()
    {
    }

    virtual void Process(std::shared_ptr<OpcUa::IOChannel>) override
    {
      ++ProcessCallCount;
    }

    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel) override
    {
    }

  public:
    unsigned ProcessCallCount;
  };

}

TEST(TcpServer, AcceptConnections)
{
  std::shared_ptr<IncomingConnectionProcessorMock> clientsProcessor(new IncomingConnectionProcessorMock);
  OpcUa::UaServer::TcpParameters params;
  params.Port = TestPort;

  std::unique_ptr<OpcUa::UaServer::TcpServer> server = OpcUa::UaServer::CreateTcpServer();
  server->Listen(params, clientsProcessor);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  {
    std::unique_ptr<OpcUa::RemoteConnection> connect;
    ASSERT_NO_THROW(connect = OpcUa::Connect("localhost", TestPort));
    ASSERT_TRUE(connect.get());
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  server->StopListen(params);

  EXPECT_EQ(clientsProcessor->ProcessCallCount, 1);
}

namespace
{
  class EchoProcessor : public OpcUa::UaServer::IncomingConnectionProcessor
  {
  public:
    virtual void Process(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
      char data[4] = {0};
      clientChannel->Receive(data, 4);
      clientChannel->Send(data, 4);
    }
    virtual void StopProcessing(std::shared_ptr<OpcUa::IOChannel> clientChannel)
    {
    }
  };
}

TEST(TcpServer, CanSendAndReceiveData)
{
  OpcUa::UaServer::TcpParameters params;
  params.Port = TestPort + 1;
  std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> clientProcessor(new EchoProcessor());
  std::unique_ptr<OpcUa::UaServer::TcpServer> server = OpcUa::UaServer::CreateTcpServer();
  server->Listen(params, clientProcessor);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::unique_ptr<OpcUa::RemoteConnection> connection = OpcUa::Connect("localhost", params.Port);

  char data[4] = {0, 1, 2, 3};
  connection->Send(data, 4);

  char dataReceived[4] = {0};
  connection->Receive(dataReceived, 4);
  connection.reset();
  server->StopListen(params);
  server.reset();
  ASSERT_EQ(memcmp(data, dataReceived, 4), 0);
}

