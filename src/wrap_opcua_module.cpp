/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>
#include <boost/python/type_id.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <functional>

#include "opc/ua/client/client.h"
#include "opc/ua/client/binary_server.h"
#include "opc/ua/node.h"
#include "opc/ua/event.h"
#include "opc/ua/server/opcuaserver.h"
#include "opc/ua/protocol/types.h"
#include "opc/ua/services/services.h"
#include "opc/ua/subscription.h"
#include "opc/ua/protocol/string_utils.h"

#include "wrap_opcua_enums.h"
#include "wrap_opcua_helpers.h"
#include "wrap_opcua_subscriptionclient.h"

using namespace boost::python;
using namespace OpcUa;


template <typename T>
T Extract(const object & obj)
{
  return extract<T>(obj)();
}

template <typename T>
list ToList(const std::vector<T> objects)
{
  list result;
  std::for_each(objects.begin(), objects.end(),
                [&result](const T & obj)
  {
    result.append(obj);
  }
               );
  return result;
}

template <typename ResultType, typename SourceType>
list ToList(const std::vector<SourceType> objects)
{
  list result;
  std::for_each(objects.begin(), objects.end(),
                [&result](const SourceType & obj)
  {
    result.append(ResultType(obj));
  }
               );
  return result;
}

template <typename T>
std::vector<T> ToVector(const object & list)
{
  std::vector<T> result;
  std::size_t listSize = len(list);

  for (std::size_t i = 0; i < listSize; ++i)
    {
      const object & element = list[i];
      const T & value = Extract<T>(element);
      result.push_back(value);
    }

  return result;
}

struct PyBrowseParameters
{
  unsigned MaxReferenciesCount;
  NodeID NodeToBrowse;
  unsigned Direction;
  NodeID ReferenceTypeID;
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

struct PyReadParameters
{
  double MaxAge;
  TimestampsToReturn TimestampsType;
  list AttributesToRead; // type of elmnts is AttributeValueID
};


struct VariantToPythonObjectConverter
{
  typedef object result_type;

  template <typename T>
  typename std::enable_if<is_container_not_string<T>::value == true, result_type>::type operator()(const T & val)
  {
    return ToList(val);
  }

  template <typename T>
  typename std::enable_if<is_container_not_string<T>::value == false, result_type>::type operator()(const T & val)
  {
    return object(val);
  }
};

object ToObject(const Variant & var)
{
  if (var.IsNul())
    {
      return object();
    }

  return var.Visit(VariantToPythonObjectConverter());
}

Variant ToVariant(const object & object)
{
  Variant var;

  if (extract<std::string>(object).check())
    {
      var = extract<std::string>(object)();
    }

  else if (extract<list>(object).check())
    {
      list plist = (list) object;

      if (len(object) == 0)
        {
        }
      else
        {
          if (extract<int>(object[0]).check())
            {
              var = ToVector<int>(object);
            }

          else if (extract<double>(object[0]).check())
            {
              var = ToVector<double>(object);
            }

          else if (extract<std::string>(object[0]).check())
            {
              var = ToVector<std::string>(object);
            }

          else
            {
              throw std::logic_error("Cannot create variant from python list. Unsupported type.");
            }
        }
    }

  else if (extract<int>(object).check())
    {
      var = Extract<int>(object);
    }

  else if (extract<double>(object).check())
    {
      var = Extract<double>(object);
    }

  else if (extract<NodeID>(object).check())
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
Variant ToVariant2(const object & object, VariantType vtype)
{
  Variant var;

  if (extract<list>(object).check())
    {

      list plist = (list) object;

      if (len(object) == 0)
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
          var = extract<bool>(object)();
          return var;

        case VariantType::UINT16:
        case VariantType::UINT32:
          var = extract<uint32_t>(object)();
          return var;

        default:
          return ToVariant(object);
        }
    }
}

struct PyVariant
{
  object Value;
  VariantType Type = VariantType::NUL;
  bool IsNull = true;

  PyVariant() = default;

