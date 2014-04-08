

//test
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
//#include <boost/python/module.hpp>
//#include <boost/python/def.hpp>
//#include <boost/python/implicit.hpp>
//test

#include <boost/python.hpp>
#include <boost/python/type_id.hpp>
#include <functional>

#include <opc/ua/node.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/client/client.h>
#include <opc/ua/opcuaserver.h>
#include <opc/ua/computer.h>



namespace OpcUa
{


  BOOST_PYTHON_MODULE(libopcua)
  {
    using namespace boost::python;

    class_<QualifiedName>("QualifiedName", init<uint, std::string>())
        .def_readwrite("namespace_index", &QualifiedName::NamespaceIndex)
        .def_readwrite("name", &QualifiedName::Name)
      ;

    class_<NodeID>("NodeId" )
          .def("from_numeric", &NumericNodeID)
          .def("get_string_identifier", &NodeID::GetStringIdentifier)
          .def("get_namespace_index", &NodeID::GetNamespaceIndex)
      ;

    std::vector<Node> (Node::*NodeBrowse)() = &Node::Browse;
    Node (Node::*NodeGetChildNode)(const std::vector<std::string>&) = &Node::GetChildNode;

    class_<Node>("Node", init<Remote::Computer::SharedPtr, NodeID>())
          .def(init<Node>())
          .def("get_node_id", &Node::GetNodeId)
          .def("read", &Node::Read)
          .def("read_value", &Node::ReadValue)
          .def("write", &Node::Write)
          .def("write_value", &Node::WriteValue)
          .def("get_properties", &Node::GetProperties)
          .def("get_variables", &Node::GetVariables)
          .def("get_browse_name", &Node::GetBrowseName)
          .def("browse", NodeBrowse)
          .def("get_child", NodeGetChildNode)
          .def("__str__", &Node::ToString)
          .def("__repr__", &Node::ToString)
      ;

    class_<std::vector<Node> >("NodeVector")
        .def(vector_indexing_suite<std::vector<Node> >())
    ;

    class_<std::vector<std::string> >("StringVector")
        .def(vector_indexing_suite<std::vector<std::string> >())
    ;


    class_<Client::Client, boost::noncopyable>("Client")
          .def("connect", &Client::Client::Connect)
          .def("disconnect", &Client::Client::Disconnect)
          .def("get_root_node", &Client::Client::GetRootNode)
          .def("get_node", &Client::Client::GetNode)
          .def("set_endpoint", &Client::Client::SetEndpoint)
          .def("get_endpoint", &Client::Client::GetEndpoint)
          .def("get_objects_node", &Client::Client::GetObjectsNode)
          .def("set_session_name", &Client::Client::SetSessionName)
          .def("get_session_name", &Client::Client::GetSessionName)
          .def("get_uri", &Client::Client::GetURI)
          .def("set_uri", &Client::Client::SetURI)
          .def("set_security_policy", &Client::Client::SetSecurityPolicy)
          .def("get_security_policy", &Client::Client::GetSecurityPolicy)
      ;


    Node (OPCUAServer::*NodeFromPathString)(const std::vector<std::string>&) = &OPCUAServer::GetNodeFromPath;
    Node (OPCUAServer::*NodeFromPathQN)(const std::vector<QualifiedName>&) = &OPCUAServer::GetNodeFromPath;
    class_<OPCUAServer, boost::noncopyable >("Server" )
          .def("start", &OPCUAServer::Start)
          .def("stop", &OPCUAServer::Stop)
          .def("get_root_node", &OPCUAServer::GetRootNode)
          .def("get_objects_node", &OPCUAServer::GetObjectsNode)
          .def("get_node", &OPCUAServer::GetNode)
          .def("get_node_from_path", NodeFromPathString)
          .def("get_node_from_qn_path", NodeFromPathQN)
          .def("set_config_file", &OPCUAServer::SetConfigFile)
          .def("set_uri", &OPCUAServer::SetURI)
          .def("add_xml_address_space", &OPCUAServer::AddAddressSpace)
          .def("set_server_name", &OPCUAServer::SetServerName)
          .def("set_endpoint", &OPCUAServer::SetEndpoint)
      ;


  }
}
