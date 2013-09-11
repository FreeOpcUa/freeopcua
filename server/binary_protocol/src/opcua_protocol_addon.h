/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include "opc_tcp_processor.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/tcp_server_addon.h>

namespace OpcUa
{
  namespace Impl
  {

    class OpcUaProtocol : public Common::Addon
    {
    public:
      OpcUaProtocol();

      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    private:
      void ApplyAddonParameters(const Common::AddonParameters& params);
      void PublishApplicationsInformation(std::vector<ApplicationDescription> applications, std::vector<EndpointDescription> endpoints, const Common::AddonsManager& addons) const;
      void StartEndpoints(std::vector<EndpointDescription> endpoints, Common::AddonsManager& addons);
      void FillEndpointDescription(const std::vector<Common::Parameter>& params, EndpointDescription& desc);

    private:
      std::shared_ptr<OpcUa::Server::TcpServerAddon> TcpAddon;
      std::shared_ptr<OpcUa::Server::ServicesRegistryAddon> InternalComputer;
      std::vector<unsigned short> Ports;
      bool Debug;
    };

  } // namespace Impl
} // namespace OpcUa
