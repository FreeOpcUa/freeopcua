/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server factory.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef _OPC_UA_TCP_SERVER_H
#define _OPC_UA_TCP_SERVER_H

#include <opc/ua/connection_listener.h>

#include <memory>

namespace OpcUa
{

  std::unique_ptr<Server::ConnectionListener> CreateTcpServer(unsigned short port);

}

#endif // _OPC_UA_TCP_SERVER_H

