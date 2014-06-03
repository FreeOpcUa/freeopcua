/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Computer wich is used by addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/services_registry_factory.h>
#include "services_registry_impl.h"

namespace OpcUa
{
  namespace UaServer
  {
    Common::Addon::UniquePtr ServicesRegistryFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new OpcUa::Impl::ServicesRegistry());
    }
  }
}
