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

#include <opc/ua/node_management.h>

namespace OpcUa
{
  namespace UaServer
  {

    void FillStandardNamespace(OpcUa::Remote::NodeManagementServices& registry, bool debug);

  } // namespace UaServer
} // namespace OpcUa

