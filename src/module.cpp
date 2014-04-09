/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>
#include <boost/python/type_id.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <opc/ua/client/remote_computer.h>
#include <opc/ua/computer.h>
#include <opc/ua/node.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/client/client.h>
#include <opc/ua/opcuaserver.h>

#include <functional>

namespace OpcUa
{

  using namespace boost;

  template <typename T>
  python::list ToList(const std::vector<T> objects)
  {
    python::list result;
    std::for_each(objects.begin(), objects.end(),
      [&result](const T& obj)
      {
        result.append(obj);
      }
    );
    return result;
  }

  template <typename ResultType, typename SourceType>
  python::list ToList(const std::vector<SourceType> objects)
  {
    python::list result;
    std::for_each(objects.begin(), objects.end(),
      [&result](const SourceType& obj)
      {
        result.append(ResultType(obj));
      }
    );
    return result;
  }

  template <typename T>
  std::vector<T> FromList(const python::object& list)
  {
    std::vector<T> result;
    std::size_t listSize = python::len(list);
    for (std::size_t i = 0; i < listSize; ++i)
    {
      python::object element = list[i];
      const T value = python::extract<T>(element);
      result.push_back(value);
    }
    return result;
  }

  struct PyNodeID
  {
    unsigned NamespaceIndex;
    python::object Identifier;
    unsigned ServerIndex;
    std::string NamespaceURI;

    PyNodeID()
      : NamespaceIndex(0)
      , ServerIndex(0)
    {
    }

    PyNodeID(const OpcUa::NodeID& node)
      : NamespaceIndex(node.GetNamespaceIndex())
      , ServerIndex(node.ServerIndex)
      , NamespaceURI(node.NamespaceURI)
    {
      if (node.IsString())
      {
        Identifier = python::str(node.GetStringIdentifier());
      }
      else if (node.IsInteger())
      {
        Identifier = python::long_(node.GetIntegerIdentifier());
      }
      else
      {
        std::stringstream stream;
        stream << "Unsupported or not implemented node type: " << node.Encoding;
        throw std::logic_error(stream.str());
      }
    }

  };

  struct PyApplicationDescription
  {
    std::string URI;
    std::string ProductURI;
    std::string Name;
    unsigned Type;
    std::string GatewayServerURI;
    std::string DiscoveryProfileURI;
    python::list DiscoveryURLs;

    PyApplicationDescription()
      : Type(0)
    {
    }

    explicit PyApplicationDescription(const OpcUa::ApplicationDescription& desc)
      : URI(desc.URI)
      , ProductURI(desc.ProductURI)
      , Name(desc.Name.Text) // NOTE: encoding mask doesn't check.
      , Type(static_cast<unsigned>(desc.Type))
      , GatewayServerURI(desc.GatewayServerURI)
      , DiscoveryProfileURI(desc.DiscoveryProfileURI)
      , DiscoveryURLs(ToList(desc.DiscoveryURLs))
    {
    }
  };

  boost::python::list ToList(const std::vector<ApplicationDescription> nativeApps)
  {
    boost::python::list resultApps;
    std::for_each(
      nativeApps.begin(),
      nativeApps.end(),
      [&resultApps](const ApplicationDescription& app)
      {
        resultApps.append(PyApplicationDescription(app));
      });

    return resultApps;
  }

  struct PyUserTokenPolicy
  {
    std::string PolicyID;
    unsigned TokenType;
    std::string IssuedTokenType;
    std::string IssuerEndpointURL;
    std::string SecurityPolicyURI;

    PyUserTokenPolicy()
      : TokenType(0)
    {
    }

    PyUserTokenPolicy(const UserTokenPolicy& policy)
      : PolicyID(policy.PolicyID)
      , TokenType(static_cast<unsigned>(policy.TokenType))
      , IssuedTokenType(policy.IssuedTokenType)
      , IssuerEndpointURL(policy.IssuerEndpointURL)
      , SecurityPolicyURI(policy.SecurityPolicyURI)
    {
    }
  };

  boost::python::list ToList(const std::vector<OpcUa::UserTokenPolicy> policies)
  {
    boost::python::list result;
    std::for_each(
      policies.begin(),
      policies.end(),
      [&result](const UserTokenPolicy& policy)
      {
        result.append(PyUserTokenPolicy(policy));
      }
    );
    return result;
  }

  struct PyEndpointDescription
  {
    std::string EndpointURL;
    PyApplicationDescription ServerDescription;
    //CertificateData ServerCertificate; TODO
    unsigned SecurityMode;
    std::string SecurityPolicyURI;
    python::list UserIdentifyTokens;
    std::string TransportProfileURI;
    unsigned SecurityLevel;

    PyEndpointDescription()
      : SecurityMode(0)
      , SecurityLevel(0)
    {
    }

    explicit PyEndpointDescription(const EndpointDescription& endpoint)
      : EndpointURL(endpoint.EndpointURL)
      , ServerDescription(endpoint.ServerDescription)
      // , ServerCertificate(endpoint.ServerCertificate)
      , SecurityMode(endpoint.SecurityMode)
      , SecurityPolicyURI(endpoint.SecurityPolicyURI)
      , UserIdentifyTokens(ToList(endpoint.UserIdentifyTokens))
      , TransportProfileURI(endpoint.TransportProfileURI)
      , SecurityLevel(endpoint.SecurityLevel)
    {
    }
  };

  boost::python::list ToList(const std::vector<EndpointDescription> endpoints)
  {
    boost::python::list resultApps;
    std::for_each(
      endpoints.begin(),
      endpoints.end(),
      [&resultApps](const EndpointDescription& endpoint)
      {
        resultApps.append(PyEndpointDescription(endpoint));
      });

    return resultApps;
  }

