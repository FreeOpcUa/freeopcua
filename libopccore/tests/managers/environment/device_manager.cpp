#include "device_manager.h"

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/device_manager/id.h>
#include <opccore/managers/device_manager/manager.h>
#include <opccore/managers/io_manager/id.h>
#include <opccore/managers/io_manager/manager.h>
#include <opccore/managers/property_tree/group.h>
#include <opccore/managers/property_tree/id.h>
#include <opccore/managers/property_tree/item.h>
#include <opccore/managers/property_tree/id.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/property_tree.h>

#include <iostream>
#include <tr1/functional>

namespace
{
  class DeviceManagerImpl 
    : public TestDeviceManager
  {
  public:
    explicit DeviceManagerImpl(const std::string& prefix, Gefest::Group::SharedPtr group)
      : Prefix(prefix)
    {
      std::shared_ptr<Gefest::Item> item;
      std::vector<Common::ObjectID> ids;
      item = group->AddItem(Prefix + "item1");
      Items.push_back(item);
      ItemIDs.push_back(item->GetID());

      item = group->AddItem(Prefix + "item2");
      Items.push_back(item);
      ItemIDs.push_back(item->GetID());
    }

  public: // DeviceManagerInterface
    virtual void Read(DeviceManager::ObjectDataMap& data)
    {
      if (Callback)
      {
        std::for_each(data.begin(), data.end(), 
          std::tr1::bind(&DeviceManagerImpl::OnDataRead, this, std::tr1::placeholders::_1));
      }
    }

    virtual void Write(DeviceManager::ObjectDataMap& data)
    {
      if (Callback)
      {
        std::for_each(data.begin(), data.end(), 
          std::tr1::bind(&DeviceManagerImpl::OnDataWrite, this, std::tr1::placeholders::_1));
      }
    }

    virtual void SetCallback(std::shared_ptr<IoCallback> callback)
    {
      Callback = callback;
    }

    virtual std::set<Common::ObjectID> GetObjectIDs() const
    {
      return std::set<Common::ObjectID>(ItemIDs.begin(), ItemIDs.end());
    }

  private:
    void OnDataRead(DeviceManager::ObjectDataMap::value_type& value)
    {
      Callback->OnReadData(static_cast<Common::ObjectID>(value.first), value.second);
    }

    void OnDataWrite(const DeviceManager::ObjectDataMap::value_type& value)
    {
      Callback->OnWriteData(static_cast<Common::ObjectID>(value.first), value.second);
    }

  private:
    const std::string Prefix;
    std::vector<std::shared_ptr<Gefest::Item> > Items;
    std::vector<Common::ObjectID> ItemIDs;
    std::shared_ptr<IoCallback> Callback;
  };

  class DeviceManagerAddon : public Common::Addon
  {
  public: // Addon interface
    DeviceManagerAddon(const std::string& prefix)
      : Prefix(prefix)
    {
    }
    virtual void Initialize()
    {
      std::shared_ptr<PropertyTree::Manager> propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(PropertyTree::ManagerID);
      std::shared_ptr<Gefest::PropertyTree> tree = propertyTreeManager->GetPropertyTree();
      std::shared_ptr<Gefest::Group> rootGroup = tree->GetRootGroup();
     
      std::shared_ptr<InputOutputManager::RequestManager> ioManager = Common::GetAddon<InputOutputManager::RequestManager>(InputOutputManager::RequestManagerID);
      ioManager->RegisterDeviceManager(DeviceManager::Manager::SharedPtr(new DeviceManagerImpl(Prefix, rootGroup)));
    }

    virtual void Stop()
    {
    }
  private:
    const std::string Prefix;
 }; 

  class DeviceManagerFactory : public Common::AddonFactory
  {
  public:
    explicit DeviceManagerFactory(const std::string& prefix)
      : Prefix(prefix)
    {
    }

    virtual std::unique_ptr<Common::Addon> CreateAddon()
    {
      return std::unique_ptr<Common::Addon>(new DeviceManagerAddon(Prefix));
    }

  private:
    const std::string Prefix;
  };

} // namespace

void RegisterDeviceManager(Common::AddonsManager& manager, Common::AddonID addonID, const std::string& propertyTreePrefix)
{
  std::vector<Common::AddonID> dependencies;
  dependencies.push_back(PropertyTree::ManagerID);
  dependencies.push_back(DeviceManager::ManagerID);
  manager.Register(addonID, std::unique_ptr<Common::AddonFactory>(new DeviceManagerFactory(propertyTreePrefix)), dependencies);
}
