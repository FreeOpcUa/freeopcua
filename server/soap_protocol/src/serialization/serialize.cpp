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
  template <typename T>
  T* CreateScalar(soap* s, T i)
  {
    T* result = (T*)soap_malloc(s, sizeof(T));
    *result = i;
    return result;
  }

  std::string* CreateString(soap* s, const std::string& str)
  {
    std::string* result = soap_new_std__string(s, 1);
    *result = str;
    return result;
  }

  time_t* CreateTimeT(soap* s, OpcUa::DateTime t)
  {
    time_t* result = (time_t*)soap_malloc(s, sizeof(time_t));
    *result = OpcUa::ToTimeT(t);
    return result;
  }

  ns3__ListOfString* CreateListOfStrings(soap* s, const std::vector<std::string>& strings)
  {
    ns3__ListOfString* lst = soap_new_ns3__ListOfString(s, 1);
    lst->String = strings;
    return lst;
  }

  ns3__ListOfDateTime* CreateListOfTimeT(soap* s, const std::vector<OpcUa::DateTime>& values)
  {
    ns3__ListOfDateTime* lst = soap_new_ns3__ListOfDateTime(s, 1);
    lst->DateTime.resize(values.size());
    std::transform(values.begin(), values.end(), lst->DateTime.begin(), OpcUa::ToTimeT);
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

  ns3__ListOfNodeId* CreateListOfNodeID(soap* s, const std::vector<OpcUa::NodeID>& ids)
  {
    ns3__ListOfNodeId* result = soap_new_ns3__ListOfNodeId(s, 1);
    result->NodeId.resize(ids.size());
    std::transform(ids.begin(), ids.end(), result->NodeId.begin(), [s](const OpcUa::NodeID& id){\
      return CreateNodeID(s, id);
    });
    return result;
  }

  ns3__ExpandedNodeId* CreateExpandedNodeID(soap* s, const OpcUa::NodeID& id)
  {
    ns3__ExpandedNodeId* result = soap_new__ns3__ExpandedNodeId(s, 1);
    std::stringstream stream;
    if (id.HasServerIndex())
    {
      stream << "srv=" << id.ServerIndex << ";";
    }
    if (id.HasNamespaceURI())
    {
      stream << "nsu=" << id.NamespaceURI << ";";
    }
    stream << GetNodeIDString(id);
    result->Identifier = CreateString(s, stream.str());
    return result;
  }

  ns3__ListOfExpandedNodeId* CreateListOfExpandedNodeID(soap* s, const std::vector<OpcUa::NodeID>& ids)
  {
    ns3__ListOfExpandedNodeId* result = soap_new_ns3__ListOfExpandedNodeId(s, 1);
    result->ExpandedNodeId.resize(ids.size());
    std::transform(ids.begin(), ids.end(), result->ExpandedNodeId.begin(), [s](const OpcUa::NodeID& id){\
      return CreateExpandedNodeID(s, id);
    });
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

  ns3__ListOfLocalizedText* CreateListOfLocalizedText(soap* s, const std::vector<OpcUa::LocalizedText>& texts)
  {
    ns3__ListOfLocalizedText* result = soap_new_ns3__ListOfLocalizedText(s, 1);
    result->LocalizedText.resize(texts.size());
    std::transform(texts.begin(), texts.end(), result->LocalizedText.begin(), [s](const OpcUa::LocalizedText& text){\
      return CreateLocalizedText(s, text);
    });
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
    result->Timestamp = OpcUa::ToTimeT(header.UtcTime);
    return result;
  }

  ns3__StatusCode* CreateStatusCode(soap* s, OpcUa::StatusCode code)
  {
    ns3__StatusCode* result = soap_new_ns3__StatusCode(s, 1);
    std::stringstream stream;
    stream << "0x" << std::hex << static_cast<uint32_t>(code);
    result->Code =  CreateString(s, stream.str());
    return result;
  }

  ns3__ListOfStatusCode* CreateListOfStatusCode(soap* s, const std::vector<OpcUa::StatusCode>& codes)
  {
    ns3__ListOfStatusCode* result = soap_new_ns3__ListOfStatusCode(s, 1);
    result->StatusCode.resize(codes.size());
    std::transform(codes.begin(), codes.end(), result->StatusCode.begin(), [s](OpcUa::StatusCode code){
      return CreateStatusCode(s, code);
    });
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
      result->Locale = CreateScalar(s, info.Locale);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_LOCALIZED_TEXT)
      result->LocalizedText = CreateScalar(s, info.LocalizedText);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_NAMESPACE)
      result->NamespaceURI = CreateScalar(s, info.NamespaceURI);
    if (info.EncodingMask & OpcUa::DiagnosticInfoMask::DIM_SYMBOLIC_ID)
      result->SymbolicId = CreateScalar(s, info.SymbolicID);
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
    result->Timestamp = OpcUa::ToTimeT(header.Timestamp);
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
    result->NamespaceIndex = CreateScalar(s, static_cast<int>(name.NamespaceIndex));
    return result;
  }

  ns3__ListOfQualifiedName* CreateListOfQualifiedName(soap* s, const std::vector<OpcUa::QualifiedName>& names)
  {
    ns3__ListOfQualifiedName* result = soap_new_ns3__ListOfQualifiedName(s, 1);
    result->QualifiedName.resize(names.size());
    std::transform(names.begin(), names.end(), result->QualifiedName.begin(), std::bind(CreateQualifiedName, s, std::placeholders::_1));
    return result;
  }

  ns3__Guid* CreateGuid(soap* s, const OpcUa::Guid& guid)
  {
    ns3__Guid* result = soap_new_ns3__Guid(s, 1);
    result->String = CreateString(s, OpcUa::ToString(guid));
    return result;
  }

  ns3__ListOfGuid* CreateListOfGuid(soap* s, const std::vector<OpcUa::Guid>& guids)
  {
    ns3__ListOfGuid* result = soap_new_ns3__ListOfGuid(s, 1);
    if (!guids.empty())
    {
      result->Guid.resize(guids.size());
      std::transform(guids.begin(), guids.end(), result->Guid.begin(), std::bind(CreateGuid, s, std::placeholders::_1));
    }
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

  ns3__ReadValueId* CreateReadValueId(soap* s, const OpcUa::AttributeValueID& attr)
  {
    ns3__ReadValueId* result = soap_new_ns3__ReadValueId(s, 1);
    result->AttributeId = static_cast<unsigned int>(attr.Attribute);
    result->DataEncoding = CreateQualifiedName(s, attr.DataEncoding);
    if (!attr.IndexRange.empty())
    {
      result->IndexRange = CreateString(s, attr.IndexRange);
    }
    result->NodeId = CreateNodeID(s, attr.Node);
    return result;
  }

  ns3__ListOfReadValueId* CreateAttributesToRead(soap* s, const std::vector<OpcUa::AttributeValueID>& attrs)
  {
    ns3__ListOfReadValueId* result = soap_new_ns3__ListOfReadValueId(s, 1);
    if (!attrs.empty())
    {
      result->ReadValueId.resize(attrs.size());
      std::transform(attrs.begin(), attrs.end(), result->ReadValueId.begin(), std::bind(CreateReadValueId, s, std::placeholders::_1));
    }
    return result;
  }

  ns3__ListOfBoolean* CreateListOfBoolean(soap* s, const std::vector<bool>& bools)
  {
    ns3__ListOfBoolean* result = soap_new_ns3__ListOfBoolean(s, 1);
    result->Boolean = bools;
    return result;
  }

  ns3__ListOfByte* CreateListOfByte(soap* s, const std::vector<uint8_t>& bytes)
  {
    ns3__ListOfByte* result = soap_new_ns3__ListOfByte(s, 1);
    result->Byte = bytes;
    return result;
  }

  ns3__ListOfSByte* CreateListOfSByte(soap* s, const std::vector<int8_t>& bytes)
  {
    ns3__ListOfSByte* result = soap_new_ns3__ListOfSByte(s, 1);
    result->SByte.assign(bytes.begin(), bytes.end());
    return result;
  }

  ns3__ListOfInt16* CreateListOfInt16(soap* s, const std::vector<int16_t>& values)
  {
    ns3__ListOfInt16* result = soap_new_ns3__ListOfInt16(s, 1);
    result->Int16.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfUInt16* CreateListOfUInt16(soap* s, const std::vector<uint16_t>& values)
  {
    ns3__ListOfUInt16* result = soap_new_ns3__ListOfUInt16(s, 1);
    result->UInt16.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfInt32* CreateListOfInt32(soap* s, const std::vector<int32_t>& values)
  {
    ns3__ListOfInt32* result = soap_new_ns3__ListOfInt32(s, 1);
    result->Int32.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfUInt32* CreateListOfUInt32(soap* s, const std::vector<uint32_t>& values)
  {
    ns3__ListOfUInt32* result = soap_new_ns3__ListOfUInt32(s, 1);
    result->UInt32.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfInt64* CreateListOfInt64(soap* s, const std::vector<int64_t>& values)
  {
    ns3__ListOfInt64* result = soap_new_ns3__ListOfInt64(s, 1);
    result->Int64.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfUInt64* CreateListOfUInt64(soap* s, const std::vector<uint64_t>& values)
  {
    ns3__ListOfUInt64* result = soap_new_ns3__ListOfUInt64(s, 1);
    result->UInt64.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfFloat* CreateListOfFloat(soap* s, const std::vector<float>& values)
  {
    ns3__ListOfFloat* result = soap_new_ns3__ListOfFloat(s, 1);
    result->Float.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfDouble* CreateListOfDouble(soap* s, const std::vector<double>& values)
  {
    ns3__ListOfDouble* result = soap_new_ns3__ListOfDouble(s, 1);
    result->Double.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfString* CreateListOfString(soap* s, const std::vector<std::string>& values)
  {
    ns3__ListOfString* result = soap_new_ns3__ListOfString(s, 1);
    result->String.assign(values.begin(), values.end());
    return result;
  }

  ns3__ListOfByteString* CreateListOfByteString(soap* s, const std::vector<OpcUa::ByteString>& values)
  {
    ns3__ListOfByteString* result = soap_new_ns3__ListOfByteString(s, 1);
    result->ByteString.resize(values.size());
    std::transform(values.begin(), values.end(), result->ByteString.begin(), [s](const OpcUa::ByteString& val){
      xsd__base64Binary result;
      result.__size = val.Data.size();
      result.__ptr = (unsigned char*)soap_malloc(s, result.__size);
      if (!result.__ptr)
      {
        result.__size = 0;
        throw std::bad_alloc();
      }
      std::copy(val.Data.begin(), val.Data.end(), result.__ptr);
      return result;
    });

    return result;
  }

  xsd__base64Binary* CreateByteString(soap* s, const OpcUa::ByteString& byteString)
  {
    return CreateByteString(s, byteString.Data);
  }

  ns3__Variant* CreateVariant(soap* s, const OpcUa::Variant& var);

  ns3__DataValue* CreateDataValue(soap* s, const OpcUa::DataValue& value)
  {
    ns3__DataValue* result = soap_new_ns3__DataValue(s, 1);
    if (value.Encoding & OpcUa::DATA_VALUE_SERVER_TIMESTAMP)
      result->ServerTimestamp = CreateTimeT(s, value.ServerTimestamp);
    if (value.Encoding & OpcUa::DATA_VALUE_SOURCE_TIMESTAMP)
      result->SourceTimestamp = CreateTimeT(s, value.SourceTimestamp);
    if (value.Encoding & OpcUa::DATA_VALUE_STATUS_CODE)
      result->StatusCode = CreateStatusCode(s, value.Status);
    if (value.Encoding & OpcUa::DATA_VALUE)
      result->Value = CreateVariant(s, value.Value);
    return result;
  }

  ns3__ListOfDataValue* CreateListOfDataValue(soap* s, const std::vector<OpcUa::DataValue>& values)
  {
    ns3__ListOfDataValue* result = soap_new_ns3__ListOfDataValue(s, 1);
    if (!values.empty())
    {
      result->DataValue.resize(values.size());
      std::transform(values.begin(), values.end(), result->DataValue.begin(), std::bind(CreateDataValue, s, std::placeholders::_1));
    }
    return result;
  }

  ns3__WriteValue* CreateWriteValue(soap* s, const OpcUa::WriteValue& value)
  {
    ns3__WriteValue* result = soap_new_ns3__WriteValue(s, 1);
    result->AttributeId = static_cast<unsigned>(value.Attribute);
    result->IndexRange = CreateString(s, value.NumericRange);
    result->NodeId = CreateNodeID(s, value.Node);
    result->Value = CreateDataValue(s, value.Data);
    return result;
  }

  ns3__ListOfWriteValue* CreateListOfNodesToWrite(soap* s, const std::vector<OpcUa::WriteValue>& values)
  {
    ns3__ListOfWriteValue* result = soap_new_ns3__ListOfWriteValue(s, 1);
    if (!values.empty())
    {
      result->WriteValue.resize(values.size());
      std::transform(values.begin(), values.end(), result->WriteValue.begin(), std::bind(CreateWriteValue, s, std::placeholders::_1));
    }
    return result;
  }



  ns3__Variant* CreateVariant(soap* s, const OpcUa::Variant& var);

  ns3__ListOfVariant* CreateListOfVariant(soap* s, const std::vector<OpcUa::Variant>& vars)
  {
    ns3__ListOfVariant* result = soap_new_ns3__ListOfVariant(s, 1);
    if (!vars.empty())
    {
      result->Variant.resize(vars.size());
      std::transform(vars.begin(), vars.end(), result->Variant.begin(), std::bind(CreateVariant, s, std::placeholders::_1));
    }
    return result;
  }


  ns3__Variant* CreateVariant(soap* s, const OpcUa::Variant& var)
  {
    if (var.IsNul())
      return nullptr;

    ns3__Variant* result = soap_new_ns3__Variant(s, 1);
    switch (var.Type)
    {
      case OpcUa::VariantType::BOOLEAN:
      {
        if (var.IsArray())
          result->ListOfBoolean = CreateListOfBoolean(s, var.Value.Boolean);
        else
          result->Boolean = CreateScalar(s, var.Value.Boolean[0]);
        break;
      }
      case OpcUa::VariantType::SBYTE:
      {
        if (var.IsArray())
          result->ListOfSByte = CreateListOfSByte(s, var.Value.SByte);
        else
          result->SByte = CreateScalar(s, static_cast<char>(var.Value.SByte[0]));
        break;
      }
      case OpcUa::VariantType::BYTE:
      {
        if (var.IsArray())
          result->ListOfByte = CreateListOfByte(s, var.Value.Byte);
        else
          result->Byte = CreateScalar(s, static_cast<unsigned char>(var.Value.Byte[0]));
        break;
      }
      case OpcUa::VariantType::INT16:
      {
        if (var.IsArray())
          result->ListOfInt16 = CreateListOfInt16(s, var.Value.Int16);
        else
          result->Int16 = CreateScalar(s, static_cast<short>(var.Value.Int16[0]));
        break;
      }
      case OpcUa::VariantType::UINT16:
      {
        if (var.IsArray())
          result->ListOfUInt16 = CreateListOfUInt16(s, var.Value.UInt16);
        else
          result->UInt16 = CreateScalar(s, static_cast<unsigned short>(var.Value.UInt16[0]));
        break;
      }
      case OpcUa::VariantType::INT32:
      {
        if (var.IsArray())
          result->ListOfInt32 = CreateListOfInt32(s, var.Value.Int32);
        else
          result->Int32 = CreateScalar(s, static_cast<int>(var.Value.Int32[0]));
        break;
      }
      case OpcUa::VariantType::UINT32:
      {
        if (var.IsArray())
          result->ListOfUInt32 = CreateListOfUInt32(s, var.Value.UInt32);
        else
          result->UInt32 = CreateScalar(s, static_cast<unsigned>(var.Value.UInt32[0]));
        break;
      }
      case OpcUa::VariantType::INT64:
      {
        if (var.IsArray())
          result->ListOfInt64 = CreateListOfInt64(s, var.Value.Int64);
        else
          result->Int64 = CreateScalar(s, static_cast<LONG64>(var.Value.Int64[0]));
        break;
      }
      case OpcUa::VariantType::UINT64:
      {
        if (var.IsArray())
          result->ListOfUInt64 = CreateListOfUInt64(s, var.Value.UInt64);
        else
          result->UInt64 = CreateScalar(s, static_cast<ULONG64>(var.Value.UInt64[0]));
        break;
      }
      case OpcUa::VariantType::FLOAT:
      {
        if (var.IsArray())
          result->ListOfFloat = CreateListOfFloat(s, var.Value.Float);
        else
          result->Float = CreateScalar(s, var.Value.Float[0]);
        break;
      }
      case OpcUa::VariantType::DOUBLE:
      {
        if (var.IsArray())
          result->ListOfDouble = CreateListOfDouble(s, var.Value.Double);
        else
          result->Double = CreateScalar(s, var.Value.Double[0]);
        break;
      }
      case OpcUa::VariantType::STRING:
      {
        if (var.IsArray())
          result->ListOfString = CreateListOfString(s, var.Value.String);
        else
          result->String = CreateString(s, var.Value.String[0]);
        break;
      }
      case OpcUa::VariantType::DATE_TIME:
      {
        if (var.IsArray())
          result->ListOfDateTime = CreateListOfTimeT(s, var.Value.Time);
        else
          result->DateTime = CreateTimeT(s, var.Value.Time[0]);
        break;
      }
      case OpcUa::VariantType::GUID:
      {
        if (var.IsArray())
          result->ListOfGuid = CreateListOfGuid(s, var.Value.Guids);
        else
          result->Guid = CreateGuid(s, var.Value.Guids[0]);
        break;
      }
      case OpcUa::VariantType::BYTE_STRING:
      {
        if (var.IsArray())
          result->ListOfByteString = CreateListOfByteString(s, var.Value.ByteStrings);
        else
          result->ByteString = CreateByteString(s, var.Value.ByteStrings[0]);
        break;
      }
      case OpcUa::VariantType::XML_ELEMENT:
      {
        break;
      }
      case OpcUa::VariantType::NODE_ID:
      case OpcUa::VariantType::EXPANDED_NODE_ID:
      {
        if (var.IsArray())
          result->ListOfExpandedNodeId = CreateListOfExpandedNodeID(s, var.Value.Node);
        else
          result->ExpandedNodeId = CreateExpandedNodeID(s, var.Value.Node[0]);
        break;
      }
      case OpcUa::VariantType::STATUS_CODE:
      {
        if (var.IsArray())
          result->ListOfStatusCode = CreateListOfStatusCode(s, var.Value.Statuses);
        else
          result->StatusCode = CreateStatusCode(s, var.Value.Statuses[0]);
        break;
      }
      case OpcUa::VariantType::QUALIFIED_NAME:
      {
        if (var.IsArray())
          result->ListOfQualifiedName = CreateListOfQualifiedName(s, var.Value.Name);
        else
          result->QualifiedName = CreateQualifiedName(s, var.Value.Name[0]);
        break;
      }
      case OpcUa::VariantType::LOCALIZED_TEXT:
      {
        if (var.IsArray())
          result->ListOfLocalizedText = CreateListOfLocalizedText(s, var.Value.Text);
        else
          result->LocalizedText = CreateLocalizedText(s, var.Value.Text[0]);
        break;
      }
      case OpcUa::VariantType::EXTENSION_OBJECT:
      {
        break;
      }
      case OpcUa::VariantType::DATA_VALUE:
      {
        if (var.IsArray())
          result->ListOfDataValue = CreateListOfDataValue(s, var.Value.Value);
        else
          result->DataValue = CreateDataValue(s, var.Value.Value[0]);
        break;
      }
      case OpcUa::VariantType::VARIANT:
      {
        result->ListOfVariant = CreateListOfVariant(s, var.Value.Variants);
        break;
      }
      case OpcUa::VariantType::DIAGNOSTIC_INFO:
      {
        if (var.IsArray())
          result->ListOfDiagnosticInfo = CreateListOfDiagnosticInfo(s, var.Value.Diagnostic);
        else
          result->DiagnosticInfo = CreateDiagnosticInfo(s, var.Value.Diagnostic[0]);
        break;
      }
      default:
      {
        break;
      }
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

  ns3__ReadRequest* Soap::Serialize(soap* s, const OpcUa::ReadRequest& opcua)
  {
    ns3__ReadRequest* result = soap_new_ns3__ReadRequest(s, 1);
    result->RequestHeader = CreateRequestHeader(s, opcua.Header);
    result->MaxAge = opcua.Parameters.MaxAge;
    result->TimestampsToReturn = static_cast<ns3__TimestampsToReturn>(opcua.Parameters.TimestampsType);
    result->NodesToRead = CreateAttributesToRead(s, opcua.Parameters.AttributesToRead);
    return result;
  }

  ns3__ReadResponse* Soap::Serialize(soap* s, const OpcUa::ReadResponse& opcua)
  {
    ns3__ReadResponse* result = soap_new_ns3__ReadResponse(s, 1);
    result->ResponseHeader = CreateResponseHeader(s, opcua.Header);
    if (!opcua.Result.Diagnostics.empty())
    {
      result->DiagnosticInfos = CreateListOfDiagnosticInfo(s, opcua.Result.Diagnostics);
    }
    result->Results = CreateListOfDataValue(s, opcua.Result.Results);
    return result;
  }

  ns3__WriteRequest* Soap::Serialize(soap* s, const OpcUa::WriteRequest& opcua)
  {
    ns3__WriteRequest* result = soap_new_ns3__WriteRequest(s, 1);
    result->RequestHeader = CreateRequestHeader(s, opcua.Header);
    result->NodesToWrite = CreateListOfNodesToWrite(s, opcua.Parameters.NodesToWrite);
    return result;
  }

  ns3__WriteResponse* Soap::Serialize(soap* s, const OpcUa::WriteResponse& opcua)
  {
    ns3__WriteResponse* result = soap_new_ns3__WriteResponse(s, 1);
    result->ResponseHeader = CreateResponseHeader(s, opcua.Header);
    result->DiagnosticInfos = CreateListOfDiagnosticInfo(s, opcua.Result.Diagnostics);
    result->Results = CreateListOfStatusCode(s, opcua.Result.StatusCodes);
    return result;
  }

  ns3__Variant* Soap::Serialize(soap* s, const OpcUa::Variant& var)
  {
    return CreateVariant(s, var);
  }
}

