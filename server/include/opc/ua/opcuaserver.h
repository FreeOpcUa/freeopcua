#ifndef OPCUASERVER_H
#define OPCUASERVER_H

#include<string>

#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opc/ua/server/tcp_server.h>
#include <opc/ua/server/addons/address_space_registry.h>
#include <opc/ua/server/endpoints_services.h>
#include <opc/ua/server/addons/opcua_protocol.h>

#include <opc/ua/server/addons/services_registry.h>
#include <opc/common/addons_core/config_file.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <opc/ua/server.h>
#include <opc/ua/node.h>


namespace OpcUa
{
  class OPCUAServer
  {
    public:
      OPCUAServer(); 
      void SetEndpoint(const std::string endpoint){this->endpoint = endpoint;}
      void SetURI(const std::string uri){this->uri = uri;}
      void SetServerName(const std::string name){this->name = name;}
      void AddAddressSpace(const std::string path) {xml_address_spaces.push_back(path);}
      void SetLoadCppAddressSpace(bool val=true){loadCppAddressSpace = val;}

      void Start();
      void Stop();
      //Node GetNode(std::vector<QualifiedName> browsepath); //Do we need that? or should we use rootnode anyway?
      //Node GetNode(std::vector<std::string> browsepath);
      Node GetRootNode();
      Node GetObjectsNode();
      Node GetNode(NodeID nodeid);
      void SetConfigFile(const std::string path);
      Node GetNodeFromPath(const std::vector<QualifiedName>& path) {return GetRootNode().GetChild(path);}
      Node GetNodeFromPath(const std::vector<std::string>& path) {return GetRootNode().GetChild(path);}

    protected:
      OpcUa::Remote::Server::SharedPtr Server; //To remove
      std::vector<std::string> xml_address_spaces;
      std::string config_path = "";
      std::string endpoint = "opc.tcp://localhost:4841"; //This is the expected address of an OPC-UA server on a machine
      std::string uri = "opcua.treww.org";
      std::string name = "Open Source OPC-UA Server";
      std::string security_mode = "none"; //should be correct class
      bool loadCppAddressSpace = false;


      UaServer::ServicesRegistryAddon::SharedPtr Registry;
      UaServer::TcpServerAddon::SharedPtr TcpServer;
      UaServer::EndpointsServicesAddon::SharedPtr EndpointsServices;
      UaServer::AddressSpace::SharedPtr AddressSpace;
      Common::Addon::UniquePtr Protocol;
      
  };

}

#endif
