/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addon interface definition
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_CORE_DYNAMIC_ADDON_FACTORY_H
#define OPC_CORE_DYNAMIC_ADDON_FACTORY_H

#include <opc/common/addons_core/addon.h>


namespace Common
{

  AddonFactory::UniquePtr CreateDynamicAddonFactory(const char* modulePath);

  inline AddonFactory::UniquePtr CreateDynamicAddonFactory(const std::string& modulePath)
  {
    return ::Common::CreateDynamicAddonFactory(modulePath.c_str());
  }

}

#endif // OPC_CORE_DYNAMIC_ADDON_FACTORY_H

