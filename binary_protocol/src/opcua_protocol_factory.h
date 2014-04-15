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

#include "opcua_protocol_addon.h"

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace UaServer
  {

    class OpcUaProtocolFactory : public Common::AddonFactory
    {
    public:
      virtual Common::Addon::UniquePtr CreateAddon()
      {
        return Common::Addon::UniquePtr(new Impl::OpcUaProtocol());
      }
    };

  } // namespace UaServer
} // namespace OpcUa
