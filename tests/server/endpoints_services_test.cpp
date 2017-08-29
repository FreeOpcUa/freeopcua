/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints_services_test.h"
#include "services_registry_test.h"

#include <src/server/endpoints_parameters.h>
#include <opc/common/logger.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/server/endpoints_services.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>


using namespace testing;

class EndpointsRegistry : public Test
{
protected:
  virtual void SetUp()
  {
    spdlog::drop_all();
    Logger = spdlog::stderr_color_mt("test");
    Logger->set_level(spdlog::level::info);
    Addons = Common::CreateAddonsManager(Logger);
    OpcUa::Test::RegisterServicesRegistry(*Addons);
    OpcUa::Test::RegisterEndpointsServicesAddon(*Addons);
    Addons->Start();
  }

  virtual void TearDown()
  {
    Addons->Stop();
    Addons.reset();
  }

protected:
  Common::Logger::SharedPtr Logger;
  std::unique_ptr<Common::AddonsManager> Addons;
};

TEST_F(EndpointsRegistry, CanBeCreated)
{
  OpcUa::Server::EndpointsRegistry::SharedPtr endpoints;

  ASSERT_NO_THROW(endpoints = Addons->GetAddon<OpcUa::Server::EndpointsRegistry>(OpcUa::Server::EndpointsRegistryAddonId));
  ASSERT_TRUE(static_cast<bool>(endpoints));
}

TEST_F(EndpointsRegistry, RegisterEndpoints)
{
  OpcUa::EndpointDescription desc;
  desc.EndpointUrl = "url";

  OpcUa::Server::EndpointsRegistry::SharedPtr endpoints = Addons->GetAddon<OpcUa::Server::EndpointsRegistry>(OpcUa::Server::EndpointsRegistryAddonId);
  endpoints->AddEndpoints(std::vector<OpcUa::EndpointDescription>(1, desc));

  OpcUa::Server::ServicesRegistry::SharedPtr services = Addons->GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);
  OpcUa::Services::SharedPtr computer = services->GetServer();
  std::vector<OpcUa::EndpointDescription> descriptions;
  ASSERT_NO_THROW(descriptions = computer->Endpoints()->GetEndpoints(OpcUa::GetEndpointsParameters()));
  ASSERT_EQ(descriptions.size(), 1);
  ASSERT_EQ(descriptions[0].EndpointUrl, "url");
}

std::vector<Common::ParametersGroup> CreateTwoEndpointsParameters()
{
  Common::ParametersGroup data;
  data.Name = "application";
  data.Parameters.push_back(Common::Parameter("discovery_profile", "DiscoveryProfileUri"));
  data.Parameters.push_back(Common::Parameter("uri", "ApplicationUri"));
  data.Parameters.push_back(Common::Parameter("product_uri", "ProductUri"));
  data.Parameters.push_back(Common::Parameter("gateway_server_uri", "GatewayServerUri"));
  data.Parameters.push_back(Common::Parameter("name", "ApplicationName"));
  data.Parameters.push_back(Common::Parameter("type", "ApplicationType"));

  Common::ParametersGroup endpoint("endpoint");
  endpoint.Parameters.push_back(Common::Parameter("url", "EndpointUrl"));
  endpoint.Parameters.push_back(Common::Parameter("security_level", "1"));
  endpoint.Parameters.push_back(Common::Parameter("security_mode", "3"));
  endpoint.Parameters.push_back(Common::Parameter("security_policy_uri", "SecurityPolicyUri"));
  endpoint.Parameters.push_back(Common::Parameter("transport_profile_uri", "TransportProfileUri"));

  Common::ParametersGroup tokenPolicy("user_token_policy");
  tokenPolicy.Parameters.push_back(Common::Parameter("issued_token_type", "IssuedTokenType"));
  tokenPolicy.Parameters.push_back(Common::Parameter("issuer_endpoint_url", "IssuerEndpointUrl"));
  tokenPolicy.Parameters.push_back(Common::Parameter("id", "PolicyId"));
  tokenPolicy.Parameters.push_back(Common::Parameter("uri", "SecurityPolicyUri"));
  tokenPolicy.Parameters.push_back(Common::Parameter("type", "user_name"));

  endpoint.Groups.push_back(tokenPolicy);
  data.Groups.push_back(endpoint);

  return std::vector<Common::ParametersGroup> {data, data};
}

