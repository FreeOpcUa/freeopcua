/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/standard_address_space.h>


#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/standard_address_space.h>
#include <opc/ua/services/node_management.h>


namespace
{

class StandardNamespaceAddon : public Common::Addon
{
public:
  DEFINE_CLASS_POINTERS(StandardNamespaceAddon)

  void Initialize(Common::AddonsManager & addons, const Common::AddonParameters & params)
  {
    OpcUa::NodeManagementServices::SharedPtr registry = addons.GetAddon<OpcUa::NodeManagementServices>(OpcUa::Server::AddressSpaceRegistryAddonId);
    OpcUa::Server::FillStandardNamespace(*registry, addons.GetLogger());
  }

  void Stop()
  {
  }
};

} // namespace

namespace OpcUa
{
namespace Server
{

Common::Addon::UniquePtr StandardNamespaceAddonFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new StandardNamespaceAddon());
}

}
}
