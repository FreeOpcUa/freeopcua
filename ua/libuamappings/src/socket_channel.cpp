/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include "socket_channel.h"

#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace OpcUa
{
  SocketChannel::SocketChannel(int sock)
    : Socket(sock)
  {
    if (Socket < 0)
    {
      throw std::logic_error("Internal error: unable to create connection on invalid socket.");
    }
  }

  SocketChannel::~SocketChannel()
  {
    int error = close(Socket);
    if (error < 0)
    {
      std::cerr << "Failed to close socket connection. " << strerror(errno) << std::endl;
    }
  }

  std::size_t SocketChannel::Receive(char* data, std::size_t size)
  {
    int received = recv(Socket, data, size, MSG_WAITALL);
    if (received < 0)
    {
      throw std::logic_error(std::string("Failed to receive data from host. ") + strerror(errno) + ".");
    }
    if (received == 0)
    {
      throw std::logic_error("Connection was closed by host.");
    }
    return (std::size_t)size;
  }

  void SocketChannel::Send(const char* message, std::size_t size)
  {
    int sent = send(Socket, message, size, 0);
    if (sent != (int)size)
    {
      throw std::logic_error(std::string("unable to send data to the host. ") + strerror(errno) + std::string("."));
    }
  }
} // namespace OpcUa

