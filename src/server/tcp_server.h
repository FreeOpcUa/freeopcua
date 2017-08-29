/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server factory.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/interface.h>
#include <opc/common/logger.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/connection_listener.h>

#include <memory>

namespace OpcUa
{
namespace Server
{
struct TcpParameters
{
  unsigned short Port;

  TcpParameters()
    : Port(0)
  {
  }
};

class TcpServer : public Common::Interface
{
public:
  DEFINE_CLASS_POINTERS(TcpServer)

  TcpServer(const Common::Logger::SharedPtr & logger)
    : Logger(logger)
  {}

  virtual void Listen(const OpcUa::Server::TcpParameters & params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor) = 0;
  virtual void StopListen(const OpcUa::Server::TcpParameters & params) = 0;

protected:
  Common::Logger::SharedPtr Logger;
};

TcpServer::UniquePtr CreateTcpServer(const Common::Logger::SharedPtr & logger);

} // namespace UaServer
} // namespace OpcUa