  struct PyBrowseParameters
  {
    unsigned MaxReferenciesCount;
    PyNodeID NodeToBrowse;
    unsigned Direction;
    PyNodeID ReferenceTypeID;
    bool IncludeSubtypes;
    unsigned NodeClasses;
    unsigned ResultMask;

    PyBrowseParameters()
      : MaxReferenciesCount(0)
      , Direction(0)
      , IncludeSubtypes(false)
      , NodeClasses(0)
      , ResultMask(0)
    {
    }
  };


  typedef OpcUa::QualifiedName PyQualifiedName;

  struct PyReferenceDescription
  {
    PyNodeID ReferenceTypeID;
    bool IsForward;
    PyNodeID TargetNodeID;
    PyQualifiedName BrowseName;
    std::string DisplayName;
    unsigned TargetNodeClass;
    PyNodeID TargetNodeTypeDefinition;

    PyReferenceDescription()
      : IsForward(false)
      , TargetNodeClass(0)
    {
    }

    explicit PyReferenceDescription(const OpcUa::ReferenceDescription& desc)
      : ReferenceTypeID(desc.ReferenceTypeID)
      , IsForward(desc.IsForward)
      , TargetNodeID(desc.TargetNodeID)
      , BrowseName(desc.BrowseName)
      , DisplayName(desc.DisplayName.Text)
      , TargetNodeClass(static_cast<unsigned>(desc.TargetNodeClass))
      , TargetNodeTypeDefinition(desc.TargetNodeTypeDefinition)
    {
    }
  };

  boost::python::list ToList(const std::vector<ReferenceDescription> descs)
  {
    boost::python::list result;
    std::for_each(
      descs.begin(),
      descs.end(),
      [&result](const ReferenceDescription& desc)
      {
        result.append(PyReferenceDescription(desc));
      });

    return result;
  }

  struct PyAttributeValueID
  {
    PyNodeID Node;
    unsigned Attribute; // type AttributeID
    std::string IndexRange;
    PyQualifiedName DataEncoding;
  };

  struct PyReadParameters
  {
    double MaxAge;
    TimestampsToReturn TimestampsType;
    python::list AttributesToRead; // type of elmnts is AttributeValueID
  };

  struct VariantToObjectConverter
  {
    python::object Result;

    template <typename T>
    void Visit(const std::vector<T>& values)
    {
      if (values.empty())
      {
        return;
      }

      if (values.size() == 1)
      {
        Result = python::object(values[0]);
        return;
      }

      Result = ToList(values);
    }
  };

  python::object ToObject(const OpcUa::Variant& var)
  {
    if (var.IsNul())
    {
      return python::object();
    }
    VariantToObjectConverter convertor;
    OpcUa::ApplyVisitor(var, convertor);
    return convertor.Result;
  }
  OpcUa::NodeID GetNode(const PyNodeID& object)
  {
    OpcUa::NodeID id;
    unsigned encoding = 0;
    if (!object.NamespaceURI.empty())
    {
      id.NamespaceURI = object.NamespaceURI;
      encoding |= NodeIDEncoding::EV_NAMESPACE_URI_FLAG;
    }

    if (object.ServerIndex)
    {
      id.ServerIndex = object.ServerIndex;
      encoding |= NodeIDEncoding::EV_SERVER_INDEX_FLAG;
    }

    if (python::extract<std::string>(object.Identifier).check())
    {
      id.StringData.Identifier = python::extract<std::string>(object.Identifier);
      id.StringData.NamespaceIndex = object.NamespaceIndex;
      encoding |= NodeIDEncoding::EV_STRING;
    }
    if (python::extract<unsigned>(object.Identifier).check())
    {
      id.NumericData.Identifier = python::extract<unsigned>(object.Identifier);
      id.NumericData.NamespaceIndex = object.NamespaceIndex;
      encoding |= NodeIDEncoding::EV_NUMERIC;
    }
    id.Encoding = static_cast<OpcUa::NodeIDEncoding>(encoding);
    return id;
  }

  Variant FromObject(const python::object object)
  {
    //FIXME: this codes seems broken. it checks if the object is element and then handle it as a list ?!?!
    Variant var;
    if (python::extract<std::string>(object).check())
    {
      var = python::extract<std::string>(object)();
    }
    else if (python::extract<uint64_t>(object).check())
    {
      var = FromList<uint64_t>(object);
    }
    else if (python::extract<float>(object).check())
    {
      var = FromList<float>(object);
    }
    else if (python::extract<double>(object).check())
    {
      var = FromList<double>(object);
    }
    else if (python::extract<PyNodeID>(object).check())
    {
      std::vector<PyNodeID> ids = FromList<PyNodeID>(object);
      std::vector<NodeID> result(ids.size());
      std::transform(ids.begin(), ids.end(), result.begin(), GetNode);
      var = result;
    }
    else if (python::extract<int>(object[0]).check())
    {
      var = FromList<int>(object);
    }
    else if (python::extract<double>(object[0]).check())
    {
      var = FromList<double>(object);
    }
    else
    {
      throw std::logic_error("Cannot create variant from python object. Unsupported type.");
    }
    return var;
  }

  struct PyDataValue
  {
    python::object Value;
    unsigned Status;
    uint64_t SourceTimestamp;
    uint16_t SourcePicoseconds;
    uint64_t ServerTimestamp;
    uint16_t ServerPicoseconds;

    PyDataValue()
      : Status(0)
      , SourceTimestamp(0)
      , SourcePicoseconds(0)
      , ServerTimestamp(0)
      , ServerPicoseconds(0)
    {
    }

