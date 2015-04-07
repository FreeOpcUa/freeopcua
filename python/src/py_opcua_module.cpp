/// @author Matthieu Bec 2014
/// @email mbec@gmto.org
/// @brief Python bindings for freeopcua.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <boost/python.hpp>
//#include <Python.h>
#include <datetime.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include "opc/ua/client/client.h"
#include "opc/ua/client/binary_client.h"
#include "opc/ua/node.h"
#include "opc/ua/event.h"
#include "opc/ua/server/server.h"
#include "opc/ua/protocol/protocol.h"
#include "opc/ua/services/services.h"
#include "opc/ua/subscription.h"
#include "opc/ua/protocol/string_utils.h"

#include "py_opcua_enums.h"
#include "py_opcua_helpers.h"
#include "py_opcua_subscriptionclient.h"
#include "py_opcua_variant.h"

#if PY_MAJOR_VERSION >= 3
  #define PyString_Check               PyUnicode_Check
  #define PyString_AsString(S)           PyBytes_AsString(PyUnicode_AsUTF8String(S)) 
#endif

using namespace boost::python;
using namespace OpcUa;

//--------------------------------------------------------------------------
// Overloads
//--------------------------------------------------------------------------

BOOST_PYTHON_FUNCTION_OVERLOADS(DateTimeFromTimeT_stub, DateTime::FromTimeT, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SubscriptionSubscribeDataChange_stubs, Subscription::SubscribeDataChange, 1, 2);
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeGetBrowseName_stubs, Node::GetBrowseName, 0, 1);


//--------------------------------------------------------------------------
// DateTime helpers
//--------------------------------------------------------------------------

static  boost::python::object ToPyDateTime(const DateTime& self )
{
  boost::posix_time::ptime ref(boost::gregorian::date(1601,1,1));
  boost::posix_time::ptime dt = ref + boost::posix_time::microseconds(self.Value/10);
  // The constructor of the python datetime objects creates a datetime object using the locale timezone
  // but returns a naive timezone objects...this sounds a bit crazy...
  // we may want to add timezone... I do not know how
  uint32_t precision = dt.time_of_day().num_fractional_digits();
  PyObject* obj = PyDateTime_FromDateAndTime((int)dt.date().year(),
					  (int)dt.date().month(),
					  (int)dt.date().day(),
					  dt.time_of_day().hours(),
					  dt.time_of_day().minutes(),
					  dt.time_of_day().seconds(),
					  dt.time_of_day().fractional_seconds() / pow(10, precision-6));
  return boost::python::object(boost::python::handle<>(obj));
}

static uint64_t ToWinEpoch(PyObject* pydate)
{
  boost::gregorian::date _date(PyDateTime_GET_YEAR(pydate), PyDateTime_GET_MONTH(pydate), PyDateTime_GET_DAY(pydate));
  boost::posix_time::time_duration _duration(PyDateTime_DATE_GET_HOUR(pydate), PyDateTime_DATE_GET_MINUTE(pydate), PyDateTime_DATE_GET_SECOND(pydate), 0);
  _duration += boost::posix_time::microseconds(PyDateTime_DATE_GET_MICROSECOND(pydate));

	boost::posix_time::ptime myptime(_date, _duration);
  boost::posix_time::ptime ref(boost::gregorian::date(1601,1,1));
  return (myptime - ref).total_microseconds() * 10;
}

static boost::shared_ptr<DateTime> makeOpcUaDateTime(const boost::python::object& bobj)
{
  PyObject* pydate = bobj.ptr();
  if ( ! PyDateTime_Check(pydate) )
  {
    throw std::runtime_error("method take a python datetime as argument");
  }

  return boost::shared_ptr<DateTime>(new DateTime(ToWinEpoch(pydate)));
}

struct DateTimeOpcUaToPythonConverter
{
  static PyObject* convert(const DateTime& dt)
  {
    return boost::python::incref( ToPyDateTime(dt).ptr());
  }
};

struct DateTimePythonToOpcUaConverter
{

  DateTimePythonToOpcUaConverter()
  {
    boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<DateTime>());
  }

  static void* convertible(PyObject* obj_ptr)
  {
    if (!PyDateTime_Check(obj_ptr)) return 0;
    return obj_ptr;
  }

  static void construct( PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
  {
      void* storage = (
        (boost::python::converter::rvalue_from_python_storage<DateTime>*)
        data)->storage.bytes;
 
      new (storage) DateTime(ToWinEpoch(obj_ptr));
 
      data->convertible = storage;
  }
};

