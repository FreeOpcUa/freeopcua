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
#include <opc/ua/server/standard_namespace.h>

#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/node_management.h>


namespace
{

  class StandardNamespaceAddon : public Common::Addon
  {
  public:
    DEFINE_CLASS_POINTERS(StandardNamespaceAddon);

    void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      OpcUa::Remote::NodeManagementServices::SharedPtr registry = addons.GetAddon<OpcUa::Remote::NodeManagementServices>(OpcUa::UaServer::AddressSpaceRegistryAddonID);
      const bool debug = false;
      OpcUa::UaServer::FillStandardNamespace(*registry, false);
    }

    void Stop()
    {
    }
  };

} // namespace

namespace OpcUa
{
  namespace UaServer
  {

    Common::Addon::UniquePtr StandardNamespaceAddonFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new StandardNamespaceAddon());
    }

  }
}
