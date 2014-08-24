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
#include <opc/ua/server/addons/opc_tcp_async.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/standard_namespace.h>
#include "tcp_server.h"

#include "daemon.h"
#include "server_options.h"

#include <iostream>


namespace
{

  void AddParameters(Common::AddonInformation& info, const Common::ParametersGroup& params)
  {
    info.Parameters.Groups = params.Groups;
    info.Parameters.Parameters = params.Parameters;
  }

  Common::AddonInformation CreateServicesRegistry()
  {
    Common::AddonInformation services;
    services.Factory = std::make_shared<OpcUa::UaServer::ServicesRegistryFactory>();
    services.ID = OpcUa::UaServer::ServicesRegistryAddonID;
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
    return opcTcp;
  }

  Common::AddonInformation CreateOpcTcpAsync(const Common::ParametersGroup& params)
  {
    Common::AddonInformation opcTcp;
    opcTcp.Factory = std::make_shared<OpcUa::UaServer::AsyncOpcTcpAddonFactory>();
    opcTcp.ID = OpcUa::UaServer::AsyncOpcTcpAddonID;
    opcTcp.Dependencies.push_back(OpcUa::UaServer::EndpointsRegistryAddonID);
    AddParameters(opcTcp, params);
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
        AddParameters(endpointsRegistry, group);
      }
      else if (group.Name == OpcUa::UaServer::OpcUaProtocolAddonID)
      {
        Common::AddonInformation binaryProtocol = CreateBinaryServer();
        AddParameters(binaryProtocol, group);
        addons.push_back(std::move(binaryProtocol));
      }
      else if (group.Name == OpcUa::UaServer::AddressSpaceRegistryAddonID)
      {
        AddParameters(addressSpaceRegistry, group);
      }
      else if (group.Name == OpcUa::UaServer::AsyncOpcTcpAddonID)
      {
        addons.push_back(CreateOpcTcpAsync(group));
      }
    }

    addons.push_back(endpointsRegistry);
    addons.push_back(addressSpaceRegistry);
    addons.push_back(CreateServicesRegistry());
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

