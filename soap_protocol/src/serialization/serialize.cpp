/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "serialize.h"

#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <functional>
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
    stream << "ns=" << id.GetNamespaceIndex() << ";";
    if (id.IsInteger())
    {
      stream << "i=" << id.GetIntegerIdentifier() << ";";
    }
    else if(id.IsString())
    {
      stream << "s=" << id.GetStringIdentifier() << ";";
    }
    else if (id.IsGuid())
    {
      stream << "g=";
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
    desc->ServerCertificate->__ptr = (unsigned char*)soap_malloc(s, opcua.ServerCertificate.size());
    desc->ServerCertificate->__size = opcua.ServerCertificate.size();
    std::copy(opcua.ServerCertificate.begin(), opcua.ServerCertificate.end(), desc->ServerCertificate->__ptr);
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

  ns3__DiagnosticInfo* CreateDiagnosticInfo(soap* s, const OpcUa::DiagnosticInfo& info)
  {
    ns3__DiagnosticInfo* result = soap_new_ns3__DiagnosticInfo(s, 1);

    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO)
      result->AdditionalInfo = CreateString(s, info.AdditionalInfo);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE)
      result->InnerStatusCode =  CreateStatusCode(s, info.InnerStatusCode);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_LOCALE)
      result->Locale = CreateInt(s, info.Locale);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT)
      result->LocalizedText = CreateInt(s, info.LocalizedText);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_NAMESPACE)
      result->NamespaceURI = CreateInt(s, info.NamespaceURI);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID)
      result->SymbolicId = CreateInt(s, info.SymbolicID);
    if ((info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_INNER_DIAGNOSTIC_INFO) && info.InnerDiagnostics)
      result->InnerDiagnosticInfo = CreateDiagnosticInfo(s, *info.InnerDiagnostics);

    return result;
  }

  ns3__ResponseHeader* CreateResponseHeader(soap* s, const OpcUa::ResponseHeader& header)
  {
    ns3__ResponseHeader* result = soap_new_ns3__ResponseHeader(s, 1);
    result->AdditionalHeader = CreateAdditionalHeader(s, header.Additional);
    result->ServiceDiagnostics = CreateDiagnosticInfo(s, header.InnerDiagnostics);
    result->RequestHandle = header.RequestHandle;
    result->ServiceResult = CreateStatusCode(s, header.ServiceResult);
    result->StringTable = CreateListOfStrings(s, header.StringTable);
    result->Timestamp = header.Timestamp;
    return result;
  }

  ns3__ViewDescription* CreateViewDescription(soap* s, const OpcUa::ViewDescription& view)
  {
    ns3__ViewDescription* result = soap_new_ns3__ViewDescription(s, 1);
    result->Timestamp = view.Timestamp;
    result->ViewId = CreateNodeID(s, view.ID);
    result->ViewVersion = view.Version;
    return result;
  }

  ns3__BrowseDescription* CreateBrowseDescription(soap* s, const OpcUa::BrowseDescription& desc)
  {
    ns3__BrowseDescription* result = soap_new_ns3__BrowseDescription(s, 1);
    result->BrowseDirection = static_cast<ns3__BrowseDirection>(desc.Direction);
    result->IncludeSubtypes = desc.IncludeSubtypes;
    result->NodeClassMask = desc.NodeClasses;
    result->NodeId = CreateNodeID(s, desc.NodeToBrowse);
    result->ReferenceTypeId = CreateNodeID(s, desc.ReferenceTypeID);
    result->ResultMask = desc.ResultMask;
    return result;
  }

  ns3__ListOfBrowseDescription* CreateViewDescriptionList(soap* s, const std::vector<OpcUa::BrowseDescription>& nodesToBrowse)
  {
    ns3__ListOfBrowseDescription* result = soap_new_ns3__ListOfBrowseDescription(s, 1);
    result->BrowseDescription.resize(nodesToBrowse.size());
    std::transform(nodesToBrowse.begin(), nodesToBrowse.end(), result->BrowseDescription.begin(), std::bind(CreateBrowseDescription, s, std::placeholders::_1));
    return result;
  }

  ns3__ListOfDiagnosticInfo* CreateListOfDiagnosticInfo(soap* s, const OpcUa::DiagnosticInfoList& diags)
  {
    ns3__ListOfDiagnosticInfo* result = soap_new_ns3__ListOfDiagnosticInfo(s, 1);
    result->DiagnosticInfo.resize(diags.size());
    std::transform(diags.begin(), diags.end(), result->DiagnosticInfo.begin(),
        [s](const OpcUa::DiagnosticInfo& info)
        {
          return CreateDiagnosticInfo(s, info);
        });

    return result;
  }



  ns3__QualifiedName* CreateQualifiedName(soap* s, const OpcUa::QualifiedName& name)
  {
    ns3__QualifiedName* result = soap_new_ns3__QualifiedName(s, 1);
    result->Name = CreateString(s, name.Name);
    result->NamespaceIndex = CreateInt(s, name.NamespaceIndex);
    return result;
  }

  ns3__ReferenceDescription* CreateReferenceDescription(soap* s, const OpcUa::ReferenceDescription& ref)
  {
    ns3__ReferenceDescription* result = soap_new_ns3__ReferenceDescription(s, 1);
    result->BrowseName = CreateQualifiedName(s, ref.BrowseName);
    result->DisplayName = CreateLocalizedText(s, ref.DisplayName);
    result->IsForward = ref.IsForward;
    result->NodeClass = static_cast<ns3__NodeClass>(ref.TargetNodeClass);
    result->NodeId = CreateExpandedNodeID(s, ref.TargetNodeID);
    result->ReferenceTypeId = CreateNodeID(s, ref.ReferenceTypeID);
    result->TypeDefinition = CreateExpandedNodeID(s, ref.TargetNodeTypeDefinition);
    return result;
  }

  ns3__ListOfReferenceDescription* CreateListOfReferenceDescription(soap* s, const std::vector<OpcUa::ReferenceDescription>& referencies)
  {
    ns3__ListOfReferenceDescription* result = soap_new_ns3__ListOfReferenceDescription(s, 1);
    if (!referencies.empty())
    {
      result->ReferenceDescription.resize(referencies.size());
      std::transform(referencies.begin(), referencies.end(), result->ReferenceDescription.begin(), std::bind(CreateReferenceDescription, s, std::placeholders::_1));
    }
    return result;
  }

  xsd__base64Binary* CreateByteString(soap* s, const std::vector<uint8_t>& bytes)
  {
    xsd__base64Binary* result = soap_new_xsd__base64Binary(s, 1);
    result->__ptr = (unsigned char*)SOAP_MALLOC(s, bytes.size());
    if (!result->__ptr)
    {
      throw std::bad_alloc();
    }
    result->__size = bytes.size();
    std::copy(bytes.begin(), bytes.end(), result->__ptr);

    return result;
  }

  ns3__BrowseResult* CreateBrowseResult(soap* s, const OpcUa::BrowseResult& browse)
  {
    ns3__BrowseResult* result = soap_new_ns3__BrowseResult(s, 1);

    if (!browse.ContinuationPoint.empty())
    {
      result->ContinuationPoint = CreateByteString(s, browse.ContinuationPoint);
    }
    result->StatusCode = CreateStatusCode(s, static_cast<OpcUa::StatusCode>(browse.Status));
    result->References = CreateListOfReferenceDescription(s, browse.Referencies);

    return result;
  }

  ns3__ListOfBrowseResult* CreateListOfBrowseResult(soap* s, const std::vector<OpcUa::BrowseResult>& opcua)
  {
    ns3__ListOfBrowseResult* result = soap_new_ns3__ListOfBrowseResult(s, 1);
    if (!opcua.empty())
    {
      result->BrowseResult.resize(opcua.size());
      std::transform(opcua.begin(), opcua.end(), result->BrowseResult.begin(), std::bind(CreateBrowseResult, s, std::placeholders::_1));
    }
    return result;
  }

}

