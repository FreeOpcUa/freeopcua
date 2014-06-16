/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Server wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/server/services_registry.h>
#include <opc/ua/server.h>

namespace OpcUa
{
  namespace UaServer
  {

    const char ServicesRegistryAddonID[] = "services_registry";

    class ServicesRegistryFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(ServicesRegistryFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  } // namespace UaServer
} // namespace OpcUa
