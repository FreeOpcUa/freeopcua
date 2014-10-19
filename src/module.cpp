/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/client/client.h>
#include <opc/ua/client/binary_server.h>
#include <opc/ua/node.h>
#include <opc/ua/event.h>
#include <opc/ua/server/opcuaserver.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/services/services.h>
#include <opc/ua/subscription.h>
#include <opc/ua/protocol/string_utils.h>

#include <boost/python.hpp>
#include <boost/python/type_id.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <functional>

namespace OpcUa
{

  using namespace boost;

  template <typename T>
  T Extract(const python::object& obj)
  {
    return python::extract<T>(obj)();
  }

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
  std::vector<T> ToVector(const python::object& list)
  {
    std::vector<T> result;
    std::size_t listSize = python::len(list);
    for (std::size_t i = 0; i < listSize; ++i)
    {
      const python::object& element = list[i];
      const T& value = Extract<T>(element);
      result.push_back(value);
    }
    return result;
  }


  struct PyNodeID
  {
  public:
    PyNodeID()
    {
//      std::cout << "PyNodeID()" << std::endl;
//      std::cout.flush();
    }

    explicit PyNodeID(const NodeID& node)
      : Id(node)
    {
//      std::cout << "PyNodeID(const NodeID& node = " << OpcUa::ToString(node) << ")" << std::endl;
//      std::cout.flush();
    }

    PyNodeID(unsigned integerId, unsigned index)
      : Id(integerId, index)
    {
//      std::cout << "PyNodeID(unsigned integerId = " << integerId << ", unsigned index = " << index << ")" << std::endl;
//      std::cout.flush();
    }

    PyNodeID(const std::string& stringId, uint16_t index)
      : Id(stringId, index)
    {
//      std::cout << "PyNodeID(const std::string& stringId = " << stringId << ", uint16_t index = " << index << ")" << std::endl;
//      std::cout.flush();
    }

    explicit PyNodeID(const std::string& encodedNodeID)
      : Id(OpcUa::ToNodeID(encodedNodeID))
    {
//      std::cout << "PyNodeID(const std::string& encodedNodeID = encodedNodeID" << encodedNodeID << ")" << std::endl;
//      std::cout.flush();
    }

    uint32_t GetNamespaceIndex() const
    {
      return Id.GetNamespaceIndex();
    }

    python::object GetIdentifier() const
    {
      if (Id.IsInteger())
      {
        return python::object(Id.GetIntegerIdentifier());
      }
      else if ( Id.IsString() )
      {
        return python::object(Id.GetStringIdentifier());
      }
      else if ( Id.IsGuid() )
      {
        return python::object(Id.GetGuidIdentifier());
      }
      else if ( Id.IsBinary() )
      {
        return python::object(Id.GetBinaryIdentifier());
      }
      else
      {
        throw std::logic_error("Error unknown identifier.");
      }
    }

    unsigned GetEncodingValue() const
    {
      return Id.Encoding;
    }

    bool IsInteger() const
    {
      return Id.IsInteger();
    }

    bool IsBinary() const
    {
      return Id.IsBinary();
    }

    bool IsGuid() const
    {
      return Id.IsGuid();
    }

    bool IsString() const
    {
      return Id.IsString();
    }

    std::string GetNamespaceURI() const
    {
      return Id.NamespaceURI;
    }

    OpcUa::NodeID GetID() const
    {
      return Id;
    }

    bool operator==(const PyNodeID& id) const
    {
      return Id == id.Id;
    }

  private:
    OpcUa::NodeID Id;
  };

  std::ostream& operator<< (std::ostream& out, const PyNodeID& pyId)
  {
    out << pyId.GetID();
    return out;
  }

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


  struct VariantToPythonObjectConverter
  {
    typedef python::object result_type;

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == true, result_type>::type operator()(const T& val)
    {
      return ToList(val);
    }

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == false, result_type>::type operator()(const T& val)
    {
      return python::object(val);
    }
  };

  python::object ToObject(const OpcUa::Variant& var)
  {
    if (var.IsNul())
    {
      return python::object();
    }
    return var.Visit(VariantToPythonObjectConverter());
  }

  Variant ToVariant(const python::object& object)
  {
    Variant var;
    if (python::extract<std::string>(object).check())
    {
      var = python::extract<std::string>(object)();
    }
    else if ( python::extract<python::list>(object).check() )
    {
      python::list plist = (python::list) object;
      if ( python::len(object) == 0 )
      {
      }
      else
      {
        if (python::extract<int>(object[0]).check())
        {
          var = ToVector<int>(object);
        }
        else if (python::extract<double>(object[0]).check())
        {
          var = ToVector<double>(object);
        }
        else if (python::extract<std::string>(object[0]).check())
        {
          var = ToVector<std::string>(object);
        }
        else
        {
          throw std::logic_error("Cannot create variant from python list. Unsupported type.");
        }
      }
    }
    else if (python::extract<int>(object).check())
    {
      var = Extract<int>(object);
    }
    else if (python::extract<double>(object).check())
    {
      var = Extract<double>(object);
    }
    else if (python::extract<NodeID>(object).check())
    {
      var = ToVector<NodeID>(object);
    }
    else
    {
      throw std::logic_error("Cannot create variant from python object. Unsupported type.");
    }
    return var;
  }


