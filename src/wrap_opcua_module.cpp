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
#include "wrap_opcua_variant.h"

using namespace boost::python;
using namespace OpcUa;

//--------------------------------------------------------------------------
// Overloads
//--------------------------------------------------------------------------

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SubscriptionSubscribeDataChange_stubs, Subscription::SubscribeDataChange, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeGetName_stubs, Node::GetName, 0, 1);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(NodeSetValue_stubs, Node::SetValue, 1, 2);
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OPCUAServerSetLoadCppAddressSpace_stubs, OPCUAServer::SetLoadCppAddressSpace, 0, 1);


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
// RemoteClient helpers
//--------------------------------------------------------------------------


Subscription * RemoteClient_CreateSubscription(RemoteClient & self, uint period, PySubscriptionClient & callback)
{
  std::unique_ptr<OpcUa::Subscription> sub  = self.CreateSubscription(period, callback);
  Subscription * psub = sub.release(); // XXX ownership
  return psub;
}

//--------------------------------------------------------------------------
// OPCUAServer helpers
//--------------------------------------------------------------------------

Subscription * OPCUAServer_CreateSubscription(OPCUAServer & self, uint period, PySubscriptionClient & callback)
{
  std::unique_ptr<OpcUa::Subscription> sub  = self.CreateSubscription(period, callback);
  Subscription * psub = sub.release(); // XXX ownership
  return psub;
}

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

  variant_from_python_converter();
  to_python_converter<Variant, variant_to_python_converter>();

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
  .def(init<std::string, uint16_t>()) // XXX A.D.D, right
  //.def("parse", &ToQualifiedName)      XXX could be def(), useless
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
  //.def_readwrite("discovery_urls", &ApplicationDescription::DiscoveryURLs) XXX getter ok, setter not
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
  .def_readwrite("security_mode", &EndpointDescription::SecurityMode)
  .def_readwrite("security_policy_uri", &EndpointDescription::SecurityPolicyURI)
  //.def_readwrite("user_identify_tokens", &EndpointDescription::UserIdentifyTokens) XXX getter ok, setter not
  .add_vector_property("user_identify_tokens", EndpointDescription, UserTokenPolicy, UserIdentifyTokens)
  .def_readwrite("transport_profile_uri", &EndpointDescription::TransportProfileURI)
  .def_readwrite("security_level", &EndpointDescription::SecurityLevel)
  ;

  to_python_converter<std::vector<EndpointDescription>, vector_to_python_converter<EndpointDescription>>();

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

//  XXX
//  class_<Variant>("Variant")
//  .def_readonly("value", &PyVariant::Value)
//  .def("type", &PyVariant::Type)
//  .def("is_null", &PyVariant::IsNull)
//  ;

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

  class_<Subscription, Subscription *, boost::noncopyable>("Subscription", no_init)
  .def("subscribe_data_change", (uint32_t (Subscription::*)(const Node &, AttributeID)) &Subscription::SubscribeDataChange, SubscriptionSubscribeDataChange_stubs((arg("node"), arg("attr") = AttributeID::VALUE)))
  .def("delete", &Subscription::Delete)
  .def("unsubscribe", (void (Subscription::*)(uint32_t)) &Subscription::UnSubscribe)
  .def("subscribe_events", (uint32_t (Subscription::*)()) &Subscription::SubscribeEvents)
  .def("subscribe_events", (uint32_t (Subscription::*)(const Node &, const Node &)) &Subscription::SubscribeEvents)
  ;

  class_<RemoteClient, boost::noncopyable>("Client", init<>())
  .def(init<bool>())
  .def("connect", &RemoteClient::Connect)
  .def("disconnect", &RemoteClient::Disconnect)
  .def("get_root_node", &RemoteClient::GetRootNode)
  .def("get_objects_node", &RemoteClient::GetObjectsNode)
  .def("get_server_node", &RemoteClient::GetServerNode)
  .def("get_node", &RemoteClient::GetNode)
  .def("set_endpoint", &RemoteClient::SetEndpoint)
  .def("get_endpoint", &RemoteClient::GetEndpoint)
  .def("set_session_name", &RemoteClient::SetSessionName)
  .def("get_session_name", &RemoteClient::GetSessionName)
  .def("get_uri", &RemoteClient::GetURI)
  .def("set_uri", &RemoteClient::SetURI)
  .def("set_security_policy", &RemoteClient::SetSecurityPolicy)
  .def("get_security_policy", &RemoteClient::GetSecurityPolicy)
  .def("create_subscription", &RemoteClient_CreateSubscription, return_value_policy<reference_existing_object>())
  ;

  class_<OPCUAServer, boost::noncopyable >("Server", init<>())
  .def(init<bool>())
  .def("start", &OPCUAServer::Start)
  .def("stop", &OPCUAServer::Stop)
  .def("get_root_node", &OPCUAServer::GetRootNode)
  .def("get_objects_node", &OPCUAServer::GetObjectsNode)
  .def("get_server_node", &OPCUAServer::GetServerNode)
  .def("get_node", &OPCUAServer::GetNode)
  //.def("get_node_from_path", (Node (OPCUAServer::*)(const std::vector<QualifiedName>&)) &OPCUAServer::GetNodeFromPath) XXX
  //.def("get_node_from_path", (Node (OPCUAServer::*)(const std::vector<std::string>&)) &OPCUAServer::GetNodeFromPath) XXX
  .def("set_uri", &OPCUAServer::SetServerURI)
  .def("add_xml_address_space", &OPCUAServer::AddAddressSpace)
  .def("set_server_name", &OPCUAServer::SetServerName)
  .def("set_endpoint", &OPCUAServer::SetEndpoint)
  .def("load_cpp_addressspace", &OPCUAServer::SetLoadCppAddressSpace, OPCUAServerSetLoadCppAddressSpace_stubs((arg("val") = true)))
  .def("create_subscription", &OPCUAServer_CreateSubscription, return_value_policy<reference_existing_object>())
  ;

}

