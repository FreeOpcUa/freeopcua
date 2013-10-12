/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "soap_addon.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>

#include <endpoint_services/src/endpoints_parameters.h>

#include <stdexcept>

using namespace OpcUa;
using namespace OpcUa::Impl;


SoapAddon::SoapAddon()
  : Debug(false)
{
}

void SoapAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
  OpcUa::Server::ServicesRegistryAddon::SharedPtr servicesRegistry = addons.GetAddon<OpcUa::Server::ServicesRegistryAddon>(OpcUa::Server::ServicesRegistryAddonID);
/*
  ApplyAddonParameters(params);
  const std::vector<ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Debug);

  std::vector<ApplicationDescription> applicationDescriptions;
  std::vector<EndpointDescription> endpointDescriptions;
  for (const ApplicationData application : applications)
  {
    applicationDescriptions.push_back(application.Application);
    endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
  }
  PublishApplicationsInformation(applicationDescriptions, endpointDescriptions, addons);
  StartEndpoints(endpointDescriptions, addons);
*/
  std::unique_ptr<SoapDiscoveryService> discoveryService(new SoapDiscoveryService(servicesRegistry->GetComputer(), SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE | SOAP_XML_INDENT));
  DiscoveryService.reset(new SoapService<SoapDiscoveryService>(8888, std::move(discoveryService)));
  DiscoveryService->Start();
}

void SoapAddon::Stop()
{
/*
  for (unsigned port : Ports)
  {
    TcpParameters params;
    params.Port = port;
    TcpAddon->StopListen(params);
  }
  TcpAddon.reset();
*/
  DiscoveryService->Stop();
}

void SoapAddon::ApplyAddonParameters(const Common::AddonParameters& params)
{
  for (const Common::Parameter parameter : params.Parameters)
  {
    if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
    {
      Debug = true;
    }
  }
}

void SoapAddon::StartEndpoints(OpcUa::Server::ServicesRegistryAddon::SharedPtr servicesRegistry)
{
/*
  InternalComputer = Common::GetAddon<OpcUa::Server::ServicesRegistryAddon>(addons, OpcUa::Server::ServicesRegistryAddonID);
  TcpAddon = Common::GetAddon<OpcUa::Server::TcpServerAddon>(addons, OpcUa::Server::TcpServerAddonID);
  for (const EndpointDescription endpoint : endpoints)
  {
    const Internal::Uri uri(endpoint.EndpointURL);
    if (uri.Scheme() == "opc.tcp")
    {
      std::shared_ptr<IncomingConnectionProcessor> processor = OpcUa::Internal::CreateOpcTcpProcessor(InternalComputer->GetComputer(), Debug);
      TcpParameters tcpParams;
      tcpParams.Port = uri.Port();
      if (Debug) std::clog << "Starting listen port " << tcpParams.Port << std::endl;
      TcpAddon->Listen(tcpParams, processor);
      Ports.push_back(tcpParams.Port);
    }
  }
*/
}

void SoapAddon::PublishApplicationsInformation(std::vector<ApplicationDescription> applications, std::vector<EndpointDescription> endpoints, const Common::AddonsManager& addons) const
{
  std::shared_ptr<OpcUa::Server::EndpointsServicesAddon> endpointsAddon = addons.GetAddon<OpcUa::Server::EndpointsServicesAddon>(OpcUa::Server::EndpointsServicesAddonID);
  if (!endpointsAddon)
  {
    std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
    return;
  }
  endpointsAddon->AddEndpoints(endpoints);
  endpointsAddon->AddApplications(applications);
}

