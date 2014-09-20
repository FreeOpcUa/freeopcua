#ifndef OPCUASERVER_H
#define OPCUASERVER_H

#include<string>

#include <opc/common/addons_core/config_file.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <opc/common/thread.h>
#include <opc/ua/node.h>
#include <opc/ua/services/services.h>
#include <opc/ua/event.h>
#include <opc/ua/subscription.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/server/endpoints_services.h>
#include <opc/ua/server/opc_tcp_async.h>
#include <opc/ua/server/services_registry.h>
#include <opc/ua/server/subscription_service.h>

#include <boost/asio.hpp>

namespace OpcUa
{
  class OPCUAServer
  {
    public:
      OPCUAServer(); 
      explicit OPCUAServer(bool debug);
      void SetEndpoint(const std::string& endpoint){this->Endpoint = endpoint;}
      void SetProductURI(const std::string& uri){this->ProductUri = uri;}
      void SetURI(const std::string& uri){this->ServerUri = uri;}
      void SetServerName(const std::string& name){this->Name = name;}
      void AddAddressSpace(const std::string& path) {xml_address_spaces.push_back(path);}
      void SetLoadCppAddressSpace(bool val=true){loadCppAddressSpace = val;}
      void EnableEventNotification();

      void Start();
      void Stop();

      Node GetRootNode() const;
      Node GetObjectsNode() const;
      Node GetServerNode() const;
      Node GetNode(const NodeID& nodeid) const;
      Node GetNodeFromPath(const std::vector<QualifiedName>& path) const;
      Node GetNodeFromPath(const std::vector<std::string>& path) const;

      void TriggerEvent(Event event);

      std::unique_ptr<Subscription> CreateSubscription(unsigned int period, SubscriptionClient& callback);

    private:
      void Run();
      void CreateServerObjectNode();

    protected:
      boost::asio::io_service IoService;
      std::shared_ptr<boost::asio::io_service::work> ServerWork;
      std::vector<std::string> xml_address_spaces;
      std::string config_path = "";
      std::string Endpoint = "opc.tcp://localhost:4841"; //This is the expected address of an OPC-UA server on a machine
      std::string ServerUri = "FreeOpcua"; //This is the expected address of an OPC-UA server on a machine
      std::string ProductUri = "freeopcua.github.io";
      std::string Name = "Open Source OPC-UA Server";
      bool Debug = false;
      OpcUa::MessageSecurityMode SecurityMode = OpcUa::MessageSecurityMode::MSM_NONE;
      bool loadCppAddressSpace = true; //Always true as long as we have not fixed the loading of xml addressspace

      Server::ServicesRegistry::SharedPtr Registry;
      Server::EndpointsRegistry::SharedPtr EndpointsServices;
      Server::AddressSpace::SharedPtr AddressSpace;
      Server::SubscriptionService::SharedPtr SubscriptionService;
      Server::AsyncOpcTcp::SharedPtr AsyncServer;
      Common::Thread::UniquePtr ListenThread;

  };

}

#endif
