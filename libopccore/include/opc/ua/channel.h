/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Input/output channels interfaces.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_CONNECT_H_
#define __OPC_UA_BINARY_CONNECT_H_

#include <cstddef>
#include <opc/common/class_pointers.h>
#include <opc/common/interface.h>

namespace OpcUa
{
  class InputChannel : protected virtual Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(InputChannel);

  public:
    /// @brief Receive data
    /// @param data data for send
    /// @param size size of data
    /// @return size of received data
    virtual std::size_t Receive(char* data, std::size_t size) = 0;
  };

  class OutputChannel : protected virtual Common::Interface
  {
   public:
    DEFINE_CLASS_POINTERS(OutputChannel);

   public:
    virtual void Send(const char* message, std::size_t size) = 0;
  };

  class IOChannel :
    public InputChannel,
    public OutputChannel
  {
  public:
    DEFINE_CLASS_POINTERS(IOChannel);
  };

}

#endif // __OPC_UA_BINARY_CONNECT_H_

