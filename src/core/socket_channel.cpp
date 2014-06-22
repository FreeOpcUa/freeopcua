/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/socket_channel.h>
#include <opc/ua/errors.h>

#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

OpcUa::SocketChannel::SocketChannel(int sock)
  : Socket(sock)
{
  if (Socket < 0)
  {
    THROW_ERROR(CannotCreateChannelOnInvalidSocket);
  }
}

OpcUa::SocketChannel::~SocketChannel()
{
  int error = close(Socket);
  if (error < 0)
  {
    std::cerr << "Failed to close socket connection. " << strerror(errno) << std::endl;
  }
}

std::size_t OpcUa::SocketChannel::Receive(char* data, std::size_t size)
{
  int received = recv(Socket, data, size, MSG_WAITALL);
  if (received < 0)
  {
    THROW_OS_ERROR("Failed to receive data from host.");
  }
  if (received == 0)
  {
    THROW_OS_ERROR("Connection was closed by host.");
  }
  return (std::size_t)size;
}

void OpcUa::SocketChannel::Send(const char* message, std::size_t size)
{
  int sent = send(Socket, message, size, 0);
  if (sent != (int)size)
  {
    THROW_OS_ERROR("unable to send data to the host. ");
  }
}

//Return 1 id data, 0 if timeout and <0 if error
int OpcUa::SocketChannel::WaitForData(float second)
{
    fd_set          readSet;
    struct timeval  timeout;
    timeout.tv_sec = 0; 
    timeout.tv_usec = second*1000000; 

    FD_ZERO(&readSet);
    //FD_SET((unsigned int) Socket, &readSet);
    FD_SET(Socket, &readSet);

    return select(Socket+1, &readSet, NULL, NULL, &timeout);
}
