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

namespace OpcUa
{
  class InputChannel
  {
  public:
    /// @brief Receive data
    /// @param data data for send
    /// @param size size of data
    /// @return size of received data
    virtual std::size_t Receive(char* data, std::size_t size) = 0;

    InputChannel(){};
    virtual ~InputChannel(){}
  private:
    InputChannel(const InputChannel&){}
    InputChannel& operator= (const InputChannel&){return *this;}
  };

  class OutputChannel
  {
  public:
    virtual void Send(const char* message, std::size_t size) = 0;

    OutputChannel(){}
    virtual ~OutputChannel(){}
  private:
    OutputChannel(const OutputChannel&){}
    OutputChannel& operator= (const OutputChannel&){return *this;}
  };

  class IOChannel :
    public virtual InputChannel,
    public virtual OutputChannel
  {
  };

}

#endif // __OPC_UA_BINARY_CONNECT_H_

