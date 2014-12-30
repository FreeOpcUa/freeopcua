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
#include <opc/ua/server/addons/common_addons.h>
namespace OpcUa
{
  namespace Test
  {

    inline void RegisterEndpointsServicesAddon(Common::AddonsManager& addons)
    {
      Common::AddonInformation endpoints = Server::CreateEndpointsRegistryAddon();
      addons.Register(endpoints);
    }

  }
}

