/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints_services_addon.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/endpoints.h>

using namespace OpcUa;
using namespace OpcUa::Impl;
using namespace OpcUa::Server;
using namespace OpcUa::Remote;

class EndpointsAddon::EndpointsImpl : public EndpointServices
{
public:
  virtual std::vector<ApplicationDescription> FindServers(const ApplicationFilter& filter) const
  {
    return std::vector<ApplicationDescription>();
  }

  virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
  {
    return Endpoints;
  }

  virtual void RegisterServer(const OpcUa::Remote::ServerParameters& parameters)
  {
  }

public:
  void AddEndpoints(std::vector<EndpointDescription> endpoints)
  {
    Endpoints.insert(Endpoints.begin(), endpoints.begin(), endpoints.end());
  }

private:
  std::vector<EndpointDescription> Endpoints;
};

void EndpointsAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
  Services.reset(new EndpointsImpl());
  InternalComputer = Common::GetAddon<ServicesRegistryAddon>(addons, ServicesRegistryAddonID);
  InternalComputer->RegisterEndpointsServices(Services);
}

void EndpointsAddon::Stop()
{
  InternalComputer->UnregisterEndpointsServices();
  InternalComputer.reset();
  Services.reset();
}

void EndpointsAddon::AddEndpoints(const std::vector<EndpointDescription>& endpoints)
{
  Services->AddEndpoints(endpoints);
}