  //similar to ToVariant but gives a hint to what c++ object type the python object should be converted to
  Variant ToVariant2(const python::object& object, VariantType vtype)
  {
    Variant var;

    if ( python::extract<python::list>(object).check() )
    {

      python::list plist = (python::list) object;
      if ( python::len(object) == 0 )
      {
        return var;
      }
      else
      {
        switch (vtype)
        {
          case VariantType::BOOLEAN:
            var = ToVector<bool>(object);
            return var;
          case VariantType::UINT32:
            var = ToVector<uint32_t>(object);
            return var;
          default:
            return ToVariant(object);
        }
      }
    }
    else
    {
      switch (vtype)
      {
        case VariantType::BOOLEAN:          
          var = python::extract<bool>(object)();
          return var;
        case VariantType::UINT16:         
        case VariantType::UINT32:          
          var = python::extract<uint32_t>(object)();
          return var;
        default:
          return ToVariant(object);
      }
    }
  }

  struct DataValueWrap : OpcUa::DataValue
  {

    DataValueWrap(const python::object& object)
    : OpcUa::DataValue(ToVariant(object))
    {}

    DataValueWrap(const python::object& object, VariantType vtype)
    : OpcUa::DataValue(ToVariant2(object, vtype))
    {}

    python::object get_value()
    { return ToObject(Value); }
    
    void set_value(const python::object& object, VariantType vtype)
    { Value = ToVariant2(object, vtype); Encoding |= DATA_VALUE; }
   
    StatusCode get_status()
    { return Status; }

    void set_status( const StatusCode & sc)
    { Status = sc; Encoding |= DATA_VALUE_STATUS_CODE; }

    DateTime get_source_timestamp()
    { return SourceTimestamp; }

    void set_source_timestamp(const DateTime & dt)
    { SourceTimestamp = dt; Encoding |= DATA_VALUE_SOURCE_TIMESTAMP; }

    uint16_t get_source_picoseconds()
    { return SourcePicoseconds; }

    void set_source_picoseconds(uint16_t ps)
    { SourcePicoseconds = ps; Encoding |= DATA_VALUE_SOURCE_PICOSECONDS; }

    DateTime get_server_timestamp()
    { return ServerTimestamp; }

    void set_server_timestamp(const DateTime & dt)
    { ServerTimestamp = dt; Encoding |= DATA_VALUE_SERVER_TIMESTAMP; }

    uint16_t get_server_picoseconds()
    { return ServerPicoseconds; }

