/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "factory.h"

#include <opc/ua/types.h>

#include <sstream>

namespace
{
  int* CreateInt(soap* s, int32_t i)
  {
    int* result = (int*)soap_malloc(s, sizeof(int));
    *result = i;
    return result;
  }

  std::string* CreateString(soap* s, const std::string& str)
  {
    std::string* result = soap_new_std__string(s, 1);
    *result = str;
    return result;
  }

  ns3__ListOfString* CreateListOfStrings(soap* s, const std::vector<std::string>& strings)
  {
    ns3__ListOfString* lst = soap_new_ns3__ListOfString(s, 1);
    lst->String = strings;
    return lst;
  }

  std::string GetNodeIDString(const OpcUa::NodeID& id)
  {
    std::stringstream stream;
    stream << "ns:" << id.GetNamespaceIndex() << ";";
    if (id.IsInteger())
    {
      stream << "i:" << id.GetIntegerIdentifier() << ";";
    }
    else if(id.IsString())
    {
      stream << "s" << id.GetStringIdentifier() << ";";
    }
    else if (id.IsGuid())
    {
      const OpcUa::Guid& guid = id.GetGuidIdentifier();
      stream << std::hex << guid.Data1 << "-";
      stream << std::hex << guid.Data2 << "-";
      stream << std::hex << guid.Data3 << "-";

      for (std::size_t i = 0; i < 8; ++i)
      {
        stream << std::hex << guid.Data4[i];
      }
    }
    return stream.str();
  }

  ns3__NodeId* CreateNodeID(soap* s, const OpcUa::NodeID& id)
  {
    ns3__NodeId* result = soap_new__ns3__NodeId(s, 1);
    result->Identifier = CreateString(s, GetNodeIDString(id));
    return result;
  }

  ns3__ExpandedNodeId* CreateExpandedNodeID(soap* s, const OpcUa::NodeID& id)
  {
    ns3__ExpandedNodeId* result = soap_new__ns3__ExpandedNodeId(s, 1);
    std::stringstream stream;
    if (id.HasServerIndex())
    {
      stream << "srv:" << id.ServerIndex << ";";
    }
    if (id.HasNamespaceURI())
    {
      stream << "nsu:" << id.NamespaceURI << ";";
    }
    stream << GetNodeIDString(id);
    result->Identifier = CreateString(s, stream.str());
    return result;
  }

  ns3__LocalizedText* CreateLocalizedText(soap* s, const OpcUa::LocalizedText& text)
  {
    ns3__LocalizedText* result = soap_new_ns3__LocalizedText(s, 1);
    if (text.Encoding & OpcUa::HAS_LOCALE)
    {
      result->Locale = CreateString(s, text.Locale);
    }
    if (text.Encoding & OpcUa::HAS_TEXT)
    {
      result->Text = CreateString(s, text.Text);
    }
    return result;
  }

  ns3__ApplicationDescription* CreateApplicationDescription(soap* s, const OpcUa::ApplicationDescription& desc)
  {
    ns3__ApplicationDescription* result = soap_new_ns3__ApplicationDescription(s, 1);
    result->ApplicationName = CreateLocalizedText(s, desc.Name);
    result->ApplicationType = static_cast<ns3__ApplicationType>(desc.Type);
    result->ApplicationUri = CreateString(s, desc.URI);
    result->DiscoveryProfileUri = CreateString(s, desc.DiscoveryProfileURI);
    result->DiscoveryUrls = CreateListOfStrings(s, desc.DiscoveryURLs);
    result->GatewayServerUri = CreateString(s, desc.GatewayServerURI);
    result->ProductUri = CreateString(s, desc.ProductURI);
    return result;
  }

  ns3__UserTokenPolicy* CreateUserTokenPolicy(soap* s, const OpcUa::UserTokenPolicy& policy)
  {
    ns3__UserTokenPolicy* result = soap_new_ns3__UserTokenPolicy(s, 1);
    result->IssuedTokenType = CreateString(s, policy.IssuedTokenType);
    result->IssuerEndpointUrl = CreateString(s, policy.IssuerEndpointURL);
    result->PolicyId = CreateString(s, policy.PolicyID);
    result->SecurityPolicyUri = CreateString(s, policy.SecurityPolicyURI);
    result->TokenType = static_cast<ns3__UserTokenType>(policy.TokenType);
    return result;
  }

  ns3__ListOfUserTokenPolicy* CreateUserTokenPolicies(soap* s, const std::vector<OpcUa::UserTokenPolicy> policies)
  {
    ns3__ListOfUserTokenPolicy* result = soap_new_ns3__ListOfUserTokenPolicy(s, 1);
    for (auto it = policies.begin(); it != policies.end(); ++it)
    {
      const OpcUa::UserTokenPolicy& policy = *it;
      result->UserTokenPolicy.push_back(CreateUserTokenPolicy(s, policy));
    }
    return result;
  }


  ns3__EndpointDescription* BuildEndpointDescription(soap* s, const OpcUa::EndpointDescription& opcua)
  {
    ns3__EndpointDescription* desc = soap_new_ns3__EndpointDescription(s, 1);
    desc->EndpointUrl = CreateString(s, opcua.EndpointURL);
    desc->SecurityLevel = opcua.SecurityLevel;
    desc->SecurityMode = static_cast<ns3__MessageSecurityMode>(opcua.SecurityMode);
    desc->SecurityPolicyUri = CreateString(s, opcua.SecurityPolicyURI);
    desc->Server = CreateApplicationDescription(s, opcua.ServerDescription);
    desc->ServerCertificate = soap_new_xsd__base64Binary(s, 1); // TODO fill
    desc->TransportProfileUri = CreateString(s, opcua.TransportProfileURI);
    desc->Server = CreateApplicationDescription(s, opcua.ServerDescription);
    desc->UserIdentityTokens = CreateUserTokenPolicies(s, opcua.UserIdentifyTokens);
    return desc;
  }

