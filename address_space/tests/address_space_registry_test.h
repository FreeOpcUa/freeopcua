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

#include "../src/address_space_addon.h"
#include <opc/common/addons_core/addon_manager.h>


namespace OpcUa
{
  namespace Test
  {

    void RegisterAddressSpace(Common::AddonsManager& addons)
    {
      Common::AddonInformation config;
      config.Factory.reset(new OpcUa::Internal::AddressSpaceAddonFactory());
      config.ID = OpcUa::Server::AddressSpaceRegistryAddonID;
      config.Dependencies.push_back(OpcUa::Server::ServicesRegistryAddonID);
      addons.Register(config);
    }

  }
}


