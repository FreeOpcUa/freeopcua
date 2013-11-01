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

  OpcUa::DiagnosticInfoList Deserialize(const ns3__DiagnosticInfo* diag)
  {
    OpcUa::DiagnosticInfoList result;
    if (!diag)
      return result;

    while (diag)
    {
      OpcUa::DiagnosticInfo tmp;
      unsigned mask = 0;
      if (diag->AdditionalInfo)
      {
        tmp.AdditionalInfo = *diag->AdditionalInfo;
        mask |= OpcUa::DiagnosticInfoMask::DIM_ADDITIONAL_INFO;
      }
      if (diag->Locale)
      {
        tmp.Locale = *diag->Locale;
        mask |= OpcUa::DiagnosticInfoMask::DIM_LOCALE;
      }
      if (diag->LocalizedText)
      {
        tmp.LocalizedText = *diag->LocalizedText;
        mask |= OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT;
      }
      if (diag->NamespaceURI)
      {
        tmp.NamespaceURI = *diag->NamespaceURI;
        mask |= OpcUa::DiagnosticInfoMask::DIM_NAMESPACE;
      }
      if (diag->SymbolicId)
      {
        tmp.SymbolicID = *diag->SymbolicId;
        mask |= OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID;
      }
      if (diag->InnerStatusCode)
      {
        tmp.InnerStatusCode = Deserialize(diag->InnerStatusCode);
        mask |= OpcUa::DiagnosticInfoMask::DIM_INNER_STATUS_CODE;
      }
      if (diag->InnerDiagnosticInfo)
      {
        mask |= OpcUa::DiagnosticInfoMask::DIM_INNER_DIAGNOSTIC_INFO;
      }
      tmp.EncodingMask = static_cast<OpcUa::DiagnosticInfoMask>(mask);
      result.push_back(tmp);
      diag = diag->InnerDiagnosticInfo;
    }
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

  std::vector<OpcUa::EndpointDescription> Deserialize(ns3__ListOfEndpointDescription* endpoints)
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

}
