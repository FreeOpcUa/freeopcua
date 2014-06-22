/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua server interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_BINARY_server_H
#define OPC_UA_BINARY_server_H

#include <opc/ua/protocol/channel.h>
#include <opc/ua/server.h>


#include <memory>

namespace OpcUa
{
  namespace Remote
  {

    /// @brief Create server based on opc ua binary protocol.
    /// @param channel channel wich will be used for sending requests data.
    std::unique_ptr<Server> CreateBinaryServer(std::shared_ptr<IOChannel> channel);

  }
}

#endif //  OPC_UA_BINARY_server_H

