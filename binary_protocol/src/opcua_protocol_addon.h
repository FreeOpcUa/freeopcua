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
#include <opc/ua/server/addons/opcua_protocol.h>

namespace OpcUa
{
  namespace Impl
  {

    class OpcUaProtocol : public Common::Addon
    {
    public:
      OpcUaProtocol();
      OpcUaProtocol(UaServer::ServicesRegistryAddon::SharedPtr Registry, OpcUa::UaServer::TcpServerAddon::SharedPtr TcpServer,  std::vector<EndpointDescription> Endpoints);

      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    private:
      void ApplyAddonParameters(const Common::AddonParameters& params);
      void PublishApplicationsInformation(std::vector<ApplicationDescription> applications, std::vector<EndpointDescription> endpoints, const Common::AddonsManager& addons) const;
      void StartEndpoints(std::vector<EndpointDescription> endpoints);
      void FillEndpointDescription(const std::vector<Common::Parameter>& params, EndpointDescription& desc);

    private:
      OpcUa::UaServer::ServicesRegistryAddon::SharedPtr InternalServer;
      OpcUa::UaServer::TcpServerAddon::SharedPtr TcpAddon;
      std::vector<unsigned short> Ports;
      bool Debug;
    };

  } // namespace Impl
} // namespace OpcUa
