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
#include <opc/ua/server/server.h>

namespace OpcUa
{
  namespace Server
  {

    class TcpServerAddon : public Common::Addon
    {
    public:
      virtual void SetConnectionProcessor(std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor) = 0;
    };

  }
}

#endif // opcua_server_tcp_addon_h

