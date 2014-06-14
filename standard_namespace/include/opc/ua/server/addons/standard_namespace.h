/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Function which fill standard opcua address space.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/node_management.h>

namespace OpcUa
{
  namespace UaServer
  {

    class StandardNamespaceAddonFactory : public Common::AddonFactory
    {
    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

    const char StandardNamespaceAddonID[] = "standard_namespace";

  } // namespace UaServer
} // namespace OpcUa

