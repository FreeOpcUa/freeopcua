/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Manager of Property tree
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "group_test.h"

#include <opccore/managers/property_tree/group.h>
#include <cppunit/config/SourcePrefix.h>
#include <memory>
#include <sstream>

CPPUNIT_TEST_SUITE_REGISTRATION( GroupTestCase );

Gefest::Item::SharedPtr TestAddingItem(Gefest::Group& group, const std::string& name)
{
  Gefest::Item::SharedPtr item;
  
  CPPUNIT_ASSERT_NO_THROW(item = group.AddItem(name));
  CPPUNIT_ASSERT_THROW(group.AddSubGroup(name), Common::Error);
  
  CPPUNIT_ASSERT(item.get() != 0);
  CPPUNIT_ASSERT(item->GetName() == name);
  
  CPPUNIT_ASSERT_NO_THROW(group.ItemExists(name));
  CPPUNIT_ASSERT(group.ItemExists(item->GetID()));
  CPPUNIT_ASSERT_NO_THROW(group.ItemExists(item->GetID()));
  CPPUNIT_ASSERT(group.ItemExists(item->GetID()));
 
  CPPUNIT_ASSERT_NO_THROW(item == group.GetItem(name));
  CPPUNIT_ASSERT(item.get());
  CPPUNIT_ASSERT(item->GetName() == name);
  
  CPPUNIT_ASSERT_NO_THROW(item = group.GetItem(item->GetID()));
  CPPUNIT_ASSERT(item.get());
  CPPUNIT_ASSERT(item->GetName() == name);

  CPPUNIT_ASSERT_THROW(group.AddItem(name), Common::Error);
  return item;
}

