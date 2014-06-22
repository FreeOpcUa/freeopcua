/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_SOCKET_CHANNEL_H
#define OPC_UA_CLIENT_SOCKET_CHANNEL_H

#include <opc/ua/protocol/channel.h>

namespace OpcUa
{

  class SocketChannel : public OpcUa::IOChannel
  {
  public:
    SocketChannel(int sock);
    virtual ~SocketChannel();

    virtual std::size_t Receive(char* data, std::size_t size);
    virtual void Send(const char* message, std::size_t size);
    virtual int WaitForData(float second);

  private:
    int Socket;
  };

}

#endif // OPC_UA_CLIENT_SOCKET_CHANNEL_H

