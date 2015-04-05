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
#include <opc/ua/protocol/session.h>
#include <opc/ua/server/endpoints_services.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>


using namespace testing;

class EndpointsRegistry :public Test
{
protected:
  virtual void SetUp()
  {
    Addons = Common::CreateAddonsManager();
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
  desc.EndpointURL = "url";

  OpcUa::Server::EndpointsRegistry::SharedPtr endpoints = Addons->GetAddon<OpcUa::Server::EndpointsRegistry>(OpcUa::Server::EndpointsRegistryAddonId);
  endpoints->AddEndpoints(std::vector<OpcUa::EndpointDescription>(1, desc));

  OpcUa::Server::ServicesRegistry::SharedPtr services = Addons->GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);
  OpcUa::Services::SharedPtr computer = services->GetServer();
  std::vector<OpcUa::EndpointDescription> descriptions;
  ASSERT_NO_THROW(descriptions = computer->Endpoints()->GetEndpoints(OpcUa::EndpointsFilter()));
  ASSERT_EQ(descriptions.size(), 1);
  ASSERT_EQ(descriptions[0].EndpointURL, "url");
}

std::vector<Common::ParametersGroup> CreateTwoEndpointsParameters()
{
  Common::ParametersGroup data;
  data.Name = "application";
  data.Parameters.push_back(Common::Parameter("discovery_profile", "DiscoveryProfileURI"));
  data.Parameters.push_back(Common::Parameter("uri", "URI"));
  data.Parameters.push_back(Common::Parameter("product_uri", "ProductURI"));
  data.Parameters.push_back(Common::Parameter("gateway_server_uri", "GatewayServerURI"));
  data.Parameters.push_back(Common::Parameter("name", "Name"));
  data.Parameters.push_back(Common::Parameter("type", "client"));

  Common::ParametersGroup endpoint("endpoint");
  endpoint.Parameters.push_back(Common::Parameter("url", "EndpointURL"));
  endpoint.Parameters.push_back(Common::Parameter("security_level", "1"));
  endpoint.Parameters.push_back(Common::Parameter("security_mode", "sign_encrypt"));
  endpoint.Parameters.push_back(Common::Parameter("security_policy_uri", "SecurityPolicyURI"));
  endpoint.Parameters.push_back(Common::Parameter("transport_profile_uri", "TransportProfileURI"));

  Common::ParametersGroup tokenPolicy("user_token_policy");
  tokenPolicy.Parameters.push_back(Common::Parameter("issued_token_type", "IssuedTokenType"));
  tokenPolicy.Parameters.push_back(Common::Parameter("issuer_endpoint_url", "IssuerEndpointURL"));
  tokenPolicy.Parameters.push_back(Common::Parameter("id", "PolicyId"));
  tokenPolicy.Parameters.push_back(Common::Parameter("uri", "SecurityPolicyURI"));
  tokenPolicy.Parameters.push_back(Common::Parameter("type", "user_name"));

  endpoint.Groups.push_back(tokenPolicy);
  data.Groups.push_back(endpoint);

  return std::vector<Common::ParametersGroup>{data, data};
}

std::vector<OpcUa::Server::ApplicationData> CreateTwoEndpointsConfiguration()
{
  OpcUa::Server::ApplicationData data;
  data.Application.DiscoveryProfileURI = "DiscoveryProfileURI";
  data.Application.URI = "URI";
  data.Application.ProductURI = "ProductURI";
  data.Application.GatewayServerURI = "GatewayServerURI";
  data.Application.Name = OpcUa::LocalizedText("Name");
  data.Application.Type = OpcUa::ApplicationType::CLIENT;

  OpcUa::EndpointDescription ed;
  ed.EndpointURL = "EndpointURL";
  ed.SecurityLevel = 1;
  ed.SecurityMode = OpcUa::MessageSecurityMode::MSM_SIGN_AND_ENCRYPT;
  ed.SecurityPolicyURI = "SecurityPolicyURI";
  ed.TransportProfileURI = "TransportProfileURI";

  OpcUa::UserTokenPolicy tokenPolicy; // By default ot os an anonimous token;
  tokenPolicy.IssuedTokenType = "IssuedTokenType";
  tokenPolicy.IssuerEndpointURL = "IssuerEndpointURL";
  tokenPolicy.PolicyId = "PolicyId";
  tokenPolicy.SecurityPolicyURI = "SecurityPolicyURI";
  tokenPolicy.TokenType = OpcUa::UserIdentifyTokenType::USERNAME;
  ed.UserIdentifyTokens.push_back(tokenPolicy);

  data.Endpoints = {ed};

  return std::vector<OpcUa::Server::ApplicationData>{data, data};
}

Common::Parameter FindParameter(const Common::ParametersGroup& group, const char* name)
{
  auto paramIt = std::find_if(group.Parameters.begin(), group.Parameters.end(), [name](const Common::Parameter& param){
    return param.Name == name;
  });

  if (paramIt != group.Parameters.end())
  {
    return *paramIt;
  }
  return Common::Parameter();
}

