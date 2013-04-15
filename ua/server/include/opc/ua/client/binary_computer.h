/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua computer interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_BINARY_COMPUTER_H
#define OPC_UA_BINARY_COMPUTER_H

#include <opc/ua/channel.h>
#include <opc/ua/computer.h>


#include <memory>

namespace OpcUa
{
  namespace Remote
  {

    /// @brief Create computer based on opc ua binary protocol.
    /// @param channel channel wich will be used for sending requests data.
    std::unique_ptr<Computer> CreateBinaryComputer(std::shared_ptr<IOChannel> channel);

  }
}

#endif //  OPC_UA_BINARY_COMPUTER_H