  explicit PyVariant(const Variant & value)
    : Value(ToObject(value))
    , Type(value.Type())
    , IsNull(value.IsNul())
  {
  }
};

class PyServer
{
public:
  explicit PyServer(const std::string & endpointUrl)
    : Impl(CreateBinaryServer(endpointUrl))
  {
  }
  /*
      list FindServers() const
      {
        const FindServersParameters params;
        const std::vector<ApplicationDescription> nativeApps = Impl->Endpoints()->FindServers(params);
        return ToList(nativeApps);
      }

      list GetEndpoints() const
      {
        const EndpointsFilter filter;
        const std::vector<EndpointDescription> endpoints = Impl->Endpoints()->GetEndpoints(filter);
        return ToList(endpoints);
      }
      */
  list Browse(const PyBrowseParameters & p) const
  {
    BrowseDescription description;
    description.NodeToBrowse = p.NodeToBrowse;
    description.ReferenceTypeID = p.ReferenceTypeID;
    description.Direction = static_cast<BrowseDirection>(p.Direction);
    description.IncludeSubtypes = p.IncludeSubtypes;
    description.NodeClasses = p.NodeClasses;
    description.ResultMask = p.ResultMask;

    NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = p.MaxReferenciesCount;
    const std::vector<ReferenceDescription> references = Impl->Views()->Browse(query);
    return ToList(references);
  }

  list Read(const PyReadParameters & in)
  {
    ReadParameters params;
    params.MaxAge = in.MaxAge;
    params.TimestampsType = static_cast<TimestampsToReturn>(in.TimestampsType);

    std::size_t listSize = len(in.AttributesToRead);

    for (std::size_t i = 0; i < listSize; ++i)
      {
        const AttributeValueID & value = extract<AttributeValueID>(in.AttributesToRead[i]); // XXX check
        AttributeValueID attr;
        attr.Attribute = value.Attribute;
        attr.DataEncoding.NamespaceIndex = value.DataEncoding.NamespaceIndex;
        attr.DataEncoding.Name = value.DataEncoding.Name;
        attr.IndexRange = value.IndexRange;
        attr.Node = value.Node;
        params.AttributesToRead.push_back(attr);
      }

    std::vector<DataValue> data = Impl->Attributes()->Read(params);
    return ToList<DataValue, DataValue>(data); // XXX std::vector<DataValue> has been wrapped.
  }

// XXX XXX XXX PyWriteValue is gone but we have it and vector<>
//  list Write(const list & in)
//  {
//    const std::vector<PyWriteValue> & pyValues = ToVector<PyWriteValue>(in);
//    std::vector<WriteValue> values;
//
//    for (std::vector<PyWriteValue>::const_iterator valueIt = pyValues.begin(); valueIt != pyValues.end(); ++valueIt)
//      {
//        const PyWriteValue & pyValue = *valueIt;
//        const WriteValue & value = GetWriteValue(pyValue);
//        values.push_back(value);
//      }
//
//    const list & result = ToList<unsigned, StatusCode>(Impl->Attributes()->Write(values));
//    return result;
//  }

private:
  Services::SharedPtr Impl;
};


class PySubscription
{
public:
  PySubscription(std::shared_ptr<Subscription> other): Sub(other) { }  //converting to shared pointer, should be ok
  PySubscription() { throw std::runtime_error("Subscription cannot be instanciated from Python"); }
  void Delete() { Sub->Delete(); }
  uint32_t SubscribeDataChange(Node node) { return Sub->SubscribeDataChange(node, AttributeID::VALUE); }
  uint32_t SubscribeDataChange2(Node node, AttributeID attr) { return Sub->SubscribeDataChange(node, attr); }
  void UnSubscribe(uint32_t id) { return Sub->UnSubscribe(id); }
  uint32_t SubscribeEvents() { return Sub->SubscribeEvents(); }
  uint32_t SubscribeEvents2(const Node node, const Node & eventtype) { return Sub->SubscribeEvents(node, eventtype); }

private:
  std::shared_ptr<Subscription> Sub;
};

class PyClient: public RemoteClient
{
public:
  using RemoteClient::RemoteClient;

  Node PyGetRootNode()
  {
    return Node(Server, ObjectID::RootFolder);
  }

  Node PyGetObjectsNode()
  {
    return Node(Server, ObjectID::ObjectsFolder);
  }

