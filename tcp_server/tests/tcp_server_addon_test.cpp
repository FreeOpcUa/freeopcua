/// @author Alexander Rykovanov 2013
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
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/addons/tcp_server.h>
#include <opc/ua/server/tcp_server.h>

#include <thread>

using namespace testing;

unsigned short TestPort = 33449;


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

  class EchoAddon : public Common::Addon
  {
  public:
    EchoAddon()
    {
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters&)
    {
      std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor(new EchoProcessor());
      TcpAddon = addons.GetAddon<OpcUa::UaServer::TcpServer>(OpcUa::UaServer::TcpServerAddonID);
      OpcUa::UaServer::TcpParameters tcpParams;
      tcpParams.Port = ++TestPort;

      TcpAddon->Listen(tcpParams, processor);
    }

    virtual void Stop()
    {
      OpcUa::UaServer::TcpParameters params;
      params.Port = TestPort;
      TcpAddon->StopListen(params);
    }

  private:
    std::shared_ptr<OpcUa::UaServer::TcpServer> TcpAddon;
  };

  class EchoAddonFactory : public Common::AddonFactory
  {
  public:
    /// @brief Create instance of addon
    virtual Common::Addon::UniquePtr CreateAddon()
    {
      return Common::Addon::UniquePtr(new EchoAddon());
    }
  };

  Common::AddonFactory::UniquePtr CreateEchoAddonFactory()
  {
    return Common::AddonFactory::UniquePtr(new EchoAddonFactory());
  }


  Common::AddonInformation CreateEchoAddonConfiguration()
  {
    Common::AddonInformation config;
    config.ID = "echo_addon";
    config.Factory =  CreateEchoAddonFactory();
    config.Dependencies = std::vector<Common::AddonID>(1, OpcUa::UaServer::TcpServerAddonID);
    return config;
  }

  Common::AddonInformation CreateTcpAddonConfig()
  {
    Common::AddonInformation tcpConfig;
    tcpConfig.ID = OpcUa::UaServer::TcpServerAddonID;
    tcpConfig.Factory.reset(new OpcUa::UaServer::TcpServerAddonFactory());
    return tcpConfig;
  }

}

TEST(TcpServerAddon, CanBeCreated)
{
  std::shared_ptr<Common::AddonsManager> addons = Common::CreateAddonsManager();
  addons->Register(CreateTcpAddonConfig());
  addons->Register(CreateEchoAddonConfiguration());
  addons->Start();
  ASSERT_TRUE(static_cast<bool>(addons->GetAddon(OpcUa::UaServer::TcpServerAddonID)));
  addons->Stop();
}

TEST(TcpServerAddon, CanSendAndReceiveData)
{
  std::shared_ptr<Common::AddonsManager> addons = Common::CreateAddonsManager();
  addons->Register(CreateTcpAddonConfig());
  addons->Register(CreateEchoAddonConfiguration());
  addons->Start();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::unique_ptr<OpcUa::RemoteConnection> connection = OpcUa::Connect("localhost", TestPort);

  char data[4] = {0, 1, 2, 3};
  connection->Send(data, 4);
  char dataReceived[4] = {0};
  connection->Receive(dataReceived, 4);

  ASSERT_EQ(memcmp(data, dataReceived, 4), 0);

  connection.reset();

  addons->Stop();
  addons.reset();
}
