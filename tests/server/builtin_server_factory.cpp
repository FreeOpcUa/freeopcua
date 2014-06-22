/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test addon wich emulate tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/builtin_server.h>

#include "builtin_server_impl.h"

Common::Addon::UniquePtr OpcUa::UaServer::BuiltingServerFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new OpcUa::Impl::BuiltinServerAddon);
}
