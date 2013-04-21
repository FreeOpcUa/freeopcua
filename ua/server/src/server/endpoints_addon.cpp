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

    std::vector<Endpoint> GetEndpoints() const
    {
      return std::vector<Endpoint>();
    }

    std::shared_ptr<IncomingConnectionProcessor> GetProcessor() const
    {
      return OpcUa::Internal::CreateOpcTcpProcessor(std::shared_ptr<OpcUa::Remote::Computer>());
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      InternalComputer = Common::GetAddon<OpcUa::Server::InternalComputerAddon>(addons, OpcUa::Server::InternalComputerAddonID);
    }

    virtual void Stop()
    {
      InternalComputer->UnregisterEndpointsServices();
      InternalComputer.reset();
    }

  private:
    std::shared_ptr<OpcUa::Server::InternalComputerAddon> InternalComputer;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::Endpoints());
}

