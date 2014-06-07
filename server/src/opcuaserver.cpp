#include <algorithm>
#include <opc/common/addons_core/config_file.h>
#include <opc/ua/server/standard_namespace.h>


#include <opc/ua/node.h>

#include <opc/ua/opcuaserver.h>

#include <iostream>

namespace OpcUa
{
  OPCUAServer::OPCUAServer()
  {
  }

  void OPCUAServer::Start()
  {
    Registry = UaServer::CreateServicesRegistry();
    TcpServer = UaServer::CreateTcpServer();
    EndpointsServices = UaServer::CreateEndpointsServices(Registry);
   
    std::vector<ApplicationDescription> Applications;
    ApplicationDescription appdesc;
    appdesc.Name = LocalizedText(Name);
    appdesc.URI = Uri;
    appdesc.Type = ApplicationType::SERVER;
    Applications.push_back(appdesc);
    std::vector<EndpointDescription> Endpoints;
    EndpointDescription ed;
    ed.EndpointURL = Endpoint;
    ed.SecurityMode = SecurityMode;
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

  }
  
  Node OPCUAServer::GetNode(NodeID nodeid)
  {
    return Node(Registry->GetServer(), nodeid);
  }

  void OPCUAServer::Stop()
  {
    std::cout << "Stopping opcua server application" << std::endl;
  }

  Node OPCUAServer::GetRootNode()
  {
    return GetNode(OpcUa::ObjectID::RootFolder);
  }

  Node OPCUAServer::GetObjectsNode()
  {
    return GetNode(OpcUa::ObjectID::ObjectsFolder);
  }
}