    explicit PyDataValue(const OpcUa::DataValue& value)
    {
      if (value.Encoding & DATA_VALUE)
      {
        Value = ToObject(value.Value);
      }
      if (value.Encoding & DATA_VALUE_STATUS_CODE)
      {
        Status = static_cast<unsigned>(value.Status);
      }
      if (value.Encoding & DATA_VALUE_SOURCE_TIMESTAMP)
      {
        SourceTimestamp = value.SourceTimestamp.Value;
      }
      if (value.Encoding & DATA_VALUE_SERVER_TIMESTAMP)
      {
        ServerTimestamp = value.ServerTimestamp.Value;
      }
      if (value.Encoding & DATA_VALUE_SOURCE_PICOSECONDS)
      {
        SourcePicoseconds = value.SourcePicoseconds;
      }
      if (value.Encoding & DATA_VALUE_SERVER_PICOSECONDS)
      {
        ServerPicoseconds = value.ServerPicoseconds;
      }
    }
  };

  struct PyWriteValue
  {
    PyNodeID Node;
    unsigned Attribute;
    std::string NumericRange;
    PyDataValue Data;

    PyWriteValue()
      : Attribute(0)
    {
    }

    explicit PyWriteValue(const OpcUa::WriteValue& value)
      : Node(value.Node)
      , Attribute(static_cast<unsigned>(value.Attribute))
      , NumericRange(value.NumericRange)
      , Data(value.Data)
    {
    }
  };

  WriteValue GetWriteValue(const PyWriteValue& pyValue)
  {
    WriteValue result;
    result.Attribute = static_cast<AttributeID>(pyValue.Attribute);
    result.Node = GetNode(pyValue.Node);
    result.NumericRange = pyValue.NumericRange;
    if (pyValue.Data.Status)
    {
      result.Data.Status = static_cast<StatusCode>(pyValue.Data.Status);
      result.Data.Encoding |= DATA_VALUE_STATUS_CODE;
    }
    if (pyValue.Data.ServerPicoseconds)
    {
      result.Data.ServerPicoseconds = pyValue.Data.ServerPicoseconds;
      result.Data.Encoding |= DATA_VALUE_SERVER_PICOSECONDS;
    }
    if (pyValue.Data.SourcePicoseconds)
    {
      result.Data.SourcePicoseconds = pyValue.Data.SourcePicoseconds;
      result.Data.Encoding |= DATA_VALUE_SOURCE_PICOSECONDS;
    }
    if (pyValue.Data.ServerTimestamp)
    {
      result.Data.ServerTimestamp.Value = pyValue.Data.ServerTimestamp;
      result.Data.Encoding |= DATA_VALUE_SERVER_TIMESTAMP;
    }
    if (pyValue.Data.SourceTimestamp)
    {
      result.Data.SourceTimestamp.Value = pyValue.Data.SourceTimestamp;
      result.Data.Encoding |= DATA_VALUE_SOURCE_TIMESTAMP;
    }
    if (pyValue.Data.Value)
    {
      result.Data.Value = FromObject(pyValue.Data.Value);
      result.Data.Encoding |= DATA_VALUE;
    }
    return result;
  }

  class PyComputer
  {
  public:
    explicit PyComputer(const std::string& endpointUrl)
      : Impl(OpcUa::Remote::Connect(endpointUrl))
    {
    }
/*
    python::list FindServers() const
    {
      const OpcUa::FindServersParameters params;
      const std::vector<ApplicationDescription> nativeApps = Impl->Endpoints()->FindServers(params);
      return ToList(nativeApps);
    }

    python::list GetEndpoints() const
    {
      const EndpointsFilter filter;
      const std::vector<EndpointDescription> endpoints = Impl->Endpoints()->GetEndpoints(filter);
      return ToList(endpoints);
    }
*/
    python::list Browse(const PyBrowseParameters& p) const
    {
      OpcUa::BrowseDescription description;
      description.NodeToBrowse = GetNode(p.NodeToBrowse);
      description.ReferenceTypeID = GetNode(p.ReferenceTypeID);
      description.Direction = static_cast<OpcUa::BrowseDirection>(p.Direction);
      description.IncludeSubtypes = p.IncludeSubtypes;
      description.NodeClasses = p.NodeClasses;
      description.ResultMask = p.ResultMask;

      OpcUa::NodesQuery query;
      query.NodesToBrowse.push_back(description);
      query.MaxReferenciesPerNode = p.MaxReferenciesCount;
      const std::vector<ReferenceDescription> references = Impl->Views()->Browse(query);
      return ToList(references);
    }

    python::list Read(const PyReadParameters& in)
    {
      OpcUa::ReadParameters params;
      params.MaxAge = in.MaxAge;
      params.TimestampsType = static_cast<TimestampsToReturn>(in.TimestampsType);

      std::size_t listSize = python::len(in.AttributesToRead);
      for (std::size_t i = 0; i < listSize; ++i)
      {
        const PyAttributeValueID& value = python::extract<PyAttributeValueID>(in.AttributesToRead[i]);
        OpcUa::AttributeValueID attr;
        attr.Attribute = static_cast<OpcUa::AttributeID>(value.Attribute);
        attr.DataEncoding.NamespaceIndex = value.DataEncoding.NamespaceIndex;
        attr.DataEncoding.Name = value.DataEncoding.Name;
        attr.IndexRange = value.IndexRange;
        attr.Node = GetNode(value.Node);
        params.AttributesToRead.push_back(attr);
      }

      std::vector<DataValue> data = Impl->Attributes()->Read(params);
      return ToList<PyDataValue, DataValue>(data);
    }

    //    std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter) = 0;
    python::list Write(const python::list& in)
    {
      const std::vector<PyWriteValue>& pyValues = FromList<PyWriteValue>(in);
      std::vector<WriteValue> values;
      for (std::vector<PyWriteValue>::const_iterator valueIt = pyValues.begin(); valueIt != pyValues.end(); ++valueIt)
      {
        const PyWriteValue& pyValue = *valueIt;
        const WriteValue& value = GetWriteValue(pyValue);
        values.push_back(value);
      }
      const python::list& result = ToList<unsigned, StatusCode>(Impl->Attributes()->Write(values));
      return result;
    }

