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
#include <opc/ua/client/remote_server.h>
#include <opc/common/addons_core/addon_manager.h>


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
      return OpcUa::Remote::Connect(url);
    }

  private:
    bool Debug = false;
  };
}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new BinaryServerAddon());
}
