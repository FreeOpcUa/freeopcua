/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "stream_computer.h"

#include <opc/common/uri_facade.h>
#include <opc/ua/protocol/binary/secure_connection.h>
#include <opc/ua/client/binary_computer.h>
#include <opc/ua/client/remote_computer.h>
#include <opc/ua/client/remote_connection.h>
#include <stdexcept>


namespace
{
  using namespace OpcUa;
  using namespace OpcUa::Remote;

  class UaComputer : public OpcUa::Remote::Computer
  {
  public:
    UaComputer(const std::string& uri)
      : ServerUri(uri)
    {
      std::shared_ptr<IOChannel>  channel = CreateSecureChannel(uri);
      Impl = CreateComputerByProtocol(ServerUri.Scheme(), channel);
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

    virtual std::shared_ptr<AttributeServices> Attributes() const
    {
      return Impl->Attributes();
    }

    virtual std::shared_ptr<SubscriptionServices> Subscriptions() const
    {
      return Impl->Subscriptions();
    }

  private:
    std::unique_ptr<OpcUa::Remote::Computer> CreateComputerByProtocol(const std::string& protocol, std::shared_ptr<IOChannel> channel) const
    {
      if (protocol == "opc.tcp")
      {
        return CreateBinaryComputer(channel);
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
    const Internal::Uri ServerUri;
    std::unique_ptr<OpcUa::Remote::Computer> Impl;
  };


}

std::unique_ptr<OpcUa::Remote::Computer> OpcUa::Remote::Connect(const std::string& url)
{
  return std::unique_ptr<OpcUa::Remote::Computer>(new UaComputer(url));
}

