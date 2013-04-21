/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "opc_tcp_processor.h"

#include <opc/ua/server/addons/endpoints.h>
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opccore/common/addons_core/addon_manager.h>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;


  class Endpoints : public EndpointsAddon
  {
  public:
    Endpoints()
    {
    }

    virtual ~Endpoints()
    {
    }

    std::vector<Endpoint> GetEndpoints() const
    {
      return std::vector<Endpoint>();
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      InternalComputer = Common::GetAddon<OpcUa::Server::InternalComputerAddon>(addons, OpcUa::Server::InternalComputerAddonID);
      TcpAddon = Common::GetAddon<OpcUa::Server::TcpServerAddon>(addons, OpcUa::Server::TcpServerAddonID);
      std::shared_ptr<IncomingConnectionProcessor> processor = OpcUa::Internal::CreateOpcTcpProcessor(InternalComputer->GetComputer());
      TcpParameters tcpParams;
      tcpParams.Port = 4841;

      TcpAddon->Listen(tcpParams, processor);
    }

    virtual void Stop()
    {
      TcpParameters params;
      params.Port = 4841;
      TcpAddon->StopListen(params);

      InternalComputer->UnregisterEndpointsServices();
      InternalComputer.reset();
      TcpAddon.reset();
    }

  private:
    std::shared_ptr<OpcUa::Server::TcpServerAddon> TcpAddon;
    std::shared_ptr<OpcUa::Server::InternalComputerAddon> InternalComputer;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::Endpoints());
}

