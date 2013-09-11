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

#include "../src/standard_namespace.h"

#include <opc/common/addons_core/addon_manager.h>

namespace OpcUa
{
  namespace Test
  {

    void RegisterStandardNamespace(Common::AddonsManager& addons)
    {
      Common::AddonConfiguration config;
      config.Factory.reset(new OpcUa::Internal::StandardNamespaceAddonFactory());
      config.ID = OpcUa::Internal::StandardNamespaceAddonID;
      config.Dependencies.push_back(OpcUa::Server::AddressSpaceRegistryAddonID);
      addons.Register(config);
    }

  }
}
