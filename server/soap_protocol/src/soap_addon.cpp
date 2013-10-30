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
  std::unique_ptr<SoapDiscoveryService> discoveryService(new SoapDiscoveryService(servicesRegistry->GetComputer(), true, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE | SOAP_XML_INDENT));
  DiscoveryService.reset(new SoapService<SoapDiscoveryService>(8888, std::move(discoveryService)));
  DiscoveryService->Start();
}

void SoapAddon::Stop()
{
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
