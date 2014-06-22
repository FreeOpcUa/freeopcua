/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Server implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoints.h"
#include "stream_attribute.h"
#include "stream_node_management.h"
#include "stream_subscription.h"
#include "stream_view.h"

#include <opc/ua/protocol/channel.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/server.h>

#include <atomic>

namespace OpcUa
{
  namespace Internal
  {

    template <class StreamType>
    class Server : public OpcUa::Remote::Server
    {
    public:
      explicit Server(std::shared_ptr<IOChannel> channel)
        : Channel(channel)
        , Stream(channel)
        , RequestHandle(0)
      {
      }

      virtual void CreateSession(const Remote::SessionParameters& parameters)
      {
        CreateSessionRequest request;
        request.Header.RequestHandle = GetRequestHandle();
        request.Header.Timeout = 10000;

        request.Parameters.ClientDescription.URI = parameters.ClientDescription.URI;
        request.Parameters.ClientDescription.ProductURI = parameters.ClientDescription.ProductURI;
        request.Parameters.ClientDescription.Name = parameters.ClientDescription.Name;
        request.Parameters.ClientDescription.Type = parameters.ClientDescription.Type; 
        request.Parameters.ClientDescription.GatewayServerURI = parameters.ClientDescription.GatewayServerURI;
        request.Parameters.ClientDescription.DiscoveryProfileURI = parameters.ClientDescription.DiscoveryProfileURI;
        request.Parameters.ClientDescription.DiscoveryURLs = parameters.ClientDescription.DiscoveryURLs;

        request.Parameters.ServerURI = parameters.ServerURI;
        request.Parameters.EndpointURL = parameters.EndpointURL; // TODO make just endpoint.URL;
        request.Parameters.SessionName = parameters.SessionName;
        request.Parameters.ClientNonce = std::vector<uint8_t>(32,0);
        request.Parameters.ClientCertificate = parameters.ClientCertificate;
        request.Parameters.RequestedSessionTimeout = parameters.Timeout;
        request.Parameters.MaxResponseMessageSize = 65536;

        Stream << request << OpcUa::Binary::flush;

        CreateSessionResponse response;
        Stream >> response;
        AuthenticationToken = response.Session.AuthenticationToken;
      }

      virtual void ActivateSession()
      {
        ActivateSessionRequest activate;
        activate.Header.SessionAuthenticationToken = AuthenticationToken;
        activate.Header.RequestHandle = GetRequestHandle();
        activate.Header.Timeout = 10000;
        activate.Parameters.LocaleIDs.push_back("en");
        Stream << activate << OpcUa::Binary::flush;

        ActivateSessionResponse response;
        Stream >> response;
      }

      virtual void CloseSession()
      {
        CloseSessionRequest closeSession;
        closeSession.Header.SessionAuthenticationToken = AuthenticationToken;
        closeSession.Header.RequestHandle = GetRequestHandle();
        closeSession.Header.Timeout = 10000;
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
        return std::shared_ptr<Remote::ViewServices>(new Internal::ViewServices<StreamType>(Channel, AuthenticationToken));
      }

      virtual std::shared_ptr<Remote::NodeManagementServices> NodeManagement() const
      {
        return std::shared_ptr<Remote::NodeManagementServices>(new Internal::NodeManagementServices<StreamType>(Channel, AuthenticationToken));
      }

      virtual std::shared_ptr<Remote::AttributeServices> Attributes() const
      {
        return std::shared_ptr<Remote::AttributeServices>(new Internal::AttributeServices<StreamType>(Channel, AuthenticationToken));
      }

      virtual std::shared_ptr<Remote::SubscriptionServices> Subscriptions() const
      {
        return std::shared_ptr<Remote::SubscriptionServices>(new Internal::SubscriptionServices<StreamType>(Channel, AuthenticationToken));
      }

   private:
      unsigned GetRequestHandle()
      {
        return ++RequestHandle;
      }

    private:
      std::shared_ptr<IOChannel> Channel;
      StreamType Stream;
      NodeID AuthenticationToken;
      std::atomic<uint32_t> RequestHandle;
    };

    typedef Server<OpcUa::Binary::IOStream<IOChannel>> BinaryServer;

  } // namespace Internal
} // namespace OpcUa

