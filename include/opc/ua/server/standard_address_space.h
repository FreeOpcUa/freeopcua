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

#include <opc/ua/services/node_management.h>

namespace OpcUa
{
namespace Server
{

void FillStandardNamespace(OpcUa::NodeManagementServices & registry, const Common::Logger::SharedPtr & logger);

} // namespace UaServer
} // namespace OpcUa

