/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Test device manager realization
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "device_manager.h"

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/device_manager/errors.h>
#include <opccore/managers/device_manager/manager.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/property_tree.h>

#include <iostream>

namespace
{
  std::pair<Common::ObjectID, Gefest::Item::SharedPtr> AddItem(Gefest::Group& group, const char* itemName)
  {
    std::shared_ptr<Gefest::Item> item = group.AddItem(itemName);
    item->SetValue(Common::Value(itemName));
    return std::make_pair(item->GetID(), item);
  }
   
  class TestDeviceManager : public DeviceManager::Manager
  {
  public:
    TestDeviceManager(Gefest::Group::SharedPtr rootGroup)
      : RootGroup(rootGroup)
    {
      Gefest::Group::SharedPtr group1 = rootGroup->AddSubGroup("group1");
      Objects.insert(AddItem(*group1, "Item1"));//->AddItem("Item1")->GetID());
      Objects.insert(AddItem(*group1, "Item2"));
      Objects.insert(AddItem(*group1, "Item3"));
      Objects.insert(AddItem(*group1, "Item4"));
 
      Gefest::Group::SharedPtr group11 = group1->AddSubGroup("group11");
      Objects.insert(AddItem(*group11, "Item111"));
      Objects.insert(AddItem(*group11, "Item112"));
      Objects.insert(AddItem(*group11, "Item113"));
      Objects.insert(AddItem(*group11, "Item114"));
      
      Gefest::Group::SharedPtr group2 = rootGroup->AddSubGroup("group2");
      Objects.insert(AddItem(*group2, "Item21"));
      Objects.insert(AddItem(*group2, "Item22"));
      Objects.insert(AddItem(*group2, "Item23"));
      Objects.insert(AddItem(*group2, "Item24"));

      Gefest::Group::SharedPtr group21 = group2->AddSubGroup("group21");
      Objects.insert(AddItem(*group21, "Item221"));
      Objects.insert(AddItem(*group21, "Item222"));
      Objects.insert(AddItem(*group21, "Item223"));
      Objects.insert(AddItem(*group21, "Item224"));
    }

  public: // Device Manager interface
    virtual std::set<Common::ObjectID> GetObjectIDs() const
    {
      std::vector<Common::ObjectID> ids(Objects.size());
      std::transform(Objects.begin(), Objects.end(), ids.begin(), [](const std::pair<Common::ObjectID, std::shared_ptr<Gefest::Item>>& object) {return object.first;});
      return std::set<Common::ObjectID>(ids.begin(), ids.end());
    }

    virtual void Read(DeviceManager::ObjectDataMap& data)
    {
      for(auto& objectData : data)
      {
        if (std::shared_ptr<Gefest::Item> item = FindItem(objectData))
        {
          objectData.second.Value = item->GetValue();
        }
      }
    }

    virtual void Write(DeviceManager::ObjectDataMap& data)
    {
      for(auto& objectData : data)
      {
        if (std::shared_ptr<Gefest::Item> item = FindItem(objectData))
        {
          item->SetValue(objectData.second.Value);
        }
      }
   }

  private:
     std::shared_ptr<Gefest::Item> FindItem(DeviceManager::ObjectDataMap::value_type& objectData)
     {
       const Common::ObjectID objectID = objectData.first;
       const auto& objectIt = Objects.find(objectID);
       if (objectIt == Objects.end())
       {
         objectData.second.Error = CREATE_ERROR1(DeviceManagerNotProcessItemWithID, objectID);
         return Gefest::Item::SharedPtr();
       }
       return objectIt->second;
     }

  private:
    const Gefest::Group::SharedPtr RootGroup;
    std::map<Common::ObjectID, Gefest::Item::SharedPtr> Objects;
  };
} // unnamed namespace

DeviceManager::Manager::UniquePtr TestServer::CreateDeviceManager(Gefest::Group::SharedPtr rootGroup)
{
  return DeviceManager::Manager::UniquePtr(new TestDeviceManager(rootGroup));
}

