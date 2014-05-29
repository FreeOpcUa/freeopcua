/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "opcua_protocol_addon.h"

#include "opc_tcp_processor.h"

#include <opc/common/uri_facade.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/tcp_server_addon.h>

#include <endpoint_services/src/endpoints_parameters.h>

#include <stdexcept>

using namespace OpcUa;
using namespace OpcUa::Impl;
using namespace OpcUa::UaServer;


OpcUaProtocol::OpcUaProtocol()
  : Debug(false)
{
}

void OpcUaProtocol::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
  ApplyAddonParameters(params);
  const std::vector<ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Debug);
  for (ApplicationData d: applications) {
    std::cout << "Endpoint is: " << d.Endpoints.front().EndpointURL << std::endl;
  }

  std::vector<ApplicationDescription> applicationDescriptions;
  std::vector<EndpointDescription> endpointDescriptions;
  for (const ApplicationData application : applications)
  {
    applicationDescriptions.push_back(application.Application);
    endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
  }
  PublishApplicationsInformation(applicationDescriptions, endpointDescriptions, addons);
  StartEndpoints(endpointDescriptions, addons);
}

void OpcUaProtocol::Stop()
{
  for (unsigned port : Ports)
  {
    TcpParameters params;
    params.Port = port;
    TcpAddon->StopListen(params);
  }
  TcpAddon.reset();
}

void OpcUaProtocol::ApplyAddonParameters(const Common::AddonParameters& params)
{
  for (const Common::Parameter parameter : params.Parameters)
  {
    if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
    {
      Debug = true;
      std::cout << "Enabled debug mod." << std::endl;
    }
  }
}

void OpcUaProtocol::StartEndpoints(std::vector<EndpointDescription> endpoints, Common::AddonsManager& addons)
{
  InternalServer = addons.GetAddon<OpcUa::UaServer::ServicesRegistryAddon>(OpcUa::UaServer::ServicesRegistryAddonID);
  TcpAddon = addons.GetAddon<OpcUa::UaServer::TcpServerAddon>(OpcUa::UaServer::TcpServerAddonID);
  for (const EndpointDescription endpoint : endpoints)
  {
    const Common::Uri uri(endpoint.EndpointURL);
    if (uri.Scheme() == "opc.tcp")
    {
      std::shared_ptr<IncomingConnectionProcessor> processor = OpcUa::Internal::CreateOpcTcpProcessor(InternalServer->GetServer(), Debug);
      TcpParameters tcpParams;
      tcpParams.Port = uri.Port();
      if (Debug) std::clog << "Starting listen port " << tcpParams.Port << std::endl;
      TcpAddon->Listen(tcpParams, processor);
      Ports.push_back(tcpParams.Port);
    }
  }
}

void OpcUaProtocol::PublishApplicationsInformation(std::vector<ApplicationDescription> applications, std::vector<EndpointDescription> endpoints, const Common::AddonsManager& addons) const
{
  std::shared_ptr<EndpointsServicesAddon> endpointsAddon = addons.GetAddon<EndpointsServicesAddon>(EndpointsServicesAddonID);
  if (!endpointsAddon)
  {
    std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
    return;
  }
  endpointsAddon->AddEndpoints(endpoints);
  endpointsAddon->AddApplications(applications);
}
