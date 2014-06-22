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

  class EndpointsAddon : public Common::Addon, public EndpointsRegistry
  {
  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params) override
    {
      ApplyAddonParameters(params);

      Endpoints = OpcUa::UaServer::CreateEndpointsRegistry();
      InternalServer = addons.GetAddon<ServicesRegistry>(ServicesRegistryAddonID);
      InternalServer->RegisterEndpointsServices(Endpoints);

      const std::vector<OpcUa::UaServer::ApplicationData>& data = OpcUa::ParseEndpointsParameters(params.Groups, Debug);
      for (const OpcUa::UaServer::ApplicationData& application : data)
      {
        Endpoints->AddApplications(std::vector<OpcUa::ApplicationDescription>(1, application.Application));
        Endpoints->AddEndpoints(application.Endpoints);
      }
    }

    virtual void Stop() override
    {
      InternalServer->UnregisterEndpointsServices();
      InternalServer.reset();
      Endpoints.reset();
    }

    virtual void AddEndpoints(const std::vector<EndpointDescription>& endpoints) override
    {
      Endpoints->AddEndpoints(endpoints);
    }

    virtual void AddApplications(const std::vector<OpcUa::ApplicationDescription>& applications) override
    {
      Endpoints->AddApplications(applications);
    }


    virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
    {
      return Endpoints->FindServers(params);
    }

    virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
    {
      return Endpoints->GetEndpoints(filter);
    }

    virtual void RegisterServer(const ServerParameters& parameters)
    {
      Endpoints->RegisterServer(parameters);
    }

  private:
    void ApplyAddonParameters(const Common::AddonParameters& addons)
    {
      for (const Common::Parameter parameter : addons.Parameters)
      {
        if (parameter.Name == "debug" && !parameter.Value.empty() && parameter.Value != "0")
        {
          Debug = true;
        }
      }
    }

  private:
    EndpointsRegistry::SharedPtr Endpoints;
    ServicesRegistry::SharedPtr InternalServer;
    bool Debug = false;
  };

} // namespace


namespace OpcUa
{
  namespace UaServer
  {
    Common::Addon::UniquePtr EndpointsRegistryAddonFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new ::EndpointsAddon());
    }
  }
}

