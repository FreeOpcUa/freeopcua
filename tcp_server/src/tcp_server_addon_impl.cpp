/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "tcp_server_addon_impl.h"

#include <opc/ua/server/tcp_server.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <stdexcept>

namespace OpcUa
{
  namespace Impl
  {
    using namespace OpcUa::UaServer;

    void TcpServerAddon::Listen(const TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor)
    {
      if (Servers.find(params.Port) != std::end(Servers))
      {
        // TODO add portnumber into message.
        throw std::logic_error("Server already started at this port.");
      }

      std::shared_ptr<ConnectionListener> listener = OpcUa::CreateTcpServer(params.Port);
      listener->Start(processor);
      Servers.insert(std::make_pair(params.Port, listener));
    }

    void TcpServerAddon::StopListen(const TcpParameters& params)
    {
      ServersMap::iterator serverIt = Servers.find(params.Port);
      if (serverIt == std::end(Servers))
      {
        return;
      }

      try
      {
        serverIt->second->Stop();
        Servers.erase(serverIt);
      }
      catch (const std::exception& exc)
      {
        // TODO add port number to the message
        std::clog << "Stopping TcpServerAddon failed with error: " << exc.what() << std::endl;
      }
    }

    void TcpServerAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
    }

    void TcpServerAddon::Stop()
    {
      for (auto server : Servers)
      {
        server.second->Stop();
      }
      Servers.clear();
    }
  }
}