std::vector<OpcUa::Server::ApplicationData> CreateTwoEndpointsConfiguration()
{
  OpcUa::Server::ApplicationData data;
  data.Application.DiscoveryProfileUri = "DiscoveryProfileUri";
  data.Application.ApplicationUri = "ApplicationUri";
  data.Application.ProductUri = "ProductUri";
  data.Application.GatewayServerUri = "GatewayServerUri";
  data.Application.ApplicationName = OpcUa::LocalizedText("Name");
  data.Application.ApplicationType = OpcUa::ApplicationType::Client;

  OpcUa::EndpointDescription ed;
  ed.EndpointUrl = "EndpointUrl";
  ed.SecurityLevel = 1;
  ed.SecurityMode = OpcUa::MessageSecurityMode::SignAndEncrypt;
  ed.SecurityPolicyUri = "SecurityPolicyUri";
  ed.TransportProfileUri = "TransportProfileUri";

  OpcUa::UserTokenPolicy tokenPolicy; // By default ot os an anonimous token;
  tokenPolicy.IssuedTokenType = "IssuedTokenType";
  tokenPolicy.IssuerEndpointUrl = "IssuerEndpointUrl";
  tokenPolicy.PolicyId = "PolicyId";
  tokenPolicy.SecurityPolicyUri = "SecurityPolicyUri";
  tokenPolicy.TokenType = OpcUa::UserTokenType::UserName;
  ed.UserIdentityTokens.push_back(tokenPolicy);

  data.Endpoints = {ed};

  return std::vector<OpcUa::Server::ApplicationData> {data, data};
}

Common::Parameter FindParameter(const Common::ParametersGroup & group, const char * name)
{
  auto paramIt = std::find_if(group.Parameters.begin(), group.Parameters.end(), [name](const Common::Parameter & param)
  {
    return param.Name == name;
  });

  if (paramIt != group.Parameters.end())
    {
      return *paramIt;
    }

  return Common::Parameter();
}

// TEST(EndpointParameters, ConvertingFromAddonParameters)
// {
//   std::vector<Common::ParametersGroup> params = CreateTwoEndpointsParameters();
//   bool debug = true;
//   std::vector<OpcUa::Server::ApplicationData> apps = OpcUa::ParseEndpointsParameters(params, debug);
//   ASSERT_EQ(apps.size(), 2);
//   OpcUa::Server::ApplicationData app = apps[0];
//   EXPECT_EQ(app.Application.DiscoveryProfileUri, "DiscoveryProfileUri");
//   EXPECT_EQ(app.Application.ApplicationUri, "ApplicationUri");
//   EXPECT_EQ(app.Application.ProductUri, "ProductUri");
//   EXPECT_TRUE(app.Application.ApplicationName.Encoding & OpcUa::HAS_TEXT);
//   EXPECT_EQ(app.Application.ApplicationName.Text, "ApplicationName");
//   EXPECT_EQ(app.Application.ApplicationType, OpcUa::ApplicationType::Client);
//   EXPECT_EQ(app.Application.GatewayServerUri, "GatewayServerUri");
//
//   ASSERT_EQ(app.Endpoints.size(), 1);
//   OpcUa::EndpointDescription ed = app.Endpoints[0];
//   EXPECT_EQ(ed.EndpointUrl, "EndpointUrl");
//   EXPECT_EQ(ed.SecurityLevel, 1);
//   EXPECT_EQ(ed.SecurityMode, OpcUa::MessageSecurityMode::SignAndEncrypt);
//   EXPECT_EQ(ed.SecurityPolicyUri, "SecurityPolicyUri");
//   //EXPECT_EQ(ed.ServerCertificate, std::vector{1,2,3,4,5});
//   //EXPECT_EQ(ed.ServerDescription, "SecurityPolicyUri");
//   EXPECT_EQ(ed.TransportProfileUri, "TransportProfileUri");
//
//   ASSERT_EQ(ed.UserIdentityTokens.size(), 1);
//   OpcUa::UserTokenPolicy tokenPolicy = ed.UserIdentityTokens[0];
//   EXPECT_EQ(tokenPolicy.IssuedTokenType, "IssuedTokenType");
//   EXPECT_EQ(tokenPolicy.IssuerEndpointUrl, "IssuerEndpointUrl");
//   EXPECT_EQ(tokenPolicy.PolicyId, "PolicyId");
//   EXPECT_EQ(tokenPolicy.SecurityPolicyUri, "SecurityPolicyUri");
//   EXPECT_EQ(tokenPolicy.TokenType, OpcUa::UserTokenType::UserName);
// }

