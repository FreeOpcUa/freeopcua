/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace UaServer
  {

    class TcpServerFactory : public Common::AddonFactory
    {
    public:
      Common::Addon::UniquePtr CreateAddon();
    };

  }
}

