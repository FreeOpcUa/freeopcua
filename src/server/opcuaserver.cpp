/******************************************************************************
 *   Copyright (C) 2013-2014 by Olivier Roulet-Dubonnet                       *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#include "opc_tcp_async_parameters.h"

#include <opc/common/uri_facade.h>
#include <opc/common/addons_core/config_file.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/standard_namespace.h>
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
    asyncparams.Port = Common::Uri(Endpoints[0].EndpointURL).Port();
    asyncparams.Host = Common::Uri(Endpoints[0].EndpointURL).Host();
    asyncparams.ThreadsNumber = 2;
    asyncparams.DebugMode = Debug;
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

  std::unique_ptr<Subscription> OPCUAServer::CreateSubscription(uint period, SubscriptionClient& callback)
  {
    SubscriptionParameters params;
    params.RequestedPublishingInterval = period;
    return std::unique_ptr<Subscription>(new Subscription (Registry->GetServer(), params, callback, Debug));
  }


  void OPCUAServer::TriggerEvent(Event event)
  {
    AddressSpace->TriggerEvent(ObjectID::Server, event);
  }

}