//--------------------------------------------------------------------------
//LocalizedText
//--------------------------------------------------------------------------

struct LocalizedTextToPythonConverter
{
  static PyObject* convert(const LocalizedText& text)
  {
    return boost::python::incref(boost::python::object(text.Text.c_str()).ptr());
  }
};

struct PythonStringToLocalizedTextConverter
{

  PythonStringToLocalizedTextConverter()
  {
    boost::python::converter::registry::push_back(&convertible, &construct, boost::python::type_id<LocalizedText>());
  }

  static void* convertible(PyObject* obj_ptr)
  {
    if (!PyString_Check(obj_ptr)) return 0;
    return obj_ptr;
  }

  static void construct( PyObject* obj_ptr, boost::python::converter::rvalue_from_python_stage1_data* data)
  {
      void* storage = ( (boost::python::converter::rvalue_from_python_storage<LocalizedText>*)data)->storage.bytes;
      const char* value = PyString_AsString(obj_ptr);
      new (storage) LocalizedText(std::string(value));
      data->convertible = storage;
  }
};


//--------------------------------------------------------------------------
// NodeId helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<NodeId> NodeId_constructor(const std::string & encodedNodeId)
{ return boost::shared_ptr<NodeId>(new NodeId(ToNodeId(encodedNodeId))); }

static object NodeId_GetIdentifier(const NodeId & self)
{
  if (self.IsInteger())
    { return object(self.GetIntegerIdentifier()); }

  else if (self.IsString())
    { return object(self.GetStringIdentifier()); }

  else if (self.IsGuid())
    { return object(self.GetGuidIdentifier()); }

  else if (self.IsBinary())
    { return object(self.GetBinaryIdentifier()); }

  else
    { throw std::logic_error("Error unknown identifier."); }
}

//--------------------------------------------------------------------------
// DataValue helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<DataValue> DataValue_constructor1(const object & obj, VariantType vtype)
{ return boost::shared_ptr<DataValue>(new DataValue(ToVariant2(obj, vtype))); }

static object DataValue_get_value(const DataValue & self)
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
{ self.ServerTimestamp = dt; self.Encoding |= DATA_VALUE_Server_TIMESTAMP; }

static uint16_t DataValue_get_server_picoseconds(const DataValue & self)
{ return self.ServerPicoseconds; }

static void DataValue_set_server_picoseconds(DataValue & self, uint16_t ps)
{ self.ServerPicoseconds = ps; self.Encoding |= DATA_VALUE_Server_PICOSECONDS; }

//--------------------------------------------------------------------------
// Node helpers
//--------------------------------------------------------------------------
//

static void Node_SetValue(Node & self, const object & obj, VariantType vtype)
{ self.SetValue(ToVariant2(obj, vtype)); }

//--------------------------------------------------------------------------
// UaClient helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<Subscription> UaClient_CreateSubscription(UaClient & self, uint period, PySubscriptionHandler & callback)
{
  std::unique_ptr<Subscription> sub  = self.CreateSubscription(period, callback);
  return boost::shared_ptr<Subscription>(sub.release());
}

static Node UaClient_GetNode(UaClient & self, ObjectId objectid)
{
  return self.GetNode(NodeId(objectid));
}

//--------------------------------------------------------------------------
// UaServer helpers
//--------------------------------------------------------------------------

static boost::shared_ptr<Subscription> UaServer_CreateSubscription(UaServer & self, uint period, PySubscriptionHandler & callback)
{
  std::unique_ptr<Subscription> sub  = self.CreateSubscription(period, callback);
  return boost::shared_ptr<Subscription>(sub.release());
}

static Node UaServer_GetNode(UaServer & self, ObjectId objectid)
{
  return self.GetNode(NodeId(objectid));
}


//--------------------------------------------------------------------------
// module
//--------------------------------------------------------------------------