  private:
    OpcUa::Remote::Computer::SharedPtr Impl;
  };

  void RegisterCommonObjectIDs()
  {
    python::enum_<OpcUa::ObjectID>("ObjectID")
      .value("NULL", OpcUa::ObjectID::Null)
      .value("BOOLEAN", OpcUa::ObjectID::Boolean)
      .value("SBYTE", OpcUa::ObjectID::SByte)
      .value("BYTE", OpcUa::ObjectID::Byte)
      .value("INT16", OpcUa::ObjectID::Int16)
      .value("UINT16", OpcUa::ObjectID::UInt16)
      .value("INT32", OpcUa::ObjectID::Int32)
      .value("UINT32", OpcUa::ObjectID::UInt32)
      .value("INT64", OpcUa::ObjectID::Int64)
      .value("UINT64", OpcUa::ObjectID::UInt64)
      .value("FLOAT", OpcUa::ObjectID::Float)
      .value("DOUBLE", OpcUa::ObjectID::Double)
      .value("STRING", OpcUa::ObjectID::String)
      .value("DATE_TIME", OpcUa::ObjectID::DateTime)
      .value("GUID", OpcUa::ObjectID::Guid)
      .value("BYTE_STRING", OpcUa::ObjectID::ByteString)
      .value("XML_ELEMENT", OpcUa::ObjectID::XmlElement)
      .value("NODE_ID", OpcUa::ObjectID::NodeID)
      .value("EXPANDED_NODE_ID", OpcUa::ObjectID::ExpandedNodeID)
      .value("STATUS_CODE", OpcUa::ObjectID::StatusCode)
      .value("QUALIFIED_NAME", OpcUa::ObjectID::QualifiedName)
      .value("QUALIFIED_NAME", OpcUa::ObjectID::QualifiedName)
      .value("LOCALIZED_TEXT", OpcUa::ObjectID::LocalizedText)
      .value("STRUCTURE", OpcUa::ObjectID::Structure)
      .value("DATA_VALUE", OpcUa::ObjectID::DataValue)
      .value("BASE_DATA_TYPE", OpcUa::ObjectID::BaseDataType)
      .value("DIAGNOSTIC_INFO", OpcUa::ObjectID::DiagnosticInfo)
      .value("NUMBER", OpcUa::ObjectID::Number)
      .value("INTEGER", OpcUa::ObjectID::Integer)
      .value("UINTEGER", OpcUa::ObjectID::UInteger)
      .value("ENUMERATION", OpcUa::ObjectID::Enumeration)
      .value("IMAGE", OpcUa::ObjectID::Image)
      .value("REFERENCES", OpcUa::ObjectID::References)
      .value("NON_HIERARCHAL_REFERENCES", OpcUa::ObjectID::NonHierarchicalReferences)
      .value("HAS_CHILD", OpcUa::ObjectID::HasChild)
      .value("ORGANIZES", OpcUa::ObjectID::Organizes)
      .value("HAS_EVENT_SOURCE", OpcUa::ObjectID::HasEventSource)
      .value("HAS_MODELLING_RULE", OpcUa::ObjectID::HasModellingRule)
      .value("HAS_ENCODING", OpcUa::ObjectID::HasEncoding)
      .value("HAS_DESCRIPTION", OpcUa::ObjectID::HasDescription)
      .value("HAS_TYPE_DEFINITION", OpcUa::ObjectID::HasTypeDefinition)
      .value("GENERATES_EVENT", OpcUa::ObjectID::GeneratesEvent)
      .value("AGGREGATES", OpcUa::ObjectID::Aggregates)
      .value("HAS_SUBTYPE", OpcUa::ObjectID::HasSubtype)
      .value("HAS_PROPERTY", OpcUa::ObjectID::HasProperty)
      .value("HAS_COMPONENT", OpcUa::ObjectID::HasComponent)
      .value("HAS_NOTIFIER", OpcUa::ObjectID::HasNotifier)
      .value("HAS_ORDERED_COMPONENT", OpcUa::ObjectID::HasOrderedComponent)
      .value("HAS_MODEL_PARENT", OpcUa::ObjectID::HasModelParent)
      .value("FROM_STATE", OpcUa::ObjectID::FromState)
      .value("TO_STATE", OpcUa::ObjectID::ToState)
      .value("HAS_CAUSE", OpcUa::ObjectID::HasCause)
      .value("HAS_EFFECT", OpcUa::ObjectID::HasEffect)
      .value("HAS_HISTORICAL_CONFIGURATION", OpcUa::ObjectID::HasHistoricalConfiguration)
      .value("HAS_HISTORICAL_EVENT_CONFIGURATION", OpcUa::ObjectID::HasHistoricalEventConfiguration)
      .value("BASE_OBJECT_TYPE", OpcUa::ObjectID::BaseObjectType)
      .value("FOLDER_TYPE", OpcUa::ObjectID::FolderType)
      .value("BASE_VARIABLE_TYPE", OpcUa::ObjectID::BaseVariableType)
      .value("BASE_DATA_VARIABLE_TYPE", OpcUa::ObjectID::BaseDataVariableType)
      .value("PROPERTY_TYPE", OpcUa::ObjectID::PropertyType)
      .value("DATA_TYPE_DESCRIPTION_TYPE", OpcUa::ObjectID::DataTypeDescriptionType)
      .value("DATA_TYPE_DICTIONARY_TYPE", OpcUa::ObjectID::DataTypeDictionaryType)
      .value("DATA_TYPE_SYSTEM_TYPE", OpcUa::ObjectID::DataTypeSystemType)
      .value("DATA_TYPE_ENCODING_TYPE", OpcUa::ObjectID::DataTypeEncodingType)
      .value("MODELLING_RULE_TYPE", OpcUa::ObjectID::ModellingRuleType)
      .value("MODELLING_RULE_MANDATORY", OpcUa::ObjectID::ModellingRuleMandatory)
      .value("MODELLING_RULE_MANDATORY_SHARED", OpcUa::ObjectID::ModellingRuleMandatoryShared)
      .value("MODELLING_RULE_OPTIONAL", OpcUa::ObjectID::ModellingRuleOptional)
      .value("MODELLING_RULE_CARDINALITY_RESTRICTION", OpcUa::ObjectID::ModellingRuleCardinalityRestriction)
      .value("MODELLING_RULE_EXPOSES_ITS_ARRAY", OpcUa::ObjectID::ModellingRuleExposesItsArray)
      .value("ROOT_FOLDER", OpcUa::ObjectID::RootFolder)
      .value("OBJECTS_FOLDER", OpcUa::ObjectID::ObjectsFolder)
      .value("TYPES_FOLDER", OpcUa::ObjectID::TypesFolder)
      .value("VIEWS_FOLDER", OpcUa::ObjectID::ViewsFolder)
      .value("OBJECT_TYPES", OpcUa::ObjectID::ObjectTypes)
      .value("VARIABLE_TYPES", OpcUa::ObjectID::VariableTypes)
      .value("DATA_TYPES", OpcUa::ObjectID::DataTypes)
      .value("REFERENCE_TYPES", OpcUa::ObjectID::ReferenceTypes)
      .value("NAMING_RULE", OpcUa::ObjectID::NamingRule)
      .value("HAS_SUBSTATE_MACHINE", OpcUa::ObjectID::HasSubStateMachine)
      .value("HAS_EVENT_HISTORY", OpcUa::ObjectID::HasEventHistory)
      .value("ID_TYPE", OpcUa::ObjectID::IdType)
      .value("NODE_CLASS", OpcUa::ObjectID::NodeClass)
      .value("DURATION", OpcUa::ObjectID::Duration)
      .value("NUMERIC_RANGE", OpcUa::ObjectID::NumericRange)
      .value("UTC_TIME", OpcUa::ObjectID::UtcTime)
      .value("LOCALE_ID", OpcUa::ObjectID::LocaleID)
      .value("STRUCTURE_ARGUMENT", OpcUa::ObjectID::StructureArgument)
;
/*




      StructureStatusResult
      MessageSecurityMode
      StructureApplicationDescription
      SecurityTokenRequestType
      StructureUserIdentifyToken
      AnonymousIdentifyToken
      UserNameIdentifyToken
      X509IdentifyToken
      StructureBuildInfo
      SoftwareCertificate
      StructureSignedSoftwareCertificate
      StructureAddNodesItem
      StructureAddReferencesItem
      StructureDeleteNodesItem
      StructureDeleteReferencesItem
      RedundancySupport
      ServerState
      StructureSamplingIntervalDiagnosticsDataType
      StructureServerDiagnosticsSummaryType
      StructureServerStatusDataType
      StructureSessionDiagnosticsDataType
      StructureSessionSecurityDiagnosticsDataType
      StructureServiceCounterDataType
      StructureSubscriptionDiagnosticsDataType
      StructureModelChangeStructureDataType
      StructureRange
      StructureEUInformation
      StructureSemanticChangeStructureDataType
      ImageBmp
      ImageGif
      ImageJpg
      ImagePng
      ServerType
      ServerArray
      NamespaceArray
      ServerStatus
      ServiceLevel
      ServerCapabilities
      ServerDiagnostics
      VendorServerInfo
      ServerRedundancy
      ServerCapabilitiesType
      ServerProfileArray
      LocaleIDArray
      MinSupportedSampleRate
      ModellingRules
      ServerDiagnosticsType
      ServerDiagnosticsSummary
      SamplingIntervalDiagnosticsArray
      SubscriptionDiagnosticsArray
      EnableFlag
      SessionDiagnosticsSummaryType
      SessionDiagnosticsObjectType
      VendorServerInfoType
      ServerRedundancyType
      RedundancySupportTypeRedundancySupport
      BaseEventType
      EventID
      EventType
      SourceNode
      SourceName
      Time
      ReceiveTime
      Message
      Severity
      SystemEventType
      DeviceFailureEventType
      BaseModelChangeEventType
      ServerVendorCapabilityType
      ServerStatusType
      ServerDiagnosticsSummaryType
      SamplingIntervalDiagnosticsArrayType
      SamplingIntervalDiagnosticsType
      SubscriptionDiagnosticsArrayType
      SubscriptionDiagnosticsType
      SessionsDiagnosticsArrayType
      ServerDiagnosticsVariableType
      SessionSecurityDiagnosticsArrayType
      SessionSecurityDiagnosticsType
      StateType
      StateNumber
      TransitionType
      HistoricalEventConfigurationType
      MaxBrowseContinuationPoints
      MaxHistoryContinuationPoints
      SemanticChangeEventType
      Auditing
      SessionsDiagnosticsSummary
      AggregateFunctions
      RefreshStartEventType
      RefreshEndEventType
      RefreshRequiredEventType
      EventTypes
      SoftwareCertificates
      AlwaysGeneratesEvent
      StartTime
      CurrentTime
      State
      BuildInfo
      ProductURI
      ManufacturerName
      ProductName
      SoftwareVersion
      BuildNumber
      BuildDate
      SecondsTillShutdown
      ShutdownReason
      LocalTime
      IdTypeEnumStrings
      MessageSecurityModeEnumStrings
      SecurityTokenRequestTypeEnumStrings
      RedundancySupportEnumStrings
      ServerStateEnumStrings
      HasTrueSubState
      HasFalseSubState
      HasCondition
*/
    ;
  }


