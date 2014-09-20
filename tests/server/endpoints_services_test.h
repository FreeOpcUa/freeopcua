/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/endpoints_services.h>

namespace OpcUa
{
  namespace Test
  {

    inline void RegisterEndpointsServicesAddon(Common::AddonsManager& addons)
    {
      Common::AddonInformation endpoints;
      endpoints.Factory.reset(new OpcUa::Server::EndpointsRegistryAddonFactory());
      endpoints.ID = OpcUa::Server::EndpointsRegistryAddonID;
      endpoints.Dependencies.push_back(OpcUa::Server::ServicesRegistryAddonID);
      addons.Register(endpoints);
    }

  }
}

