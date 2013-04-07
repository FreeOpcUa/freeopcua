/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/tcp_server.h>

#include <chrono>
#include <thread>


using namespace testing;

const unsigned short TestPort = 33445;


namespace 
{
    class IncomingConnectionProcessorGarbage : public OpcUa::Server::IncomingConnectionProcessor
    {
    public:
      virtual void Process(std::unique_ptr<OpcUa::IOChannel>)
      {
        DoProcess();
      }

      virtual void DoProcess() = 0;
   };


    class IncomingConnectionProcessorMock : public IncomingConnectionProcessorGarbage
    {
    public:
      //StrictMock<IncomingConnectionProcessorMock> Strict;
      //NiceMock<IncomingConnectionProcessorMock> Nice;

    public:
      MOCK_METHOD0(DoProcess, void());
    };
}


TEST(TcpServer, AcceptConnections)
{
  std::unique_ptr<IncomingConnectionProcessorMock> clientsProcessor(new IncomingConnectionProcessorMock);
  EXPECT_CALL(*clientsProcessor, DoProcess()).Times(1);

  std::unique_ptr<OpcUa::Server::ConnectionListener> server = OpcUa::CreateTcpServer(TestPort);
  server->Start(std::unique_ptr<OpcUa::Server::IncomingConnectionProcessor>(clientsProcessor.release()));
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::unique_ptr<OpcUa::RemoteConnection> connect;
  ASSERT_NO_THROW(connect = OpcUa::Connect("localhost", TestPort));
  ASSERT_TRUE(connect.get());
  server->Stop();
}

namespace
{
  class EchoProcessor : public OpcUa::Server::IncomingConnectionProcessor
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

TEST(TcpServer, CanSendAndReceiveData)
{
  const unsigned int port = TestPort + 1;
  std::unique_ptr<OpcUa::Server::IncomingConnectionProcessor> clientsProcessor(new EchoProcessor());
  std::unique_ptr<OpcUa::Server::ConnectionListener> server = OpcUa::CreateTcpServer(port);
  server->Start(std::unique_ptr<OpcUa::Server::IncomingConnectionProcessor>(clientsProcessor.release()));
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::unique_ptr<OpcUa::RemoteConnection> connection = OpcUa::Connect("localhost", port);

  char data[4] = {0, 1, 2, 3};
  connection->Send(data, 4);

  char dataReceived[4] = {0};
  connection->Receive(dataReceived, 4);
  connection.reset();
  server->Stop();
  server.reset();
  ASSERT_EQ(memcmp(data, dataReceived, 4), 0);
}

