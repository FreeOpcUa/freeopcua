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
#include <opc/ua/endpoints.h>

using namespace OpcUa;
using namespace OpcUa::UaServer;
using namespace OpcUa::Remote;


namespace
{

  class EndpointServicesRegistry : public OpcUa::UaServer::EndpointsRegistry
  {
  public:
    DEFINE_CLASS_POINTERS(EndpointServices);

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

    virtual void RegisterServer(const OpcUa::Remote::ServerParameters& parameters) override
    {
    }

  public:
    virtual void AddEndpoints(const std::vector<EndpointDescription>& endpoints) override
    {
      Endpoints.insert(Endpoints.begin(), endpoints.begin(), endpoints.end());
    }

    void AddApplications(const std::vector<OpcUa::ApplicationDescription>& applications) override
    {
      Applications.insert(Applications.begin(), applications.begin(), applications.end());
    }

  private:
    UaServer::ServicesRegistry::SharedPtr InternalServer;
    bool Debug;

    std::vector<EndpointDescription> Endpoints;
    std::vector<ApplicationDescription> Applications;
  };

} // namespace


namespace OpcUa
{
  UaServer::EndpointsRegistry::UniquePtr UaServer::CreateEndpointsRegistry()
  {
    return UaServer::EndpointsRegistry::UniquePtr(new EndpointServicesRegistry);
  }
}

