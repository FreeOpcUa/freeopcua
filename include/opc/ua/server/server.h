#pragma once


#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/event.h>
#include <opc/ua/node.h>
#include <opc/ua/server/services_registry.h>
#include <opc/ua/server/subscription_service.h>
#include <opc/ua/services/services.h>
#include <opc/ua/subscription.h>

namespace OpcUa
{
  class UaServer
  {
    public:
      /// @brief create high level server
      // this class is meant to be used to quickly/easily add OPCUA interface to an application
      // it may not offer absolutely all features available in protocol
      // you may want to look at code and implement your own server if you need 
      // debug argument will make freeopcua write A LOT to stdout
      UaServer();
      explicit UaServer(bool debug);

      /// @brief set endpoint uri on wich server will listen.
      /// opc.tcp://localhost:4841/opcua/server
      /// opc.tcp://192.168.1.1:4840/opcua/server
      /// opc.tcp://server.freeopca.org:4841/opcua/server
      void SetEndpoint(const std::string& endpoint);

      /// @brief setup server description
      void SetProductURI(const std::string& uri);
      void SetServerURI(const std::string& uri);
      void SetServerName(const std::string& name);

      /// @brief load xml addressspace. This is not implemented yet!!!
      void AddAddressSpace(const std::string& path);

      /// @brief Enable event notification on Server node
      /// this is necessary if you want to be able to send custom events
      // (Not for datachange events!)
      void EnableEventNotification();

      /// @brief Register your application namespace
      // Every application will probably have its own namespace
      // example: urn:mydomain:myfancyapplication
      // the returned index will then be used
      uint32_t RegisterNamespace(std::string uri);
      uint32_t GetNamespaceIndex(std::string uri);
      
      /// @brief Start and Stop server
      // if you do not stop server you may get into trouble!
      // we have several threads running
      void Start();
      void Stop();

      /// @brief Get a specific node by nodeid
      // you can also access a standard node from addressspace using
      // ObjectId, for example:
      // Node mynode = GetNode(ObjectId::Server); 
      // using a string is also possible:
      // Node mynode = GetNode("ns=3;i=55"); 
      Node GetNode(const NodeId& nodeid) const;
      Node GetNode(const std::string& nodeid) const;

      /// @brief helper methods for node you will probably want to access
      Node GetRootNode() const;
      Node GetObjectsNode() const;
      Node GetServerNode() const;

      /// @brief Get a node using its browsepath
      Node GetNodeFromPath(const std::vector<QualifiedName>& path) const;
      Node GetNodeFromPath(const std::vector<std::string>& path) const;

      /// @brief Trigger and event
      // Event will be send from Server node.
      // It is possible to send events from arbitrarily nodes but it looks like
      // most servers only send events from Server node
      void TriggerEvent(Event event);

      /// @brief Create a subscription objects
      // returned object can then be used to subscribe to 
      // datachange or custom events on server side
      std::unique_ptr<Subscription> CreateSubscription(unsigned int period, SubscriptionHandler& callback);

    protected:
      std::vector<std::string> XmlAddressSpaces;
      // defined some sensible defaults that should let most clients connects
      std::string Endpoint;
      std::string ServerUri = "urn:freeopcua:server"; 
      std::string ProductUri = "urn:freeopcua.github.no:server";
      std::string Name = "FreeOpcUa Server";
      bool Debug = false;
      bool LoadCppAddressSpace = true;
      OpcUa::MessageSecurityMode SecurityMode = OpcUa::MessageSecurityMode::None;
      void CheckStarted() const;

      Common::AddonsManager::SharedPtr Addons;
      Server::ServicesRegistry::SharedPtr Registry;
      Server::SubscriptionService::SharedPtr SubscriptionService;
  };

}
