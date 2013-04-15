/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/endpoints.h>
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
  public:
    TcpServerAddon()
    {
    }

    virtual ~TcpServerAddon()
    {
    }

/*
    virtual void Listen(const TcpParameters& params, std::shared_ptr<IncomingConnectionProcessor> processor)
    {
    }

    virtual void StopListen(const TcpParameters& tcpParams)
    {
    }
*/
  public: // Common::Addon
    virtual void Initialize(Common::AddonsManager& addons)
    {
      std::shared_ptr<EndpointsAddon> endpoints = Common::GetAddon<EndpointsAddon>(addons, OpcUa::Server::EndpointsAddonID);
      TcpParameters params;
      params.Port = 4841;
      Server = OpcUa::CreateTcpServer(params.Port);
      std::shared_ptr<OpcUa::Server::IncomingConnectionProcessor> processor = endpoints->GetProcessor();
      Server->Start(processor);
    }

    virtual void Stop()
    {
      try
      {
        Server->Stop();
      }
      catch (const std::exception& exc)
      {
        std::clog << "Stopping TcpServerAddon failed with error: " << exc.what() << std::endl;
      }
    }

  private:
    std::shared_ptr<ConnectionListener> Server;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::TcpServerAddon());
}

