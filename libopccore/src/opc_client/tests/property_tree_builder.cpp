/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Builder of property tree used in tests
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "property_tree_builder.h"
#include <opccore/opc_client/server.h>
#include <opccore/common/addons_core/addon_manager.h>
#include "../../managers/common_managers/common_managers.h"
#include <opccore/managers/device_manager/manager.h>
#include <opccore/managers/io_manager/manager.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/group.h>
#include <opccore/managers/property_tree/property_tree.h>

void BuildSubTree(Gefest::Group& group)
{
  group.AddSubGroup("Group1");
  group.AddSubGroup("Group2");
  group.AddSubGroup("Group3");
  group.AddSubGroup("Group4");
  boost::posix_time::ptime current = boost::posix_time::microsec_clock::local_time();
  group.AddItem("Item1")->SetValue(Common::Value(1, current));
  group.AddItem("Item2")->SetValue(Common::Value(2, current));
  current -= boost::posix_time::hours(5);
  group.AddItem("Item3")->SetValue(Common::Value(3, current));
}

void BuildPropertyTree(Gefest::PropertyTree& tree)
{
  std::shared_ptr<Gefest::Group> rootGroup = tree.GetRootGroup();
  BuildSubTree(*rootGroup);
  BuildSubTree(*rootGroup->GetSubGroup("Group1"));
}


class DeviceManagerAddon 
  : public DeviceManager::Manager
  , public Common::Addon
{
  public:  // Common::Addon
    virtual void Initialize()
    {
      std::shared_ptr<InputOutputManager::RequestManager> ioManager = Common::GetAddon<InputOutputManager::RequestManager>(Common::ADDON_ID_IO_MANAGER);
      std::shared_ptr<DeviceManager::Manager> deviceManager = Common::GetAddon<DeviceManager::Manager>(Common::ADDON_ID_DEVICE_IO_MANAGER);
      ioManager->RegisterDeviceManager(deviceManager);
    }
    virtual void Stop()
    {
    }
  
  public:
    virtual std::set<Common::ObjectID> GetObjectIDs() const
    {
      std::shared_ptr<PropertyTree::Manager> propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE);
      std::shared_ptr<Gefest::PropertyTree> propertyTree = propertyTreeManager->GetPropertyTree();
      BuildPropertyTree(*propertyTree);
      std::set<Common::ObjectID> ids;
      ids.insert(propertyTree->FindItem("/Item1")->GetID());
      ids.insert(propertyTree->FindItem("/Item2")->GetID());
      ids.insert(propertyTree->FindItem("/Item3")->GetID());
      ids.insert(propertyTree->FindItem("/Group1/Item1")->GetID());
      ids.insert(propertyTree->FindItem("/Group1/Item2")->GetID());
      ids.insert(propertyTree->FindItem("/Group1/Item3")->GetID());
      return ids;
    }

    virtual void Read(DeviceManager::ObjectDataMap& data)
    {
      for (DeviceManager::ObjectDataMap::iterator it = data.begin(); it != data.end(); ++it)
      {
        it->second.Value = Common::Value(-1);
        it->second.Error = Common::Error();
      }
    }

    virtual void Write(DeviceManager::ObjectDataMap& data)
    {
    }

 private: 
 };


  class DeviceManagerFactory : public Common::AddonFactory
  {
  public:
    virtual std::unique_ptr<Common::Addon> CreateAddon()
    {
      return std::unique_ptr<Common::Addon>(new DeviceManagerAddon());
    }
  };


extern "C" void RegisterServerManagers()
{
  const std::shared_ptr<Common::AddonsManager> manager = Common::GetAddonsManager();
  std::vector<Common::AddonID> dependencies;
  dependencies.push_back(Common::ADDON_ID_IO_MANAGER);
  dependencies.push_back(Common::ADDON_ID_PROPERTY_TREE);
  manager->Register(
    Common::ADDON_ID_DEVICE_IO_MANAGER,
    std::unique_ptr<Common::AddonFactory>(new DeviceManagerFactory()),
    dependencies);
}


void PrepareTests(bool needRegisterCommonAddons)
{
  if (needRegisterCommonAddons)
  {
    Common::RegisterCommonManagers();
    RegisterServerManagers();
    Common::GetAddonsManager()->Start();
  }
}

void StopTests()
{
  Common::GetAddonsManager()->Stop();
}