    void set_server_picoseconds(uint16_t ps)
    { ServerPicoseconds = ps; Encoding |= DATA_VALUE_SERVER_PICOSECONDS; }

  };

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
    result.Node = pyValue.Node.GetID();
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
      result.Data.Value = ToVariant(pyValue.Data.Value);
      result.Data.Encoding |= DATA_VALUE;
    }
    return result;
  }


  struct PyVariant
  {
    python::object Value;
    VariantType Type = VariantType::NUL;
    bool IsNull = true;

    PyVariant() = default;

    explicit PyVariant(const OpcUa::Variant& value)
      : Value(ToObject(value))
      , Type(value.Type())
      , IsNull(value.IsNul())
    {
    }
  };

  class PyServer
  {
  public:
    explicit PyServer(const std::string& endpointUrl)
      : Impl(OpcUa::CreateBinaryServer(endpointUrl))
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
      description.NodeToBrowse = p.NodeToBrowse.GetID();
      description.ReferenceTypeID = p.ReferenceTypeID.GetID();
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
        attr.Node = value.Node.GetID();
        params.AttributesToRead.push_back(attr);
      }

      std::vector<DataValue> data = Impl->Attributes()->Read(params);
      return ToList<PyDataValue, DataValue>(data);
    }

    //    std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter) = 0;
    python::list Write(const python::list& in)
    {
      const std::vector<PyWriteValue>& pyValues = ToVector<PyWriteValue>(in);
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
    OpcUa::Services::SharedPtr Impl;
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
    PyNode(OpcUa::Services::SharedPtr srv, const NodeID& id)\
      : Node(srv, id)
    {
    }

    PyNode (const Node& other)
      : Node(other.GetServices(), other.GetId(), other.GetName())
    {
    }

    python::object PyGetValue()
    {
      return ToObject(Node::GetValue());
    }

    python::object PyGetName()
    {
      return ToObject(Node::GetName());
    }

    PyNodeID PyGetNodeID()
    {
      return PyNodeID(Node::GetId());
    }

    python::object PySetValue(const python::object& val, VariantType hint = VariantType::NUL, DateTime t = CurrentDateTime())
    {
      Variant var = ToVariant2(val, hint);
      OpcUa::StatusCode code = Node::SetValue(var, t);
      return ToObject(code);
    }

    python::object PySetDataValue(const DataValueWrap &dval)
    {
      OpcUa::StatusCode code = Node::SetValue(dval);
      return ToObject(code);
    }

    python::list PyGetChildren()
    {
      python::list result;
      for (Node n: Node::GetChildren())
      {
        result.append(PyNode(n));
      }
      return result;
    }

    PyNode PyGetChild(const python::object& path)
    {
      if (python::extract<std::string>(path).check())
      {
        Node n = Node::GetChild(python::extract<std::string>(path)());
        return PyNode(n);
      }
      else
      {
        Node n = Node::GetChild(ToVector<std::string>(path));
        return PyNode(n);
      }
    }

    PyNode PyAddFolder(const std::string& browsename)
    {
      return PyNode(Node::AddFolder(browsename));
    }

    PyNode PyAddFolder2(const std::string& nodeid, const std::string& browsename)
    {
      return PyNode(Node::AddFolder(nodeid, browsename));
    }

    PyNode PyAddFolder3(const PyNodeID& nodeid, const QualifiedName browsename)
    {
      return PyNode(Node::AddFolder(nodeid.GetID(), browsename));
    }

    PyNode PyAddObject(const std::string& browsename)
    {
      return PyNode(Node::AddObject(browsename));
    }

    PyNode PyAddObject2(const std::string& nodeid, const std::string& browsename)
    {
      return PyNode(Node::AddObject(OpcUa::ToNodeID(nodeid), OpcUa::ToQualifiedName(browsename, 0)));
    }

    PyNode PyAddObject3(const PyNodeID& nodeid, const QualifiedName& browsename)
    {
      return PyNode(Node::AddObject(nodeid.GetID(), browsename));
    }

    PyNode PyAddVariable(const std::string& browsename, const python::object& val)
    {
      return PyNode(Node::AddVariable(browsename, ToVariant(val)));
    }

    PyNode PyAddVariable2(const std::string& nodeid, const std::string& browsename, const python::object& val)
    {
      return PyNode(Node::AddVariable(nodeid, browsename, ToVariant(val)));
    }

    PyNode PyAddVariable3(const PyNodeID& nodeid, const QualifiedName& browsename, const python::object& val)
    {
      return PyNode(Node::AddVariable(nodeid.GetID(), browsename, ToVariant(val)));
    }

    PyNode PyAddProperty(const std::string& browsename, const python::object& val)
    {
      return PyNode(Node::AddProperty(browsename, ToVariant(val)));
    }

    PyNode PyAddProperty2(const std::string& nodeid, const std::string& browsename, const python::object& val)
    {
      return PyNode(Node::AddProperty(nodeid, browsename, ToVariant(val)));
    }

    PyNode PyAddProperty3(const PyNodeID& nodeid, const QualifiedName& browsename, const python::object& val)
    {
      return PyNode(Node::AddProperty(nodeid.GetID(), browsename, ToVariant(val)));
    }
  };

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PyNodeSetValue_stubs, PyNode::PySetValue, 1, 3);

  class PyEvent : public Event
  {
    public:
      using Event::Event;
      PyEvent(const Event& other) : Event(other) {}

      void PySetValue(const std::string& name, const python::object& val)
      {
        SetValue(name, ToVariant(val));
      }

      python::object PyGetValue(const std::string& name) 
      {
        return ToObject(GetValue(name));
      }

      void SetMessage(const std::string& name) 
      {
        Message = LocalizedText(name);
      }

      python::object GetMessage() 
      {
        std::cout << "Getting message: " << Message.Text << std::endl;
        return ToObject(Message.Text);
      }
       void SetSourceNode(NodeID node)
      {
        SourceNode = node;
      }

      PyNodeID GetSourceNode()
      {
        return PyNodeID(SourceNode);
      }

      python::object GetTime()
      {
        return ToObject(Time);
      };

      void SetTime(python::object datetime)
      {
        //FIXME: implement
      }

  };