TEST(EndpointParameters, ConvertingFromAddonParameters)
{
  std::vector<Common::ParametersGroup> params = CreateTwoEndpointsParameters();
  bool debug = true;
  std::vector<OpcUa::Server::ApplicationData> apps = OpcUa::ParseEndpointsParameters(params, debug);
  ASSERT_EQ(apps.size(), 2);
  OpcUa::Server::ApplicationData app = apps[0];
  EXPECT_EQ(app.Application.DiscoveryProfileURI, "DiscoveryProfileURI");
  EXPECT_EQ(app.Application.URI, "URI");
  EXPECT_EQ(app.Application.ProductURI, "ProductURI");
  EXPECT_TRUE(app.Application.Name.Encoding & OpcUa::HAS_TEXT);
  EXPECT_EQ(app.Application.Name.Text, "Name");
  EXPECT_EQ(app.Application.Type, OpcUa::ApplicationType::CLIENT);
  EXPECT_EQ(app.Application.GatewayServerURI, "GatewayServerURI");

  ASSERT_EQ(app.Endpoints.size(), 1);
  OpcUa::EndpointDescription ed = app.Endpoints[0];
  EXPECT_EQ(ed.EndpointURL, "EndpointURL");
  EXPECT_EQ(ed.SecurityLevel, 1);
  EXPECT_EQ(ed.SecurityMode, OpcUa::MessageSecurityMode::MSM_SIGN_AND_ENCRYPT);
  EXPECT_EQ(ed.SecurityPolicyURI, "SecurityPolicyURI");
  //EXPECT_EQ(ed.ServerCertificate, std::vector{1,2,3,4,5});
  //EXPECT_EQ(ed.ServerDescription, "SecurityPolicyURI");
  EXPECT_EQ(ed.TransportProfileURI, "TransportProfileURI");

  ASSERT_EQ(ed.UserIdentifyTokens.size(), 1);
  OpcUa::UserTokenPolicy tokenPolicy = ed.UserIdentifyTokens[0];
  EXPECT_EQ(tokenPolicy.IssuedTokenType, "IssuedTokenType");
  EXPECT_EQ(tokenPolicy.IssuerEndpointURL, "IssuerEndpointURL");
  EXPECT_EQ(tokenPolicy.PolicyId, "PolicyId");
  EXPECT_EQ(tokenPolicy.SecurityPolicyURI, "SecurityPolicyURI");
  EXPECT_EQ(tokenPolicy.TokenType, OpcUa::UserIdentifyTokenType::USERNAME);
}

TEST(EndpointParameters, ConvertingToAddonParameters)
{
  const std::vector<OpcUa::Server::ApplicationData>& application = CreateTwoEndpointsConfiguration();
  bool debug = true;
  const std::vector<Common::ParametersGroup>& result = OpcUa::CreateCommonParameters(application, debug);
  ASSERT_EQ(result.size(), 2);
  const Common::ParametersGroup applicationGroup = result[0];
  EXPECT_EQ(applicationGroup.Name, "application");
  EXPECT_EQ(FindParameter(applicationGroup, "discovery_profile").Value, "DiscoveryProfileURI");
  EXPECT_EQ(FindParameter(applicationGroup, "product_uri").Value, "ProductURI");
  EXPECT_EQ(FindParameter(applicationGroup, "uri").Value, "URI");
  EXPECT_EQ(FindParameter(applicationGroup, "name").Value, "Name");
  EXPECT_EQ(FindParameter(applicationGroup, "type").Value, "client");
  EXPECT_EQ(FindParameter(applicationGroup, "gateway_server_uri").Value, "GatewayServerURI");

  ASSERT_EQ(applicationGroup.Groups.size(), 1) << "Application group has no subgroups with endpoints";
  Common::ParametersGroup endpoint = applicationGroup.Groups[0];
  EXPECT_EQ(endpoint.Name, "endpoint");
  EXPECT_EQ(FindParameter(endpoint, "url").Value, "EndpointURL");
  EXPECT_EQ(FindParameter(endpoint, "security_level").Value, "1");
  EXPECT_EQ(FindParameter(endpoint, "security_mode").Value, "sign_encrypt");
  EXPECT_EQ(FindParameter(endpoint, "security_policy_uri").Value, "SecurityPolicyURI");
  //EXPECT_EQ(FindParameter(endpoint, "server_certificate").Value, "SecurityPolicyURI");
  EXPECT_EQ(FindParameter(endpoint, "transport_profile_uri").Value, "TransportProfileURI");

  ASSERT_EQ(endpoint.Groups.size(), 1);
  Common::ParametersGroup tokenPolicy = endpoint.Groups[0];
  EXPECT_EQ(tokenPolicy.Name, "user_token_policy");
  EXPECT_EQ(FindParameter(tokenPolicy, "issued_token_type").Value, "IssuedTokenType");
  EXPECT_EQ(FindParameter(tokenPolicy, "issuer_endpoint_url").Value, "IssuerEndpointURL");
  EXPECT_EQ(FindParameter(tokenPolicy, "id").Value, "PolicyId");
  EXPECT_EQ(FindParameter(tokenPolicy, "id").Value, "PolicyId");
  EXPECT_EQ(FindParameter(tokenPolicy, "uri").Value, "SecurityPolicyURI");
  EXPECT_EQ(FindParameter(tokenPolicy, "type").Value, "user_name");
}
