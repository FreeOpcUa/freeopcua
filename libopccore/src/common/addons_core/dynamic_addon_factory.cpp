/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon factory.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "dynamic_library.h"

#include <opc/common/addons_core/errors.h>
#include <opc/common/addons_core/dynamic_addon.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <opc/common/class_pointers.h>


namespace
{
  using namespace Common;

  class DynamicAddonFactory : public AddonFactory
  {
  public:
    DEFINE_CLASS_POINTERS(DynamicAddonFactory);

  public:
    DynamicAddonFactory(const std::string& modulePath);

    virtual Addon::UniquePtr CreateAddon();

  private:
    DynamicLibrary Library;
  };

  DynamicAddonFactory::DynamicAddonFactory(const std::string& modulePath)
    : Library(modulePath)
  {
  }

  Addon::UniquePtr DynamicAddonFactory::CreateAddon()
  {
    return Library.Find<CreateAddonFunc>("CreateAddon")();
  }
}

Common::AddonFactory::UniquePtr Common::CreateDynamicAddonFactory(const char* modulePath)
{
  return Common::AddonFactory::UniquePtr(new DynamicAddonFactory(modulePath));
}
