/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Addon for accessing opcua server inside process.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/services/services.h>

namespace OpcUa
{
  namespace Server
  {

    const char BuiltinServerAddonId[] = "builtin_server_addon";

    class BuiltingServerFactory : public Common::AddonFactory
    {
    public:
        DEFINE_CLASS_POINTERS(BuiltingServerFactory);

      public:
        virtual Common::Addon::UniquePtr CreateAddon();
    };


  } // namespace UaServer
} // namespace OpcUa
