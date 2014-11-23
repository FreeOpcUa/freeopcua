/******************************************************************************
 *   Copyright (C) 2014-2014 by Sintef Raufoss Manufacturing                  *
 *   olivier.roulet@gmail.com                  *
 *                                          *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as          *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                          *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.              *
 *                                          *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                  *
 *   Free Software Foundation, Inc.,                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ******************************************************************************/

#include <opc/ua/client/client.h>

#include <opc/common/uri_facade.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/services/services.h>
#include <opc/ua/node.h>
#include <opc/ua/protocol/string_utils.h>


namespace OpcUa
{

  void KeepAliveThread::Start(Node node, Duration period)
  {
    NodeToRead = node;
    Period = period;
    Running = true;
    Thread = std::thread([this] { this->Run(); });
  }


  void KeepAliveThread::Run()
  {
    if (Debug)  { std::cout << "KeepAliveThread | Starting." << std::endl; }
    while ( ! StopRequest )
    {
      std::unique_lock<std::mutex> lock(Mutex);
      std::cv_status status = Condition.wait_for(lock, std::chrono::milliseconds((int64_t)Period)); 
      if (status == std::cv_status::no_timeout ) 
      {
        break;
      }
      if (Debug)  { std::cout << "KeepAliveThread | Reading " << std::endl; }
      NodeToRead.GetValue();
    }
    Running = false;
  }

  void KeepAliveThread::Stop()
  {
    if (Debug)  { std::cout << "KeepAliveThread | Stopping." << std::endl; }
    StopRequest = true;
    Condition.notify_all();
  }

  void KeepAliveThread::Join()
  {
    if (Debug)  { std::cout << "KeepAliveThread | Joining." << std::endl; }
    if ( ! Running )
    {
      if (Debug)  { std::cout << "KeepAliveThread | Thread was not running..." << std::endl; }
    }
    Thread.join();
    if (Debug)  { std::cout << "KeepAliveThread | Join successfull." << std::endl; }
  }

  std::vector<EndpointDescription> RemoteClient::GetServerEndpoints()
  {
    bool doConnect = false;
    if ( ! Server ) //to avoid surprises we create channel
    {
      doConnect = true;
      const Common::Uri serverUri(Endpoint);
      OpcUa::IOChannel::SharedPtr channel = OpcUa::Connect(serverUri.Host(), serverUri.Port());
    
      OpcUa::SecureConnectionParams params;
      params.EndpointUrl = Endpoint;
      params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

      Server = OpcUa::CreateBinaryServer(channel, params, Debug);
    }

    EndpointsFilter filter;
    filter.EndpointURL = Endpoint;
    filter.ProfileUries.push_back("http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");
    filter.LocaleIDs.push_back("http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary");
    std::vector<EndpointDescription> endpoints =  Server->Endpoints()->GetEndpoints(filter);
    
    if ( doConnect ) 
    {
      Server.reset();
    }

    return endpoints;
  }

  EndpointDescription RemoteClient::SelectEndpoint()
  {
    std::vector<EndpointDescription> endpoints = GetServerEndpoints();
    if (Debug)  { std::cout << "RemoteClient | Going through server endpoints and selected one we support" << std::endl; }
    for ( EndpointDescription ed : endpoints)
    {
      if (Debug)  { std::cout << "RemoteClient | Examining endpoint: " << ed.EndpointURL << " with security: " << ed.SecurityPolicyURI <<  std::endl; }
      if ( ed.SecurityPolicyURI == "http://opcfoundation.org/UA/SecurityPolicy#None")
      {
        if (Debug)  { std::cout << "RemoteClient | Security policy is OK, now looking at user token" <<  std::endl; }
        if (ed.UserIdentifyTokens.empty() )
        {
          if (Debug)  { std::cout << "RemoteClient | Server does not use user token, OK" <<  std::endl; }
          return ed;
        }
        for (  UserTokenPolicy token : ed.UserIdentifyTokens)
        {
          if (token.TokenType == UserIdentifyTokenType::ANONYMOUS )
          {
            if (Debug)  { std::cout << "RemoteClient | Endpoint selected " <<  std::endl; }
            return ed;
          }
        }
      }
    }
    throw std::runtime_error("No supported endpoints found on server");
  }

  void RemoteClient::Connect()
  {
    EndpointDescription endpoint = SelectEndpoint();
    Connect(endpoint);
  }
   
  void RemoteClient::Connect(const EndpointDescription& endpoint)
  {
    const Common::Uri serverUri(Endpoint);
    OpcUa::IOChannel::SharedPtr channel = OpcUa::Connect(serverUri.Host(), serverUri.Port());

    OpcUa::SecureConnectionParams params;
    params.EndpointUrl = Endpoint;
    params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

    Server = OpcUa::CreateBinaryServer(channel, params, Debug);


    if (Debug)  { std::cout << "RemoteClient | Creating session " <<  std::endl; }
    OpcUa::RemoteSessionParameters session;
    session.ClientDescription.URI = Uri;
    session.ClientDescription.ProductURI = ProductUri;
    session.ClientDescription.Name = LocalizedText(SessionName);
    session.ClientDescription.Type = OpcUa::ApplicationType::CLIENT;
    session.SessionName = SessionName;
    session.EndpointURL = endpoint.EndpointURL;
    session.Timeout = 1200000;
    session.ServerURI = endpoint.ServerDescription.URI;

    CreateSessionResponse response = Server->CreateSession(session);
    CheckStatusCode(response.Header.ServiceResult);
    ActivateSessionResponse aresponse = Server->ActivateSession();
    CheckStatusCode(aresponse.Header.ServiceResult);

    if (response.Session.RevisedSessionTimeout != 0)
    {
      KeepAlive.Start(Node(Server, ObjectID::Server_ServerStatus_State), 0.4 * response.Session.RevisedSessionTimeout);
    }
  }

  void RemoteClient::Disconnect()
  {
    KeepAlive.Stop();
    KeepAlive.Join();

    if (  Server ) 
    {
      CloseSessionResponse response = Server->CloseSession();
      if (Debug) { std::cout << "CloseSession response is " << ToString(response.Header.ServiceResult) << std::endl; }
    }
    Server.reset(); //SecureChannel is not closed until we destroy server object
  }

  uint32_t RemoteClient::GetNamespaceIndex(std::string uri)
  {
    if ( ! Server ) { throw NotConnectedError();}
    Node namespacearray(Server, ObjectID::Server_NamespaceArray);
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


  Node RemoteClient::GetNode(const std::string& nodeId) const
  {
    return Node(Server, ToNodeID(nodeId));
  }

  Node RemoteClient::GetNode(const NodeID& nodeId) const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, nodeId);
  }

  Node RemoteClient::GetRootNode() const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, OpcUa::ObjectID::RootFolder);
  }

  Node RemoteClient::GetObjectsNode() const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, OpcUa::ObjectID::ObjectsFolder);
  }

  Node RemoteClient::GetServerNode() const
  {
    if ( ! Server ) { throw NotConnectedError();}
    return Node(Server, OpcUa::ObjectID::Server);
  }

  std::unique_ptr<Subscription> RemoteClient::CreateSubscription(unsigned int period, SubscriptionClient& callback)
  {
    SubscriptionParameters params;
    params.RequestedPublishingInterval = period;

    return std::unique_ptr<Subscription>(new Subscription (Server, params, callback, Debug));
  }
} // namespace OpcUa

