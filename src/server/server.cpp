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

#include <opc/ua/server/server.h>

#include <opc/ua/server/addons/common_addons.h>
#include <opc/ua/protocol/string_utils.h>

#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/subscription_service.h>
#include <iostream>

namespace OpcUa
{
  UaServer::UaServer()
  {
  }

  UaServer::UaServer(bool debug)
    : Debug(debug)
  {
  }

  void UaServer::SetEndpoint(const std::string& endpoint)
  {
	  Endpoint = endpoint;
  }

  void UaServer::SetProductURI(const std::string& uri)
  {
	  ProductUri = uri;
  }

  void UaServer::SetServerURI(const std::string& uri)
  {
	  ServerUri = uri;
  }

  void UaServer::SetServerName(const std::string& name)
  {
	  Name = name;
  }

  void UaServer::AddAddressSpace(const std::string& path)
  {
	  XmlAddressSpaces.push_back(path);
  }

  void UaServer::CheckStarted() const
  {
    if ( ! Registry )
    {
      throw(std::runtime_error("Server is not started"));
    }
  }

  uint32_t UaServer::RegisterNamespace(std::string uri)
  {
    CheckStarted();
    Node namespacearray(Registry->GetServer(), ObjectId::Server_NamespaceArray);
    std::vector<std::string> uris = namespacearray.GetValue().As<std::vector<std::string>>();
    uint32_t index = uris.size();
    uris.push_back(uri);
    namespacearray.SetValue(uris);
    return index;
  }

  uint32_t UaServer::GetNamespaceIndex(std::string uri)
  {
    CheckStarted();
    Node namespacearray(Registry->GetServer(), ObjectId::Server_NamespaceArray);
    std::vector<std::string> uris = namespacearray.GetValue().As<std::vector<std::string>>();;
    for ( uint32_t i=0; i<uris.size(); ++i)
    {
      if (uris[i] == uri )
      {
        return i;
      }
    }
    throw(std::runtime_error("Error namespace uri does not exists in server")); 
    //return -1;
  }

  void UaServer::Start()
  {
    ApplicationDescription appDesc;
    appDesc.Name = LocalizedText(Name);
    appDesc.URI = ServerUri;
    appDesc.Type = ApplicationType::Server;
    appDesc.ProductURI = ProductUri;

    OpcUa::Server::Parameters params;
    params.Debug = Debug;
    params.Endpoint.ServerDescription = appDesc;
    params.Endpoint.EndpointURL = Endpoint;
    params.Endpoint.SecurityMode = SecurityMode;
    params.Endpoint.SecurityPolicyURI = "http://opcfoundation.org/UA/SecurityPolicy#None";
    params.Endpoint.TransportProfileURI = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
    //setting up policy is required for some client, this should be in a constructor
    UserTokenPolicy policy;
    policy.TokenType = UserIdentifyTokenType::ANONYMOUS;
    params.Endpoint.UserIdentifyTokens.push_back(policy);

    Addons = Common::CreateAddonsManager();
    Server::RegisterCommonAddons(params, *Addons);
    Addons->Start();

    Registry = Addons->GetAddon<Server::ServicesRegistry>(Server::ServicesRegistryAddonId);
    SubscriptionService = Addons->GetAddon<Server::SubscriptionService>(Server::SubscriptionServiceAddonId);

    Node ServerArray = GetNode(OpcUa::ObjectId::Server_ServerArray);
    ServerArray.SetValue(std::vector<std::string>({Endpoint}));

    EnableEventNotification(); //Enabling event notification, it probably does hurt anyway and users will forgot to set it up
  }

  Node UaServer::GetNode(const std::string& nodeid) const
  {
    return GetNode(ToNodeId(nodeid));
  }

  Node UaServer::GetNode(const NodeId& nodeid) const
  {
    CheckStarted();
    return Node(Registry->GetServer(), nodeid);
  }

  Node UaServer::GetNodeFromPath(const std::vector<QualifiedName>& path) const
  {
    return GetRootNode().GetChild(path);
  }

  Node UaServer::GetNodeFromPath(const std::vector<std::string>& path) const
  {
    return GetRootNode().GetChild(path);
  }

  void UaServer::Stop()
  {
    std::cout << "Stopping opcua server application" << std::endl;
    CheckStarted();
    Addons->Stop();
  }

  Node UaServer::GetRootNode() const
  {
    return GetNode(OpcUa::ObjectId::RootFolder);
  }

  Node UaServer::GetObjectsNode() const
  {
    return GetNode(ObjectId::ObjectsFolder);
  }

  Node UaServer::GetServerNode() const
  {
    return GetNode(ObjectId::Server);
  }

  void UaServer::EnableEventNotification()
  {
    Node server = GetServerNode();
    
    uint8_t notifierval = 0;
    notifierval |= EventNotifier::SubscribeToEvents;

    DataValue dval(notifierval);
    dval.SetSourceTimestamp(DateTime::Current());

    server.SetAttribute(AttributeId::EventNotifier, dval);
  }

  std::unique_ptr<Subscription> UaServer::CreateSubscription(unsigned int period, SubscriptionHandler& callback)
  {
    CheckStarted();
    SubscriptionParameters params;
    params.RequestedPublishingInterval = period;
    return std::unique_ptr<Subscription>(new Subscription (Registry->GetServer(), params, callback, Debug));
  }


  void UaServer::TriggerEvent(Event event)
  {
    SubscriptionService->TriggerEvent(ObjectId::Server, event);
  }

}