  ns3__ExtensionObject* CreateAdditionalHeader(soap* s, const OpcUa::AdditionalHeader& opcua)
  {
    ns3__ExtensionObject* result = soap_new_ns3__ExtensionObject(s, 1);
    result->Body = soap_new__ns3__ExtensionObject_Body(s, 1);
    result->TypeId = CreateExpandedNodeID(s, opcua.TypeID);
    return result;
  }


  ns3__RequestHeader* CreateRequestHeader(soap* s, const OpcUa::RequestHeader& header)
  {
    ns3__RequestHeader* result = soap_new_ns3__RequestHeader(s, 1);
    result->AdditionalHeader = CreateAdditionalHeader(s, header.Additional);
    result->AuditEntryId = CreateString(s, header.AuditEntryID);
    result->AuthenticationToken = CreateNodeID(s, header.SessionAuthenticationToken);
    result->RequestHandle = header.RequestHandle;
    result->ReturnDiagnostics = header.ReturnDiagnostics;
    result->TimeoutHint = header.Timeout;
    result->Timestamp = header.UtcTime;
    return result;
  }

  ns3__StatusCode* CreateStatusCode(soap* s, OpcUa::StatusCode code)
  {
    ns3__StatusCode* result = soap_new_ns3__StatusCode(s, 1);
    result->Code =  CreateString(s, std::to_string(static_cast<uint32_t>(code)));
    return result;
  }

  ns3__DiagnosticInfo* CreateDiagnosticInfo(soap* s, const OpcUa::DiagnosticInfoList& opcua)
  {
    if (opcua.empty())
    {
      return nullptr;
    }

    ns3__DiagnosticInfo* result = soap_new_ns3__DiagnosticInfo(s, 1);
    ns3__DiagnosticInfo* tmp = result;
    for (OpcUa::DiagnosticInfoList::const_iterator it = opcua.begin(); it != opcua.end(); ++it)
    {
      if (it != opcua.begin())
      {
        tmp->InnerDiagnosticInfo = soap_new_ns3__DiagnosticInfo(s, 1);
        tmp = tmp->InnerDiagnosticInfo;
      }
      const OpcUa::DiagnosticInfo& info = *it;
      if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO)
        tmp->AdditionalInfo = CreateString(s, info.AdditionalInfo);
      if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE)
        tmp->InnerStatusCode =  CreateStatusCode(s, info.InnerStatusCode);
      if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_LOCALE)
        tmp->Locale = CreateInt(s, info.Locale);
      if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT)
        tmp->LocalizedText = CreateInt(s, info.LocalizedText);
      if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_NAMESPACE)
        tmp->NamespaceURI = CreateInt(s, info.NamespaceURI);
      if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID)
        tmp->SymbolicId = CreateInt(s, info.SymbolicID);
    }

    return result;
  }

  ns3__ResponseHeader* CreateResponseHeader(soap* s, const OpcUa::ResponseHeader& header)
  {
    ns3__ResponseHeader* result = soap_new_ns3__ResponseHeader(s, 1);
    result->AdditionalHeader = CreateAdditionalHeader(s, header.Additional);
    if (!header.InnerDiagnostics.empty())
    {
      result->ServiceDiagnostics = CreateDiagnosticInfo(s, header.InnerDiagnostics);
    }
    result->RequestHandle = header.RequestHandle;
    result->ServiceResult = CreateStatusCode(s, header.ServiceResult);
    result->StringTable = CreateListOfStrings(s, header.StringTable);
    result->Timestamp = header.Timestamp;
    return result;
  }

}

namespace OpcUa
{

  ns3__GetEndpointsRequest* Soap::BuildEndpointsRequest(soap* s, const OpcUa::GetEndpointsRequest& opcua)
  {
    ns3__GetEndpointsRequest* request = soap_new_ns3__GetEndpointsRequest(s, 1);
    request->RequestHeader = CreateRequestHeader(s, opcua.Header);
    request->EndpointUrl = CreateString(s, opcua.Filter.EndpointURL);
    request->LocaleIds = CreateListOfStrings(s, opcua.Filter.LocaleIDs);
    request->ProfileUris = CreateListOfStrings(s, opcua.Filter.ProfileUries);
    return request;
  }

  ns3__GetEndpointsResponse* Soap::BuildEndpointsResponse(soap* s, const OpcUa::GetEndpointsResponse& opcua)
  {
    ns3__GetEndpointsResponse* resp = soap_new_ns3__GetEndpointsResponse(s, 1);
    resp->ResponseHeader = CreateResponseHeader(s, opcua.Header);
    resp->Endpoints = soap_new_ns3__ListOfEndpointDescription(s, 1);
    for (auto endpointIt = opcua.Endpoints.begin(); endpointIt != opcua.Endpoints.end(); ++endpointIt)
    {
      const OpcUa::EndpointDescription& desc = *endpointIt;
      resp->Endpoints->EndpointDescription.push_back(BuildEndpointDescription(s, desc));
    }
    return resp;
  }
}

