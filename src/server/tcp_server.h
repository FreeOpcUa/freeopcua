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
#include <opc/common/class_pointers.h>
#include <opc/ua/connection_listener.h>

#include <memory>

namespace OpcUa
{
  namespace UaServer
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
      DEFINE_CLASS_POINTERS(TcpServer);

      virtual void Listen(const OpcUa::UaServer::TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor) = 0;
      virtual void StopListen(const OpcUa::UaServer::TcpParameters& params) = 0;
    };

    TcpServer::UniquePtr CreateTcpServer();

  } // namespace UaServer
} // namespace OpcUa

