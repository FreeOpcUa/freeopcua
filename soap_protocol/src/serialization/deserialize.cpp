/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "deserialize.h"

#include <algorithm>
#include <sstream>

namespace
{

  std::string GetNodeField(const std::string& data, const char* field)
  {
    std::size_t colon = data.find(field);
    if (colon == std::string::npos)
    {
      return std::string();
    }
    colon += strlen(field);
    const std::size_t semicolon = data.find(";", colon);
    if (semicolon == std::string::npos)
    {
      data.substr(colon);
    }
    return data.substr(colon, semicolon - colon);
  }


  uint32_t GetInteger(const std::string&value)
  {
    if (value.empty())
    {
      return 0;
    }
    return std::stoul(value);
  }

  OpcUa::NodeID Deserialize(const ns3__NodeId* id)
  {
    if (!id || !id->Identifier)
    {
      return OpcUa::NodeID();
    }
    const std::string& data = *id->Identifier;
    const std::string ns = GetNodeField(data, "ns=");
    const std::string integer = GetNodeField(data, "i=");
    if (!integer.empty())
    {
      return OpcUa::NumericNodeID(GetInteger(integer), GetInteger(ns));
    }
    const std::string str = GetNodeField(data, "s=");
    if (!str.empty())
    {
      return OpcUa::StringNodeID(str, GetInteger(ns));
    }

    return OpcUa::NodeID();
  }

  OpcUa::NodeID Deserialize(const ns3__ExpandedNodeId* id)
  {
    if (!id || !id->Identifier)
    {
      return OpcUa::NodeID();
    }

    const std::string& data = *id->Identifier;
    const std::string srv = GetNodeField(data, "srv=");
    const std::string nsu = GetNodeField(data, "nsu=");
    const std::string ns = GetNodeField(data, "ns=");
    const std::string integer = GetNodeField(data, "i=");
    if (!integer.empty())
    {
      OpcUa::NodeID result = OpcUa::NumericNodeID(GetInteger(integer), GetInteger(ns));
      result.ServerIndex = GetInteger(srv);
      result.NamespaceURI = nsu;
      return result;
    }

    const std::string str = GetNodeField(data, "s=");
    if (!str.empty())
    {
      OpcUa::NodeID result = OpcUa::StringNodeID(str, GetInteger(ns));
      result.ServerIndex = GetInteger(srv);
      result.NamespaceURI = nsu;
      return result;
    }
    // TODO Guid and binary.
    return OpcUa::NodeID();
  }

  OpcUa::StatusCode Deserialize(const ns3__StatusCode* status)
  {
    if (status && status->Code)
    {
      return static_cast<OpcUa::StatusCode>(std::stoul(*status->Code));
    }
    return OpcUa::StatusCode::Good;
  }

  OpcUa::AdditionalHeader Deserialize(const ns3__ExtensionObject* obj)
  {
    OpcUa::AdditionalHeader result;
    if (obj->TypeId)
    {
      result.TypeID = Deserialize(obj->TypeId);
    }
    return result;
  }

  OpcUa::RequestHeader Deserialize(const ns3__RequestHeader* header)
  {
    OpcUa::RequestHeader result;
    if (!header)
      return result;

    if (header->AdditionalHeader)
      result.Additional = Deserialize(header->AdditionalHeader);
    if (header->AuditEntryId)
      result.AuditEntryID = *header->AuditEntryId;
    if (header->AuthenticationToken)
      result.SessionAuthenticationToken = Deserialize(header->AuthenticationToken);
    result.RequestHandle = header->RequestHandle;
    result.ReturnDiagnostics = header->ReturnDiagnostics;
    result.Timeout = header->TimeoutHint;
    result.UtcTime = header->Timestamp;

    return result;
  }

