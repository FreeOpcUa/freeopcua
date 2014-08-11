#include <opc/common/addons_core/config_file.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/standard_namespace.h>
#include "opc_tcp_async_parameters.h"
#include <opc/ua/node.h>

#include <opc/ua/server/opcuaserver.h>

#include <iostream>
#include <algorithm>

namespace OpcUa
{

  OPCUAServer::OPCUAServer()
  {
  }

  void OPCUAServer::Start()
  {
    EndpointsServices = UaServer::CreateEndpointsRegistry();
   
    std::vector<ApplicationDescription> Apps;
    ApplicationDescription appdesc;
    appdesc.Name = LocalizedText(Name);
    appdesc.URI = ServerUri;
    appdesc.Type = ApplicationType::SERVER;
    appdesc.ProductURI = ProductUri;

    Apps.push_back(appdesc);

    std::vector<EndpointDescription> Endpoints;
    EndpointDescription ed;
    ed.ServerDescription = appdesc;
    ed.EndpointURL = Endpoint;
    ed.SecurityMode = SecurityMode;
    ed.SecurityPolicyURI = "http://opcfoundation.org/UA/SecurityPolicy#None";
    ed.TransportProfileURI = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";

    Endpoints.push_back(ed);

    EndpointsServices->AddApplications(Apps);
    EndpointsServices->AddEndpoints(Endpoints);

    Registry = UaServer::CreateServicesRegistry();
    Registry->RegisterEndpointsServices(EndpointsServices);

    AddressSpace = UaServer::CreateAddressSpace(Debug);
    Registry->RegisterViewServices(AddressSpace);
    Registry->RegisterAttributeServices(AddressSpace);
    Registry->RegisterNodeManagementServices(AddressSpace);
    Registry->RegisterSubscriptionServices(AddressSpace);

    UaServer::FillStandardNamespace(*Registry->GetServer()->NodeManagement(), Debug);


    UaServer::AsyncOpcTcp::Parameters asyncparams;
    asyncparams.Port = 4841; //FIXME!!!!
    asyncparams.ThreadsNumber = 2;
    asyncparams.DebugMode = true;
    AsyncServer = UaServer::CreateAsyncOpcTcp(asyncparams, Registry->GetServer());
    

    ListenThread.reset(new Common::Thread([this](){
          AsyncServer->Listen();
     }));


    //TcpServer = UaServer::CreateTcpServer();
    //Protocol = UaServer::CreateOpcUaProtocol(TcpServer, Debug);
    //Protocol->StartEndpoints(Endpoints, Registry->GetServer());
  }
  
  Node OPCUAServer::GetNode(const NodeID& nodeid)
  {
    return Node(Registry->GetServer(), nodeid);
  }

  void OPCUAServer::Stop()
  {
    std::cout << "Stopping opcua server application" << std::endl;
    AsyncServer->Shutdown();
    ListenThread->Join();
    AsyncServer.reset();
 
  }

  Node OPCUAServer::GetRootNode()
  {
    return GetNode(OpcUa::ObjectID::RootFolder);
  }

  Node OPCUAServer::GetObjectsNode()
  {
    return GetNode(ObjectID::ObjectsFolder);
  }

  Subscription OPCUAServer::CreateSubscription(uint period, SubscriptionClient& callback)
  {
    SubscriptionParameters params;
    params.RequestedPublishingInterval = period;
    return Subscription(Registry->GetServer(), params, callback);
  }


  void OPCUAServer::TriggerEvent(Event event)
  {
    AddressSpace->TriggerEvent(ObjectID::Server, event);
  }

}
