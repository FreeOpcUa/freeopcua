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
#include <opc/ua/server/addons/standard_address_space.h>

namespace OpcUa
{
  namespace Test
  {

    inline void RegisterStandardNamespace(Common::AddonsManager& addons)
    {
      Common::AddonInformation config = Server::CreateStandardNamespaceAddon();
      addons.Register(config);
    }

  }
}
