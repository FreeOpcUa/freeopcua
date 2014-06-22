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
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/services_registry.h>


namespace OpcUa
{
  namespace Test
  {

    inline void RegisterAddressSpace(Common::AddonsManager& addons)
    {
      Common::AddonInformation config;
      config.Factory.reset(new OpcUa::UaServer::AddressSpaceAddonFactory());
      config.ID = OpcUa::UaServer::AddressSpaceRegistryAddonID;
      config.Dependencies.push_back(OpcUa::UaServer::ServicesRegistryAddonID);
      addons.Register(config);
    }

  }
}