namespace OpcUa
{

  ns3__GetEndpointsRequest* Soap::Serialize(soap* s, const GetEndpointsRequest& opcua)
  {
    ns3__GetEndpointsRequest* request = soap_new_ns3__GetEndpointsRequest(s, 1);
    request->RequestHeader = CreateRequestHeader(s, opcua.Header);
    if (!opcua.Filter.EndpointURL.empty())
      request->EndpointUrl = CreateString(s, opcua.Filter.EndpointURL);
    if (!opcua.Filter.LocaleIDs.empty())
      request->LocaleIds = CreateListOfStrings(s, opcua.Filter.LocaleIDs);
    if (!opcua.Filter.ProfileUries.empty())
      request->ProfileUris = CreateListOfStrings(s, opcua.Filter.ProfileUries);
    return request;
  }

  ns3__GetEndpointsResponse* Soap::Serialize(soap* s, const OpcUa::GetEndpointsResponse& opcua)
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

  ns3__BrowseRequest* Soap::Serialize(soap* s, const OpcUa::BrowseRequest& request)
  {
    ns3__BrowseRequest* req = soap_new_ns3__BrowseRequest(s, 1);
    req->RequestHeader = CreateRequestHeader(s, request.Header);
    req->RequestedMaxReferencesPerNode = request.Query.MaxReferenciesPerNode;
    req->View = CreateViewDescription(s, request.Query.View);
    req->NodesToBrowse = CreateViewDescriptionList(s, request.Query.NodesToBrowse);
    return req;
  }

  ns3__BrowseResponse* Soap::Serialize(soap* s, const OpcUa::BrowseResponse& opcua)
  {
    ns3__BrowseResponse* result = soap_new_ns3__BrowseResponse(s, 1);
    result->ResponseHeader = CreateResponseHeader(s, opcua.Header);
    if (!opcua.Diagnostics.empty())
    {
      result->DiagnosticInfos = CreateListOfDiagnosticInfo(s, opcua.Diagnostics);
    }
    result->Results = CreateListOfBrowseResult(s, opcua.Results);
    return result;
  }
}

