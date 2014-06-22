/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tcp server addon..
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/tcp_server.h>
#include <opc/ua/server/tcp_server.h>


namespace
{

  class TcpServerAddon : public Common::Addon, public OpcUa::UaServer::TcpServer
  {
  public: // Common::Addon
    void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Server = OpcUa::UaServer::CreateTcpServer();
    }

    void Stop()
    {
      Server.reset();
    }

  public: // TcpServer
    void Listen(const OpcUa::UaServer::TcpParameters& params, std::shared_ptr<OpcUa::UaServer::IncomingConnectionProcessor> processor)
    {
      Server->Listen(params, processor);
    }

    void StopListen(const OpcUa::UaServer::TcpParameters& params)
    {
      Server->StopListen(params);
    }

  private:
    OpcUa::UaServer::TcpServer::UniquePtr Server;
  };

} // namespace


namespace OpcUa
{
  namespace UaServer
  {

    Common::Addon::UniquePtr TcpServerAddonFactory::CreateAddon()
    {
      return Common::Addon::UniquePtr(new ::TcpServerAddon());
    }

  } // namespace UaServer
} // namespace OpcUa

