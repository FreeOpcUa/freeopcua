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

#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netinet/tcp.h>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

OpcUa::SocketChannel::SocketChannel(int sock)
  : Socket(sock)
{
  int flag = 1;
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
  if (Socket < 0)
  {
    THROW_ERROR(CannotCreateChannelOnInvalidSocket);
  }
}

OpcUa::SocketChannel::~SocketChannel()
{
  Stop();
}

void OpcUa::SocketChannel::Stop()
{
  int error = shutdown(Socket, 2);
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