  class PyNode: public Node
  {
    public:
      PyNode(OpcUa::Remote::Computer::SharedPtr server, NodeID nodeid) : Node(server, nodeid) {}
      PyNode (const Node& other): Node( other.GetServer(), other.GetNodeId()) {}
      //PyNode static FromNode(const Node& other) { return PyNode(other.GetServer(), other.GetNodeId()); }
      python::object PyReadValue() { return ToObject(Node::ReadValue()); }
      python::object PyReadBrowseName() { return ToObject(Node::ReadBrowseName()); }
      python::object PyWriteValue(python::object val) 
      { 
        OpcUa::StatusCode code = Node::WriteValue(FromObject(val)); 
        return ToObject(code); 
      }
      python::list PyBrowse()
      {
        python::list result;
        for (Node n: Node::Browse())
        {
          result.append(PyNode(n));
        }
        return result;
      }
      PyNode PyGetChildNode(python::object path) 
      {
        Node n = Node::GetChildNode(FromList<std::string>(path));
        return PyNode(n);
      }
  };

  class PyClient: public Client::Client
  {
    public:
      PyNode PyGetRootNode() { return PyNode(server, OpcUa::ObjectID::RootFolder); }
      PyNode PyGetObjectsNode() { return PyNode(server, OpcUa::ObjectID::ObjectsFolder); }
      PyNode PyGetNode(NodeID nodeid) { return PyNode(Client::Client::GetNode(nodeid)); }
      //PyNode PyGetNodeFromPath(const python::object& path) { return Client::Client::GetNodeFromPath(FromList<std::string>(path)); }
  };