  OpcUa::DiagnosticInfo Deserialize(const ns3__DiagnosticInfo* diag)
  {
    OpcUa::DiagnosticInfo result;
    if (!diag)
      return result;

    unsigned mask = 0;
    if (diag->AdditionalInfo)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO;
      result.AdditionalInfo = *diag->AdditionalInfo;
    }
    if (diag->Locale)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_LOCALE;
      result.Locale = *diag->Locale;
    }
    if (diag->LocalizedText)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT;
      result.LocalizedText = *diag->LocalizedText;
    }
    if (diag->NamespaceURI)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_NAMESPACE;
      result.NamespaceURI = *diag->NamespaceURI;
    }
    if (diag->SymbolicId)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID;
      result.SymbolicID = *diag->SymbolicId;
    }
    if (diag->InnerStatusCode)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE;
      result.InnerStatusCode = Deserialize(diag->InnerStatusCode);
    }
    if (diag->InnerDiagnosticInfo)
    {
      mask |= OpcUa::DiagnosticInfoMask::DIM_INNER_DIAGNOSTIC_INFO;
      result.InnerDiagnostics.reset(new OpcUa::DiagnosticInfo);
      *result.InnerDiagnostics = Deserialize(diag->InnerDiagnosticInfo);
    }

    result.EncodingMask = static_cast<OpcUa::DiagnosticInfoMask>(mask);

    return result;
  }

  OpcUa::ResponseHeader Deserialize(const ns3__ResponseHeader* header)
  {
    OpcUa::ResponseHeader result;
    if (!header)
      return result;

    if (header->AdditionalHeader)
    {
      result.Additional = Deserialize(header->AdditionalHeader);
    }
    if (header->ServiceDiagnostics)
    {
      result.InnerDiagnostics = Deserialize(header->ServiceDiagnostics);
    }
    if (header->StringTable)
    {
      result.StringTable = header->StringTable->String;
    }
    if (header->ServiceResult)
    {
      result.ServiceResult = Deserialize(header->ServiceResult);
    }
    result.RequestHandle = header->RequestHandle;
    result.Timestamp = header->Timestamp;
    return result;
  }

  OpcUa::DiagnosticInfoList Deserialize(const ns3__ListOfDiagnosticInfo* diags)
  {
    OpcUa::DiagnosticInfoList result;
    if (!diags)
      return result;

    result.resize(diags->DiagnosticInfo.size());
    std::transform(diags->DiagnosticInfo.begin(), diags->DiagnosticInfo.end(), result.begin(),
      [](const ns3__DiagnosticInfo* val)
      {
        return Deserialize(val);
      }
    );

    return result;
  }

  OpcUa::LocalizedText Deserialize(const ns3__LocalizedText* text)
  {
    OpcUa::LocalizedText result;
    if (!text)
      return result;

    if (text->Locale)
    {
      result.Locale = *text->Locale;
      result.Encoding |= OpcUa::HAS_LOCALE;
    }
    if (text->Text)
    {
      result.Text = *text->Text;
      result.Encoding |= OpcUa::HAS_TEXT;
    }
    return result;
  }

  OpcUa::ApplicationDescription Deserialize(const ns3__ApplicationDescription* desc)
  {
    OpcUa::ApplicationDescription result;
    if (!desc)
      return result;

    if (desc->ApplicationName)
    {
      result.Name = Deserialize(desc->ApplicationName);
    }
    if (desc->DiscoveryProfileUri)
    {
      result.DiscoveryProfileURI = *desc->DiscoveryProfileUri;
    }
    if (desc->ApplicationUri)
    {
      result.URI = *desc->ApplicationUri;
    }
    if (desc->DiscoveryUrls)
    {
      result.DiscoveryURLs = desc->DiscoveryUrls->String;
    }
    if (desc->GatewayServerUri)
    {
      result.GatewayServerURI = *desc->GatewayServerUri;
    }
    if (desc->ProductUri)
    {
      result.ProductURI = *desc->ProductUri;
    }
    result.Type = static_cast<OpcUa::ApplicationType>(desc->ApplicationType);
    return result;
  }

  OpcUa::EndpointDescription Deserialize(const ns3__EndpointDescription* desc)
  {
    OpcUa::EndpointDescription result;
    if (!desc)
      return result;

    if (desc->EndpointUrl)
    {
      result.EndpointURL = *desc->EndpointUrl;
    }
    result.SecurityLevel = desc->SecurityLevel;
    result.SecurityMode = static_cast<OpcUa::MessageSecurityMode>(desc->SecurityMode);
    if (desc->SecurityPolicyUri)
    {
      result.SecurityPolicyURI = *desc->SecurityPolicyUri;
    }
    if (desc->Server)
    {
      result.ServerDescription = Deserialize(desc->Server);
    }
    if (desc->ServerCertificate && desc->ServerCertificate->__ptr && desc->ServerCertificate->__size)
    {
      result.ServerCertificate.assign(desc->ServerCertificate->__ptr, desc->ServerCertificate->__ptr + desc->ServerCertificate->__size);
    }
    if (desc->TransportProfileUri)
    {
      result.TransportProfileURI = *desc->TransportProfileUri;
    }
    /*
    if (desc->UserIdentityTokens)
    {
      result.UserIdentifyTokens = Deserialize(desc->UserIdentityTokens);
    }
    */
    return result;
  }

  std::vector<OpcUa::EndpointDescription> Deserialize(const ns3__ListOfEndpointDescription* endpoints)
  {
    std::vector<OpcUa::EndpointDescription> result;
    if (!endpoints)
      return result;

    for (auto endpointIt = endpoints->EndpointDescription.begin(); endpointIt != endpoints->EndpointDescription.end(); ++endpointIt)
    {
      ns3__EndpointDescription* tmp = *endpointIt;
      const OpcUa::EndpointDescription desc = Deserialize(tmp);
      result.push_back(desc);
    }
    return result;
  }

  OpcUa::BrowseDescription Deserialize(const ns3__BrowseDescription* desc)
  {
    OpcUa::BrowseDescription result;
    if (!desc)
    {
      return result;
    }
    result.Direction = static_cast<OpcUa::BrowseDirection>(desc->BrowseDirection);
    result.IncludeSubtypes = desc->IncludeSubtypes;
    result.NodeClasses = desc->NodeClassMask;
    result.ResultMask = desc->ResultMask;
    if (desc->NodeId)
    {
      result.NodeToBrowse = Deserialize(desc->NodeId);
    }
    if (desc->ReferenceTypeId)
    {
      result.ReferenceTypeID = Deserialize(desc->ReferenceTypeId);
    }
    return result;
  }

  std::vector<OpcUa::BrowseDescription> Deserialize(const ns3__ListOfBrowseDescription* desc)
  {
    std::vector<OpcUa::BrowseDescription> result;
    if (!desc)
      return result;

    result.resize(desc->BrowseDescription.size());
    std::transform(desc->BrowseDescription.begin(), desc->BrowseDescription.end(), result.begin(),
        [](const ns3__BrowseDescription* value)
        {
          return Deserialize(value);
        });

    return result;
  }

  std::vector<uint8_t> Deserialize(xsd__base64Binary* binary)
  {
    std::vector<uint8_t> result;
    if (binary && binary->__ptr && binary->__size)
    {
      result.resize(binary->__size);
      std::size_t size = binary->__size;
      const char* data = (char*)binary->__ptr;
      std::copy(data, data + size, result.begin());
    }
    return result;
  }

  OpcUa::QualifiedName Deserialize(ns3__QualifiedName* name)
  {
    OpcUa::QualifiedName result;
    if (!name)
      return result;

    if (name->Name)
      result.Name = *name->Name;
    if (name->NamespaceIndex)
      result.NamespaceIndex = *name->NamespaceIndex;

    return result;
  }

  OpcUa::ReferenceDescription Deserialize(const ns3__ReferenceDescription* desc)
  {
    OpcUa::ReferenceDescription result;
    if (!desc)
      return result;

    if (desc->BrowseName)
      result.BrowseName = Deserialize(desc->BrowseName);
    if (desc->DisplayName)
      result.DisplayName = Deserialize(desc->DisplayName);
    if (desc->IsForward)
      result.IsForward = desc->IsForward;
    if (desc->NodeClass)
      result.TargetNodeClass = static_cast<OpcUa::NodeClass>(desc->NodeClass);
    if (desc->NodeId)
      result.TargetNodeID = Deserialize(desc->NodeId);
    if (desc->ReferenceTypeId)
      result.ReferenceTypeID = Deserialize(desc->ReferenceTypeId);
    if (desc->TypeDefinition)
      result.TargetNodeTypeDefinition = Deserialize(desc->TypeDefinition);

    return result;
  }

  std::vector<OpcUa::ReferenceDescription> Deserialize(const ns3__ListOfReferenceDescription* refs)
  {
    std::vector<OpcUa::ReferenceDescription> result;

    result.resize(refs->ReferenceDescription.size());
    std::transform(refs->ReferenceDescription.begin(), refs->ReferenceDescription.end(), result.begin(),
        [](const ns3__ReferenceDescription* value)
        {
          return Deserialize(value);
        });

    return result;
  }

  OpcUa::BrowseResult Deserialize(const ns3__BrowseResult* browse)
  {
    OpcUa::BrowseResult result;
    result.ContinuationPoint = Deserialize(browse->ContinuationPoint);
    result.Referencies = Deserialize(browse->References);
    result.Status = Deserialize(browse->StatusCode);
    return result;
  }

  std::vector<OpcUa::BrowseResult> Deserialize(const ns3__ListOfBrowseResult* browse)
  {
    std::vector<OpcUa::BrowseResult> result;
    if (!browse)
      return result;

    result.resize(browse->BrowseResult.size());
    std::transform(browse->BrowseResult.begin(), browse->BrowseResult.end(), result.begin(),
        [](const ns3__BrowseResult* value)
        {
          return Deserialize(value);
        });

    return result;
  }
}

