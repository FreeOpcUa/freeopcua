/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua computer interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_COMPUTER_H
#define OPC_UA_CLIENT_COMPUTER_H

#include <opc/ua/server.h>

#include <memory>

namespace OpcUa
{
  namespace Remote
  {

    /// @brief connect to remote computer
    /// @param url url of the server.
    /// @note url in the form: opc.tcp://host:port connect with opc ua binary protocol
    /// @note url in the form https://host:port
    Server::UniquePtr Connect(const std::string& url);

  }
}

#endif //  OPC_UA_CLIENT_COMPUTER_H
