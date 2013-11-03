/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "common.h"

#include "../src/serialization/serialize.h"
#include "../src/serialization/deserialize.h"



TEST(GetEndpoints, Request)
{
  OpcUa::GetEndpointsRequest opcua;
  opcua.Header = OpcUa::Test::CreateRequestHeader();
  opcua.Filter.EndpointURL = "url";
  opcua.Filter.LocaleIDs.push_back("ru");
  opcua.Filter.ProfileUries.push_back("profile");

  soap service;
  ns3__GetEndpointsRequest* soapRequest = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(soapRequest, nullptr);

  OpcUa::Test::AssertRequestHeaderValid(soapRequest->RequestHeader);

  ASSERT_NE(soapRequest->EndpointUrl, nullptr);
  ASSERT_EQ(*soapRequest->EndpointUrl, "url");

  ASSERT_NE(soapRequest->LocaleIds, nullptr);
  ASSERT_EQ(soapRequest->LocaleIds->String, opcua.Filter.LocaleIDs);

  ASSERT_NE(soapRequest->ProfileUris, nullptr);
  ASSERT_EQ(soapRequest->ProfileUris->String, opcua.Filter.ProfileUries);

  OpcUa::GetEndpointsRequest deserialized = OpcUa::Soap::Deserialize(soapRequest);
  OpcUa::Test::AssertRequestHeaderEq(deserialized.Header, opcua.Header);

  ASSERT_EQ(deserialized.Filter.EndpointURL, opcua.Filter.EndpointURL);
  ASSERT_EQ(deserialized.Filter.LocaleIDs, opcua.Filter.LocaleIDs);
  ASSERT_EQ(deserialized.Filter.ProfileUries, opcua.Filter.ProfileUries);
}