  Node PyGetServerNode()
  {
    return Node(Server, ObjectID::Server);
  }

  Node PyGetNode(NodeID nodeid)
  {
    return Node(RemoteClient::GetNode(nodeid));
  }

  //Node GetNodeFromPath(const object& path) { return Client::Client::GetNodeFromPath(ToVector<std::string>(path)); }
  PySubscription CreateSubscription(uint period, PySubscriptionClient & callback)
  {
    return PySubscription(RemoteClient::CreateSubscription(period, callback));
  }
};

class PyOPCUAServer: public OPCUAServer
{
public:
  using OPCUAServer::OPCUAServer;

  Node PyGetRootNode() const
  {
    return Node(Registry->GetServer(), ObjectID::RootFolder);
  }

  Node PyGetObjectsNode() const
  {
    return Node(Registry->GetServer(), ObjectID::ObjectsFolder);
  }

  Node PyGetServerNode() const
  {
    return Node(Registry->GetServer(), ObjectID::Server);
  }

  Node PyGetNode(const NodeID & nodeid) const
  {
    return Node(OPCUAServer::GetNode(nodeid));
  }

  Node PyGetNodeFromPath(const object & path) const
  {
    return OPCUAServer::GetNodeFromPath(ToVector<std::string>(path));
  }

  PySubscription CreateSubscription(uint period, PySubscriptionClient & callback)
  {
    return PySubscription(OPCUAServer::CreateSubscription(period, callback));
  }
};


//--------------------------------------------------------------------------
// Overloads
//--------------------------------------------------------------------------

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SubscriptionSubscribeDataChange_stubs, Subscription::SubscribeDataChange, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeGetName_stubs, Node::GetName, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeSetValue_stubs, Node::SetValue, 1, 2);


//--------------------------------------------------------------------------
// NodeID helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<NodeID> NodeID_constructor(const std::string & encodedNodeID)
{
  return boost::shared_ptr<NodeID>(new NodeID(ToNodeID(encodedNodeID)));
}

static object NodeID_GetIdentifier(const NodeID & self)
{
  if (self.IsInteger())
    {
      return object(self.GetIntegerIdentifier());
    }

  else if (self.IsString())
    {
      return object(self.GetStringIdentifier());
    }

  else if (self.IsGuid())
    {
      return object(self.GetGuidIdentifier());
    }

  else if (self.IsBinary())
    {
      return object(self.GetBinaryIdentifier());
    }

  else
    {
      throw std::logic_error("Error unknown identifier.");
    }
}

//--------------------------------------------------------------------------
// DataValue helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<DataValue> DataValue_constructor1(const object & obj)
{
  return boost::shared_ptr<DataValue>(new DataValue(ToVariant(obj)));
}

static boost::shared_ptr<DataValue> DataValue_constructor2(const object & obj, VariantType vtype)
{
  return boost::shared_ptr<DataValue>(new DataValue(ToVariant2(obj, vtype)));
}

static object  DataValue_get_value(const DataValue & self)
{ return ToObject(self.Value); }

static void DataValue_set_value(DataValue & self, const object & obj, VariantType vtype)
{ self.Value = ToVariant2(obj, vtype); self.Encoding |= DATA_VALUE; }

static StatusCode DataValue_get_status(const DataValue & self)
{ return self.Status; }

static void DataValue_set_status(DataValue & self, const StatusCode & sc)
{ self.Status = sc; self.Encoding |= DATA_VALUE_STATUS_CODE; }

static DateTime DataValue_get_source_timestamp(const DataValue & self)
{ return self.SourceTimestamp; }

static void DataValue_set_source_timestamp(DataValue & self, const DateTime & dt)
{ self.SourceTimestamp = dt; self.Encoding |= DATA_VALUE_SOURCE_TIMESTAMP; }

static uint16_t DataValue_get_source_picoseconds(const DataValue & self)
{ return self.SourcePicoseconds; }

static void DataValue_set_source_picoseconds(DataValue & self, uint16_t ps)
{ self.SourcePicoseconds = ps; self.Encoding |= DATA_VALUE_SOURCE_PICOSECONDS; }

static DateTime DataValue_get_server_timestamp(const DataValue & self)
{ return self.ServerTimestamp; }

