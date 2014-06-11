/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/services_registry.h>

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  OpcUa::UaServer::ServicesRegistryFactory factory;
  return factory.CreateAddon();
}
