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

namespace
{

  using namespace OpcUa::Server;

  class TcpServerAddon : public ::OpcUa::Server::TcpServerAddon
  {
    typedef std::map<unsigned short, std::shared_ptr<ConnectionListener>> ServersMap;
  public:
    TcpServerAddon()
    {
    }

    virtual ~TcpServerAddon()
    {
    }


    virtual void Listen(const TcpParameters& params, std::shared_ptr<IncomingConnectionProcessor> processor)
    {
      std::shared_ptr<ConnectionListener> server = OpcUa::CreateTcpServer(params.Port);
      server->Start(processor);
      Servers.insert(std::make_pair(params.Port, server));
    }

    virtual void StopListen(unsigned port)
    {
      ServersMap::iterator serverIt = Servers.find(port);
      if (serverIt == Servers.end())
      {
        std::cerr << "Was an attempt to stop listening unknown tcp server." << std::endl;
        return;
      }
      serverIt->second->Stop();
      Servers.erase(serverIt);
    }

  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& manager)
    {
    }

    virtual void Stop()
    {
      try
      {
        for (ServersMap::iterator serverIt = Servers.begin(); serverIt != Servers.end(); ++serverIt)
        {
          serverIt->second->Stop();
        }
        Servers.clear();
      }
      catch (const std::exception& exc)
      {
        std::clog << "Stopping TcpServerAddon failed with error: " << exc.what() << std::endl;
      }
    }

  private:
    ServersMap Servers;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::TcpServerAddon());
}

