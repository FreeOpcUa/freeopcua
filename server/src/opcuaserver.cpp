#include <algorithm>
#include <opc/common/addons_core/config_file.h>
#include <opc/ua/server/standard_namespace.h>


#include <opc/ua/node.h>

#include <opc/ua/opcuaserver.h>

#include <iostream>

namespace OpcUa
{
  void OPCUAServer::SetConfigFile(const std::string path)
  {
    config_path = path;
  }

  OPCUAServer::OPCUAServer()
  {
  }

  void OPCUAServer::Start()
  {
    UaServer::ServicesRegistryAddon::SharedPtr Registry = UaServer::CreateServicesRegistry();

    UaServer::TcpServerAddon::SharedPtr TcpServer = UaServer::CreateTcpServer();
    EndpointsServices = UaServer::CreateEndpointsServices(Registry);
   
    std::vector<ApplicationDescription> Applications;
    ApplicationDescription appdesc;
    appdesc.Name = LocalizedText(name);
    appdesc.URI = uri;
    appdesc.Type = ApplicationType::SERVER;
    Applications.push_back(appdesc);
    std::vector<EndpointDescription> Endpoints;
    EndpointDescription ed;
    ed.EndpointURL = endpoint;
    ed.SecurityMode = MessageSecurityMode::MSM_NONE;
    ed.SecurityPolicyURI = "http://opcfoundation.org/UA/SecurityPolicy#None";
    ed.TransportProfileURI = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
    Endpoints.push_back(ed);
      

    AddressSpace = UaServer::CreateAddressSpace(); 
    Registry->RegisterViewServices(AddressSpace);
    Registry->RegisterAttributeServices(AddressSpace);
    Registry->RegisterNodeManagementServices(AddressSpace);
    Registry->RegisterSubscriptionServices(AddressSpace);

    UaServer::CreateStandardNamespace(Registry->GetServer()->NodeManagement());

    Protocol = UaServer::CreateOpcUaProtocol(Registry, TcpServer, Endpoints);
    Server = Registry->GetServer(); //Not necessary when we have a link to Registry anyway

 
  }
  
  Node OPCUAServer::GetNode(NodeID nodeid)
  {
    return Node(Server, nodeid);
  }

  void OPCUAServer::Stop()
  {
    std::cout << "Stopping opcua server application" << std::endl;
  }

  Node OPCUAServer::GetRootNode()
  {
    return Node(Server, OpcUa::ObjectID::RootFolder);
  }

  Node OPCUAServer::GetObjectsNode()
  {
    return Node(Server, OpcUa::ObjectID::ObjectsFolder);
  }
}
