/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/server/opcua_protocol.h>

#include "endpoints_parameters.h"

#include <opc/common/uri_facade.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/tcp_server.h>


#include <stdexcept>


namespace
{
  class OpcUaProtocol : public Common::Addon
  {
  public:
    OpcUaProtocol()
      : Debug(false)
    {
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params) override;
    virtual void Stop() override;

  private:
    void ApplyAddonParameters(const Common::AddonParameters& params);
    void PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const;

  private:
    OpcUa::UaServer::ServicesRegistry::SharedPtr InternalServer;
    OpcUa::UaServer::TcpServer::SharedPtr TcpServer;
    OpcUa::UaServer::OpcUaProtocol::SharedPtr Protocol;
    bool Debug;
  };

  void OpcUaProtocol::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
  {
    ApplyAddonParameters(params);
    const std::vector<OpcUa::UaServer::ApplicationData> applications = OpcUa::ParseEndpointsParameters(params.Groups, Debug);
    for (OpcUa::UaServer::ApplicationData d: applications) {
      std::cout << "Endpoint is: " << d.Endpoints.front().EndpointURL << std::endl;
    }

    std::vector<OpcUa::ApplicationDescription> applicationDescriptions;
    std::vector<OpcUa::EndpointDescription> endpointDescriptions;
    for (const OpcUa::UaServer::ApplicationData application : applications)
    {
      applicationDescriptions.push_back(application.Application);
      endpointDescriptions.insert(endpointDescriptions.end(), application.Endpoints.begin(), application.Endpoints.end());
    }
    PublishApplicationsInformation(applicationDescriptions, endpointDescriptions, addons);
    InternalServer = addons.GetAddon<OpcUa::UaServer::ServicesRegistry>(OpcUa::UaServer::ServicesRegistryAddonID);
    TcpServer = addons.GetAddon<OpcUa::UaServer::TcpServer>(OpcUa::UaServer::TcpServerAddonID);
    Protocol = OpcUa::UaServer::CreateOpcUaProtocol(TcpServer, Debug);
    Protocol->StartEndpoints(endpointDescriptions, InternalServer->GetServer());
  }

  void OpcUaProtocol::Stop()
  {
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

  void OpcUaProtocol::PublishApplicationsInformation(std::vector<OpcUa::ApplicationDescription> applications, std::vector<OpcUa::EndpointDescription> endpoints, const Common::AddonsManager& addons) const
  {
    OpcUa::UaServer::EndpointsRegistry::SharedPtr endpointsAddon = addons.GetAddon<OpcUa::UaServer::EndpointsRegistry>(OpcUa::UaServer::EndpointsRegistryAddonID);
    if (!endpointsAddon)
    {
      std::cerr << "Cannot save information about endpoints. Endpoints services addon didn't' registered." << std::endl;
      return;
    }
    endpointsAddon->AddEndpoints(endpoints);
    endpointsAddon->AddApplications(applications);
  }

} // namespace


namespace OpcUa
{
  namespace UaServer
  {
    Common::Addon::UniquePtr OpcUaProtocolAddonFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new ::OpcUaProtocol());
    }
  }
}
