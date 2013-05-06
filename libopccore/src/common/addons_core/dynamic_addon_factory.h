/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon factory.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_CORE_DYNAMIC_ADDON_FACTORY_H
#define OPC_CORE_DYNAMIC_ADDON_FACTORY_H

#include "dynamic_library.h"

#include <opc/common/addons_core/dynamic_addon.h>
#include <opc/common/class_pointers.h>

namespace Common
{

  class DynamicAddonFactory : public AddonFactory
  {
  public:
    DEFINE_CLASS_POINTERS(DynamicAddonFactory);

  public:
    DynamicAddonFactory(const std::string& modulePath);

    virtual Addon::UniquePtr CreateAddon();

  private:
    DynamicLibrary Library;
    CreateAddonFunc Create;
  };

}

#endif // OPC_CORE_DYNAMIC_ADDON_FACTORY_H

