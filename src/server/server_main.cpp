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
#include <opc/ua/server/addons/asio_addon.h>
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/opc_tcp_async.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/standard_namespace.h>
#include "tcp_server.h"

#include "daemon.h"
#include "server_object_addon.h"
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
    services.Factory = std::make_shared<OpcUa::Server::ServicesRegistryFactory>();
    services.ID = OpcUa::Server::ServicesRegistryAddonID;
    return services;
  }

  Common::AddonInformation CreateAddressSpace()
  {
    Common::AddonInformation config;
    config.Factory = std::make_shared<OpcUa::Server::AddressSpaceAddonFactory>();
    config.ID = OpcUa::Server::AddressSpaceRegistryAddonID;
    config.Dependencies.push_back(OpcUa::Server::ServicesRegistryAddonID);
    return config;
  }

  Common::AddonInformation CreateStandardNamespace()
  {
    Common::AddonInformation config;
    config.Factory = std::make_shared<OpcUa::Server::StandardNamespaceAddonFactory>();
    config.ID = OpcUa::Server::StandardNamespaceAddonID;
    config.Dependencies.push_back(OpcUa::Server::AddressSpaceRegistryAddonID);
    return config;
  }

  Common::AddonInformation CreateEndpointsRegistry()
  {
    Common::AddonInformation endpoints;
    endpoints.Factory = std::make_shared<OpcUa::Server::EndpointsRegistryAddonFactory>();
    endpoints.ID = OpcUa::Server::EndpointsRegistryAddonID;
    endpoints.Dependencies.push_back(OpcUa::Server::ServicesRegistryAddonID);
    return endpoints;
  }

  Common::AddonInformation CreateBinaryServer()
  {
    Common::AddonInformation opcTcp;
    opcTcp.Factory = std::make_shared<OpcUa::Server::OpcUaProtocolAddonFactory>();
    opcTcp.ID = OpcUa::Server::OpcUaProtocolAddonID;
    opcTcp.Dependencies.push_back(OpcUa::Server::EndpointsRegistryAddonID);
    return opcTcp;
  }

  Common::AddonInformation CreateOpcTcpAsync(const Common::ParametersGroup& params)
  {
    Common::AddonInformation opcTcp;
    opcTcp.Factory = std::make_shared<OpcUa::Server::AsyncOpcTcpAddonFactory>();
    opcTcp.ID = OpcUa::Server::AsyncOpcTcpAddonID;
    opcTcp.Dependencies.push_back(OpcUa::Server::EndpointsRegistryAddonID);
    opcTcp.Dependencies.push_back(OpcUa::Server::AsioAddonID);
    AddParameters(opcTcp, params);
    return opcTcp;
  }

  Common::AddonInformation CreateServerObject()
  {
    Common::AddonInformation serverObjectAddon;
    serverObjectAddon.Factory = std::make_shared<OpcUa::Server::ServerObjectFactory>();
    serverObjectAddon.ID = OpcUa::Server::ServerObjectAddonID;
    serverObjectAddon.Dependencies.push_back(OpcUa::Server::StandardNamespaceAddonID);
    return serverObjectAddon;
  }

  Common::AddonInformation CreateAsio()
  {
    Common::AddonInformation asioAddon;
    asioAddon.Factory = std::make_shared<OpcUa::Server::AsioAddonFactory>();
    asioAddon.ID = OpcUa::Server::AsioAddonID;
    return asioAddon;
  }


  void AddStandardModules(const Common::AddonParameters& params, std::vector<Common::AddonInformation>& addons)
  {
    Common::AddonInformation endpointsRegistry = CreateEndpointsRegistry();
    Common::AddonInformation addressSpaceRegistry = CreateAddressSpace();
    Common::AddonInformation asioAddon = CreateAsio();

    for (const Common::ParametersGroup& group : params.Groups)
    {
      if (group.Name == OpcUa::Server::EndpointsRegistryAddonID)
      {
        AddParameters(endpointsRegistry, group);
      }
      else if (group.Name == OpcUa::Server::OpcUaProtocolAddonID)
      {
        Common::AddonInformation binaryProtocol = CreateBinaryServer();
        AddParameters(binaryProtocol, group);
        addons.push_back(std::move(binaryProtocol));
      }
      else if (group.Name == OpcUa::Server::AddressSpaceRegistryAddonID)
      {
        AddParameters(addressSpaceRegistry, group);
      }
      else if (group.Name == OpcUa::Server::AsyncOpcTcpAddonID)
      {
        addons.push_back(CreateOpcTcpAsync(group));
      }
      else if (group.Name == OpcUa::Server::AsioAddonID)
      {
        AddParameters(asioAddon, group);
      }
    }

    addons.push_back(endpointsRegistry);
    addons.push_back(addressSpaceRegistry);
    addons.push_back(asioAddon);
    addons.push_back(CreateServicesRegistry());
    addons.push_back(CreateStandardNamespace());
    addons.push_back(CreateServerObject());
  }

}

int main(int argc, char** argv)
{
  try
  {
    const char** arv = (const char**)argv;
    OpcUa::Server::CommandLine options(argc, arv);
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

