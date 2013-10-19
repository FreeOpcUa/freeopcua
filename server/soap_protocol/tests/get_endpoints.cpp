/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <gtest/gtest.h>
#include "../src/serialize.h"
#include "../src/deserialize.h"



TEST(GetEndpoints, Request)
{
  OpcUa::GetEndpointsRequest opcua;
  opcua.Header.Additional.Encoding = 1;
  opcua.Header.Additional.TypeID = OpcUa::NumericNodeID(2);
  opcua.Header.AuditEntryID = "audit";
  opcua.Header.RequestHandle = 3;
  opcua.Header.ReturnDiagnostics = 4;
  opcua.Header.SessionAuthenticationToken = OpcUa::NumericNodeID(3);
  opcua.Header.Timeout = 5;
  opcua.Header.UtcTime = 6;
  opcua.Filter.EndpointURL = "url";
  opcua.Filter.LocaleIDs.push_back("ru");
  opcua.Filter.ProfileUries.push_back("profile");

  soap service;
  ns3__GetEndpointsRequest* soapRequest = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(soapRequest, nullptr);

  ASSERT_NE(soapRequest->RequestHeader->AdditionalHeader, nullptr);
  ASSERT_NE(soapRequest->RequestHeader->AdditionalHeader->Body, nullptr);
  ASSERT_NE(soapRequest->RequestHeader->AdditionalHeader->TypeId, nullptr);
  ASSERT_EQ(*soapRequest->RequestHeader->AdditionalHeader->TypeId->Identifier, "ns=0;i=2;");

  ASSERT_NE(soapRequest->RequestHeader, nullptr);
  ASSERT_NE(soapRequest->RequestHeader->AuditEntryId, nullptr);
  ASSERT_EQ(*soapRequest->RequestHeader->AuditEntryId, "audit");

  ASSERT_EQ(soapRequest->RequestHeader->RequestHandle, 3);
  ASSERT_EQ(soapRequest->RequestHeader->ReturnDiagnostics, 4);

  ASSERT_NE(soapRequest->RequestHeader->AuthenticationToken, nullptr);
  ASSERT_NE(soapRequest->RequestHeader->AuthenticationToken->Identifier, nullptr);
  ASSERT_EQ(*soapRequest->RequestHeader->AuthenticationToken->Identifier, "ns=0;i=3;");

  ASSERT_EQ(soapRequest->RequestHeader->TimeoutHint, 5);
  ASSERT_EQ(soapRequest->RequestHeader->Timestamp, 6);

  ASSERT_NE(soapRequest->EndpointUrl, nullptr);
  ASSERT_EQ(*soapRequest->EndpointUrl, "url");

  ASSERT_NE(soapRequest->LocaleIds, nullptr);
  ASSERT_EQ(soapRequest->LocaleIds->String, opcua.Filter.LocaleIDs);

  ASSERT_NE(soapRequest->ProfileUris, nullptr);
  ASSERT_EQ(soapRequest->ProfileUris->String, opcua.Filter.ProfileUries);

  OpcUa::GetEndpointsRequest deserialized = OpcUa::Soap::Deserialize(soapRequest);
//TODO  ASSERT_EQ(deserialized.Header.Additional.Encoding, opcua.Header.Additional.Encoding);
  ASSERT_EQ(deserialized.Header.Additional.TypeID, opcua.Header.Additional.TypeID);
  ASSERT_EQ(deserialized.Header.SessionAuthenticationToken, opcua.Header.SessionAuthenticationToken);
  ASSERT_EQ(deserialized.Header.AuditEntryID, opcua.Header.AuditEntryID);
  ASSERT_EQ(deserialized.Header.RequestHandle, opcua.Header.RequestHandle);
  ASSERT_EQ(deserialized.Header.ReturnDiagnostics, opcua.Header.ReturnDiagnostics);
  ASSERT_EQ(deserialized.Header.Timeout, opcua.Header.Timeout);
  ASSERT_EQ(deserialized.Header.UtcTime, opcua.Header.UtcTime);

  ASSERT_EQ(deserialized.Filter.EndpointURL, opcua.Filter.EndpointURL);
  ASSERT_EQ(deserialized.Filter.LocaleIDs, opcua.Filter.LocaleIDs);
  ASSERT_EQ(deserialized.Filter.ProfileUries, opcua.Filter.ProfileUries);
}

