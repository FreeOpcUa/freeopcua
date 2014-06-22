/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "opcua_protocol_addon_test.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/server/addons/builtin_server.h>
#include <opc/ua/server/builtin_server.h>

#include <opc/ua/server/addons/opcua_protocol.h>
#include "address_space_registry_test.h"
#include "endpoints_services_test.h"
#include "services_registry_test.h"
#include "standard_namespace_test.h"
#include "builtin_server_test.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

using namespace testing;

class OpcUaProtocolAddonTest : public Test
{
public:
  void SetUp()
  {
    Addons = Common::CreateAddonsManager();

    OpcUa::Test::RegisterServicesRegistry(*Addons);
    OpcUa::Test::RegisterAddressSpace(*Addons);
    OpcUa::Test::RegisterStandardNamespace(*Addons);
    OpcUa::Test::RegisterBuiltinServerAddon(*Addons);
    OpcUa::Test::RegisterOpcTcpAddon(*Addons);
    OpcUa::Test::RegisterEndpointsServicesAddon(*Addons);

    Addons->Start();
  }

  void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  std::unique_ptr<Common::AddonsManager> Addons;
};

TEST_F(OpcUaProtocolAddonTest, Loads)
{
  ASSERT_TRUE(static_cast<bool>(Addons->GetAddon(OpcUa::UaServer::OpcUaProtocolAddonID)));
}

TEST_F(OpcUaProtocolAddonTest, CanGetServerWhichOpensAndClosesSecureChannel)
{
  std::shared_ptr<OpcUa::UaServer::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::UaServer::BuiltinServer>(OpcUa::UaServer::TcpServerAddonID);
  ASSERT_TRUE(static_cast<bool>(computerAddon));
  std::shared_ptr<OpcUa::Remote::Server> computer = computerAddon->GetServer();
  ASSERT_TRUE(static_cast<bool>(computer));
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanListEndpoints)
{
  std::shared_ptr<OpcUa::UaServer::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::UaServer::BuiltinServer>(OpcUa::UaServer::TcpServerAddonID);
  std::shared_ptr<OpcUa::Remote::Server> computer = computerAddon->GetServer();
  std::shared_ptr<OpcUa::Remote::EndpointServices> endpoints = computer->Endpoints();
  std::vector<OpcUa::EndpointDescription> desc;
  ASSERT_NO_THROW(desc = endpoints->GetEndpoints(OpcUa::EndpointsFilter()));
  ASSERT_EQ(desc.size(), 1);
  endpoints.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanFindServers)
{
  std::shared_ptr<OpcUa::UaServer::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::UaServer::BuiltinServer>(OpcUa::UaServer::TcpServerAddonID);
  std::shared_ptr<OpcUa::Remote::Server> computer = computerAddon->GetServer();
  std::shared_ptr<OpcUa::Remote::EndpointServices> endpoints = computer->Endpoints();
  std::vector<OpcUa::ApplicationDescription> servers;
  ASSERT_NO_THROW(servers = endpoints->FindServers(OpcUa::FindServersParameters()));
  ASSERT_EQ(servers.size(), 1);
  endpoints.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanBrowseRootFolder)
{
  std::shared_ptr<OpcUa::UaServer::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::UaServer::BuiltinServer>(OpcUa::UaServer::TcpServerAddonID);
  std::shared_ptr<OpcUa::Remote::Server> computer = computerAddon->GetServer();
  std::shared_ptr<OpcUa::Remote::ViewServices> views = computer->Views();

  OpcUa::BrowseDescription description;
  description.NodeToBrowse = OpcUa::ObjectID::RootFolder;
  description.Direction = OpcUa::BrowseDirection::Forward;
  description.ReferenceTypeID = OpcUa::ReferenceID::Organizes;
  description.IncludeSubtypes = true;
  description.NodeClasses = OpcUa::NODE_CLASS_OBJECT;
  description.ResultMask = OpcUa::REFERENCE_ALL;
  OpcUa::NodesQuery query;
  query.NodesToBrowse.push_back(description);

  std::vector<OpcUa::ReferenceDescription> referencies = views->Browse(query);
  ASSERT_EQ(referencies.size(), 3);

  views.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanCreateSession)
{
  std::shared_ptr<OpcUa::UaServer::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::UaServer::BuiltinServer>(OpcUa::UaServer::TcpServerAddonID);
  std::shared_ptr<OpcUa::Remote::Server> computer = computerAddon->GetServer();

  OpcUa::Remote::SessionParameters session;
  session.ClientDescription.Name.Text = "opcua client";
  session.SessionName = "opua command line";
  session.EndpointURL = "opc.tcp://localhost:4841";
  session.Timeout = 1000;

  ASSERT_NO_THROW(computer->CreateSession(session));
  ASSERT_NO_THROW(computer->ActivateSession());
  ASSERT_NO_THROW(computer->CloseSession());

  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, ManipulateSubscriptions)
{
  std::shared_ptr<OpcUa::UaServer::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::UaServer::BuiltinServer>(OpcUa::UaServer::TcpServerAddonID);
  std::shared_ptr<OpcUa::Remote::Server> computer = computerAddon->GetServer();
  std::shared_ptr<OpcUa::Remote::SubscriptionServices> subscriptions = computer->Subscriptions();

  OpcUa::SubscriptionParameters params;
  params.MaxNotificationsPerPublish = 3;
  params.Priority = 0;
  params.PublishingEnabled = true;
  params.RequestedLifetimeCount = 3;
  params.RequestedMaxKeepAliveCount = 3;
  params.RequestedPublishingInterval = 1000;

  OpcUa::SubscriptionData data;
  ASSERT_NO_THROW(data = subscriptions->CreateSubscription(params));

  subscriptions.reset();
  computer.reset();
}
