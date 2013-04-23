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

#include <internal/uri_facade.h>
#include <opc/ua/server/addons/opcua_protocol.h>
#include <opc/ua/server/addons/endpoints_services.h>
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/server/addons/tcp_server_addon.h>
#include <opccore/common/addons_core/addon_manager.h>

#include <stdexcept>

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
      const std::vector<EndpointDescription> endpoints = GetEndpointDescriptions(params);
      std::shared_ptr<EndpointsServicesAddon> endpointsAddon = Common::GetAddon<EndpointsServicesAddon>(addons, EndpointsServicesAddonID);
      endpointsAddon->AddEndpoints(endpoints);

      InternalComputer = Common::GetAddon<OpcUa::Server::InternalComputerAddon>(addons, OpcUa::Server::InternalComputerAddonID);
      TcpAddon = Common::GetAddon<OpcUa::Server::TcpServerAddon>(addons, OpcUa::Server::TcpServerAddonID);
      for (auto endpoint : endpoints)
      {
        const Internal::Uri uri(endpoint.EndpointURL);
        if (uri.Scheme() == "opc.tcp")
        {
          std::shared_ptr<IncomingConnectionProcessor> processor = OpcUa::Internal::CreateOpcTcpProcessor(InternalComputer->GetComputer());
          TcpParameters tcpParams;
          tcpParams.Port = uri.Port();
          TcpAddon->Listen(tcpParams, processor);
          Ports.push_back(tcpParams.Port);
        }
      }
    }

    virtual void Stop()
    {
      for (unsigned port : Ports)
      {
        TcpParameters params;
        params.Port = port;
        TcpAddon->StopListen(params);
      }
      TcpAddon.reset();
    }

  private:
    std::vector<EndpointDescription> GetEndpointDescriptions(const Common::AddonParameters& params)
    {
      EndpointDescription tmpDesc;
      FillEndpointDescription(params.Parameters, tmpDesc);

      std::vector<EndpointDescription> result;
      for (auto group : params.Groups)
      {
        if (group.Name == "endpoint")
        {
          EndpointDescription desc = tmpDesc;
          FillEndpointDescription(group.Parameters, desc);
          result.push_back(desc);

          for (auto subGroup : group.Groups)
          {
            if (subGroup.Name == "user_token_policy")
            {
              const UserTokenPolicy tokenPolicy = GetUserTokenPolicy(subGroup.Parameters);
              desc.UserIdentifyTokens.push_back(tokenPolicy);
            }
          }

        }
      }
      return result;
    }

  void FillEndpointDescription(const std::vector<Common::Parameter>& params, EndpointDescription& desc)
  {
    for (auto param : params)
    {
      if (param.Name == "application_uri")
      {
        desc.ServerDescription.URI = param.Value;
        desc.ServerDescription.ProductURI = param.Value;
      }
      else if (param.Name == "application_name")
      {
        desc.ServerDescription.Name.Encoding = HAS_TEXT;
        desc.ServerDescription.Name.Text = param.Value;
      }
      else if (param.Value == "application_type")
      {
        desc.ServerDescription.Type = GetApplicationType(param.Value);
      }
      else if (param.Name == "security_mode")
      {
          desc.SecurityMode = GetSecurityMode(param.Value);
      }
      else if (param.Name == "transport_profile_uri")
      {
        desc.TransportProfileURI = param.Value;//"http://opcfoundation.org/UA-Profile/Transport/uatcp-uasc-uabinary";
      }
      else if (param.Name == "url")
      {
        desc.EndpointURL = param.Value;//"opc.tcp://localhost:4841";
      }
    }

  }

  UserTokenPolicy GetUserTokenPolicy(const std::vector<Common::Parameter>& params)
  {
    UserTokenPolicy tokenPolicy;
    for (auto param : params)
    {
      if (param.Name == "id")
      {
        tokenPolicy.PolicyID = param.Value;//"Anonymous";
      }
      if (param.Name == "type")
      {
        tokenPolicy.TokenType = GetTokenType(param.Value);
      }
      if (param.Name == "uri")
      {
        tokenPolicy.SecurityPolicyURI = param.Value; //"http://opcfoundation.org/UA/SecurityPolicy#None";
      }
    }
    return tokenPolicy;
  }

  UserIdentifyTokenType GetTokenType(const std::string& typeName) const
  {
    if (typeName == "anonymous")
    {
      return UserIdentifyTokenType::ANONYMOUS;
    }
    if (typeName == "user_name")
    {
      return UserIdentifyTokenType::USERNAME;
    }
    if (typeName == "certificate")
    {
      return UserIdentifyTokenType::CERTIFICATE;
    }
    if (typeName == "issued_token")
    {
      return UserIdentifyTokenType::ISSUED_TOKEN;
    }
    throw std::logic_error("Unknown token type '" + typeName + "'");
  }

  ApplicationType GetApplicationType(const std::string& typeName) const
  {
    if (typeName == "server")
    {
      return ApplicationType::SERVER;
    }
    if (typeName == "client")
    {
      return ApplicationType::CLIENT;
    }
    if (typeName == "client_and_server")
    {
      return ApplicationType::CLIENT_AND_SERVER;
    }
    if (typeName == "discovery_server")
    {
      return ApplicationType::DISCOVERY_SERVER;
    }
    throw std::logic_error("Invalid name of type application type: " + typeName);
  }

  MessageSecurityMode GetSecurityMode(const std::string& modeName) const
  {
    if (modeName == "none")
    {
      return MessageSecurityMode::MSM_NONE;
    }
    if (modeName == "sign")
    {
      return MessageSecurityMode::MSM_SIGN;
    }
    if (modeName == "sign_encrypt")
    {
      return MessageSecurityMode::MSM_SIGN_AND_ENCRYPT;
    }
    throw std::logic_error("Unknown security mode name: " + modeName);
  }


  private:
    std::shared_ptr<OpcUa::Server::TcpServerAddon> TcpAddon;
    std::shared_ptr<OpcUa::Server::InternalComputerAddon> InternalComputer;
    std::vector<unsigned short> Ports;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new OpcUaProtocol());
}

