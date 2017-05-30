/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/endpoints_services.h>

#include "endpoints_parameters.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/services/endpoints.h>

using namespace OpcUa;
using namespace OpcUa::Server;

namespace
{

class EndpointServicesRegistry : public OpcUa::Server::EndpointsRegistry
{
public:
  DEFINE_CLASS_POINTERS(EndpointServices)

public:
  virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters & params) const override
  {
    // TODO apply filter
    return Applications;
  }

  virtual std::vector<EndpointDescription> GetEndpoints(const GetEndpointsParameters & filter) const override
  {
    // TODO apply filter.
    return Endpoints;
  }

  virtual void RegisterServer(const OpcUa::ServerParameters & parameters) override
  {
  }

public:
  virtual void AddEndpoints(const std::vector<EndpointDescription> & endpoints) override
  {
    Endpoints.insert(Endpoints.begin(), endpoints.begin(), endpoints.end());
  }

  void AddApplications(const std::vector<OpcUa::ApplicationDescription> & applications) override
  {
    Applications.insert(Applications.begin(), applications.begin(), applications.end());
  }

private:
  Server::ServicesRegistry::SharedPtr InternalServer;

  std::vector<EndpointDescription> Endpoints;
  std::vector<ApplicationDescription> Applications;
};

} // namespace


namespace OpcUa
{
Server::EndpointsRegistry::UniquePtr Server::CreateEndpointsRegistry()
{
  return Server::EndpointsRegistry::UniquePtr(new EndpointServicesRegistry);
}
}

