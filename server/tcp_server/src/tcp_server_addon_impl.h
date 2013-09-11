/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opc/ua/server/tcp_server.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>

namespace OpcUa
{
  namespace Impl
  {

    class TcpServerAddon : public OpcUa::Server::TcpServerAddon
    {
    public:
      virtual void Listen(const OpcUa::Server::TcpParameters& params, std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor);
      virtual void StopListen(const OpcUa::Server::TcpParameters& params);

    public: // Common::Addon
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    private:
      typedef std::map<unsigned short, std::shared_ptr<OpcUa::Server::ConnectionListener>> ServersMap;
      ServersMap Servers;
    };
  }
}
