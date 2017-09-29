/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/client/remote_connection.h>
#include <opc/ua/errors.h>
#include <opc/ua/socket_channel.h>
#include <opc/ua/protocol/utils.h>


#include <errno.h>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <sys/types.h>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

namespace
{

unsigned long GetIPAddress(const std::string & hostName)
{
  // TODO Use getaddrinfo
  hostent * host = gethostbyname(hostName.c_str());

  if (!host)
    {
      THROW_OS_ERROR("Unable to to resolve host '" + hostName + "'.");
    }

  return *(unsigned long *)host->h_addr_list[0];
}

int ConnectToRemoteHost(const std::string & host, unsigned short port)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
    {
      THROW_OS_ERROR("Unable to create socket for connecting to the host '" + host + ".");
    }

  sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = GetIPAddress(host);

  int error = connect(sock, (sockaddr *)& addr, sizeof(addr));

  if (error < 0)
    {
#ifdef _WIN32
      closesocket(sock);
#else
      close(sock);
#endif
      THROW_OS_ERROR(std::string("Unable connect to host '") + host + std::string("'. "));
    }

  return sock;
}

class BinaryConnection : public OpcUa::RemoteConnection
{
public:
  BinaryConnection(int sock, const std::string & host, unsigned short port, const Common::Logger::SharedPtr & logger)
    : HostName(host)
    , Port(port)
    , Channel(sock)
    , Logger(logger)
  {
  }

  virtual ~BinaryConnection()
  {
  }

  virtual std::size_t Receive(char * data, std::size_t size)
  {
    return Channel.Receive(data, size);
  }

  virtual void Send(const char * message, std::size_t size)
  {
    LOG_TRACE(Logger, "binary_connection     | send: {}", OpcUa::ToHexDump(message, size));
    return Channel.Send(message, size);
  }


  virtual void Stop()
  {
    Channel.Stop();
  }

  virtual std::string GetHost() const
  {
    return HostName;
  }

  virtual unsigned GetPort() const
  {
    return Port;
  }

private:
  const std::string HostName;
  const unsigned Port;
  OpcUa::SocketChannel Channel;
  Common::Logger::SharedPtr Logger;
};

}

std::unique_ptr<OpcUa::RemoteConnection> OpcUa::Connect(const std::string & host, unsigned port, const Common::Logger::SharedPtr & logger)
{
  const int sock = ConnectToRemoteHost(host, port);
  return std::unique_ptr<RemoteConnection>(new BinaryConnection(sock, host, port, logger));
}

