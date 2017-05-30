/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#pragma once

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/addons/common_addons.h>

namespace OpcUa
{
namespace Test
{

inline void RegisterAddressSpace(Common::AddonsManager & addons)
{
  Common::AddonInformation config = Server::CreateAddressSpaceAddon();
  addons.Register(config);
}

}
}