// TEST(EndpointParameters, ConvertingToAddonParameters)
// {
//   const std::vector<OpcUa::Server::ApplicationData>& application = CreateTwoEndpointsConfiguration();
//   bool debug = true;
//   const std::vector<Common::ParametersGroup>& result = OpcUa::CreateCommonParameters(application, debug);
//   ASSERT_EQ(result.size(), 2);
//   const Common::ParametersGroup applicationGroup = result[0];
//   EXPECT_EQ(applicationGroup.Name, "application");
//   EXPECT_EQ(FindParameter(applicationGroup, "discovery_profile").Value, "DiscoveryProfileUri");
//   EXPECT_EQ(FindParameter(applicationGroup, "product_uri").Value, "ProductUri");
//   EXPECT_EQ(FindParameter(applicationGroup, "application_uri").Value, "ApplicationUri");
//   EXPECT_EQ(FindParameter(applicationGroup, "application_name").Value, "ApplicationName");
//   EXPECT_EQ(FindParameter(applicationGroup, "Application_type").Value, "client");
//   EXPECT_EQ(FindParameter(applicationGroup, "gateway_server_uri").Value, "GatewayServerUri");
//
//   ASSERT_EQ(applicationGroup.Groups.size(), 1) << "Application group has no subgroups with endpoints";
//   Common::ParametersGroup endpoint = applicationGroup.Groups[0];
//   EXPECT_EQ(endpoint.Name, "endpoint");
//   EXPECT_EQ(FindParameter(endpoint, "endpoint_url").Value, "EndpointUrl");
//   EXPECT_EQ(FindParameter(endpoint, "security_level").Value, "1");
//   EXPECT_EQ(FindParameter(endpoint, "security_mode").Value, "3");
//   EXPECT_EQ(FindParameter(endpoint, "security_policy_uri").Value, "SecurityPolicyUri");
//   //EXPECT_EQ(FindParameter(endpoint, "server_certificate").Value, "SecurityPolicyUri");
//   EXPECT_EQ(FindParameter(endpoint, "transport_profile_uri").Value, "TransportProfileUri");
//
//   ASSERT_EQ(endpoint.Groups.size(), 1);
//   Common::ParametersGroup tokenPolicy = endpoint.Groups[0];
//   EXPECT_EQ(tokenPolicy.Name, "user_token_policy");
//   EXPECT_EQ(FindParameter(tokenPolicy, "issued_token_type").Value, "IssuedTokenType");
//   EXPECT_EQ(FindParameter(tokenPolicy, "issuer_endpoint_url").Value, "IssuerEndpointUrl");
//   EXPECT_EQ(FindParameter(tokenPolicy, "id").Value, "PolicyId");
//   EXPECT_EQ(FindParameter(tokenPolicy, "id").Value, "PolicyId");
//   EXPECT_EQ(FindParameter(tokenPolicy, "uri").Value, "SecurityPolicyUri");
//   EXPECT_EQ(FindParameter(tokenPolicy, "type").Value, "user_name");
// }
