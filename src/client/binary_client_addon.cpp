/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/client/addon.h>
#include <opc/ua/client/binary_client.h>
#include <opc/common/addons_core/addon_manager.h>

#include <opc/common/uri_facade.h>
#include <opc/ua/client/binary_client.h>
#include <opc/ua/client/remote_connection.h>



namespace
{
class BinaryClientAddon : public OpcUa::Client::Addon
{
public:
  virtual void Initialize(Common::AddonsManager & manager, const Common::AddonParameters & parameters)
  {
    /*
    for (auto param = parameters.Parameters.begin(); param != parameters.Parameters.begin(); ++param)
      {
        if (param->Name == "debug" && param->Value != "false" && param->Value != "0")
          {
            Logger = manager.GetLogger();
          }
      }
     */
    Logger = manager.GetLogger();
  }

  virtual void Stop()
  {
  }

public:
  virtual std::string GetProtocol() const
  {
    return "opc.tcp";
  }

  virtual OpcUa::Services::SharedPtr Connect(const std::string & url)
  {
    return OpcUa::CreateBinaryClient(url, Logger);
  }

private:
  Common::Logger::SharedPtr Logger;
};
}

extern "C" Common::Addon * CreateAddon()
{
  return new BinaryClientAddon();
}
