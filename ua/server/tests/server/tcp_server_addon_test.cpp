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
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/addons/builtin_computer.h>
#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opc/ua/server/addons/endpoints.h>
#include <opc/ua/server/server.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/addons_core/dynamic_addon_factory.h>

#include <chrono>
#include <thread>

#include "common.h"

using namespace testing;

const unsigned short TestPort = 33449;


namespace 
{

  class EchoProcessor : public OpcUa::Server::IncomingConnectionProcessor
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

  class EchoAddon : public OpcUa::Server::EndpointsAddon
  {
  public:
    EchoAddon()
    {
    }

    std::vector<OpcUa::Server::Endpoint> GetEndpoints() const
    {
      return std::vector<OpcUa::Server::Endpoint>();
    }

    std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> GetProcessor() const
    {
      return std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor>(new EchoProcessor());
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters&)
    {
    }

    virtual void Stop()
    {
    }

  private:
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


  Common::AddonConfiguration CreateEchoAddonConfiguration()
  {
    Common::AddonConfiguration config;
    config.ID = OpcUa::Server::EndpointsAddonID;
    config.Factory =  CreateEchoAddonFactory();
    return config;
  }

  Common::AddonConfiguration CreateBuiltinComputerAddonConfiguration()
  {
    Common::AddonConfiguration config;
    const std::string modulePath = "./.libs/libbuiltin_computer_addon.so";
    config.ID = OpcUa::Server::BuiltinComputerAddonID;
    config.Factory =  Common::CreateDynamicAddonFactory(modulePath.c_str());
    config.Dependencies = std::vector<Common::AddonID>(1, OpcUa::Server::EndpointsAddonID);
    return config;
  }


  Common::AddonConfiguration CreateTcpAddonConfig()
  {
    Common::AddonConfiguration tcpConfig;
    tcpConfig.ID = OpcUa::Server::TcpServerAddonID;
    tcpConfig.Factory = Common::CreateDynamicAddonFactory(OpcUa::Tests::GetTcpServerAddonPath().c_str());
    tcpConfig.Dependencies = std::vector<Common::AddonID>(1, OpcUa::Server::EndpointsAddonID);
    return tcpConfig;
  }

  Common::AddonConfiguration CreateEndpointsAddonConfig()
  {
    Common::AddonConfiguration endpointsConfig;
    endpointsConfig.ID = OpcUa::Server::EndpointsAddonID;
    endpointsConfig.Factory = CreateEchoAddonFactory();
    return endpointsConfig;
  }
}

TEST(TcpServerAddon, CanBeLoadedLoaded)
{
  std::shared_ptr<Common::AddonsManager> addons = Common::CreateAddonsManager();
  addons->Register(CreateTcpAddonConfig());
  addons->Register(CreateEndpointsAddonConfig());
  addons->Start();
  ASSERT_TRUE(static_cast<bool>(addons->GetAddon(OpcUa::Server::TcpServerAddonID)));
  addons->Stop();
}

TEST(TcpServerAddon, CanSendAndReceiveData)
{
  std::shared_ptr<Common::AddonsManager> addons = Common::CreateAddonsManager();
  addons->Register(CreateTcpAddonConfig());
  addons->Register(CreateEndpointsAddonConfig());
  addons->Start();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  std::unique_ptr<OpcUa::RemoteConnection> connection = OpcUa::Connect("localhost", 4841);

  char data[4] = {0, 1, 2, 3};
  connection->Send(data, 4);
  char dataReceived[4] = {0};
  connection->Receive(dataReceived, 4);

  ASSERT_EQ(memcmp(data, dataReceived, 4), 0);

  connection.reset();

  addons->Stop();
  addons.reset();
}

TEST(BuiltinComputerAddon, CanBeLoadedLoaded)
{
  std::shared_ptr<Common::AddonsManager> addons = Common::CreateAddonsManager();
  addons->Register(CreateEchoAddonConfiguration());
  addons->Register(CreateBuiltinComputerAddonConfiguration());
  addons->Start();

  ASSERT_TRUE(static_cast<bool>(addons->GetAddon(OpcUa::Server::BuiltinComputerAddonID)));

  addons->Stop();
}