namespace OpcUa
{

  GetEndpointsRequest Soap::Deserialize(const ns3__GetEndpointsRequest* request)
  {
    GetEndpointsRequest result;
    if (!request)
      return result;

    if (request->RequestHeader)
    {
      result.Header = ::Deserialize(request->RequestHeader);
    }
    if (request->EndpointUrl)
    {
      result.Filter.EndpointURL = *request->EndpointUrl;
    }
    if (request->LocaleIds)
    {
      result.Filter.LocaleIDs = request->LocaleIds->String;
    }
    if (request->ProfileUris)
    {
      result.Filter.ProfileUries = request->ProfileUris->String;
    }
    return result;
  }

  GetEndpointsResponse Soap::Deserialize(const ns3__GetEndpointsResponse* response)
  {
    GetEndpointsResponse result;
    if (!response)
      return result;

    if (response->ResponseHeader)
    {
      result.Header = ::Deserialize(response->ResponseHeader);
    }
    if (response->Endpoints)
    {
      result.Endpoints = ::Deserialize(response->Endpoints);
    }
    return result;
  }

  BrowseRequest Soap::Deserialize(const ns3__BrowseRequest* request)
  {
    BrowseRequest result;
    if (!request)
      return result;

    if (request->RequestHeader)
    {
      result.Header = ::Deserialize(request->RequestHeader);
    }
    if (request->NodesToBrowse)
    {
      result.Query.NodesToBrowse = ::Deserialize(request->NodesToBrowse);
    }
    result.Query.MaxReferenciesPerNode = request->RequestedMaxReferencesPerNode;
    return  result;
  }

  BrowseResponse Soap::Deserialize(const ns3__BrowseResponse* response)
  {
    BrowseResponse result;
    result.Header = ::Deserialize(response->ResponseHeader);
    result.Diagnostics = ::Deserialize(response->DiagnosticInfos);
    result.Results = ::Deserialize(response->Results);
    return result;
  }
}
