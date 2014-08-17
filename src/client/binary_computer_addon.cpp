/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#include <opc/ua/client/addon.h>
#include <opc/ua/client/binary_server.h>
#include <opc/common/addons_core/addon_manager.h>

#include <opc/common/uri_facade.h>
#include <opc/ua/client/binary_server.h>
#include <opc/ua/client/remote_connection.h>



namespace
{
  class BinaryServerAddon : public OpcUa::Client::Addon
  {
  public:
    virtual void Initialize(Common::AddonsManager& manager, const Common::AddonParameters& parameters)
    {

    }

    virtual void Stop()
    {
    }

  public:
    virtual std::string GetProtocol() const
    {
      return "opc.tcp";
    }

    virtual OpcUa::Remote::Server::SharedPtr Connect(const std::string& url)
    {
      const Common::Uri serverUri(url);
      OpcUa::IOChannel::SharedPtr channel = OpcUa::Connect(serverUri.Host(), serverUri.Port());

      OpcUa::Remote::SecureConnectionParams params;
      params.EndpointUrl = url;
      params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";
      return CreateBinaryServer(channel, params);
    }

  private:
    bool Debug = false;
  };
}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new BinaryServerAddon());
}