BOOST_PYTHON_MODULE(opcua)
{
  PyDateTime_IMPORT; 

  using self_ns::str; // hack to enable __str__ in python classes with str(self)

  PyEval_InitThreads();

  py_opcua_enums();

  DateTimePythonToOpcUaConverter(); 
  PythonStringToLocalizedTextConverter(); 
  to_python_converter<LocalizedText, LocalizedTextToPythonConverter>(); 
  // Enable next line to return PyDateTime instead og DateTime in python
  //to_python_converter<DateTime, DateTimeOpcUaToPythonConverter>(); 



  to_python_converter<std::vector<std::string>, vector_to_python_converter<std::string>>();
  vector_from_python_converter<std::string>();

  variant_from_python_converter();
  to_python_converter<Variant, variant_to_python_converter>();

  class_<DateTime>("DateTime", init<>())
  .def(init<int64_t>())
  .def("now", &DateTime::DateTime::Current)
  .def("__init__", make_constructor(makeOpcUaDateTime))
  .def("from_time_t", &DateTime::FromTimeT, DateTimeFromTimeT_stub((arg("sec"), arg("usec")=0)))
  .staticmethod("from_time_t")
  .def("to_datetime", &ToPyDateTime)
  .def("to_time_t", &DateTime::ToTimeT)
  .def_readwrite("value", &DateTime::Value)
  ;

  //class_<LocalizedText>("LocalizedText")
  //.def_readwrite("Encoding", &LocalizedText::Encoding)
  //.def_readwrite("Locale", &LocalizedText::Locale)
  //.def_readwrite("Text", &LocalizedText::Text)
  //;

  
  class_<NodeId, boost::shared_ptr<NodeId>>("NodeId")
  .def(init<uint32_t, uint16_t>())
  .def(init<std::string, uint16_t>())
  .def("__init__", make_constructor(NodeId_constructor)) // XXX add this constructor to freeopcua
  .add_property("namespace_index", &NodeId::GetNamespaceIndex)
  .add_property("identifier", &NodeId_GetIdentifier)
  .add_property("encoding", &NodeId::GetEncodingValue)
  .add_property("is_integer", &NodeId::IsInteger)
  .add_property("is_binary", &NodeId::IsBinary)
  .add_property("is_guid", &NodeId::IsGuid)
  .add_property("is_string", &NodeId::IsString)
  .def_readonly("namespace_uri", &NodeId::NamespaceURI)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  to_python_converter<std::vector<QualifiedName>, vector_to_python_converter<QualifiedName>>();
  to_python_converter<std::vector<std::vector<QualifiedName>>, vector_to_python_converter<std::vector<QualifiedName>>>();
  class_<QualifiedName>("QualifiedName")
  .def(init<uint16_t, std::string>())
  .def(init<std::string, uint16_t>()) // XXX A.D.D, right
  //.def("parse", &ToQualifiedName)      XXX def()
  .def_readwrite("namespace_index", &QualifiedName::NamespaceIndex)
  .def_readwrite("name", &QualifiedName::Name)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  class_<DataValue, boost::shared_ptr<DataValue>>("DataValue")
  .def(init<const Variant &>())
  .def("__init__", make_constructor(DataValue_constructor1))  // Variant, VariantType
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
  .def_readwrite_vector("discovery_urls", &ApplicationDescription::DiscoveryURLs)
  ;

  to_python_converter<std::vector<ApplicationDescription>, vector_to_python_converter<ApplicationDescription>>();

  class_<UserTokenPolicy>("UserTokenPolicy")
  .def_readwrite("policy_id", &UserTokenPolicy::PolicyId)
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
  .def_readwrite("security_mode", &EndpointDescription::SecurityMode)
  .def_readwrite("security_policy_uri", &EndpointDescription::SecurityPolicyURI)
  .def_readwrite_vector("user_identify_tokens", &EndpointDescription::UserIdentifyTokens)
  .def_readwrite("transport_profile_uri", &EndpointDescription::TransportProfileURI)
  .def_readwrite("security_level", &EndpointDescription::SecurityLevel)
  ;

  to_python_converter<std::vector<EndpointDescription>, vector_to_python_converter<EndpointDescription>>();

  class_<ReferenceDescription>("ReferenceDescription")
  .def_readwrite("reference_type_id", &ReferenceDescription::ReferenceTypeId)
  .def_readwrite("is_forward", &ReferenceDescription::IsForward)
  .def_readwrite("target_node_id", &ReferenceDescription::TargetNodeId)
  .def_readwrite("browse_name", &ReferenceDescription::BrowseName)
  .def_readwrite("display_name", &ReferenceDescription::DisplayName)
  .def_readwrite("target_node_class", &ReferenceDescription::TargetNodeClass)
  .def_readwrite("target_node_type_definition", &ReferenceDescription::TargetNodeTypeDefinition)
  ;

  to_python_converter<std::vector<ReferenceDescription>, vector_to_python_converter<ReferenceDescription>>();

  class_<ReadValueId>("ReadValueId")
  .def_readwrite("node_id", &ReadValueId::NodeId)
  .def_readwrite("attribute_id", &ReadValueId::AttributeId)
  .def_readwrite("index_range", &ReadValueId::IndexRange)
  .def_readwrite("data_encoding", &ReadValueId::DataEncoding)
  ;

  to_python_converter<std::vector<ReadValueId>, vector_to_python_converter<ReadValueId>>();

  class_<WriteValue>("WriteValue")
  .def_readwrite("node_id", &WriteValue::NodeId)
  .def_readwrite("attribute_id", &WriteValue::AttributeId)
  .def_readwrite("index_range", &WriteValue::IndexRange)
  .def_readwrite("value", &WriteValue::Value)
  ;

  to_python_converter<std::vector<WriteValue>, vector_to_python_converter<WriteValue>>();
  vector_from_python_converter<WriteValue>();

//  XXX
//  class_<Variant>("Variant")
//  .def("value", &VariantValue)
//  .def("type", &Variant::Type)
//  .def("is_array", &Variant::IsArray)
//  .def("is_scalar", &Variant::IsScalar)
//  .def("is_null", &Variant::IsNul)
//  ;

  class_<Node>("Node", init<Services::SharedPtr, NodeId>())
  .def(init<Node>())
  .def("get_id", &Node::GetId)
  .def("get_attribute", &Node::GetAttribute)
  .def("set_attribute", &Node::SetAttribute)
  .def("get_value", &Node::GetValue)
  .def("set_value", (void(Node::*)(const DataValue &) const) &Node::SetValue)
  .def("set_value", (void(Node::*)(const Variant &) const) &Node::SetValue)
  .def("set_value", &Node_SetValue)
  .def("get_properties", &Node::GetProperties)
  .def("get_variables", &Node::GetVariables)
  .def("get_browse_name", &Node::GetBrowseName)
  .def("get_children", (std::vector<Node> (Node::*)() const) &Node::GetChildren)
  .def("get_child", (Node(Node::*)(const std::vector<std::string> &) const) &Node::GetChild)
  .def("get_child", (Node(Node::*)(const std::string &) const) &Node::GetChild)
  .def("add_folder", (Node(Node::*)(const NodeId &, const QualifiedName &) const) &Node::AddFolder)
  .def("add_folder", (Node(Node::*)(const std::string &, const std::string &) const) &Node::AddFolder)
  .def("add_folder", (Node(Node::*)(uint32_t, const std::string &) const) &Node::AddFolder)
  .def("add_object", (Node(Node::*)(const NodeId &, const QualifiedName &) const) &Node::AddObject)
  .def("add_object", (Node(Node::*)(const std::string &, const std::string &) const) &Node::AddObject)
  .def("add_object", (Node(Node::*)(uint32_t, const std::string &) const) &Node::AddObject)
  .def("add_variable", (Node(Node::*)(const NodeId &, const QualifiedName &, const Variant &) const) &Node::AddVariable)
  .def("add_variable", (Node(Node::*)(const std::string &, const std::string &, const Variant &) const) &Node::AddVariable)
  .def("add_variable", (Node(Node::*)(uint32_t, const std::string &, const Variant &) const) &Node::AddVariable)
  .def("add_property", (Node(Node::*)(const NodeId &, const QualifiedName &, const Variant &) const) &Node::AddProperty)
  .def("add_property", (Node(Node::*)(const std::string &, const std::string &, const Variant &) const) &Node::AddProperty)
  .def("add_property", (Node(Node::*)(uint32_t, const std::string &, const Variant &) const) &Node::AddProperty)
  .def(str(self))
  .def(repr(self))
  .def(self == self)
  ;

  to_python_converter<std::vector<Node>, vector_to_python_converter<Node>>();
  vector_from_python_converter<Node>();
  
  class_<SubscriptionHandler, PySubscriptionHandler, boost::noncopyable>("SubscriptionHandler", init<>())
  .def("data_change", &PySubscriptionHandler::DefaultDataChange)
  .staticmethod("data_change")
  .def("event", &PySubscriptionHandler::DefaultEvent)
  .staticmethod("event")
  .def("status_change", &PySubscriptionHandler::DefaultStatusChange)
  .staticmethod("status_change")
  ;

  class_<Event>("Event", init<const NodeId &>())
  .def(init<>())
  .def("get_value", (Variant(Event::*)(const std::string &) const) &Event::GetValue)
  .def("set_value", (void (Event::*)(const std::string &, Variant)) &Event::SetValue)
  .def("get_value_keys", &Event::GetValueKeys)
  .def_readwrite("event_id", &Event::EventId)
  .def_readwrite("event_type", &Event::EventType)
  .def_readwrite("local_time", &Event::LocalTime)
  .add_property("message", make_getter(&Event::Message, return_value_policy<return_by_value>()), make_setter(&Event::Message, return_value_policy<return_by_value>()))
  .def_readwrite("receive_time", &Event::ReceiveTime)
  .def_readwrite("severity", &Event::Severity)
  .def_readwrite("source_name", &Event::SourceName)
  .def_readwrite("source_node", &Event::SourceNode)
  .def_readwrite("time", &Event::Time)
  .def(str(self))
  .def(repr(self))
  ;

  class_<Subscription, boost::shared_ptr<Subscription>, boost::noncopyable>("Subscription", no_init)
  .def("subscribe_data_change", (uint32_t (Subscription::*)(const Node &, AttributeId)) &Subscription::SubscribeDataChange, SubscriptionSubscribeDataChange_stubs((arg("node"), arg("attr") = AttributeId::Value)))
  .def("delete", &Subscription::Delete)
  .def("unsubscribe", (void (Subscription::*)(uint32_t)) &Subscription::UnSubscribe)
  .def("subscribe_events", (uint32_t (Subscription::*)()) &Subscription::SubscribeEvents)
  .def("subscribe_events", (uint32_t (Subscription::*)(const Node &, const Node &)) &Subscription::SubscribeEvents)
  //.def(str(self))
  //.def(repr(self))
  ;

  class_<UaClient, boost::noncopyable>("Client", init<>())
  .def(init<bool>())
  .def("connect", (void (UaClient::*)(const std::string&)) &UaClient::Connect)
  .def("connect", (void (UaClient::*)(const EndpointDescription&)) &UaClient::Connect)
  .def("disconnect", &UaClient::Disconnect)
  .def("get_namespace_index", &UaClient::GetNamespaceIndex)
  .def("get_root_node", &UaClient::GetRootNode)
  .def("get_objects_node", &UaClient::GetObjectsNode)
  .def("get_server_node", &UaClient::GetServerNode)
  .def("get_node", (Node(UaClient::*)(const std::string&) const) &UaClient::GetNode)
  .def("get_node", (Node(UaClient::*)(const NodeId&) const) &UaClient::GetNode)
  .def("get_node", &UaClient_GetNode)
  .def("get_endpoint", &UaClient::GetEndpoint)
  .def("get_server_endpoints", (std::vector<EndpointDescription> (UaClient::*)(const std::string&)) &UaClient::GetServerEndpoints)
  .def("get_server_endpoints", (std::vector<EndpointDescription> (UaClient::*)()) &UaClient::GetServerEndpoints)
  .def("set_session_name", &UaClient::SetSessionName)
  .def("get_session_name", &UaClient::GetSessionName)
  .def("get_application_uri", &UaClient::GetApplicationURI)
  .def("set_application_uri", &UaClient::SetApplicationURI)
  .def("set_security_policy", &UaClient::SetSecurityPolicy)
  .def("get_security_policy", &UaClient::GetSecurityPolicy)
  .def("create_subscription", &UaClient_CreateSubscription)
  //.def(str(self))
  //.def(repr(self))
  ;

  class_<UaServer, boost::noncopyable >("Server", init<>())
  .def(init<bool>())
  .def("start", &UaServer::Start)
  .def("stop", &UaServer::Stop)
  .def("register_namespace", &UaServer::RegisterNamespace)
  .def("get_namespace_index", &UaServer::GetNamespaceIndex)
  .def("get_root_node", &UaServer::GetRootNode)
  .def("get_objects_node", &UaServer::GetObjectsNode)
  .def("get_server_node", &UaServer::GetServerNode)
  .def("get_node", (Node(UaServer::*)(const std::string&) const) &UaServer::GetNode)
  .def("get_node", (Node(UaServer::*)(const NodeId&) const) &UaServer::GetNode)
  .def("get_node", &UaServer_GetNode)
  .def("set_uri", &UaServer::SetServerURI)
  .def("add_xml_address_space", &UaServer::AddAddressSpace)
  .def("set_server_name", &UaServer::SetServerName)
  .def("set_endpoint", &UaServer::SetEndpoint)
  .def("create_subscription", &UaServer_CreateSubscription)
  .def("trigger_event", &UaServer::TriggerEvent)
  //.def(str(self))
  //.def(repr(self))
  ;

}

