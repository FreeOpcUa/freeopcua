/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/tcp_server.h>

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  OpcUa::UaServer::TcpServerAddonFactory factory;
  return factory.CreateAddon();
}