  class PyOPCUAServer: public OPCUAServer
  {
    public:
      PyNode PyGetRootNode() { return PyNode(server, OpcUa::ObjectID::RootFolder); }
      PyNode PyGetObjectsNode() { return PyNode(server, OpcUa::ObjectID::ObjectsFolder); }
      //PyNode GetNode(NodeID nodeid) { return PyNode::FromNode(OPCUAServer::GetNode(nodeid)); }
      PyNode PyGetNode(NodeID nodeid) { return PyNode(OPCUAServer::GetNode(nodeid)); }
      PyNode PyGetNodeFromPath(const python::object& path) { return OPCUAServer::GetNodeFromPath(FromList<std::string>(path)); }
  };
}


BOOST_PYTHON_MODULE(libopcua) // MODULE_NAME specifies via preprocessor in command line
{
  using namespace boost::python;
  using namespace OpcUa;

  RegisterCommonObjectIDs();

  enum_<OpcUa::ApplicationType>("ApplicationType")
    .value("CLIENT", OpcUa::ApplicationType::CLIENT)
    .value("CLIENT_AND_SERVER", OpcUa::ApplicationType::CLIENT_AND_SERVER)
    .value("DISCOVERY_SERVER", OpcUa::ApplicationType::DISCOVERY_SERVER)
    .value("SERVER", OpcUa::ApplicationType::SERVER);

  enum_<OpcUa::MessageSecurityMode>("MessageSecurityMode")
    .value("NONE", OpcUa::MessageSecurityMode::MSM_NONE)
    .value("SIGN", OpcUa::MessageSecurityMode::MSM_SIGN)
    .value("SIGN_AND_ENCRYPT", OpcUa::MessageSecurityMode::MSM_SIGN);

  enum_<OpcUa::UserIdentifyTokenType>("UserIdentifyTokenType")
    .value("ANONYMOUS", OpcUa::UserIdentifyTokenType::ANONYMOUS)
    .value("USERNAME", OpcUa::UserIdentifyTokenType::USERNAME)
    .value("CERTIFICATE", OpcUa::UserIdentifyTokenType::CERTIFICATE)
    .value("ISSUED_TOKEN", OpcUa::UserIdentifyTokenType::ISSUED_TOKEN);

  enum_<OpcUa::BrowseDirection>("BrowseDirection")
    .value("BOTH", OpcUa::BrowseDirection::Both)
    .value("FORWARD", OpcUa::BrowseDirection::Forward)
    .value("INVERSE", OpcUa::BrowseDirection::Inverse);

  enum_<OpcUa::NodeClass>("NodeClass")
    .value("ALL", OpcUa::NodeClass::All)
    .value("OBJECT", OpcUa::NodeClass::Object)
    .value("VARIABLE", OpcUa::NodeClass::Variable)
    .value("METHOD", OpcUa::NodeClass::Method)
    .value("OBJECT_TYPE", OpcUa::NodeClass::ObjectType)
    .value("VARIABLE_TYPE", OpcUa::NodeClass::VariableType)
    .value("REFERENCE_TYPE", OpcUa::NodeClass::ReferenceType)
    .value("DATA_TYPE", OpcUa::NodeClass::DataType)
    .value("VIEW", OpcUa::NodeClass::View);

  enum_<OpcUa::TimestampsToReturn>("TimestampsToReturn")
    .value("SOURCE", OpcUa::TimestampsToReturn::SOURCE)
    .value("SERVER", OpcUa::TimestampsToReturn::SERVER)
    .value("BOTH",   OpcUa::TimestampsToReturn::BOTH)
    .value("SERVER", OpcUa::TimestampsToReturn::NEITHER);

  enum_<OpcUa::AttributeID>("AttributeID")
    .value("ACCESS_LEVEL", OpcUa::AttributeID::ACCESS_LEVEL)
    .value("ARRAY_DIMENSIONS", OpcUa::AttributeID::ARRAY_DIMENSIONS)
    .value("BROWSE_NAME", OpcUa::AttributeID::BROWSE_NAME)
    .value("CONTAINS_NO_LOOPS", OpcUa::AttributeID::CONTAINS_NO_LOOPS)
    .value("DATA_TYPE", OpcUa::AttributeID::DATA_TYPE)
    .value("DESCRIPTION", OpcUa::AttributeID::DESCRIPTION)
    .value("DISPLAY_NAME", OpcUa::AttributeID::DISPLAY_NAME)
    .value("EVENT_NOTIFIER", OpcUa::AttributeID::EVENT_NOTIFIER)
    .value("EXECUTABLE", OpcUa::AttributeID::EXECUTABLE)
    .value("HISTORIZING", OpcUa::AttributeID::HISTORIZING)
    .value("INVERSE_NAME", OpcUa::AttributeID::INVERSE_NAME)
    .value("IS_ABSTRACT", OpcUa::AttributeID::IS_ABSTRACT)
    .value("MINIMUM_SAMPLING_INTERVAL", OpcUa::AttributeID::MINIMUM_SAMPLING_INTERVAL)
    .value("NODE_CLASS", OpcUa::AttributeID::NODE_CLASS)
    .value("NODE_ID", OpcUa::AttributeID::NODE_ID)
    .value("SYMMETRIC", OpcUa::AttributeID::SYMMETRIC)
    .value("UNKNOWN", OpcUa::AttributeID::UNKNOWN)
    .value("USER_ACCESS_LEVEL", OpcUa::AttributeID::USER_ACCESS_LEVEL)
    .value("USER_EXECUTABLE", OpcUa::AttributeID::USER_EXECUTABLE)
    .value("VALUE", OpcUa::AttributeID::VALUE)
    .value("VALUE_RANK", OpcUa::AttributeID::VALUE_RANK)
    .value("WRITE_MASK", OpcUa::AttributeID::WRITE_MASK);

  class_<PyNodeID>("NodeID")
    .def_readwrite("namespace_index", &PyNodeID::NamespaceIndex)
    .def_readwrite("identifier", &PyNodeID::Identifier)
    .def_readwrite("server_index", &PyNodeID::ServerIndex)
    .def_readwrite("namespace_uri", &PyNodeID::NamespaceURI);

  class_<PyApplicationDescription>("ApplicationDescription")
    .def_readwrite("uri", &PyApplicationDescription::URI)
    .def_readwrite("product_uri", &PyApplicationDescription::ProductURI)
    .def_readwrite("name", &PyApplicationDescription::Name)
    .def_readwrite("type", &PyApplicationDescription::Type)
    .def_readwrite("gateway_server_uri", &PyApplicationDescription::GatewayServerURI)
    .def_readwrite("discovery_profile_uri", &PyApplicationDescription::DiscoveryProfileURI)
    .def_readwrite("discovery_urls", &PyApplicationDescription::DiscoveryURLs);

  class_<PyEndpointDescription>("EndpointDescription")
    .def_readwrite("url", &PyEndpointDescription::EndpointURL)
    .def_readwrite("server_description", &PyEndpointDescription::ServerDescription)
    //.def_readwrite("certificate", &PyEndpointDescription::ServerCertificate)
    .def_readwrite("security_mode", &PyEndpointDescription::SecurityMode)
    .def_readwrite("security_policy_uri", &PyEndpointDescription::SecurityPolicyURI)
    .def_readwrite("user_identify_tokens", &PyEndpointDescription::UserIdentifyTokens)
    .def_readwrite("transport_profile_uri", &PyEndpointDescription::TransportProfileURI)
    .def_readwrite("security_level", &PyEndpointDescription::SecurityLevel);

  class_<PyUserTokenPolicy>("UserTokenPolicy")
    .def_readwrite("policy_id", &PyUserTokenPolicy::PolicyID)
    .def_readwrite("token_type", &PyUserTokenPolicy::TokenType)
    .def_readwrite("issued_token_type", &PyUserTokenPolicy::IssuedTokenType)
    .def_readwrite("issuer_endpoint_url", &PyUserTokenPolicy::IssuerEndpointURL)
    .def_readwrite("security_policy_uri", &PyUserTokenPolicy::SecurityPolicyURI);

  class_<PyBrowseParameters>("BrowseParameters")
    .def_readwrite("max_referencies_count", &PyBrowseParameters::MaxReferenciesCount)
    .def_readwrite("node_to_browse", &PyBrowseParameters::NodeToBrowse)
    .def_readwrite("direction", &PyBrowseParameters::Direction)
    .def_readwrite("reference_type_id", &PyBrowseParameters::ReferenceTypeID)
    .def_readwrite("include_subtypes", &PyBrowseParameters::IncludeSubtypes)
    .def_readwrite("node_classes", &PyBrowseParameters::NodeClasses)
    .def_readwrite("result_mask", &PyBrowseParameters::ResultMask);

  class_<PyReferenceDescription>("ReferenceDescription")
    .def_readwrite("reference_type_id", &PyReferenceDescription::ReferenceTypeID)
    .def_readwrite("is_forward", &PyReferenceDescription::IsForward)
    .def_readwrite("target_node_id", &PyReferenceDescription::TargetNodeID)
    .def_readwrite("browse_name", &PyReferenceDescription::BrowseName)
    .def_readwrite("display_name", &PyReferenceDescription::DisplayName)
    .def_readwrite("target_node_class", &PyReferenceDescription::TargetNodeClass)
    .def_readwrite("target_node_type_definition", &PyReferenceDescription::TargetNodeTypeDefinition);

  class_<PyQualifiedName>("QualifiedName")
    .def_readwrite("namespace_index", &PyQualifiedName::NamespaceIndex)
    .def_readwrite("name", &PyQualifiedName::Name);

  class_<PyReadParameters>("ReadParameters")
    .def_readwrite("max_age", &PyReadParameters::MaxAge)
    .def_readwrite("timestamps_to_return", &PyReadParameters::TimestampsType)
    .def_readwrite("attributes_to_read", &PyReadParameters::AttributesToRead);

  class_<PyAttributeValueID>("AttributeValueID", "Description of attribute value to read.")
    .def_readwrite("node", &PyAttributeValueID::Node)
    .def_readwrite("attribute", &PyAttributeValueID::Attribute)
    .def_readwrite("index_range", &PyAttributeValueID::IndexRange)
    .def_readwrite("data_encoding", &PyAttributeValueID::DataEncoding);

  class_<PyDataValue>("DataValue", "Parameters of read data.")
    .def_readwrite("value", &PyDataValue::Value)
    .def_readwrite("status", &PyDataValue::Status)
    .def_readwrite("source_timestamp", &PyDataValue::SourceTimestamp)
    .def_readwrite("source_picoseconds", &PyDataValue::SourcePicoseconds)
    .def_readwrite("server_timestamp", &PyDataValue::ServerTimestamp)
    .def_readwrite("server_picoseconds", &PyDataValue::ServerPicoseconds);

  class_<PyWriteValue>("WriteValue", "Parameters data for writing.")
    .def_readwrite("node", &PyWriteValue::Node)
    .def_readwrite("attribute", &PyWriteValue::Attribute)
    .def_readwrite("numeric_range", &PyWriteValue::NumericRange)
    .def_readwrite("data", &PyWriteValue::Data);

    class_<PyComputer>("Computer", "Interface for remote opc ua server.", init<std::string>())
    //.def("find_servers", &PyComputer::FindServers)
    //.def("get_endpoints", &PyComputer::GetEndpoints)
    .def("browse", &PyComputer::Browse)
    .def("read", &PyComputer::Read)
    .def("write", &PyComputer::Write);



    def("VariantToObject", ToObject);
    def("ObjectToVariant", FromObject);

    class_<Variant>("Variant")
        .def_readonly("value", &Variant::Value)
        .def_readonly("type", &Variant::Type)
        .def("is_null", &Variant::IsNul)
        //.def("get_type", &Variant::GetType)
      ;


    class_<NodeID>("NodeId" )
          .def("from_numeric", &NumericNodeID)
          .def("get_string_identifier", &NodeID::GetStringIdentifier)
          .def("get_namespace_index", &NodeID::GetNamespaceIndex)
      ;

    std::vector<Node> (Node::*NodeBrowse)() = &Node::Browse;
    Node (Node::*NodeGetChildNode)(const std::vector<std::string>&) = &Node::GetChildNode;

    class_<PyNode>("Node", init<Remote::Computer::SharedPtr, NodeID>())
          .def(init<Node>())
          .def("get_node_id", &PyNode::GetNodeId)
          .def("read", &PyNode::Read)
          .def("write", &PyNode::Write)
          .def("read_value", &PyNode::PyReadValue)
          .def("write_value", &PyNode::PyWriteValue)
          .def("get_properties", &PyNode::GetProperties)
          .def("get_variables", &PyNode::GetVariables)
          .def("get_browse_name", &PyNode::PyReadBrowseName)
          .def("browse", &PyNode::PyBrowse)
          .def("get_child", &PyNode::PyGetChildNode)
          .def("__str__", &PyNode::ToString)
          .def("__repr__", &PyNode::ToString)
      ;

    class_<std::vector<Node> >("NodeVector")
        .def(vector_indexing_suite<std::vector<Node> >())
    ;

    class_<std::vector<std::string> >("StringVector")
        .def(vector_indexing_suite<std::vector<std::string> >())
    ;


    class_<PyClient, boost::noncopyable>("Client")
          .def("connect", &PyClient::Connect)
          .def("disconnect", &PyClient::Disconnect)
          .def("get_root_node", &PyClient::PyGetRootNode)
          .def("get_objects_node", &PyClient::PyGetObjectsNode)
          .def("get_node", &PyClient::PyGetNode)
          .def("set_endpoint", &PyClient::SetEndpoint)
          .def("get_endpoint", &PyClient::GetEndpoint)
          .def("set_session_name", &PyClient::SetSessionName)
          .def("get_session_name", &PyClient::GetSessionName)
          .def("get_uri", &PyClient::GetURI)
          .def("set_uri", &PyClient::SetURI)
          .def("set_security_policy", &PyClient::SetSecurityPolicy)
          .def("get_security_policy", &PyClient::GetSecurityPolicy)
      ;


    //Node (OPCUAServer::*NodeFromPathString)(const std::vector<std::string>&) = &OPCUAServer::GetNodeFromPath;
    //Node (OPCUAServer::*NodeFromPathQN)(const std::vector<QualifiedName>&) = &OPCUAServer::GetNodeFromPath;

    class_<PyOPCUAServer, boost::noncopyable >("Server" )
          .def("start", &PyOPCUAServer::Start)
          .def("stop", &PyOPCUAServer::Stop)
          .def("get_root_node", &PyOPCUAServer::PyGetRootNode)
          .def("get_objects_node", &PyOPCUAServer::PyGetObjectsNode)
          .def("get_node", &PyOPCUAServer::PyGetNode)
          .def("get_node_from_path", &PyOPCUAServer::PyGetNodeFromPath)
          //.def("get_node_from_qn_path", NodeFromPathQN)
          .def("set_config_file", &PyOPCUAServer::SetConfigFile)
          .def("set_uri", &PyOPCUAServer::SetURI)
          .def("add_xml_address_space", &PyOPCUAServer::AddAddressSpace)
          .def("set_server_name", &PyOPCUAServer::SetServerName)
          .def("set_endpoint", &PyOPCUAServer::SetEndpoint)
      ;













}
