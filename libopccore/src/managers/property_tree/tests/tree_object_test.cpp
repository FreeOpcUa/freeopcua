#include "tree_object_test.h"
#include <cppunit/config/SourcePrefix.h>
#include <memory>
#include <opccore/managers/property_tree/property_tree.h>


CPPUNIT_TEST_SUITE_REGISTRATION( TreeObjectTestCase );

//
// building tree
//
// --/root
//     |
//     +--Group_1
//     |
//     +--Group_2
//          |
//          +--Item_2_1
//          |    +--Property_2_1_1
//          |    +--Property_2_1_2
//          +--Item_2_2
//          |    +--Property_2_2_1
//          |    +--Property_2_2_2
//          +--Group_2_1
//

void BuildTree(Gefest::PropertyTree::SharedPtr tree)
{
  Gefest::Group::SharedPtr root = tree->GetRootGroup();
  root->AddSubGroup("Group_1");
  const Gefest::Group::SharedPtr& group_2 = root->AddSubGroup("Group_2");

  Gefest::Item::SharedPtr item = group_2->AddItem("Item_2_1");
  item->AddProperty(Gefest::Property("Property_2_1_1"));
  item->AddProperty(Gefest::Property("Property_2_1_2"));
  
  item = group_2->AddItem("Item_2_2");
  item->AddProperty(Gefest::Property("Property_2_2_1"));
  item->AddProperty(Gefest::Property("Property_2_2_2"));
  group_2->AddSubGroup("Group_2_1");
}

void TestGroupForSuccess(const Gefest::PropertyTree& tree, const std::string& groupName, const std::string& groupPath)
{ 
  Gefest::Group::SharedPtr group;
  CPPUNIT_ASSERT_NO_THROW(group = tree.FindGroup(groupPath));
  CPPUNIT_ASSERT(group);
  CPPUNIT_ASSERT(group->GetName() == groupName);
  CPPUNIT_ASSERT(group->GetObjectPath() == groupPath);
}

void TestGroupForFail(const Gefest::PropertyTree& tree, const std::string& groupPath)
{ 
  Gefest::Group::SharedPtr group;
  CPPUNIT_ASSERT_NO_THROW(group = tree.FindGroup(groupPath));
  CPPUNIT_ASSERT(!group);
}

void TestItemForSuccess(const Gefest::PropertyTree& tree, const std::string& itemName, const std::string& itemPath)
{ 
  Gefest::Item::SharedPtr item;
  CPPUNIT_ASSERT_NO_THROW(item = tree.FindItem(itemPath));
  CPPUNIT_ASSERT(item);
  CPPUNIT_ASSERT(item->GetName() == itemName);
  CPPUNIT_ASSERT(item->GetObjectPath() == itemPath);
}

void TestItemForFail(const Gefest::PropertyTree& tree, const std::string& itemPath)
{ 
  Gefest::Item::SharedPtr item;
  CPPUNIT_ASSERT_NO_THROW(item  = tree.FindItem(itemPath));
  CPPUNIT_ASSERT(!item);
}

void TestPropertyForSuccess(const Gefest::PropertyTree& tree, const std::string& propertyName, const std::string& propertyPath)
{ 
  Gefest::Property property;
  CPPUNIT_ASSERT_NO_THROW(property = tree.FindProperty(propertyPath));
  CPPUNIT_ASSERT(property.Name == propertyName);
}

void TestPropertyForFail(const Gefest::PropertyTree& tree, const std::string& itemPath)
{ 
  Gefest::Property property;
  CPPUNIT_ASSERT_THROW(property = tree.FindProperty(itemPath), Common::Error);
}


void TreeObjectTestCase::TestObject()
{
  Gefest::PropertyTree::SharedPtr tree(Gefest::PropertyTree::Create());
  BuildTree(tree);

  CPPUNIT_ASSERT_THROW(tree->FindGroup("Group_1"), Common::Error);
  CPPUNIT_ASSERT_THROW(tree->FindItem("Group_1"), Common::Error);
  
  TestGroupForSuccess(*tree, "Group_1", "/Group_1");
  TestGroupForFail(*tree, "/Group_11");
  
  TestGroupForSuccess(*tree, "Group_2", "/Group_2");
  TestGroupForFail(*tree, "/Group_21");
  TestGroupForSuccess(*tree, "Group_2_1", "/Group_2/Group_2_1");

  TestItemForSuccess(*tree, "Item_2_1", "/Group_2/Item_2_1");
  TestItemForFail(*tree, "/Group_2/Item_Fail");
  TestItemForSuccess(*tree, "Item_2_2", "/Group_2/Item_2_2");
 
  TestPropertyForSuccess(*tree, "Property_2_1_1", "/Group_2/Item_2_1/Property_2_1_1");
  TestPropertyForSuccess(*tree, "Property_2_1_2", "/Group_2/Item_2_1/Property_2_1_2");
  TestPropertyForFail(*tree, "/Group_2/Item_2_1/Property_2_1_1_FAIL");
}

