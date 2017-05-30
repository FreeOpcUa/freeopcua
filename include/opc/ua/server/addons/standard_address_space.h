/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Function which fill standard opcua address space.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/services/node_management.h>

namespace OpcUa
{
namespace Server
{

class StandardNamespaceAddonFactory : public Common::AddonFactory
{
public:
  virtual Common::Addon::UniquePtr CreateAddon();
};

const char StandardNamespaceAddonId[] = "standard_namespace";

} // namespace UaServer
} // namespace OpcUa

