/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Server implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "stream_server.h"

#include <opc/common/uri_facade.h>
#include <opc/ua/protocol/binary/secure_connection.h>
#include <opc/ua/client/binary_server.h>
#include <opc/ua/client/remote_server.h>
#include <opc/ua/client/remote_connection.h>
#include <stdexcept>


namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class UaServer : public OpcUa::Remote::Server
  {
  public:
    UaServer(const std::string& uri)
      : ServerUri(uri)
    {
      std::shared_ptr<IOChannel>  channel = CreateSecureChannel(uri);
      Impl = CreateServerByProtocol(ServerUri.Scheme(), channel);
    }

    virtual void CreateSession(const Remote::SessionParameters& parameters)
    {
      Impl->CreateSession(parameters);
    }

    virtual void ActivateSession()
    {
      Impl->ActivateSession();
    }

    virtual void CloseSession()
    {
      Impl->CloseSession();
    }

    virtual std::shared_ptr<EndpointServices> Endpoints() const
    {
      return Impl->Endpoints();
    }

    virtual std::shared_ptr<ViewServices> Views() const
    {
      return Impl->Views();
    }

    virtual std::shared_ptr<NodeManagementServices> NodeManagement() const
    {
      return Impl->NodeManagement();
    }


    virtual std::shared_ptr<AttributeServices> Attributes() const
    {
      return Impl->Attributes();
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions() const
    {
      return Impl->Subscriptions();
    }

  private:
    std::unique_ptr<OpcUa::Remote::Server> CreateServerByProtocol(const std::string& protocol, std::shared_ptr<IOChannel> channel) const
    {
      if (protocol == "opc.tcp")
      {
        return CreateBinaryServer(channel);
      }
      throw std::invalid_argument("Unknown protocol: " + ServerUri.Scheme());
    }

    std::shared_ptr<OpcUa::IOChannel> CreateSecureChannel(const std::string& endpointUrl) const
    {
      std::shared_ptr<IOChannel> connection(OpcUa::Connect(ServerUri.Host(), ServerUri.Port()));
      Binary::SecureConnectionParams params;
      params.EndpointUrl = endpointUrl;
      params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
      return OpcUa::Binary::CreateSecureChannel(connection, params);
    }

  private:
    const Common::Uri ServerUri;
    std::unique_ptr<OpcUa::Remote::Server> Impl;
  };


}

OpcUa::Remote::Server::UniquePtr OpcUa::Remote::Connect(const std::string& url)
{
  return std::unique_ptr<OpcUa::Remote::Server>(new UaServer(url));
}