std::string parse_python_exception(){
  PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
  // Fetch the exception info from the Python C API
  PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
  // Fallback error
  std::string ret("Unfetchable Python error");
  // If the fetch got a type pointer, parse the type into the exception string
  if(type_ptr != NULL){
    python::handle<> h_type(type_ptr);
    python::str type_pstr(h_type);
    // Extract the string from the boost::python object
    python::extract<std::string> e_type_pstr(type_pstr);
    // If a valid string extraction is available, use it
    // otherwise use fallback
    if(e_type_pstr.check())
      ret = e_type_pstr();
    else
      ret = "Unknown exception type";
  }
  // Do the same for the exception value (the stringification of the exception)
  if(value_ptr != NULL){
    python::handle<> h_val(value_ptr);
    python::str a(h_val);
    python::extract<std::string> returned(a);
    if(returned.check())
      ret += ": " + returned();
    else
      ret += std::string(": Unparseable Python error: ");
  }
  // Parse lines from the traceback using the Python traceback module
  if(traceback_ptr != NULL){
    python::handle<> h_tb(traceback_ptr);
    // Load the traceback module and the format_tb function
    python::object tb(python::import("traceback"));
    python::object fmt_tb(tb.attr("format_tb"));
    // Call format_tb to get a list of traceback strings
    python::object tb_list(fmt_tb(h_tb));
    // Join the traceback strings into a single string
    python::object tb_str(python::str("\n").join(tb_list));
    // Extract the string, check the extraction, and fallback in necessary
    python::extract<std::string> returned(tb_str);
    if(returned.check())
      ret += ": " + returned();
    else
      ret += std::string(": Unparseable Python traceback");
  }
  return ret;
}

    class PySubscriptionClient: public SubscriptionClient
  {
    public:
      PySubscriptionClient(PyObject* p) : self(p) {}
      //PySubscriptionClient(PyObject *p, const SubscriptionClient& x)  : SubscriptionClient(x), self(p) {} //copy construct

      void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeID attribute) const override
      {
        PyGILState_STATE state = PyGILState_Ensure();
        try {
          python::call_method<void>(self, "data_change", handle, PyNode(node), ToObject(val) , (uint32_t) attribute);
        }
        catch (const python::error_already_set& ex)
        {
          std::string perror_str = parse_python_exception();
          std::cout << "Error in Python: " << perror_str << std::endl;
        }
        PyGILState_Release(state);
      };

      static void DefaultDataChange(const SubscriptionClient& self_, uint32_t handle, const PyNode& node, const python::object& val, uint32_t attribute)  
      {
        std::cout << "No callback defined in python for DataChange" << std::endl;
      }

      void Event(uint32_t handle, const OpcUa::Event& event) const override
      {
        PyGILState_STATE state = PyGILState_Ensure();
        try {
          python::call_method<void>(self, "event", handle, PyEvent(event));
        }
        catch (const python::error_already_set& ex)
        {
          std::string perror_str = parse_python_exception();
          std::cout << "Error in Python: " << perror_str << std::endl;
        }
        PyGILState_Release(state);
      };

      static void DefaultEvent(const SubscriptionClient& self_, uint32_t handle, const OpcUa::PyEvent& event)  
      {
        std::cout << "No callback defined in python for Event" << std::endl;
      }

      void StatusChange(StatusCode status)  const override
      {
        PyGILState_STATE state = PyGILState_Ensure();
        try {
          python::call_method<void>(self, "status_change", status);
        }
        catch (const python::error_already_set& ex)
        {
          std::string perror_str = parse_python_exception();
          std::cout << "Error in Python: " << perror_str << std::endl;
        }
        PyGILState_Release(state);
      };

      static void DefaultStatusChange(const SubscriptionClient& self_, StatusCode status)  
      {
        std::cout << "No callback defined in python for StatusChange" << std::endl;
      }




    private:
      PyObject* const self;
  };

  class PySubscription
  {
    public:
      PySubscription (std::shared_ptr<Subscription> other): Sub(other) { } //converting to shared pointer, should be ok
      PySubscription () { throw std::runtime_error("Subscription cannot be instanciated from Python"); }
      void Delete() { Sub->Delete(); }
      uint32_t SubscribeDataChange(PyNode node) { return Sub->SubscribeDataChange(node, AttributeID::VALUE); }
      uint32_t SubscribeDataChange2(PyNode node, AttributeID attr) { return Sub->SubscribeDataChange(node, attr); }
      void UnSubscribe(uint32_t id) { return Sub->UnSubscribe(id); }
      uint32_t SubscribeEvents() { return Sub->SubscribeEvents(); }
      uint32_t SubscribeEvents2(const Node node, const Node& eventtype) { return Sub->SubscribeEvents(node, eventtype); }
      
    private:
      std::shared_ptr<Subscription> Sub;
  };

  class PyClient: public RemoteClient
  {
  public:
    using RemoteClient::RemoteClient;

    PyNode PyGetRootNode()
    {
      return PyNode(Server, OpcUa::ObjectID::RootFolder);
    }

    PyNode PyGetObjectsNode()
    {
      return PyNode(Server, OpcUa::ObjectID::ObjectsFolder);
    }

    PyNode PyGetServerNode()
    {
      return PyNode(Server, OpcUa::ObjectID::Server);
    }

    PyNode PyGetNode(PyNodeID nodeid)
    {
      return PyNode(RemoteClient::GetNode(nodeid.GetID()));
    }

    //PyNode PyGetNodeFromPath(const python::object& path) { return Client::Client::GetNodeFromPath(ToVector<std::string>(path)); }
    PySubscription CreateSubscription(uint period, PySubscriptionClient& callback)
    {
      return PySubscription(RemoteClient::CreateSubscription(period, callback));
    }
  };

  class PyOPCUAServer: public OPCUAServer
  {
  public:
    using OPCUAServer::OPCUAServer;

    PyNode PyGetRootNode() const
    {
      return PyNode(Registry->GetServer(), OpcUa::ObjectID::RootFolder);
    }

    PyNode PyGetObjectsNode() const
    {
      return PyNode(Registry->GetServer(), OpcUa::ObjectID::ObjectsFolder);
    }

    PyNode PyGetServerNode() const
    {
      return PyNode(Registry->GetServer(), OpcUa::ObjectID::Server);
    }

    PyNode PyGetNode(const PyNodeID& nodeid) const
    {
      return PyNode(OPCUAServer::GetNode(nodeid.GetID()));
    }
    PyNode PyGetNodeFromPath(const python::object& path) const
    {
      return OPCUAServer::GetNodeFromPath(ToVector<std::string>(path));
    }
    PySubscription CreateSubscription(uint period, PySubscriptionClient& callback)
    {
      return PySubscription(OPCUAServer::CreateSubscription(period, callback));
    }
  };
}


