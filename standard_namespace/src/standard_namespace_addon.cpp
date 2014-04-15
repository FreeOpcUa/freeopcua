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
      UaServer::AddressSpaceRegistry::SharedPtr registry = addons.GetAddon<UaServer::AddressSpaceRegistry>(UaServer::AddressSpaceRegistryAddonID);
      FillStandardNamespace(*registry);
    }

    void StandardNamespaceAddon::Stop()
    {
    }

  } // namespace Internal
} // namespace OpcUa
