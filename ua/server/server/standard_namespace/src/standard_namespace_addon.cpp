/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "standard_namespace.h"

namespace OpcUa
{
  namespace Internal
  {

    void StandardNamespaceAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Server::AddressSpaceRegistry::SharedPtr registry = Common::GetAddon<Server::AddressSpaceRegistry>(addons, Server::AddressSpaceRegistryAddonID);
      FillStandardNamespace(*registry);
    }

    void StandardNamespaceAddon::Stop()
    {
    }

  } // namespace Internal
} // namespace OpcUa

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new OpcUa::Internal::StandardNamespaceAddon());
}