TEST(GetEndpoints, Response)
{
  OpcUa::GetEndpointsResponse response;
  response.Header.Additional.Encoding = 1;
  response.Header.Additional.TypeID = OpcUa::NumericNodeID(2);

  OpcUa::DiagnosticInfo diagnostic;
  unsigned mask =
      OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO |
      OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE |
      OpcUa::DiagnosticInfoMask::DIM_LOCALE |
      OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT |
      OpcUa::DiagnosticInfoMask::DIM_NAMESPACE |
      OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID |
      OpcUa::DiagnosticInfoMask::DIM_INNER_DIAGNOSTIC_INFO;
  diagnostic.EncodingMask = static_cast<OpcUa::DiagnosticInfoMask>(mask);
  diagnostic.AdditionalInfo = "additional";
  diagnostic.InnerStatusCode = OpcUa::StatusCode::BadNotImplemented;
  diagnostic.Locale = 1;
  diagnostic.LocalizedText = 2;
  diagnostic.NamespaceURI = 3;
  diagnostic.SymbolicID = 4;
  response.Header.InnerDiagnostics.push_back(diagnostic);
  response.Header.InnerDiagnostics.push_back(OpcUa::DiagnosticInfo());

  response.Header.RequestHandle = 3;
  response.Header.ServiceResult = OpcUa::StatusCode::BadNotReadable;
  response.Header.StringTable.push_back("table");
  response.Header.Timestamp = 4;

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
  ASSERT_NE(soapResponse->ResponseHeader, nullptr);
  ASSERT_NE(soapResponse->ResponseHeader->AdditionalHeader, nullptr);
  ASSERT_NE(soapResponse->ResponseHeader->AdditionalHeader->Body, nullptr);
  ASSERT_NE(soapResponse->ResponseHeader->AdditionalHeader->TypeId, nullptr);
  ASSERT_EQ(*soapResponse->ResponseHeader->AdditionalHeader->TypeId->Identifier, "ns=0;i=2;");

  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics, nullptr);
  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->AdditionalInfo, nullptr);
  ASSERT_EQ(*soapResponse->ResponseHeader->ServiceDiagnostics->AdditionalInfo, "additional");

  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->InnerStatusCode, nullptr);
  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->InnerStatusCode->Code, nullptr);
  ASSERT_FALSE(soapResponse->ResponseHeader->ServiceDiagnostics->InnerStatusCode->Code->empty());

  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->Locale, nullptr);
  ASSERT_EQ(*soapResponse->ResponseHeader->ServiceDiagnostics->Locale, 1);

  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->LocalizedText, nullptr);
  ASSERT_EQ(*soapResponse->ResponseHeader->ServiceDiagnostics->LocalizedText, 2);

  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->NamespaceURI, nullptr);
  ASSERT_EQ(*soapResponse->ResponseHeader->ServiceDiagnostics->NamespaceURI, 3);

  ASSERT_NE(soapResponse->ResponseHeader->ServiceDiagnostics->SymbolicId, nullptr);
  ASSERT_EQ(*soapResponse->ResponseHeader->ServiceDiagnostics->SymbolicId, 4);

  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->AdditionalInfo, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->InnerStatusCode, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->Locale, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->LocalizedText, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->NamespaceURI, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->SymbolicId, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->ServiceDiagnostics->InnerDiagnosticInfo->InnerDiagnosticInfo, nullptr);

  ASSERT_EQ(soapResponse->ResponseHeader->RequestHandle, 3);
  ASSERT_NE(soapResponse->ResponseHeader->ServiceResult, nullptr);
  ASSERT_NE(soapResponse->ResponseHeader->ServiceResult->Code, nullptr);
  ASSERT_FALSE(soapResponse->ResponseHeader->ServiceResult->Code->empty());
  ASSERT_NE(soapResponse->ResponseHeader->StringTable, nullptr);
  ASSERT_EQ(soapResponse->ResponseHeader->StringTable->String.size(), 1);
  ASSERT_EQ(soapResponse->ResponseHeader->StringTable->String[0], "table");
  ASSERT_EQ(soapResponse->ResponseHeader->Timestamp, 4);

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
  ASSERT_EQ(deserialized.TypeID, response.TypeID);
  // TODO ASSERT_EQ(deserialized.Header.Additional.Encoding, response.Header.Additional.Encoding);
  ASSERT_EQ(deserialized.Header.Additional.TypeID, response.Header.Additional.TypeID);
  ASSERT_EQ(deserialized.Header.RequestHandle, response.Header.RequestHandle);
  ASSERT_EQ(deserialized.Header.ServiceResult, response.Header.ServiceResult);
  ASSERT_EQ(deserialized.Header.StringTable, response.Header.StringTable);
  ASSERT_EQ(deserialized.Header.Timestamp, response.Header.Timestamp);

  ASSERT_EQ(deserialized.Header.InnerDiagnostics.size(), 2);
  const OpcUa::DiagnosticInfo& diag1 = deserialized.Header.InnerDiagnostics[0];
  const OpcUa::DiagnosticInfo& srcDiag = response.Header.InnerDiagnostics[0];
  ASSERT_EQ(diag1.EncodingMask, mask);
  ASSERT_EQ(diag1.AdditionalInfo, srcDiag.AdditionalInfo);
  ASSERT_EQ(diag1.InnerStatusCode, srcDiag.InnerStatusCode);
  ASSERT_EQ(diag1.Locale, srcDiag.Locale);
  ASSERT_EQ(diag1.LocalizedText, srcDiag.LocalizedText);
  ASSERT_EQ(diag1.NamespaceURI, srcDiag.NamespaceURI);
  ASSERT_EQ(diag1.SymbolicID, srcDiag.SymbolicID);
  ASSERT_EQ(diag1.EncodingMask, mask);

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
