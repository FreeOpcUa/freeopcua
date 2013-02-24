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

#include "gps_device_manager.h"
#include "gps_tree_updater.h"
#include "gpsd.h"
#include "gpsd_data_processor.h"

#include <common/addons_core/addon.h>
#include <common/addons_core/addon_ids.h>
#include <common/addons_core/addon_manager.h>
#include <managers/device_manager/manager.h>
#include <managers/io_manager/manager.h>
#include <managers/property_tree/manager/manager.h>
#include <managers/property_tree/property_tree.h>

#include <iostream>
#include <set>

namespace
{
  class GpsDeviceManager 
    : public Common::Addon
    , public DeviceManager::Manager
  {
  public:
    GpsDeviceManager()
    {
      std::cout << "GpsDeviceManager Created" << std::endl;
    }

    virtual ~GpsDeviceManager()
    {
      std::cout << "GpsDeviceManager Destroying..." << std::endl;
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
      std::cout << "GpsDeviceManager initializing..." << std::endl;
      std::cout << "creating Gps::Gpsd" << std::endl;
      PropertyTree::Manager::SharedPtr propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE);
      Gefest::Group::SharedPtr rootGroup = propertyTreeManager->GetPropertyTree()->GetRootGroup();
      Impl = Gps::CreateDeviceManager(rootGroup);
      RequestManager = Common::GetAddon<InputOutputManager::RequestManager>(Common::ADDON_ID_IO_MANAGER);
      RequestManager->RegisterDeviceManager(Impl);
      std::cout << "GpsDeviceManager initialized." << std::endl;
    }

    virtual void Stop()
    {
      std::cout << "GpsDeviceManager stopping...." << std::endl;
      if (RequestManager)
      {
        RequestManager->UnregisterDeviceManager(Impl);
      }
      Impl.reset();
      std::cout << "GpsDeviceManager stopped. " << std::endl;
    }

  private:
    DeviceManager::Manager::SharedPtr Impl;
    InputOutputManager::RequestManager::SharedPtr RequestManager;
  };

  class GpsDeviceManagerFactory : public Common::AddonFactory
  {
  public:
    virtual Common::Addon::UniquePtr CreateAddon()
    {
      return Common::Addon::UniquePtr(new GpsDeviceManager());
    }
  };

} // unnamed namespace

namespace Gps
{
  void RegisterDeviceManager()
  {
    Common::AddonsManager::SharedPtr manager = Common::GetAddonsManager();
    // Device manager uses property tree manager - it should be initilized before us.
    std::vector<Common::AddonID> dependencies;
    dependencies.push_back(Common::ADDON_ID_PROPERTY_TREE);
    dependencies.push_back(Common::ADDON_ID_IO_MANAGER);
    Common::AddonFactory::UniquePtr addonFactory(new GpsDeviceManagerFactory());

    manager->Register(Common::ADDON_ID_DEVICE_IO_MANAGER, std::move(addonFactory), dependencies);
  }
} // namespace Gps