static void DataValue_set_server_timestamp(DataValue & self, const DateTime & dt)
{ self.ServerTimestamp = dt; self.Encoding |= DATA_VALUE_SERVER_TIMESTAMP; }

static uint16_t DataValue_get_server_picoseconds(const DataValue & self)
{ return self.ServerPicoseconds; }

static void DataValue_set_server_picoseconds(DataValue & self, uint16_t ps)
{ self.ServerPicoseconds = ps; self.Encoding |= DATA_VALUE_SERVER_PICOSECONDS; }

//--------------------------------------------------------------------------
// module
//--------------------------------------------------------------------------

BOOST_PYTHON_MODULE(opcua)
{

  using self_ns::str; // hack to enable __str__ in python classes with str(self)

  PyEval_InitThreads();

  wrap_opcua_enums();

  to_python_converter<std::vector<std::string>, vector_to_python_converter<std::string>>();
  vector_from_python_converter<std::string>();

  class_<DateTime>("DateTime", init<>())
  .def(init<int64_t>())
  ;

  def("CurrentDateTime", &CurrentDateTime);
  def("ToDateTime", &ToDateTime);
  def("ToTimeT", &ToTimeT);

  class_<LocalizedText>("LocalizedText")
  .def_readwrite("Encoding", &LocalizedText::Encoding)
  .def_readwrite("Locale", &LocalizedText::Locale)
  .def_readwrite("Text", &LocalizedText::Text)
  ;

  class_<NodeID, boost::shared_ptr<NodeID>>("NodeID")
  .def(init<uint32_t, uint16_t>())
  .def(init<std::string, uint16_t>())
  .def("__init__", make_constructor(NodeID_constructor)) // XXX add this constructor to freeopcua
  .add_property("namespace_index", &NodeID::GetNamespaceIndex)
  .add_property("identifier", &NodeID_GetIdentifier)
  .add_property("encoding", &NodeID::GetEncodingValue)
  .add_property("is_integer", &NodeID::IsInteger)
  .add_property("is_binary", &NodeID::IsBinary)
  .add_property("is_guid", &NodeID::IsGuid)
  .add_property("is_string", &NodeID::IsString)
  .def_readonly("namespace_uri", &NodeID::NamespaceURI)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  class_<QualifiedName>("QualifiedName")
  .def(init<uint16_t, std::string>())
  .def(init<std::string, uint16_t>()) // XXX ah, right
  //.def("parse", &ToQualifiedName)      XXX could be def(), dropped it's mostly useless
  .def_readwrite("namespace_index", &QualifiedName::NamespaceIndex)
  .def_readwrite("name", &QualifiedName::Name)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  class_<DataValue, boost::shared_ptr<DataValue>>("DataValue")
  .def("__init__", make_constructor(DataValue_constructor1))  // XXX Variant vs object
  .def("__init__", make_constructor(DataValue_constructor2)) // XXX Variant,VariantType vs object,VariantType
#define _property(X) add_property( #X, &DataValue_get_ ## X, &DataValue_set_ ## X)
  ._property(value)
  ._property(status)
  ._property(source_timestamp)
  ._property(source_picoseconds)
  ._property(server_timestamp)
  ._property(server_picoseconds)
#undef _property
  ;

  to_python_converter<std::vector<DataValue>, vector_to_python_converter<DataValue>>();

  class_<ApplicationDescription>("ApplicationDescription")
  .def_readwrite("uri", &ApplicationDescription::URI)
  .def_readwrite("product_uri", &ApplicationDescription::ProductURI)
  .def_readwrite("name", &ApplicationDescription::Name)
  .def_readwrite("type", &ApplicationDescription::Type)
  .def_readwrite("gateway_server_uri", &ApplicationDescription::GatewayServerURI)
  .def_readwrite("discovery_profile_uri", &ApplicationDescription::DiscoveryProfileURI)
  //.def_readwrite("discovery_urls", &ApplicationDescription::DiscoveryURLs) XXX
  .add_vector_property("discovery_urls", ApplicationDescription, std::string, DiscoveryURLs)
  ;

  to_python_converter<std::vector<ApplicationDescription>, vector_to_python_converter<ApplicationDescription>>();

  class_<UserTokenPolicy>("UserTokenPolicy")
  .def_readwrite("policy_id", &UserTokenPolicy::PolicyID)
  .def_readwrite("token_type", &UserTokenPolicy::TokenType)
  .def_readwrite("issued_token_type", &UserTokenPolicy::IssuedTokenType)
  .def_readwrite("issuer_endpoint_url", &UserTokenPolicy::IssuerEndpointURL)
  .def_readwrite("security_policy_uri", &UserTokenPolicy::SecurityPolicyURI)
  ;

  to_python_converter<std::vector<UserTokenPolicy>, vector_to_python_converter<UserTokenPolicy>>();
  vector_from_python_converter<UserTokenPolicy>();

  class_<EndpointDescription>("EndpointDescription")
  .def_readwrite("url", &EndpointDescription::EndpointURL)
  .def_readwrite("server_description", &EndpointDescription::ServerDescription)
  //.def_readwrite("certificate", &EndpointDescription::ServerCertificate)
  .def_readwrite("security_mode", &EndpointDescription::SecurityMode)
  .def_readwrite("security_policy_uri", &EndpointDescription::SecurityPolicyURI)
  //.def_readwrite("user_identify_tokens", &EndpointDescription::UserIdentifyTokens) XXX
  .add_vector_property("user_identify_tokens", EndpointDescription, UserTokenPolicy, UserIdentifyTokens)
  .def_readwrite("transport_profile_uri", &EndpointDescription::TransportProfileURI)
  .def_readwrite("security_level", &EndpointDescription::SecurityLevel)
  ;

  to_python_converter<std::vector<EndpointDescription>, vector_to_python_converter<EndpointDescription>>();

  // XXX delete
  class_<PyBrowseParameters>("BrowseParameters")
  .def_readwrite("max_referencies_count", &PyBrowseParameters::MaxReferenciesCount)
  .def_readwrite("node_to_browse", &PyBrowseParameters::NodeToBrowse)
  .def_readwrite("direction", &PyBrowseParameters::Direction)
  .def_readwrite("reference_type_id", &PyBrowseParameters::ReferenceTypeID)
  .def_readwrite("include_subtypes", &PyBrowseParameters::IncludeSubtypes)
  .def_readwrite("node_classes", &PyBrowseParameters::NodeClasses)
  .def_readwrite("result_mask", &PyBrowseParameters::ResultMask)
  ;

  class_<ReferenceDescription>("ReferenceDescription")
  .def_readwrite("reference_type_id", &ReferenceDescription::ReferenceTypeID)
  .def_readwrite("is_forward", &ReferenceDescription::IsForward)
  .def_readwrite("target_node_id", &ReferenceDescription::TargetNodeID)
  .def_readwrite("browse_name", &ReferenceDescription::BrowseName)
  .def_readwrite("display_name", &ReferenceDescription::DisplayName)
  .def_readwrite("target_node_class", &ReferenceDescription::TargetNodeClass)
  .def_readwrite("target_node_type_definition", &ReferenceDescription::TargetNodeTypeDefinition)
  ;

  to_python_converter<std::vector<ReferenceDescription>, vector_to_python_converter<ReferenceDescription>>();

  // XXX delete
  class_<PyReadParameters>("ReadParameters")
  .def_readwrite("max_age", &PyReadParameters::MaxAge)
  .def_readwrite("timestamps_to_return", &PyReadParameters::TimestampsType)
  .def_readwrite("attributes_to_read", &PyReadParameters::AttributesToRead)
  ;

  class_<AttributeValueID>("AttributeValueID")
  .def_readwrite("node", &AttributeValueID::Node)
  .def_readwrite("attribute", &AttributeValueID::Attribute)
  .def_readwrite("index_range", &AttributeValueID::IndexRange)
  .def_readwrite("data_encoding", &AttributeValueID::DataEncoding)
  ;

  to_python_converter<std::vector<AttributeValueID>, vector_to_python_converter<AttributeValueID>>();


  class_<WriteValue>("WriteValue")
  .def_readwrite("node", &WriteValue::Node)
  .def_readwrite("attribute", &WriteValue::Attribute)
  .def_readwrite("numeric_range", &WriteValue::NumericRange)
  .def_readwrite("data", &WriteValue::Data)
  ;

  to_python_converter<std::vector<WriteValue>, vector_to_python_converter<WriteValue>>();
  vector_from_python_converter<WriteValue>();

  // XXX todo
  class_<PyVariant>("Variant")
  .def_readonly("value", &PyVariant::Value)
  .def_readonly("type", &PyVariant::Type)
  .def_readonly("is_null", &PyVariant::IsNull)
  ;

  class_<Node>("Node", init<Services::SharedPtr, NodeID>())
  .def(init<Node>())
  .def("get_id", &Node::GetId)
  .def("get_attribute", &Node::GetAttribute)
  .def("set_attribute", &Node::SetAttribute)
  .def("get_value", &Node::GetValue)
  .def("set_value", (StatusCode(Node::*)(const Variant &, const DateTime &) const) &Node::SetValue, NodeSetValue_stubs((arg("value"), arg("DateTime") = CurrentDateTime()), "set a node value."))
  .def("set_value", (StatusCode(Node::*)(const DataValue &) const) &Node::SetValue)
  .def("get_properties", &Node::GetProperties)
  .def("get_variables", &Node::GetVariables)
  .def("get_name", &Node::GetName, NodeGetName_stubs((arg("force") = false)))
  .def("get_children", (std::vector<Node> (Node::*)() const) &Node::GetChildren)
  .def("get_child", (Node(Node::*)(const std::vector<std::string> &) const) &Node::GetChild)
  .def("get_child", (Node(Node::*)(const std::string &) const) &Node::GetChild)
  .def("add_folder", (Node(Node::*)(const NodeID &, const QualifiedName &) const) &Node::AddFolder)
  .def("add_folder", (Node(Node::*)(const std::string &, const std::string &) const) &Node::AddFolder)
  .def("add_folder", (Node(Node::*)(const std::string &) const) &Node::AddFolder)
  .def("add_object", (Node(Node::*)(const NodeID &, const QualifiedName &) const) &Node::AddObject)
  .def("add_object", (Node(Node::*)(const std::string &, const std::string &) const) &Node::AddObject)
  .def("add_object", (Node(Node::*)(const std::string &) const) &Node::AddObject)
  .def("add_variable", (Node(Node::*)(const NodeID &, const QualifiedName &, const Variant &) const) &Node::AddVariable)
  .def("add_variable", (Node(Node::*)(const std::string &, const std::string &, const Variant &) const) &Node::AddVariable)
  .def("add_variable", (Node(Node::*)(const std::string &, const Variant &) const) &Node::AddVariable)
  .def("add_property", (Node(Node::*)(const NodeID &, const QualifiedName &, const Variant &) const) &Node::AddProperty)
  .def("add_property", (Node(Node::*)(const std::string &, const std::string &, const Variant &) const) &Node::AddProperty)
  .def("add_property", (Node(Node::*)(const std::string &, const Variant &) const) &Node::AddProperty)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  to_python_converter<std::vector<Node>, vector_to_python_converter<Node>>();
  vector_from_python_converter<Node>();

  class_<SubscriptionClient, PySubscriptionClient, boost::noncopyable>("SubscriptionClient", init<>())
  .def("data_change", &PySubscriptionClient::DefaultDataChange)
  .def("event", &PySubscriptionClient::DefaultEvent)
  .def("status_change", &PySubscriptionClient::DefaultStatusChange)
  ;

  class_<Event>("Event", init<const NodeID &>())
  .def("get_value", (Variant(Event::*)(const std::string &) const) &Event::GetValue)
  .def("set_value", (void (Event::*)(const std::string &, Variant)) &Event::SetValue)
  .def_readwrite("event_id", &Event::EventId)
  .def_readwrite("event_type", &Event::EventType)
  .def_readwrite("local_time", &Event::LocalTime)
  .def_readwrite("message", &Event::Message)
  .def_readwrite("receive_time", &Event::ReceiveTime)
  .def_readwrite("severity", &Event::Severity)
  .def_readwrite("source_name", &Event::SourceName)
  .def_readwrite("source_node", &Event::SourceNode)
  .def_readwrite("time", &Event::Time)
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

}

