/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addon interface definition
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_CORE_DYNAMIC_ADDON_H
#define OPC_CORE_DYNAMIC_ADDON_H

#include <opc/common/addons_core/addon.h>

extern "C" Common::Addon::UniquePtr CreateAddon();

typedef Common::Addon::UniquePtr (*CreateAddonFunc)();

#endif // OPC_CORE_DYNAMIC_ADDON_H

