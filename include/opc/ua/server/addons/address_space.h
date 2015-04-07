/// @author Olivier Roulet-Dubonnet 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    const char AddressSpaceRegistryAddonId[] = "address_space_registry";

    class AddressSpaceAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  } // namespace UaServer
} // namespace OpcUa
