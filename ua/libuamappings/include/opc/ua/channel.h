/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Input/output channels interfaces.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __OPC_UA_BINARY_CONNECT_H_
#define __OPC_UA_BINARY_CONNECT_H_


#include <opc/ua/interface.h>
#include <cstddef>

namespace OpcUa
{
  class InputChannel : protected virtual Interface
  {
   public:
     /// @brief Receive data
     /// @param data data for send
     /// @param size size of data
     /// @return size of received data
     virtual std::size_t Receive(char* data, std::size_t size) = 0;
  };

  class OutputChannel : protected virtual Interface
  {
  public:
     virtual void Send(const char* message, std::size_t size) = 0;
  };

  class IOChannel :
    public virtual InputChannel,
    public virtual OutputChannel
  {
  };

}

#endif // __OPC_UA_BINARY_CONNECT_H_