BOOST_PYTHON_MODULE(MODULE_NAME) // MODULE_NAME specifies via preprocessor in command line
{
  using namespace boost::python;
  using namespace OpcUa;

  using self_ns::str; //hack to enable __str__ in python classes with str(self)

  if ( ! PyEval_ThreadsInitialized() )
  {
    PyEval_InitThreads(); //Seems to be necessary for callback from another thread
  }

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

  class_<DateTime>("DateTime", init<>())
    .def(init<int64_t>())
  ;

  def("CurrentDateTime", &CurrentDateTime);
  def("ToDateTime", &ToDateTime);
  def("ToTimeT", &ToTimeT);

  class_<PyNodeID>("NodeID")
    .def(init<unsigned, unsigned>())
    .def(init<std::string, uint16_t>())
    .def(init<std::string>())
    .def_readonly("namespace_index", &PyNodeID::GetNamespaceIndex)
    .def_readonly("identifier", &PyNodeID::GetIdentifier)
    .def("get_encoding", &PyNodeID::GetEncodingValue)
    .def("is_integer", &PyNodeID::IsInteger)
    .def("is_binary", &PyNodeID::IsBinary)
    .def("is_guid", &PyNodeID::IsGuid)
    .def("is_string", &PyNodeID::IsString)
    .def_readonly("namespace_uri", &PyNodeID::GetNamespaceURI)
    .def(str(self))
    .def(repr(self))
    .def(self == self)
    ;

  class_<PyQualifiedName>("QualifiedName")
    .def(init<uint16_t, std::string>())
    .def(init<std::string, uint16_t>())
    .def("parse", &OpcUa::ToQualifiedName)
    .def_readwrite("namespace_index", &PyQualifiedName::NamespaceIndex)
    .def_readwrite("name", &PyQualifiedName::Name)
    .def(str(self))
    .def(repr(self))
    //.def(self_ns::str(self_ns::self))
    //.def("__str__", operator<<)
    .def(self == self)
    ;

  class_<DataValue>("DataValue", "Parameters of read data.") // XXX "DataValue"
    .def_readwrite("value", &DataValue::Value)
    .def_readwrite("status", &DataValue::Status)
    .def_readwrite("source_timestamp", &DataValue::SourceTimestamp)
    .def_readwrite("source_picoseconds", &DataValue::SourcePicoseconds)
    .def_readwrite("server_timestamp", &DataValue::ServerTimestamp)
    .def_readwrite("server_picoseconds", &DataValue::ServerPicoseconds);

  class_<DataValueWrap>("DataValueWrap", "Parameters of read data.", init<const python::object&>())
    .def(init<const python::object&, VariantType>())
    #define dvproperty(X) add_property( #X, &DataValueWrap::get_ ## X, &DataValueWrap::set_ ## X)
    .dvproperty(value)
    .dvproperty(status)
    .dvproperty(source_timestamp)
    .dvproperty(source_picoseconds)
    .dvproperty(server_timestamp)
    .dvproperty(server_picoseconds)
    #undef dvproperty
    ;

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
  class_<PyReadParameters>("ReadParameters")
    .def_readwrite("max_age", &PyReadParameters::MaxAge)
    .def_readwrite("timestamps_to_return", &PyReadParameters::TimestampsType)
    .def_readwrite("attributes_to_read", &PyReadParameters::AttributesToRead);

  class_<PyAttributeValueID>("AttributeValueID", "Description of attribute value to read.")
    .def_readwrite("node", &PyAttributeValueID::Node)
    .def_readwrite("attribute", &PyAttributeValueID::Attribute)
    .def_readwrite("index_range", &PyAttributeValueID::IndexRange)
    .def_readwrite("data_encoding", &PyAttributeValueID::DataEncoding);

  class_<PyDataValue>("DataValue", "Parameters of read data.") // XXX "DataValue"
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

  class_<PyVariant>("Variant")
      .def_readonly("value", &PyVariant::Value)
      .def_readonly("type", &PyVariant::Type)
      .def_readonly("is_null", &PyVariant::IsNull)
    ;

  enum_<StatusCode>("StatusCode")
#define scvalue(XXX) value(#XXX, StatusCode:: XXX )
    .scvalue(Good)
    .scvalue(BadUnexpectedError)
    .scvalue(BadInternalError)
    .scvalue(BadOutOfMemory)
    .scvalue(BadResourceUnavailable)
    .scvalue(BadCommunicationError)
    .scvalue(BadEncodingError)
    .scvalue(BadDecodingError)
    .scvalue(BadEncodingLimitsExceeded)
    .scvalue(BadUnknownResponse)
    .scvalue(BadTimeout)
    .scvalue(BadServiceUnsupported)
    .scvalue(BadShutdown)
    .scvalue(BadServerNotConnected)
    .scvalue(BadServerHalted)
    .scvalue(BadNothingToDo)
    .scvalue(BadTooManyOperations)
    .scvalue(BadDataTypeIdUnknown)
    .scvalue(BadCertificateInvalid)
    .scvalue(BadSecurityChecksFailed)
    .scvalue(BadCertificateTimeInvalid)
    .scvalue(BadCertificateIssuerTimeInvalid)
    .scvalue(BadCertificateHostNameInvalid)
    .scvalue(BadCertificateUriInvalid)
    .scvalue(BadCertificateUseNotAllowed)
    .scvalue(BadCertificateIssuerUseNotAllowed)
    .scvalue(BadCertificateUntrusted)
    .scvalue(BadCertificateRevocationUnknown)
    .scvalue(BadCertificateIssuerRevocationUnknown)
    .scvalue(BadCertificateRevoked)
    .scvalue(BadCertificateIssuerRevoked)
    .scvalue(BadUserAccessDenied)
    .scvalue(BadIdentityTokenInvalid)
    .scvalue(BadIdentityTokenRejected)
    .scvalue(BadSecureChannelIdInvalid)
    .scvalue(BadInvalidTimestamp)
    .scvalue(BadNonceInvalid)
    .scvalue(BadSessionIdInvalid)
    .scvalue(BadSessionClosed)
    .scvalue(BadSessionNotActivated)
    .scvalue(BadSubscriptionIdInvalid)
    .scvalue(BadRequestHeaderInvalid)
    .scvalue(BadTimestampsToReturnInvalid)
    .scvalue(BadRequestCancelledByClient)
    .scvalue(GoodSubscriptionTransferred)
    .scvalue(GoodCompletesAsynchronously)
    .scvalue(GoodOverload)
    .scvalue(GoodClamped)
    .scvalue(BadNoCommunication)
    .scvalue(BadWaitingForInitialData)
    .scvalue(BadNodeIdInvalid)
    .scvalue(BadNodeIdUnknown)
    .scvalue(BadAttributeIdInvalid)
    .scvalue(BadIndexRangeInvalid)
    .scvalue(BadIndexRangeNoData)
    .scvalue(BadDataEncodingInvalid)
    .scvalue(BadDataEncodingUnsupported)
    .scvalue(BadNotReadable)
    .scvalue(BadNotWritable)
    .scvalue(BadOutOfRange)
    .scvalue(BadNotSupported)
    .scvalue(BadNotFound)
    .scvalue(BadObjectDeleted)
    .scvalue(BadNotImplemented)
    .scvalue(BadMonitoringModeInvalid)
    .scvalue(BadMonitoredItemIdInvalid)
    .scvalue(BadMonitoredItemFilterInvalid)
    .scvalue(BadMonitoredItemFilterUnsupported)
    .scvalue(BadFilterNotAllowed)
    .scvalue(BadStructureMissing)
    .scvalue(BadEventFilterInvalid)
    .scvalue(BadContentFilterInvalid)
    .scvalue(BadFilterOperandInvalid)
    .scvalue(BadContinuationPointInvalid)
    .scvalue(BadNoContinuationPoints)
    .scvalue(BadReferenceTypeIdInvalid)
    .scvalue(BadBrowseDirectionInvalid)
    .scvalue(BadNodeNotInView)
    .scvalue(BadServerUriInvalid)
    .scvalue(BadServerNameMissing)
    .scvalue(BadDiscoveryUrlMissing)
    .scvalue(BadSempahoreFileMissing)
    .scvalue(BadRequestTypeInvalid)
    .scvalue(BadSecurityModeRejected)
    .scvalue(BadSecurityPolicyRejected)
    .scvalue(BadTooManySessions)
    .scvalue(BadUserSignatureInvalid)
    .scvalue(BadApplicationSignatureInvalid)
    .scvalue(BadNoValidCertificates)
    .scvalue(BadRequestCancelledByRequest)
    .scvalue(BadParentNodeIdInvalid)
    .scvalue(BadReferenceNotAllowed)
    .scvalue(BadNodeIdRejected)
    .scvalue(BadNodeIdExists)
    .scvalue(BadNodeClassInvalid)
    .scvalue(BadBrowseNameInvalid)
    .scvalue(BadBrowseNameDuplicated)
    .scvalue(BadNodeAttributesInvalid)
    .scvalue(BadTypeDefinitionInvalid)
    .scvalue(BadSourceNodeIdInvalid)
    .scvalue(BadTargetNodeIdInvalid)
    .scvalue(BadDuplicateReferenceNotAllowed)
    .scvalue(BadInvalidSelfReference)
    .scvalue(BadReferenceLocalOnly)
    .scvalue(BadNoDeleteRights)
    .scvalue(BadServerIndexInvalid)
    .scvalue(BadViewIdUnknown)
    .scvalue(Uncertain_ReferenceOutOfServer)
    .scvalue(BadTooManyMatches)
    .scvalue(BadQueryTooComplex)
    .scvalue(BadNoMatch)
    .scvalue(BadMaxAgeInvalid)
    .scvalue(BadHistoryOperationInvalid)
    .scvalue(BadHistoryOperationUnsupported)
    .scvalue(BadWriteNotSupported)
    .scvalue(BadTypeMismatch)
    .scvalue(BadMethodInvalid)
    .scvalue(BadArgumentsMissing)
    .scvalue(BadTooManySubscriptions)
    .scvalue(BadTooManyPublishRequests)
    .scvalue(BadNoSubscription)
    .scvalue(BadSequenceNumberUnknown)
    .scvalue(BadMessageNotAvailable)
    .scvalue(BadInsufficientClientProfile)
    .scvalue(BadTcpServerTooBusy)
    .scvalue(BadTcpMessageTypeInvalid)
    .scvalue(BadTcpSecureChannelUnknown)
    .scvalue(BadTcpMessageTooLarge)
    .scvalue(BadTcpNotEnoughResources)
    .scvalue(BadTcpInternalError)
    .scvalue(BadTcpEndpointUrlInvalid)
    .scvalue(BadRequestInterrupted)
    .scvalue(BadRequestTimeout)
    .scvalue(BadSecureChannelClosed)
    .scvalue(BadSecureChannelTokenUnknown)
    .scvalue(BadSequenceNumberInvalid)
    .scvalue(BadConfigurationError)
    .scvalue(BadNotConnected)
    .scvalue(BadDeviceFailure)
    .scvalue(BadSensorFailure)
    .scvalue(BadOutOfService)
    .scvalue(BadDeadbandFilterInvalid)
    .scvalue(Uncertain_NoCommunicationLastUsableValue)
    .scvalue(Uncertain_LastUsableValue)
    .scvalue(Uncertain_SubstituteValue)
    .scvalue(Uncertain_InitialValue)
    .scvalue(Uncertain_SensorNotAccurate)
    .scvalue(Uncertain_EngineeringUnitsExceeded)
    .scvalue(Uncertain_SubNormal)
    .scvalue(GoodLocalOverride)
    .scvalue(BadRefreshInProgress)
    .scvalue(BadConditionAlreadyDisabled)
    .scvalue(BadConditionDisabled)
    .scvalue(BadEventIdUnknown)
    .scvalue(BadNoData)
    .scvalue(BadNoBound)
    .scvalue(BadDataLost)
    .scvalue(BadDataUnavailable)
    .scvalue(BadEntryExists)
    .scvalue(BadNoEntryExists)
    .scvalue(BadTimestampNotSupported)
    .scvalue(GoodEntryInserted)
    .scvalue(GoodEntryReplaced)
    .scvalue(Uncertain_DataSubNormal)
    .scvalue(GoodNoData)
    .scvalue(GoodMoreData)
    .scvalue(GoodCommunicationEvent)
    .scvalue(GoodShutdownEvent)
    .scvalue(GoodCallAgain)
    .scvalue(GoodNonCriticalTimeout)
    .scvalue(BadInvalidArgument)
    .scvalue(BadConnectionRejected)
    .scvalue(BadDisconnect)
    .scvalue(BadConnectionClosed)
    .scvalue(BadInvalidState)
    .scvalue(BadEndOfStream)
    .scvalue(BadNoDataAvailable)
    .scvalue(BadWaitingForResponse)
    .scvalue(BadOperationAbandoned)
    .scvalue(BadExpectedStreamToBlock)
    .scvalue(BadWouldBlock)
    .scvalue(BadSyntaxError)
    .scvalue(BadMaxConnectionsReached)
    .scvalue(BadRequestTooLarge)
    .scvalue(BadResponseTooLarge)
    .scvalue(GoodResultsMayBeIncomplete)
    .scvalue(BadEventNotAcknowledgeable)
    .scvalue(Uncertain_ReferenceNotDeleted)
    .scvalue(BadInvalidTimestampArgument)
    .scvalue(BadProtocolVersionUnsupported)
    .scvalue(BadStateNotActive)
    .scvalue(Uncertain_NotAllNodesAvailable)
    .scvalue(BadFilterOperatorInvalid)
    .scvalue(BadFilterOperatorUnsupported)
    .scvalue(BadFilterOperandCountMismatch)
    .scvalue(BadFilterElementInvalid)
    .scvalue(BadFilterLiteralInvalid)
    .scvalue(BadIdentityChangeNotSupported)
    .scvalue(BadNotTypeDefinition)
    .scvalue(BadViewTimestampInvalid)
    .scvalue(BadViewParameterMismatch)
    .scvalue(BadViewVersionInvalid)
    .scvalue(BadConditionAlreadyEnabled)
    .scvalue(BadDialogNotActive)
    .scvalue(BadDialogResponseInvalid)
    .scvalue(BadConditionBranchAlreadyAcked)
    .scvalue(BadConditionBranchAlreadyConfirmed)
    .scvalue(BadConditionAlreadyShelved)
    .scvalue(BadConditionNotShelved)
    .scvalue(BadShelvingTimeOutOfRange)
#undef scvalue
    ;

  enum_<VariantType>("VariantType")
    .value("uint16", VariantType::UINT16)
    .value("uint32", VariantType::UINT32)
    .value("uint64", VariantType::UINT64)
    .value("bool", VariantType::BOOLEAN)
  ;

  class_<PyNode>("Node", init<Services::SharedPtr, NodeID>())
    .def(init<Node>())
    .def("get_id", &PyNode::PyGetNodeID)
    .def("get_attribute", &PyNode::GetAttribute)
    .def("set_attribute", &PyNode::SetAttribute)
    .def("get_value", &PyNode::PyGetValue)
    .def("set_value", &PyNode::PySetValue, PyNodeSetValue_stubs((python::arg("value"), python::arg("hint")=VariantType::NUL, python::arg("DateTime")=CurrentDateTime()), "set a node value."))
    .def("set_value", &PyNode::PySetDataValue)
    .def("get_properties", &PyNode::GetProperties)
    .def("get_variables", &PyNode::GetVariables)
    .def("get_name", &PyNode::PyGetName)
    .def("get_children", &PyNode::PyGetChildren)
    .def("get_child", &PyNode::PyGetChild)
    .def("add_folder", &PyNode::PyAddFolder)
    .def("add_folder", &PyNode::PyAddFolder2)
    .def("add_object", &PyNode::PyAddObject)
    .def("add_object", &PyNode::PyAddObject2)
    .def("add_variable", &PyNode::PyAddVariable)
    .def("add_variable", &PyNode::PyAddVariable2)
    .def("add_variable", &PyNode::PyAddVariable3)
    .def("add_property", &PyNode::PyAddProperty)
    .def("add_property", &PyNode::PyAddProperty2)
    .def("add_property", &PyNode::PyAddProperty3)
    .def(str(self))
    .def(repr(self))
    .def(self == self)
  ;

  class_<std::vector<Node> >("NodeVector")
    .def(vector_indexing_suite<std::vector<Node> >())
  ;

  class_<std::vector<std::string> >("StringVector")
    .def(vector_indexing_suite<std::vector<std::string> >())
  ;

  class_<SubscriptionClient, PySubscriptionClient, boost::noncopyable>("SubscriptionClient", init<>())
    .def("data_change", &PySubscriptionClient::DefaultDataChange)
    .def("event", &PySubscriptionClient::DefaultEvent)
    .def("status_chane", &PySubscriptionClient::DefaultStatusChange)
  ;


  class_<PyEvent>("Event", init<const NodeID&>())
    .def("get_value", &PyEvent::PyGetValue)
    .def("set_value", &PyEvent::PySetValue)
    .def_readwrite("event_id", &PyEvent::EventId)
    .def_readwrite("event_type", &PyEvent::EventType)
    .def_readwrite("local_time", &PyEvent::LocalTime)
    .def_readwrite("receive_time", &PyEvent::ReceiveTime)
    .def_readwrite("time", &PyEvent::Time)
    .def_readwrite("source_name", &PyEvent::SourceName)
    .add_property("message2", &PyEvent::GetMessage, &PyEvent::SetMessage)
    .def_readwrite("severity", &PyEvent::Severity)
    .add_property("message", &PyEvent::GetMessage, &PyEvent::SetMessage)
    .add_property("source_node", &PyEvent::GetSourceNode, &PyEvent::SetSourceNode)
    .add_property("time", &PyEvent::GetTime, &PyEvent::SetTime)
  ;

  class_<PySubscription>("Subscription", init<std::shared_ptr<Subscription>>())
    .def("subscribe_data_change", &PySubscription::SubscribeDataChange)
    .def("subscribe_data_change", &PySubscription::SubscribeDataChange2)
    .def("delete", &PySubscription::Delete)
    .def("unsubscribe", &PySubscription::UnSubscribe)
    .def("subscribe_events", &PySubscription::SubscribeEvents)
    .def("subscribe_events", &PySubscription::SubscribeEvents2)
  ;

  class_<PyClient, boost::noncopyable>("Client", init<>())
    .def(init<bool>())
    .def("connect", &PyClient::Connect)
    .def("disconnect", &PyClient::Disconnect)
    .def("get_root_node", &PyClient::PyGetRootNode)
    .def("get_objects_node", &PyClient::PyGetObjectsNode)
    .def("get_server_node", &PyClient::PyGetServerNode)
    .def("get_node", &PyClient::PyGetNode)
    .def("set_endpoint", &PyClient::SetEndpoint)
    .def("get_endpoint", &PyClient::GetEndpoint)
    .def("set_session_name", &PyClient::SetSessionName)
    .def("get_session_name", &PyClient::GetSessionName)
    .def("get_uri", &PyClient::GetURI)
    .def("set_uri", &PyClient::SetURI)
    .def("set_security_policy", &PyClient::SetSecurityPolicy)
    .def("get_security_policy", &PyClient::GetSecurityPolicy)
    .def("create_subscription", &PyClient::CreateSubscription)
  ;

  class_<PyOPCUAServer, boost::noncopyable >("Server", init<>())
    .def(init<bool>())
    .def("start", &PyOPCUAServer::Start)
    .def("stop", &PyOPCUAServer::Stop)
    .def("get_root_node", &PyOPCUAServer::PyGetRootNode)
    .def("get_objects_node", &PyOPCUAServer::PyGetObjectsNode)
    .def("get_server_node", &PyOPCUAServer::PyGetServerNode)
    .def("get_node", &PyOPCUAServer::PyGetNode)
    //.def("get_node_from_path", &PyOPCUAServer::PyGetNodeFromPath)
    //.def("get_node_from_qn_path", NodeFromPathQN)
    .def("set_uri", &PyOPCUAServer::SetServerURI)
    .def("add_xml_address_space", &PyOPCUAServer::AddAddressSpace)
    .def("set_server_name", &PyOPCUAServer::SetServerName)
    .def("set_endpoint", &PyOPCUAServer::SetEndpoint)
    .def("load_cpp_addressspace", &PyOPCUAServer::SetLoadCppAddressSpace)
    .def("create_subscription", &PyOPCUAServer::CreateSubscription)
  ;

  class_<LocalizedText>("LocalizedText")
    .def_readwrite("Encoding", &LocalizedText::Encoding)
    .def_readwrite("Locale", &LocalizedText::Locale)
    .def_readwrite("Text", &LocalizedText::Text)
  ;  

}