void TestItemsCount(const Gefest::Group& group, const std::vector<std::string>& names, const std::vector<Common::ObjectID>& ids)
{
  std::vector<std::string> groupNames; 
  CPPUNIT_ASSERT_NO_THROW(groupNames = group.GetItemsNames());
  CPPUNIT_ASSERT(groupNames.size() == names.size());
  for (std::vector<std::string>::const_iterator it = groupNames.begin(); it != groupNames.end(); ++it)
  {
    CPPUNIT_ASSERT(std::find(names.begin(), names.end(), *it) != names.end());
  }

  std::vector<Common::ObjectID> groupIDs; 
  CPPUNIT_ASSERT_NO_THROW(groupIDs = group.GetItemsIDs());
  CPPUNIT_ASSERT(groupIDs.size() == ids.size());
  for (std::vector<Common::ObjectID>::const_iterator it = groupIDs.begin(); it != groupIDs.end(); ++it)
  {
    CPPUNIT_ASSERT(std::find(ids.begin(), ids.end(), *it) != ids.end());
  }

  typedef std::vector<std::shared_ptr<Gefest::Item> > ItemsContainer;
  const ItemsContainer& items = group.GetItems();
  CPPUNIT_ASSERT(items.size() == names.size());
  for (ItemsContainer::const_iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
  {
    std::shared_ptr<Gefest::Item> item = *itemIt;
    CPPUNIT_ASSERT(item);
    CPPUNIT_ASSERT(std::find(names.begin(), names.end(), item->GetName()) != names.end());
    CPPUNIT_ASSERT(std::find(ids.begin(), ids.end(), item->GetID()) != ids.end());
  }
}

void TestRemovingItems(Gefest::Group& group, const std::vector<std::string>& names, const std::vector<Common::ObjectID>& ids)
{
  CPPUNIT_ASSERT_MESSAGE("Illegal input parameters", names.size() == ids.size());
  for (std::size_t i = 0; i < names.size(); ++i)
  {
    const std::string& name = names[i];
    const Common::ObjectID& id = ids[i];
    CPPUNIT_ASSERT(group.ItemExists(name));
    CPPUNIT_ASSERT(group.ItemExists(id));

    CPPUNIT_ASSERT_THROW(group.RemoveSubGroup(name), Common::Error);
    CPPUNIT_ASSERT_NO_THROW(group.RemoveItem(name));
    CPPUNIT_ASSERT_THROW(group.RemoveSubGroup(name), Common::Error);

    CPPUNIT_ASSERT(group.ItemExists(name) == false);
    CPPUNIT_ASSERT(group.ItemExists(id) == false);
    
    CPPUNIT_ASSERT(group.GetItemsNames().size() == names.size() - i - 1);
    CPPUNIT_ASSERT(group.GetItemsIDs().size() == ids.size() - i - 1);
  }
}

Gefest::Group::SharedPtr TestAddingSubGroup(Gefest::Group& group, const std::string& name)
{
  Gefest::Group::SharedPtr subGroup;
  
  CPPUNIT_ASSERT_NO_THROW(subGroup = group.AddSubGroup(name));
  CPPUNIT_ASSERT_THROW(group.AddItem(name), Common::Error);
 
  CPPUNIT_ASSERT(subGroup.get() != 0);
  CPPUNIT_ASSERT(subGroup->GetName() == name);
  
  CPPUNIT_ASSERT_NO_THROW(group.SubGroupExists(name));
  CPPUNIT_ASSERT(group.SubGroupExists(subGroup->GetID()));
  CPPUNIT_ASSERT_NO_THROW(group.SubGroupExists(subGroup->GetID()));
  CPPUNIT_ASSERT(group.SubGroupExists(subGroup->GetID()));
  
  
  CPPUNIT_ASSERT_NO_THROW(subGroup = group.GetSubGroup(name));
  CPPUNIT_ASSERT(subGroup.get());
  CPPUNIT_ASSERT(subGroup->GetName() == name);
  
  CPPUNIT_ASSERT_NO_THROW(subGroup = group.GetSubGroup(subGroup->GetID()));
  CPPUNIT_ASSERT(subGroup.get());
  CPPUNIT_ASSERT(subGroup->GetName() == name);

  CPPUNIT_ASSERT_THROW(group.AddSubGroup(name), Common::Error);
  return subGroup;
}

void TestSubGroupsCount(const Gefest::Group& group, const std::vector<std::string>& names, const std::vector<Common::ObjectID>& ids)
{
  std::vector<std::string> groupNames;
  CPPUNIT_ASSERT_NO_THROW(groupNames = group.GetSubGroupsNames());
  CPPUNIT_ASSERT(groupNames.size() == names.size());
  for (std::vector<std::string>::const_iterator it = groupNames.begin(); it != groupNames.end(); ++it)
  {
    CPPUNIT_ASSERT(std::find(names.begin(), names.end(), *it) != names.end());
  }

  std::vector<Common::ObjectID> groupIDs; 
  CPPUNIT_ASSERT_NO_THROW(groupIDs = group.GetSubGroupsIDs());
  CPPUNIT_ASSERT(groupIDs.size() == ids.size());
  for (std::vector<Common::ObjectID>::const_iterator it = groupIDs.begin(); it != groupIDs.end(); ++it)
  {
    CPPUNIT_ASSERT(std::find(ids.begin(), ids.end(), *it) != ids.end());
  }

  typedef std::vector<std::shared_ptr<Gefest::Group> > GroupsContainer;
  const GroupsContainer& groups = group.GetSubGroups();
  CPPUNIT_ASSERT(groups.size() == groups.size());
  for (GroupsContainer::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
  {
    std::shared_ptr<Gefest::Group> group = *groupIt;
    CPPUNIT_ASSERT(group);
    CPPUNIT_ASSERT(std::find(names.begin(), names.end(), group->GetName()) != names.end());
    CPPUNIT_ASSERT(std::find(ids.begin(), ids.end(), group->GetID()) != ids.end());
  }
}

void TestRemovingSubGroups(Gefest::Group& group, const std::vector<std::string>& names, const std::vector<Common::ObjectID>& ids)
{
  CPPUNIT_ASSERT_MESSAGE("Illegal input parameters", names.size() == ids.size());
  for (std::size_t i = 0; i < names.size(); ++i)
  {
    const std::string& name = names[i];
    const Common::ObjectID& id = ids[i];
    CPPUNIT_ASSERT(group.SubGroupExists(name));
    CPPUNIT_ASSERT(group.SubGroupExists(id));

    CPPUNIT_ASSERT_THROW(group.RemoveItem(name), Common::Error);
    CPPUNIT_ASSERT_NO_THROW(group.RemoveSubGroup(name));
    CPPUNIT_ASSERT_THROW(group.RemoveItem(name), Common::Error);

    CPPUNIT_ASSERT(group.SubGroupExists(name) == false);
    CPPUNIT_ASSERT(group.SubGroupExists(id) == false);
    
    CPPUNIT_ASSERT(group.GetSubGroupsNames().size() == names.size() - i - 1);
    CPPUNIT_ASSERT(group.GetSubGroupsIDs().size() == ids.size() - i - 1);
  }
}

void GroupTestCase::TestGroup()
{
  const Gefest::Group::SharedPtr& group(Gefest::Group::Create());

  std::vector<std::string> names;
  std::vector<Common::ObjectID> ids;
  const std::string name1 = "name1";
  Gefest::Item::SharedPtr item1 = TestAddingItem(*group, name1);

  names.push_back(name1);
  ids.push_back(item1->GetID());
  TestItemsCount(*group, names, ids);

  const std::string name2("name2");
  Gefest::Item::SharedPtr item2 = TestAddingItem(*group, name2);
  names.push_back(name2);
  ids.push_back(item2->GetID());
  TestItemsCount(*group, names, ids);



  std::vector<std::string> subGroupsNames;
  std::vector<Common::ObjectID> subGroupsIDs;
  const std::string subGroupName1 = "subGroup1";
  Gefest::Group::SharedPtr subGroup1 = TestAddingSubGroup(*group, subGroupName1);  
  subGroupsNames.push_back(subGroupName1);
  subGroupsIDs.push_back(subGroup1->GetID());
  TestSubGroupsCount(*group, subGroupsNames, subGroupsIDs);

  const std::string subGroupName2("subGroup2");
  Gefest::Group::SharedPtr subGroup2 = TestAddingSubGroup(*group, subGroupName2);
  CPPUNIT_ASSERT_THROW(subGroup2->SetName(subGroupName1), Common::Error);
  subGroupsNames.push_back(subGroupName2);
  subGroupsIDs.push_back(subGroup2->GetID());
  TestSubGroupsCount(*group, subGroupsNames, subGroupsIDs);

  TestRemovingItems(*group, names, ids);
  TestRemovingSubGroups(*group, subGroupsNames, subGroupsIDs);
}

