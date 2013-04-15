/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opcua_server_tcp_addon_h
#define opcua_server_tcp_addon_h

#include <opccore/common/addons_core/addon.h>
//#include <opc/ua/server/server.h>

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

    class TcpServerAddon : public Common::Addon
    {
    public:
//      virtual void Listen(const TcpParameters& params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor) = 0;
//      virtual void StopListen(const TcpParameters& tcpParams) = 0;
    };

    const char TcpServerAddonID[] = "tcp_server";

  }
}

#endif // opcua_server_tcp_addon_h

