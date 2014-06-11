/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/server/addons/opcua_protocol.h>


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  OpcUa::UaServer::OpcUaProtocolAddonFactory factory;
  return factory.CreateAddon();
}

