/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints.h"

#include <opc/ua/channel.h>
#include <opc/ua/client/computer.h>
#include <opc/ua/protocol/session.h>

namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class Computer : public OpcUa::Remote::Computer
    {
    public:
      explicit Computer(std::shared_ptr<IOChannel> channel)
        : Channel(channel)
        , Stream(channel)
      {
      }

      virtual void CreateSession(const Remote::SessionParameters& parameters)
      {
        CreateSessionRequest request;

        request.ClientDescription.URI = parameters.ClientDescription.URI;
        request.ClientDescription.ProductURI = parameters.ClientDescription.ProductURI;
        request.ClientDescription.Name = parameters.ClientDescription.Name;
        request.ClientDescription.Type = parameters.ClientDescription.Type;    
        request.ClientDescription.GatewayServerURI = parameters.ClientDescription.GatewayServerURI;
        request.ClientDescription.DiscoveryProfileURI = parameters.ClientDescription.DiscoveryProfileURI;
        request.ClientDescription.DiscoveryURLs = parameters.ClientDescription.DiscoveryURLs;

        request.ServerURI = parameters.ServerURI;
        request.EndpointURL = parameters.EndpointURL; // TODO make just endpoint.URL;
        request.SessionName = parameters.SessionName;
        request.ClientNonce = std::vector<uint8_t>(32,0);
        request.ClientCertificate = parameters.ClientCertificate;
        request.RequestedSessionTimeout = parameters.Timeout;
        request.MaxResponseMessageSize = 65536;

        Stream << request << OpcUa::Binary::flush;

        CreateSessionResponse response;
        Stream >> response;
        AuthenticationToken = response.AuthenticationToken;
      }

      virtual void ActivateSession()
      {
        ActivateSessionRequest activate;
        activate.Header.SessionAuthenticationToken = AuthenticationToken;
        activate.LocaleIDs.push_back("en");
        Stream << activate << OpcUa::Binary::flush;

        ActivateSessionResponse response;
        Stream >> response;
      }

      virtual void CloseSession()
      {
        CloseSessionRequest closeSession;
        closeSession.Header.SessionAuthenticationToken = AuthenticationToken;
        Stream << closeSession << OpcUa::Binary::flush;

        CloseSessionResponse closeResponse;
        Stream >> closeResponse;
      }

      virtual std::shared_ptr<Remote::EndpointServices> Endpoints() const
      {
        return std::shared_ptr<Remote::EndpointServices>(new Internal::EndpointServices<StreamType>(Channel, AuthenticationToken));
      }

      virtual std::shared_ptr<Remote::ViewServices> Views() const
      {
        return std::shared_ptr<Remote::ViewServices>();
      }

      virtual std::shared_ptr<Remote::AttributeServices> Attributes() const
      {
        return std::shared_ptr<Remote::AttributeServices>();
      }

    private:
      std::shared_ptr<IOChannel> Channel;
      StreamType Stream;
      NodeID AuthenticationToken;
    };

  } // namespace Internal
} // namespace OpcUa

