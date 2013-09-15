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

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/services_registry.h>

#include "discovery_service.h"

namespace OpcUa
{
  namespace Impl
  {

    class SoapAddon : public Common::Addon
    {
    public:
      SoapAddon();

      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    private:
      void ApplyAddonParameters(const Common::AddonParameters& params);
      void PublishApplicationsInformation(std::vector<ApplicationDescription> applications, std::vector<EndpointDescription> endpoints, const Common::AddonsManager& addons) const;
      void StartEndpoints(std::vector<EndpointDescription> endpoints, Common::AddonsManager& addons);
      void FillEndpointDescription(const std::vector<Common::Parameter>& params, EndpointDescription& desc);

    private:
      bool Debug;
      SoapDiscoveryService DiscoveryService;
    };

  } // namespace Impl
} // namespace OpcUa

