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

#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opccore/common/addons_core/addon_manager.h>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;


  class OpcUaProtocol : public Common::Addon
  {
  public:
    OpcUaProtocol()
    {
    }

    virtual ~OpcUaProtocol()
    {
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      std::shared_ptr<EndpointsServicesAddon> endpointsAddon = Common::GetAddon<EndpointsServicesAddon>(addons, EndpointsServicesAddonID);
      EndpointDescription desc;
      desc.EndpointURL = "opc.tcp://localhost:4841";
      desc.ServerDescription.URI = "server.opcua.treww.org";
      desc.ServerDescription.ProductURI = "server.opcua.treww.org";
      desc.ServerDescription.Name.Encoding = HAS_TEXT;
      desc.ServerDescription.Name.Text = "opcua_server";
      desc.ServerDescription.Type = ApplicationType::SERVER;
      desc.SecurityMode = MessageSecurityMode::MSM_NONE;
      UserTokenPolicy tokenPolicy;
      tokenPolicy.PolicyID = "Anonymous";
      tokenPolicy.TokenType = UserIdentifyTokenType::ANONYMOUS;
      tokenPolicy.SecurityPolicyURI = "http://opcfoundation.org/UA/SecurityPolicy#None";
      desc.UserIdentifyTokens.push_back(tokenPolicy);
      desc.TransportProfileURI = "http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
      endpointsAddon->AddEndpoints(std::vector<EndpointDescription>(1, desc));


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
      TcpAddon.reset();
    }

  private:
    std::shared_ptr<OpcUa::Server::TcpServerAddon> TcpAddon;
    std::shared_ptr<OpcUa::Server::InternalComputerAddon> InternalComputer;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new OpcUaProtocol());
}

