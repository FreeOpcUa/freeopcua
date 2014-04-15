/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server factory.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/connection_listener.h>

#include <memory>

namespace OpcUa
{

  std::unique_ptr<UaServer::ConnectionListener> CreateTcpServer(unsigned short port);

}

