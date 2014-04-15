/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa binary protocol connection processor.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/server.h>
#include <opc/ua/connection_listener.h>

namespace OpcUa
{
  namespace Internal
  {

    std::unique_ptr<OpcUa::UaServer::IncomingConnectionProcessor> CreateOpcTcpProcessor(std::shared_ptr<OpcUa::Remote::Server> server, bool debug);

  }
}
