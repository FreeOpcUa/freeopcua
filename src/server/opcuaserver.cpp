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
#include <opc/ua/model.h>
#include <opc/ua/node.h>
#include <opc/ua/server/opcuaserver.h>

#include <iostream>
#include <algorithm>

namespace OpcUa
{

  OPCUAServer::OPCUAServer()
  {
  }

  OPCUAServer::OPCUAServer(bool debug)
    : Debug(debug)
  {
  }

  void OPCUAServer::Start()
  {
    ServerWork.reset(new boost::asio::io_service::work(IoService));
    EndpointsServices = Server::CreateEndpointsRegistry();
   
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
    //setting up policy is required for some client, this should be in a constructor
    UserTokenPolicy policy;
    policy.TokenType = UserIdentifyTokenType::ANONYMOUS;
    ed.UserIdentifyTokens.push_back(policy);

    Endpoints.push_back(ed);

    EndpointsServices->AddApplications(Apps);
    EndpointsServices->AddEndpoints(Endpoints);

    Registry = Server::CreateServicesRegistry();
    Registry->RegisterEndpointsServices(EndpointsServices);

    AddressSpace = Server::CreateAddressSpace(Debug);
    SubscriptionService = Server::CreateSubscriptionService(AddressSpace, IoService, Debug);
    Registry->RegisterViewServices(AddressSpace);
    Registry->RegisterAttributeServices(AddressSpace);
    Registry->RegisterNodeManagementServices(AddressSpace);
    Registry->RegisterSubscriptionServices(SubscriptionService);

    Server::FillStandardNamespace(*Registry->GetServer()->NodeManagement(), Debug);
    CreateServerObjectNode();


    const Common::Uri uri(Endpoints[0].EndpointURL);
    Server::AsyncOpcTcp::Parameters asyncparams;
    asyncparams.Host = uri.Host();
    asyncparams.Port = uri.Port();
    asyncparams.DebugMode = Debug;
    AsyncServer = Server::CreateAsyncOpcTcp(asyncparams, Registry->GetServer(), IoService);
    AsyncServer->Listen();
    ListenThread.reset(new Common::Thread([this](){
      Run();
    }));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  void OPCUAServer::Run()
  {
    try
    {
      IoService.run();
    }
    catch (const std::exception& exc)
    {
      std::cout << exc.what() << std::endl;
    }
    std::cout << "OPCUAServer| service thread exited." << std::endl;
  }
  
  Node OPCUAServer::GetNode(const NodeID& nodeid) const
  {
    return Node(Registry->GetServer(), nodeid);
  }

  Node OPCUAServer::GetNodeFromPath(const std::vector<QualifiedName>& path) const
  {
    return GetRootNode().GetChild(path);
  }

  Node OPCUAServer::GetNodeFromPath(const std::vector<std::string>& path) const
  {
    return GetRootNode().GetChild(path);
  }

  void OPCUAServer::Stop()
  {
    std::cout << "Stopping opcua server application" << std::endl;
    AsyncServer->Shutdown();
    AsyncServer.reset();
    ServerWork.reset();
    IoService.stop();
    ListenThread->Join();
  }

  Node OPCUAServer::GetRootNode() const
  {
    return GetNode(OpcUa::ObjectID::RootFolder);
  }

  Node OPCUAServer::GetObjectsNode() const
  {
    return GetNode(ObjectID::ObjectsFolder);
  }

  Node OPCUAServer::GetServerNode() const
  {
    return GetNode(ObjectID::Server);
  }

  void OPCUAServer::EnableEventNotification()
  {
    Node server = GetServerNode();
    uint8_t notifierval = 0;
    notifierval |= EventNotifier::SubscribeToEvents; 

    server.SetAttribute(AttributeID::EVENT_NOTIFIER, notifierval);
  }

  std::unique_ptr<Subscription> OPCUAServer::CreateSubscription(unsigned int period, SubscriptionClient& callback)
  {
    SubscriptionParameters params;
    params.RequestedPublishingInterval = period;
    return std::unique_ptr<Subscription>(new Subscription (Registry->GetServer(), params, callback, Debug));
  }


  void OPCUAServer::TriggerEvent(Event event)
  {
    SubscriptionService->TriggerEvent(ObjectID::Server, event);
  }

  void OPCUAServer::CreateServerObjectNode()
  {
    Model::Server server(Registry->GetServer());
    Model::Object root = server.GetObject(ObjectID::ObjectsFolder);
    Model::ObjectType serverType = server.GetObjectType(ObjectID::ServerType);
    root.CreateObject(ObjectID::Server, serverType, QualifiedName(OpcUa::Names::Server));
  }

}
