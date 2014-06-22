/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/common/application.h>

#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <opc/common/errors.h>

#include <stdexcept>

namespace
{

  using namespace OpcUa;

  class OpcUaServer : public OpcUa::Application
  {
  public:
    void Start(const std::vector<Common::AddonInformation>& addonInformation)
    {
      if (Addons.get())
      {
        THROW_ERROR(ApplicationAlreayStarted);
      }

      Addons = Common::CreateAddonsManager();
      for (const Common::AddonInformation& config : addonInformation)
      {
        Addons->Register(config);
      }
      Addons->Start();
    }

    virtual Common::AddonsManager& GetAddonsManager()
    {
      if (!Addons.get())
      {
        THROW_ERROR(ApplicationNotStarted);
      }
      return *Addons;
    }

    virtual void Stop()
    {
      Addons->Stop();
      Addons.reset();
    }

  private:
    Common::AddonsManager::UniquePtr Addons;
  };

}

OpcUa::Application::UniquePtr OpcUa::CreateApplication()
{
  return OpcUa::Application::UniquePtr(new OpcUaServer());
}
