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
using namespace OpcUa::Soap;


SoapAddon::SoapAddon()
  : Debug(false)
{
}

void SoapAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
  ApplyAddonParameters(params);
  OpcUa::Server::ServicesRegistryAddon::SharedPtr servicesRegistry = addons.GetAddon<OpcUa::Server::ServicesRegistryAddon>(OpcUa::Server::ServicesRegistryAddonID);
  OpcUa::Remote::Computer::SharedPtr computer = servicesRegistry->GetComputer();

  ServerInstance.reset(new Soap::Server(8888, Debug));
  ServerInstance->AddService(std::unique_ptr<ServiceImpl<DiscoveryService>>(new ServiceImpl<DiscoveryService>(computer, Debug)));
  ServerInstance->AddService(std::unique_ptr<ServiceImpl<EndpointService>>(new ServiceImpl<EndpointService>(computer, Debug)));
  ServerInstance->Run();
}

void SoapAddon::Stop()
{
  ServerInstance->Stop();
  ServerInstance.reset();
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
