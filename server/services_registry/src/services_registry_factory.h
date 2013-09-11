/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/server/addons/services_registry.h>

namespace OpcUa
{
  namespace Server
  {

    class ServicesRegistryFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(ServicesRegistryFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  } // namespace Server
} // namespace OpcUa

