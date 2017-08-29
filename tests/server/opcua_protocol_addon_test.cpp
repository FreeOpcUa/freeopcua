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
#include "builtin_server_addon.h"
#include "builtin_server.h"

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
    spdlog::drop_all();
    Logger = spdlog::stderr_color_mt("test");
    Logger->set_level(spdlog::level::info);
    Addons = Common::CreateAddonsManager(Logger);

    OpcUa::Test::RegisterServicesRegistry(*Addons);
    OpcUa::Test::RegisterAddressSpace(*Addons);
    OpcUa::Test::RegisterStandardNamespace(*Addons);
    OpcUa::Test::RegisterBuiltinServerAddon(*Addons);
    //OpcUa::Test::RegisterOpcTcpAddon(*Addons);
    OpcUa::Test::RegisterEndpointsServicesAddon(*Addons);
    Addons->Register(OpcUa::Server::CreateSubscriptionServiceAddon());
    Addons->Register(OpcUa::Server::CreateAsioAddon());
    Addons->Start();
  }

  void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  Common::Logger::SharedPtr Logger;
  std::unique_ptr<Common::AddonsManager> Addons;
};

TEST_F(OpcUaProtocolAddonTest, Loads)
{
  ASSERT_TRUE(static_cast<bool>(Addons->GetAddon(OpcUa::Server::OpcUaProtocolAddonId)));
}

TEST_F(OpcUaProtocolAddonTest, CanGetServerWhichOpensAndClosesSecureChannel)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  ASSERT_TRUE(static_cast<bool>(computerAddon));
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();
  ASSERT_TRUE(static_cast<bool>(computer));
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanListEndpoints)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();
  std::shared_ptr<OpcUa::EndpointServices> endpoints = computer->Endpoints();
  std::vector<OpcUa::EndpointDescription> desc;
  ASSERT_NO_THROW(desc = endpoints->GetEndpoints(OpcUa::GetEndpointsParameters()));
  ASSERT_EQ(desc.size(), 1);
  endpoints.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanFindServers)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();
  std::shared_ptr<OpcUa::EndpointServices> endpoints = computer->Endpoints();
  std::vector<OpcUa::ApplicationDescription> servers;
  ASSERT_NO_THROW(servers = endpoints->FindServers(OpcUa::FindServersParameters()));
  ASSERT_EQ(servers.size(), 1);
  endpoints.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanBrowseRootFolder)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();
  std::shared_ptr<OpcUa::ViewServices> views = computer->Views();

  OpcUa::BrowseDescription description;
  description.NodeToBrowse = OpcUa::ObjectId::RootFolder;
  description.Direction = OpcUa::BrowseDirection::Forward;
  description.ReferenceTypeId = OpcUa::ReferenceId::Organizes;
  description.IncludeSubtypes = true;
  description.NodeClasses = OpcUa::NodeClass::Object;
  description.ResultMask = OpcUa::BrowseResultMask::All;
  OpcUa::NodesQuery query;
  query.NodesToBrowse.push_back(description);

  std::vector<OpcUa::BrowseResult> results = views->Browse(query);
  ASSERT_EQ(results.size(), 1);
  ASSERT_EQ(results[0].Referencies.size(), 3);

  views.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanCreateSession)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();

  OpcUa::RemoteSessionParameters session;
  session.ClientDescription.ApplicationName.Text = "opcua client";
  session.SessionName = "opua command line";
  session.EndpointUrl = "opc.tcp://localhost:4841";
  session.Timeout = 1000;

  OpcUa::ActivateSessionParameters session_parameters;

  ASSERT_NO_THROW(computer->CreateSession(session));
  ASSERT_NO_THROW(computer->ActivateSession(session_parameters));
  ASSERT_NO_THROW(computer->CloseSession());

  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, ManipulateSubscriptions)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();
  std::shared_ptr<OpcUa::SubscriptionServices> subscriptions = computer->Subscriptions();

  OpcUa::CreateSubscriptionParameters params;
  params.MaxNotificationsPerPublish = 3;
  params.Priority = 0;
  params.PublishingEnabled = true;
  params.RequestedLifetimeCount = 3;
  params.RequestedMaxKeepAliveCount = 3;
  params.RequestedPublishingInterval = 1000;


  OpcUa::CreateSubscriptionRequest req;
  req.Parameters = params;
  OpcUa::SubscriptionData data;
  ASSERT_NO_THROW(data = subscriptions->CreateSubscription(req, [](OpcUa::PublishResult)
  {

  }));

  subscriptions.reset();
  computer.reset();
}

TEST_F(OpcUaProtocolAddonTest, CanReadAttributes)
{
  std::shared_ptr<OpcUa::Server::BuiltinServer> computerAddon = Addons->GetAddon<OpcUa::Server::BuiltinServer>(OpcUa::Server::OpcUaProtocolAddonId);
  std::shared_ptr<OpcUa::Services> computer = computerAddon->GetServices();
  std::shared_ptr<OpcUa::AttributeServices> attributes = computer->Attributes();

  OpcUa::ReadParameters params;
  params.AttributesToRead.push_back(OpcUa::ToReadValueId(OpcUa::ObjectId::RootFolder, OpcUa::AttributeId::BrowseName));

  std::vector<OpcUa::DataValue> values = attributes->Read(params);
  ASSERT_EQ(values.size(), 1);

  attributes.reset();
  computer.reset();
}
