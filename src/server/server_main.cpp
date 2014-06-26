/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Server main.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/common/application.h>
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/standard_namespace.h>
#include <opc/ua/server/addons/tcp_server.h>

#include "daemon.h"
#include "server_options.h"

#include <iostream>


namespace
{

  Common::AddonInformation CreateServicesRegistry()
  {
    Common::AddonInformation services;
    services.Factory = std::make_shared<OpcUa::UaServer::ServicesRegistryFactory>();
    services.ID = OpcUa::UaServer::ServicesRegistryAddonID;
    return services;
  }

  Common::AddonInformation CreateTcpServer()
  {
    Common::AddonInformation services;
    services.Factory = std::make_shared<OpcUa::UaServer::TcpServerAddonFactory>();
    services.ID = OpcUa::UaServer::TcpServerAddonID;
    return services;
  }

  Common::AddonInformation CreateAddressSpace()
  {
    Common::AddonInformation config;
    config.Factory = std::make_shared<OpcUa::UaServer::AddressSpaceAddonFactory>();
    config.ID = OpcUa::UaServer::AddressSpaceRegistryAddonID;
    config.Dependencies.push_back(OpcUa::UaServer::ServicesRegistryAddonID);
    return config;
  }

  Common::AddonInformation CreateStandardNamespace()
  {
    Common::AddonInformation config;
    config.Factory = std::make_shared<OpcUa::UaServer::StandardNamespaceAddonFactory>();
    config.ID = OpcUa::UaServer::StandardNamespaceAddonID;
    config.Dependencies.push_back(OpcUa::UaServer::AddressSpaceRegistryAddonID);
    return config;
  }

  Common::AddonInformation CreateEndpointsRegistry()
  {
    Common::AddonInformation endpoints;
    endpoints.Factory = std::make_shared<OpcUa::UaServer::EndpointsRegistryAddonFactory>();
    endpoints.ID = OpcUa::UaServer::EndpointsRegistryAddonID;
    endpoints.Dependencies.push_back(OpcUa::UaServer::ServicesRegistryAddonID);
    return endpoints;
  }

  Common::AddonInformation CreateBinaryServer()
  {
    Common::AddonInformation opcTcp;
    opcTcp.Factory = std::make_shared<OpcUa::UaServer::OpcUaProtocolAddonFactory>();
    opcTcp.ID = OpcUa::UaServer::OpcUaProtocolAddonID;
    opcTcp.Dependencies.push_back(OpcUa::UaServer::EndpointsRegistryAddonID);
    opcTcp.Dependencies.push_back(OpcUa::UaServer::TcpServerAddonID);
    return opcTcp;
  }

  void AddStandardModules(const Common::AddonParameters& params, std::vector<Common::AddonInformation>& addons)
  {
    Common::AddonInformation endpointsRegistry = CreateEndpointsRegistry();
    Common::AddonInformation addressSpaceRegistry = CreateAddressSpace();

    for (const Common::ParametersGroup& group : params.Groups)
    {
      if (group.Name == OpcUa::UaServer::EndpointsRegistryAddonID)
      {
        endpointsRegistry.Parameters.Groups = group.Groups;
        endpointsRegistry.Parameters.Parameters = group.Parameters;
      }
      if (group.Name == OpcUa::UaServer::OpcUaProtocolAddonID)
      {
        Common::AddonInformation binaryProtocol = CreateBinaryServer();
        binaryProtocol.Parameters.Groups = group.Groups;
        binaryProtocol.Parameters.Parameters = group.Parameters;
        addons.push_back(binaryProtocol);
      }
      if (group.Name == OpcUa::UaServer::AddressSpaceRegistryAddonID)
      {
        addressSpaceRegistry.Parameters.Groups = group.Groups;
        addressSpaceRegistry.Parameters.Parameters = group.Parameters;
      }
    }

    addons.push_back(endpointsRegistry);
    addons.push_back(addressSpaceRegistry);
    addons.push_back(CreateServicesRegistry());
    addons.push_back(CreateTcpServer());
    addons.push_back(CreateStandardNamespace());
  }

}

int main(int argc, char** argv)
{
  try
  {
    const char** arv = (const char**)argv;
    OpcUa::UaServer::CommandLine options(argc, arv);
    if (!options.NeedStartServer())
    {
      return 0;
    }

    OpcUa::Daemon daemon;
    if (options.IsDaemonMode())
    {
      daemon.Daemonize(options.GetLogFile());
    }

    std::vector<Common::AddonInformation> addons = options.GetModules();
    AddStandardModules(options.GetParameters(), addons);

    OpcUa::Application::UniquePtr application = OpcUa::CreateApplication();
    application->Start(addons);
    daemon.WaitForTerminate();

    application->Stop();
    return 0;
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown error." << std::endl;
  }
  return -1;
}

