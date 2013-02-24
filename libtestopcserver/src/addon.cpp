/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Gps device manager realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "addon.h"

#include "device_manager.h"

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/device_manager/manager.h>
#include <opccore/managers/io_manager/manager.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/property_tree.h>

#include <iostream>
#include <set>

namespace
{
  class TestDeviceManagerAddon
    : public Common::Addon
    , public DeviceManager::Manager
  {
  public:
    TestDeviceManagerAddon()
    {
      std::clog << "TestDeviceManagerAddon Created" << std::endl;
    }

    virtual ~TestDeviceManagerAddon()
    {
      std::clog << "TestDeviceManagerAddon Destroyed" << std::endl;
    }

  public: // DeviceManager::Manager interface
    virtual std::set<Common::ObjectID> GetObjectIDs() const
    {
      return Impl->GetObjectIDs();
    }

    virtual void Read(DeviceManager::ObjectDataMap& data)
    {
      Impl->Read(data);
    }

    virtual void Write(DeviceManager::ObjectDataMap& data)
    {
      Impl->Write(data);
    }

  public: // Addon interface
    virtual void Initialize()
    {
      std::clog << "TestDeviceManagerAddon initializing..." << std::endl;
      PropertyTree::Manager::SharedPtr propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE);
      Gefest::Group::SharedPtr rootGroup = propertyTreeManager->GetPropertyTree()->GetRootGroup();
      Impl = TestServer::CreateDeviceManager(rootGroup);
      RequestManager = Common::GetAddon<InputOutputManager::RequestManager>(Common::ADDON_ID_IO_MANAGER);
      RequestManager->RegisterDeviceManager(Impl);
      std::clog << "TestDeviceManagerAddon initialized." << std::endl;
    }

    virtual void Stop()
    {
      std::clog << "TestDeviceManagerAddon stopping...." << std::endl;
      if (RequestManager)
      {
        RequestManager->UnregisterDeviceManager(Impl);
      }
      Impl.reset();
      std::clog << "TestDeviceManagerAddon stopped. " << std::endl;
    }

  private:
    DeviceManager::Manager::SharedPtr Impl;
    InputOutputManager::RequestManager::SharedPtr RequestManager;
  };

  class TestDeviceManagerFactory : public Common::AddonFactory
  {
  public:
    virtual Common::Addon::UniquePtr CreateAddon()
    {
      return Common::Addon::UniquePtr(new TestDeviceManagerAddon());
    }
  };

} // unnamed namespace

namespace TestServer
{
  void RegisterDeviceManager()
  {
    const Common::AddonsManager::SharedPtr manager = Common::GetAddonsManager();
    // Device manager uses property tree manager - it should be initilized before us.
    std::vector<Common::AddonID> dependencies;
    dependencies.push_back(Common::ADDON_ID_PROPERTY_TREE);
    dependencies.push_back(Common::ADDON_ID_IO_MANAGER);
    Common::AddonFactory::UniquePtr addonFactory(new TestDeviceManagerFactory());

    manager->Register(Common::ADDON_ID_DEVICE_IO_MANAGER, std::move(addonFactory), dependencies);
  }
} // namespace TestServer

