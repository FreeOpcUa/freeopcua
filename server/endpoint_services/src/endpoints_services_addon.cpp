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

#include "endpoints_parameters.h"

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
  virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
  {
    // TODO appky filter
    return Applications;
  }

  virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
  {
    // TODO apply filter.
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

  void AddApplications(const std::vector<OpcUa::ApplicationDescription>& applications)
  {
    Applications.insert(Applications.begin(), applications.begin(), applications.end());
  }

private:
  std::vector<EndpointDescription> Endpoints;
  std::vector<ApplicationDescription> Applications;
};

void EndpointsAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
{
  ApplyAddonParameters(params);

  Services.reset(new EndpointsImpl());
  InternalComputer = Common::GetAddon<ServicesRegistryAddon>(addons, ServicesRegistryAddonID);
  InternalComputer->RegisterEndpointsServices(Services);

  const std::vector<OpcUa::ApplicationData>& data = OpcUa::ParseEndpointsParameters(params.Groups, Debug);
  for (const OpcUa::ApplicationData& application : data)
  {
    AddApplications(std::vector<OpcUa::ApplicationDescription>(1, application.Application));
  }
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

void EndpointsAddon::AddApplications(const std::vector<OpcUa::ApplicationDescription>& applications)
{
  Services->AddApplications(applications);
}

void EndpointsAddon::ApplyAddonParameters(const Common::AddonParameters& addons)
{
  for (const Common::Parameter parameter : addons.Parameters)
  {
    if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
    {
      Debug = true;
    }
  }
}