TEST(GetEndpoints, Response)
{
  OpcUa::GetEndpointsResponse response;
  response.Header = OpcUa::Test::CreateResponseHeader();

  OpcUa::EndpointDescription endpoint;
  endpoint.EndpointURL = "url";
  endpoint.SecurityLevel = 5;
  endpoint.SecurityMode = OpcUa::MessageSecurityMode::MSM_NONE;
  endpoint.SecurityPolicyURI = "security_policy";
  endpoint.ServerCertificate.push_back(1);
  endpoint.TransportProfileURI = "transport_profile_uri";
  endpoint.ServerDescription.DiscoveryProfileURI = "profile_uri";
  endpoint.ServerDescription.DiscoveryURLs.push_back("discovery");
  endpoint.ServerDescription.GatewayServerURI = "gateway";
  endpoint.ServerDescription.Name.Encoding = OpcUa::HAS_LOCALE | OpcUa::HAS_TEXT;
  endpoint.ServerDescription.Name.Locale = "ru";
  endpoint.ServerDescription.Name.Text = "text";
  endpoint.ServerDescription.Type = OpcUa::ApplicationType::CLIENT_AND_SERVER;
  endpoint.ServerDescription.URI = "uri";
  endpoint.ServerDescription.ProductURI = "product";
  OpcUa::UserTokenPolicy token;
  token.IssuedTokenType = "issued";
  token.IssuerEndpointURL = "endpoint";
  token.PolicyID = "policy_uri";
  token.SecurityPolicyURI = "security_policy";
  token.TokenType = OpcUa::UserIdentifyTokenType::ISSUED_TOKEN;
  endpoint.UserIdentifyTokens.push_back(token);
  response.Endpoints.push_back(endpoint);

  soap service;
  ns3__GetEndpointsResponse* soapResponse = OpcUa::Soap::Serialize(&service, response);

  ASSERT_NE(soapResponse, nullptr);
  OpcUa::Test::AssertResponseHeaderValid(soapResponse->ResponseHeader);

  ASSERT_NE(soapResponse->Endpoints, nullptr);
  ASSERT_EQ(soapResponse->Endpoints->EndpointDescription.size(), 1);
  ASSERT_NE(soapResponse->Endpoints->EndpointDescription[0], nullptr);
  ns3__EndpointDescription* desc = soapResponse->Endpoints->EndpointDescription[0];
  ASSERT_NE(desc->EndpointUrl, nullptr);
  ASSERT_EQ(*desc->EndpointUrl, "url");
  ASSERT_EQ(desc->SecurityLevel, 5);
  ASSERT_EQ(desc->SecurityMode, ns3__MessageSecurityMode__None_USCORE1);
  ASSERT_NE(desc->SecurityPolicyUri, nullptr);
  ASSERT_EQ(*desc->SecurityPolicyUri, "security_policy");
  ASSERT_NE(desc->ServerCertificate, nullptr); // TODO check content
  ASSERT_NE(desc->TransportProfileUri, nullptr);
  ASSERT_EQ(*desc->TransportProfileUri, "transport_profile_uri");

  ASSERT_NE(desc->Server, nullptr);
  ASSERT_NE(desc->Server->ApplicationName, nullptr);
  ASSERT_NE(desc->Server->ApplicationName->Locale, nullptr);
  ASSERT_EQ(*desc->Server->ApplicationName->Locale, "ru");
  ASSERT_NE(desc->Server->ApplicationName->Text, nullptr);
  ASSERT_EQ(*desc->Server->ApplicationName->Text, "text");
  ASSERT_EQ(desc->Server->ApplicationType, ns3__ApplicationType::ns3__ApplicationType__ClientAndServer_USCORE2);
  ASSERT_NE(desc->Server->ApplicationUri, nullptr);
  ASSERT_EQ(*desc->Server->ApplicationUri, "uri");
  ASSERT_NE(desc->Server->DiscoveryProfileUri, nullptr);
  ASSERT_EQ(*desc->Server->DiscoveryProfileUri, "profile_uri");
  ASSERT_NE(desc->Server->DiscoveryUrls, nullptr);
  ASSERT_EQ(desc->Server->DiscoveryUrls->String.size(), 1);
  ASSERT_EQ(desc->Server->DiscoveryUrls->String[0], "discovery");
  ASSERT_NE(desc->Server->GatewayServerUri, nullptr);
  ASSERT_EQ(*desc->Server->GatewayServerUri, "gateway");
  ASSERT_NE(desc->Server->ProductUri, nullptr);
  ASSERT_EQ(*desc->Server->ProductUri, "product");

  ASSERT_NE(desc->UserIdentityTokens, nullptr);
  ASSERT_EQ(desc->UserIdentityTokens->UserTokenPolicy.size(), 1);
  ns3__UserTokenPolicy* soapPolicy = desc->UserIdentityTokens->UserTokenPolicy[0];
  ASSERT_NE(soapPolicy->IssuedTokenType, nullptr);
  ASSERT_EQ(*soapPolicy->IssuedTokenType, "issued");
  ASSERT_NE(soapPolicy->IssuerEndpointUrl, nullptr);
  ASSERT_EQ(*soapPolicy->IssuerEndpointUrl, "endpoint");
  ASSERT_NE(soapPolicy->PolicyId, nullptr);
  ASSERT_EQ(*soapPolicy->PolicyId, "policy_uri");
  ASSERT_NE(soapPolicy->SecurityPolicyUri, nullptr);
  ASSERT_EQ(*soapPolicy->SecurityPolicyUri, "security_policy");
  ASSERT_EQ(soapPolicy->TokenType, ns3__UserTokenType::ns3__UserTokenType__IssuedToken_USCORE3);

  OpcUa::GetEndpointsResponse deserialized = OpcUa::Soap::Deserialize(soapResponse);
  ASSERT_RESPONSE_HEADER_EQ(deserialized.Header, response.Header);

  ASSERT_EQ(deserialized.Endpoints.size(), 1);
  const OpcUa::EndpointDescription& d = deserialized.Endpoints[0];
  const OpcUa::EndpointDescription& sd = response.Endpoints[0];
  ASSERT_EQ(d.EndpointURL, sd.EndpointURL);
  ASSERT_EQ(d.SecurityLevel, sd.SecurityLevel);
  ASSERT_EQ(d.SecurityMode, sd.SecurityMode);
  ASSERT_EQ(d.SecurityPolicyURI, sd.SecurityPolicyURI);
  ASSERT_EQ(d.TransportProfileURI, sd.TransportProfileURI);
  ASSERT_EQ(d.ServerCertificate, sd.ServerCertificate);

  ASSERT_EQ(d.ServerDescription.Name.Encoding, sd.ServerDescription.Name.Encoding);
  ASSERT_EQ(d.ServerDescription.Name.Locale, sd.ServerDescription.Name.Locale);
  ASSERT_EQ(d.ServerDescription.Name.Text, sd.ServerDescription.Name.Text);
  ASSERT_EQ(d.ServerDescription.DiscoveryProfileURI, sd.ServerDescription.DiscoveryProfileURI);
  ASSERT_EQ(d.ServerDescription.DiscoveryURLs, sd.ServerDescription.DiscoveryURLs);
  ASSERT_EQ(d.ServerDescription.GatewayServerURI, sd.ServerDescription.GatewayServerURI);
  ASSERT_EQ(d.ServerDescription.ProductURI, sd.ServerDescription.ProductURI);
  ASSERT_EQ(d.ServerDescription.Type, sd.ServerDescription.Type);
  ASSERT_EQ(d.ServerDescription.URI, sd.ServerDescription.URI);
}
