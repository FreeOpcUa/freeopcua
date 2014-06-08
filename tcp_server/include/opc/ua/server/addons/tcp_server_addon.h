/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/connection_listener.h>

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

    class TcpServerAddon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(TcpServerAddon);

      virtual void Listen(const TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor) = 0;
      virtual void StopListen(const TcpParameters& tcpParams) = 0;
    };

    const char TcpServerAddonID[] = "tcp_server";

    TcpServerAddon::UniquePtr CreateTcpServerAddon();

  }
}

