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

#include "tcp_server_addon_impl.h"
#include <opc/ua/server/addons/tcp_server_factory.h>

namespace OpcUa
{
  namespace UaServer
  {

    Common::Addon::UniquePtr TcpServerFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new OpcUa::Impl::TcpServerAddon());
    }

  }
}

