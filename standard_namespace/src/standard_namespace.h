/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef OPC_UA_SERVER_STANDARD_NAME_SPACE_H
#define OPC_UA_SERVER_STANDARD_NAME_SPACE_H

#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/addon_manager.h>

#include <opc/ua/server/addons/address_space_registry.h>

namespace OpcUa
{
  namespace Internal
  {

    void FillStandardNamespace(OpcUa::Remote::NodeManagementServices& registry, bool debug);

    class StandardNamespaceAddon : public Common::Addon
    {
    public:
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();
    };


    class StandardNamespaceAddonFactory : public Common::AddonFactory
    {
    public:
      virtual Common::Addon::UniquePtr CreateAddon()
      {
        return Common::Addon::UniquePtr(new StandardNamespaceAddon());
      }
    };

    const char StandardNamespaceAddonID[] = "standard_namespace";

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_SERVER_STANDARD_NAME_SPACE_H
