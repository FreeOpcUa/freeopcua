/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon factory.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "dynamic_addon_factory.h"

#include <opc/common/addons_core/errors.h>


namespace Common
{

  DynamicAddonFactory::DynamicAddonFactory(const std::string& modulePath)
    : Library(modulePath)
  {
  }

  Addon::UniquePtr DynamicAddonFactory::CreateAddon()
  {
    return Library.Find<CreateAddonFunc>("CreateAddon")();
  }

  Common::AddonFactory::UniquePtr CreateDynamicAddonFactory(const char* modulePath)
  {
    return Common::AddonFactory::UniquePtr(new DynamicAddonFactory(modulePath));
  }

  Common::AddonFactory::UniquePtr CreateDynamicAddonFactory(const std::string& modulePath)
  {
    return Common::AddonFactory::UniquePtr(new DynamicAddonFactory(modulePath));
  }
}
