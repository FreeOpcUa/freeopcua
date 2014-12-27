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


#pragma once

#include <opc/ua/node.h>
#include <opc/ua/services/services.h>
#include <opc/ua/subscription.h>
#include <opc/ua/client/binary_server.h>

#include <thread>
#include <condition_variable>
#include <chrono>
#include <atomic>


namespace OpcUa
{

  class NotConnectedError : public std::runtime_error 
  {
    public:
      NotConnectedError() : std::runtime_error("NotConnectedError") { }
  };

  class KeepAliveThread
  {
    public:
      /// @brief Internal 
      // Send keepalive request to server so it does not disconnect us
      KeepAliveThread(bool debug=false) : Debug(debug), StopRequest(false), Running(false) {}
      void Start(Node node, Duration period);
      void Stop();
      void Join();

    private:
      void Run();
      mutable std::thread Thread;
      bool Debug = false;
      Node NodeToRead;
      Duration Period = 1200000;
      std::atomic<bool> StopRequest;
      std::atomic<bool> Running;
      std::condition_variable Condition;
      std::mutex Mutex;
  };


  class UaClient
  {
  public:
    /// @brief create high level client
    /// this class is meant to be used to quickly/easily connect to an OPCUA server
    /// it may not offer absolutely all features available in protocol
    /// you may want to look at code and implement your own client if you need 
    /// debug argument will make freeopcua write A LOT to stdout
    /// set endpoint uri on wich server will listen.
    /// opc.tcp://localhost:4841/opcua/server
    /// opc.tcp://192.168.1.1:4840/opcua/server
    /// opc.tcp://server.freeopca.org:4841/opcua/server
    UaClient(bool debug=false) : Debug(debug), KeepAlive(debug) {}
    ~UaClient(); 

    UaClient(const UaClient&&) = delete;
    UaClient(const UaClient&) = delete;
    UaClient& operator=(const UaClient&) = delete;

    /// @brief set session name 
    void SetSessionName(const std::string& str) { SessionName = str; }
    std::string GetSessionName() const { return SessionName; }

    /// @brief  connect to a server, specify endpoint as string
    // a connection will be made to server to get endpoint description
    // an endpoint description will be selected and then a connection will attempted 
    void Connect(const std::string& endpoint);

    /// @brief connect to a server, specify endpoint as EndpointDesciption
    // EndpointDescription can be defined by hand or gotten through
    // a call to GetServerEndpoints() 
    void Connect(const EndpointDescription&);

    /// @brief Disconnect from server
    // close all threads and subcsriptions
    void Disconnect();

    /// @brief  Connect to server and get endpoints
    std::vector<EndpointDescription> GetServerEndpoints(const std::string& endpoint);

    /// @brief  Connect to server and select one endpoint
    EndpointDescription SelectEndpoint(const std::string&); 

    /// @brief  get endpoints from server, assume we are already connected
    std::vector<EndpointDescription> GetServerEndpoints();
    std::string GetEndpoint() const { return Endpoint.EndpointURL; }

    /// @brief  set application description 
    std::string GetApplicationURI() const { return ApplicationUri; }
    void SetApplicationURI(std::string uri) { ApplicationUri = uri; }
    std::string GetProductURI() const { return ProductUri; }
    void SetProductURI(std::string uri) { ProductUri = uri; }

    /// @brief  set security policy
    // anyway freeopcua currently only support None 
    void SetSecurityPolicy(std::string sec) {SecurityPolicy = sec;}
    std::string GetSecurityPolicy() const { return SecurityPolicy; }

    /// @brief Get namespaces used by server.
    // Deduce index from order or call GetNamespaceIndex(uri)
    std::vector<std::string>  GetServerNamespaces();
    uint32_t GetNamespaceIndex(std::string uri);

    /// @brief Get a specific node by nodeid
    // you can also access a standard node from addressspace using
    // ObjectID, for example:
    // Node mynode = GetNode(ObjectID::Server); 
    // using a string is also possible:
    // Node mynode = GetNode("ns=3;i=55"); 
    Node GetNode(const NodeID& nodeid) const;
    Node GetNode(const std::string& nodeid) const;

    /// @brief helper methods for node you will probably want to access
    Node GetRootNode() const;
    Node GetObjectsNode() const;
    Node GetServerNode() const;


    /// @brief Create a subscription objects
    // returned object can then be used to subscribe 
    // to datachange or custom events from server
    std::unique_ptr<Subscription> CreateSubscription(unsigned int period, SubscriptionClient& client);

  private:
    EndpointDescription Endpoint;
    // defined some sensible defaults that should let us connect to most servers
    std::string SessionName = "Open source OPC-UA Client Session";
    std::string ApplicationUri = "urn:freeopcua:client";
    std::string ProductUri = "urn:freeopcua.github.no:client";
    std::string SecurityPolicy = "none";
    bool Debug = false; 

  protected:
    Services::SharedPtr Server;

  private:
    KeepAliveThread KeepAlive;
    
  };

} // namespace OpcUa
