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

    class TcpServerAddon : public OpcUa::UaServer::TcpServerAddon
    {
    public:
      virtual void Listen(const OpcUa::UaServer::TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor);
      virtual void StopListen(const OpcUa::UaServer::TcpParameters& params);

    public: // Common::Addon
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    private:
      typedef std::map<unsigned short, std::shared_ptr<OpcUa::UaServer::ConnectionListener>> ServersMap;
      ServersMap Servers;
    };
  }
}

