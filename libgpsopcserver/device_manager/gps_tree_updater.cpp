/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Class builder of gps property tree and supplier data for it.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $



#include "gps_tree_updater.h"
#include "errors.h"
#include <managers/property_tree/item.h>
#include <managers/property_tree/group.h>

#include <functional>
#include <map>

namespace
{
  const char LATITUDE_ITEM_NAME[] = "latitude";
  const char LONGITUDE_ITEM_NAME[] = "longitude";
  const char ALTITUDE_ITEM_NAME[] = "altitude";
  const char MODE_ITEM_NAME[] = "gps_mode";
  const char IS_ONLINE_ITEM_NAME[] = "is_online";

  enum ItemID
  {
    ITEM_ID_LATITUDE,
    ITEM_ID_LONGITUDE,
    ITEM_ID_ALTITUDE,
    ITEM_ID_MODE,
    ITEM_ID_IS_ONLINE,
  };

  typedef std::map<ItemID, Gefest::Item::SharedPtr> ItemsMap;
}

struct Gps::PropertyTreeUpdater::PropertiesHolder
{
  ItemsMap Items;
};

Gps::PropertyTreeUpdater::PropertyTreeUpdater(Gefest::Group::SharedPtr rootGroup)
  : RootGroup(rootGroup)
  , Properties(new PropertiesHolder)
{
  assert(RootGroup);
  BuildSubTree();
}

Gps::PropertyTreeUpdater::~PropertyTreeUpdater()
{
  DestroySubTree();
}

void Gps::PropertyTreeUpdater::BuildSubTree()
{
  Properties->Items[ITEM_ID_LATITUDE] = RootGroup->AddItem(LATITUDE_ITEM_NAME);
  Properties->Items[ITEM_ID_LONGITUDE] = RootGroup->AddItem(LONGITUDE_ITEM_NAME);
  Properties->Items[ITEM_ID_ALTITUDE] = RootGroup->AddItem(ALTITUDE_ITEM_NAME);
  Properties->Items[ITEM_ID_MODE] = RootGroup->AddItem(MODE_ITEM_NAME);
  Properties->Items[ITEM_ID_IS_ONLINE] = RootGroup->AddItem(IS_ONLINE_ITEM_NAME);
}

void Gps::PropertyTreeUpdater::DestroySubTree() throw()
{
  Properties->Items.clear();
  RootGroup->RemoveItem(LATITUDE_ITEM_NAME);
  RootGroup->RemoveItem(LONGITUDE_ITEM_NAME);
  RootGroup->RemoveItem(ALTITUDE_ITEM_NAME);
  RootGroup->RemoveItem(MODE_ITEM_NAME);
}

void Gps::PropertyTreeUpdater::SetLatitude(const Common::Value& value)
{
  Properties->Items[ITEM_ID_LATITUDE]->SetValue(value);
}

void Gps::PropertyTreeUpdater::SetLongitude(const Common::Value& value)
{
  Properties->Items[ITEM_ID_LONGITUDE]->SetValue(value);
}

void Gps::PropertyTreeUpdater::SetAltitude(const Common::Value& value)
{
  Properties->Items[ITEM_ID_ALTITUDE]->SetValue(value);
}

void Gps::PropertyTreeUpdater::SetGpsMode(const Common::Value& value)
{
  Properties->Items[ITEM_ID_MODE]->SetValue(value);
}

void Gps::PropertyTreeUpdater::SetIsOnline(const Common::Value& value)
{
  Properties->Items[ITEM_ID_IS_ONLINE]->SetValue(value);
}


std::set<Common::ObjectID> Gps::PropertyTreeUpdater::GetObjectIDs() const
{
  std::vector<Common::ObjectID> ids(Properties->Items.size());
  std::transform(
    Properties->Items.begin(),
    Properties->Items.end(),
    ids.begin(),
    std::bind(&Gefest::Item::GetID, std::bind(&ItemsMap::value_type::second, std::placeholders::_1)));
  return std::set<Common::ObjectID>(ids.begin(), ids.end());
}
// TODO refactor this unreadable hell.
void Gps::PropertyTreeUpdater::GetData(DeviceManager::ObjectDataMap& data) const
{
  for (DeviceManager::ObjectDataMap::iterator dataIt = data.begin(); dataIt != data.end(); ++dataIt)
  {
    ItemsMap::const_iterator itemIt;
    for (itemIt = Properties->Items.begin(); itemIt != Properties->Items.end(); ++itemIt)
    {
       if (itemIt->second->GetID() == dataIt->first)
       {
        break;
       }
    }
    if (itemIt == Properties->Items.end())
    {
      dataIt->second.Error = CREATE_ERROR1(GpsInvalidItemID, dataIt->first);
      dataIt->second.Value = Common::Value();
      continue;
    }
    dataIt->second.Value = itemIt->second->GetValue();
    dataIt->second.Error = Common::Error();
  }
}

