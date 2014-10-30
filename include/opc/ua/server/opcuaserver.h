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
  class OPCUAServer
  {
    public:
      OPCUAServer();
      explicit OPCUAServer(bool debug);

      /// @brief set endpoint uri on wich server will listen.
      /// opc.tcp://localhost:4841
      /// opc.tcp://192.168.1.1:4840
      /// opc.tcp://server.freeopca.org:4841
      void SetEndpoint(const std::string& endpoint);
      void SetProductURI(const std::string& uri);
      void SetServerURI(const std::string& uri);
      void SetServerName(const std::string& name);
      void AddAddressSpace(const std::string& path);
      void SetLoadCppAddressSpace(bool val = true);
      void EnableEventNotification();

      void Start();
      void Stop();

      Node GetRootNode() const;
      Node GetObjectsNode() const;
      Node GetServerNode() const;
      Node GetNode(const NodeID& nodeid) const;
      Node GetNode(const std::string& nodeid) const;
      Node GetNodeFromPath(const std::vector<QualifiedName>& path) const;
      Node GetNodeFromPath(const std::vector<std::string>& path) const;

      void TriggerEvent(Event event);

      std::unique_ptr<Subscription> CreateSubscription(unsigned int period, SubscriptionClient& callback);

    protected:
      std::vector<std::string> XmlAddressSpaces;
      std::string Endpoint = "opc.tcp://localhost:4841"; //This is the expected address of an OPC-UA server on a machine
      std::string ServerUri = "FreeOpcua"; //This is the expected address of an OPC-UA server on a machine
      std::string ProductUri = "freeopcua.github.io";
      std::string Name = "Open Source OPC-UA Server";
      bool Debug = false;
      bool LoadCppAddressSpace = true;
      OpcUa::MessageSecurityMode SecurityMode = OpcUa::MessageSecurityMode::MSM_NONE;

      Common::AddonsManager::SharedPtr Addons;
      Server::ServicesRegistry::SharedPtr Registry;
      Server::SubscriptionService::SharedPtr SubscriptionService;
  };

}
