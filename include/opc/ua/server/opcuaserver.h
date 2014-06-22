#ifndef OPCUASERVER_H
#define OPCUASERVER_H

#include<string>

#include <opc/common/addons_core/config_file.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <opc/ua/node.h>
#include <opc/ua/server.h>
#include <opc/ua/server/tcp_server.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/server/endpoints_services.h>
#include <opc/ua/server/opcua_protocol.h>
#include <opc/ua/server/services_registry.h>


namespace OpcUa
{
  class OPCUAServer
  {
    public:
      OPCUAServer(); 
      void SetEndpoint(const std::string endpoint){this->Endpoint = endpoint;}
      void SetURI(const std::string uri){this->Uri = uri;}
      void SetServerName(const std::string name){this->Name = name;}
      void AddAddressSpace(const std::string path) {xml_address_spaces.push_back(path);}
      void SetLoadCppAddressSpace(bool val=true){loadCppAddressSpace = val;}

      void Start();
      void Stop();
      //Node GetNode(std::vector<QualifiedName> browsepath); //Do we need that? or should we use rootnode anyway?
      //Node GetNode(std::vector<std::string> browsepath);
      Node GetRootNode();
      Node GetObjectsNode();
      Node GetNode(NodeID nodeid);
      Node GetNodeFromPath(const std::vector<QualifiedName>& path) {return GetRootNode().GetChild(path);}
      Node GetNodeFromPath(const std::vector<std::string>& path) {return GetRootNode().GetChild(path);}

    protected:
      std::vector<std::string> xml_address_spaces;
      std::string config_path = "";
      std::string Endpoint = "opc.tcp://localhost:4841"; //This is the expected address of an OPC-UA server on a machine
      std::string Uri = "freeopcua.github.io";
      std::string Name = "Open Source OPC-UA Server";
      bool Debug = false;
      OpcUa::MessageSecurityMode SecurityMode = OpcUa::MessageSecurityMode::MSM_NONE;
      bool loadCppAddressSpace = true; //Always true as long as we have not fixed the loading of xml addressspace

      UaServer::ServicesRegistry::SharedPtr Registry;
      UaServer::TcpServer::SharedPtr TcpServer;
      UaServer::EndpointsRegistry::SharedPtr EndpointsServices;
      UaServer::AddressSpace::SharedPtr AddressSpace;
      UaServer::OpcUaProtocol::SharedPtr Protocol;
  };

}

#endif
