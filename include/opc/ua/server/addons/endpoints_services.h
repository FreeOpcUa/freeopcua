/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/types.h>

namespace OpcUa
{
  namespace Server
  {

    const char EndpointsRegistryAddonId[] = "endpoints_services";


    class EndpointsRegistryAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(EndpointsRegistryAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon() override;
    };

  } // namespace UaServer
} // nmespace OpcUa

