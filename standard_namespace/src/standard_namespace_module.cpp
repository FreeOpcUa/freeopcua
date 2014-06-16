/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/server/addons/standard_namespace.h>

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  OpcUa::UaServer::StandardNamespaceAddonFactory factory;
  return factory.CreateAddon();
}
