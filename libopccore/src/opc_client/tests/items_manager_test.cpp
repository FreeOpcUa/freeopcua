/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of OpcBrowse inteerfaces
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

#include <opc/opc_client/errors.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/errors.h>
#include <opc/common/exception.h>
#include <opc/opc_client/items_manager.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

#include <tr1/functional>
#include <iostream>

class ItemsManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ItemsManagerTestCase);
  CPPUNIT_TEST(TestAddingItem);
  CPPUNIT_TEST(TestValidatingItem);
  CPPUNIT_TEST(TestSetActiveStateItem);
  CPPUNIT_TEST(TestSetClientIDs);
  CPPUNIT_TEST(TestRemoveItem);
  CPPUNIT_TEST(TestSetDataType);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestAddingItem();
  void TestValidatingItem();
  void TestSetActiveStateItem();
  void TestSetClientIDs();
  void TestRemoveItem();
  void TestSetDataType();
};

CPPUNIT_TEST_SUITE_REGISTRATION( ItemsManagerTestCase );

template <typename T>
void AddInvalidItemID(std::vector<T>& defs)
{
  // invalid item ID
  T addDef;
  addDef.ItemID = "/Group/Invalid_item";
  defs.push_back(addDef);
}

template <typename T>
void AddValidItemID(std::vector<T>& defs)
{
  // valid item ID
  T addDef;
  addDef.ItemID = "/Group1/Item1";
  defs.push_back(addDef);
}

template <typename T>
void AddInvalidRequestedDataType(std::vector<T>& defs)
{
  // invalid RequestedDataType
  T addDef;
  addDef.ItemID = "/Group1/Item1";
  addDef.RequestedDataType = Common::ValueType(100);
  defs.push_back(addDef);
}

template <typename T>
void AddUnknownRequestedDataType(std::vector<T>& defs)
{
  // valid RequestedDataType
  T addDef;
  addDef.ItemID = "/Group1/Item2";
  addDef.RequestedDataType = Common::UNKNOWN_TYPE;
  defs.push_back(addDef);
}


template <typename T>
void AddCharRequestedDataType(std::vector<T>& defs)
{
  // valid RequestedDataType
  T addDef;
  addDef.ItemID = "/Group1/Item3";
  addDef.RequestedDataType = Common::CHAR_TYPE;
  defs.push_back(addDef);
}

template <typename T>
void AddNotEmptyAccessPath(std::vector<T>& defs)
{
  // 2. access path not empty.
  T addDef;
  addDef.ItemID = "/Group1/Item1";
  addDef.AccessPath = "AccessPath";
  defs.push_back(addDef);
}

template <typename T>
void AddNotEmptyBlob(std::vector<T>& defs)
{
  // 3. blob not empty.
  T addDef;
  addDef.ItemID = "/Group1/Item1";
  addDef.AccessPath = "";
  addDef.Blob.push_back('1');
  defs.push_back(addDef);
}

void ItemsManagerTestCase::TestAddingItem()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();

  const std::unique_ptr<OpcClient::ItemsManager> itemsManager = OpcClient::CreateItemsManager();
  std::vector<OpcClient::AddItemDef> defs;

  // invalid items
  AddInvalidItemID(defs);
  AddInvalidRequestedDataType(defs);
  AddNotEmptyAccessPath(defs);
  AddNotEmptyBlob(defs);
  // valid items
  AddValidItemID(defs); 
  AddUnknownRequestedDataType(defs);
  AddCharRequestedDataType(defs);

  // Add items
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Add(defs));
  const std::map<Common::ObjectID, OpcClient::ItemAttributes> items = itemsManager->GetItemsAttributes();
  CPPUNIT_ASSERT(items.size() == 3);

  CPPUNIT_ASSERT_MESSAGE(defs[0].ResultError.GetFullMessage().c_str(), defs[0].ResultError.GetCode() == ItemPathNotFound.ErrorCode);
  CPPUNIT_ASSERT_MESSAGE(defs[1].ResultError.GetFullMessage().c_str(), defs[1].ResultError.GetCode() == InvalidValueType.ErrorCode);
  CPPUNIT_ASSERT_MESSAGE(defs[2].ResultError.GetFullMessage().c_str(), defs[2].ResultError.GetCode() == InvalidAccessPath.ErrorCode);
  CPPUNIT_ASSERT_MESSAGE(defs[3].ResultError.GetFullMessage().c_str(), defs[3].ResultError.GetCode() == BlobsUnsupported.ErrorCode);

  OpcClient::ItemAttributes attr = itemsManager->GetItemAttributes(defs[4].ResultServerID);
  CPPUNIT_ASSERT(attr.ItemID == defs[4].ItemID);

  attr = itemsManager->GetItemAttributes(defs[5].ResultServerID);
  CPPUNIT_ASSERT(attr.RequestedDataType == defs[5].RequestedDataType && attr.RequestedDataType == Common::UNKNOWN_TYPE);

  attr = itemsManager->GetItemAttributes(defs[6].ResultServerID);
  CPPUNIT_ASSERT(attr.RequestedDataType == defs[6].RequestedDataType && attr.RequestedDataType == Common::CHAR_TYPE);
  CPPUNIT_ASSERT(attr.CanonicalDataType == defs[6].ResultCanonicalDataType);
  CPPUNIT_ASSERT(attr.AccessRights == defs[6].ResultAccessRights);
}

void ItemsManagerTestCase::TestValidatingItem()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();

  const std::unique_ptr<OpcClient::ItemsManager> itemsManager = OpcClient::CreateItemsManager();
  std::vector<OpcClient::ValidateItemDef> defs;
  // invalid items
  AddInvalidItemID(defs);
  AddInvalidRequestedDataType(defs);
  AddNotEmptyAccessPath(defs);
  AddNotEmptyBlob(defs);
  // valid items
  AddValidItemID(defs); 
  AddUnknownRequestedDataType(defs);
  AddCharRequestedDataType(defs);
  // validate items
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Validate(defs));
  CPPUNIT_ASSERT_MESSAGE(defs[0].ResultError.GetFullMessage().c_str(), defs[0].ResultError.GetCode() == ItemPathNotFound.ErrorCode);
  CPPUNIT_ASSERT_MESSAGE(defs[1].ResultError.GetFullMessage().c_str(), defs[1].ResultError.GetCode() == InvalidValueType.ErrorCode);
  CPPUNIT_ASSERT_MESSAGE(defs[2].ResultError.GetFullMessage().c_str(), defs[2].ResultError.GetCode() == InvalidAccessPath.ErrorCode);
  CPPUNIT_ASSERT_MESSAGE(defs[3].ResultError.GetFullMessage().c_str(), defs[3].ResultError.GetCode() == BlobsUnsupported.ErrorCode);

  CPPUNIT_ASSERT(defs[4].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(defs[5].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(defs[6].ResultError.GetCode() == 0);
}

void ItemsManagerTestCase::TestSetActiveStateItem()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();

  const std::unique_ptr<OpcClient::ItemsManager> itemsManager = OpcClient::CreateItemsManager();
  std::vector<OpcClient::AddItemDef> defs;

  // valid items
  AddValidItemID(defs); 

  // Add items
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Add(defs));
  CPPUNIT_ASSERT(defs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(defs[0].IsActive == true);
  CPPUNIT_ASSERT(defs[0].ResultServerID != 0);
  CPPUNIT_ASSERT(itemsManager->GetItemsAttributes()[0].IsActive == true);

  OpcClient::SetActiveStateItemDef activeDef;
  activeDef.IsActive = false;
  activeDef.ServerID = defs[0].ResultServerID;
  std::vector<OpcClient::SetActiveStateItemDef> setActiveDefs(1, activeDef);
  CPPUNIT_ASSERT_NO_THROW(itemsManager->SetActiveState(setActiveDefs));
  CPPUNIT_ASSERT(setActiveDefs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(itemsManager->GetItemAttributes(defs[0].ResultServerID).IsActive == false);
}

void ItemsManagerTestCase::TestSetClientIDs()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();

  const std::unique_ptr<OpcClient::ItemsManager> itemsManager = OpcClient::CreateItemsManager();
  std::vector<OpcClient::AddItemDef> defs;

  // valid items
  AddValidItemID(defs); 

  // Add items
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Add(defs));
  CPPUNIT_ASSERT(defs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(defs[0].ResultServerID != 0);
  CPPUNIT_ASSERT(itemsManager->GetItemsAttributes()[0].IsActive == true);

  OpcClient::SetClientIDsItemDef itemDef;
  itemDef.ClientID = 12345;
  itemDef.ServerID = defs[0].ResultServerID;
  std::vector<OpcClient::SetClientIDsItemDef> setDefs(1, itemDef);
  CPPUNIT_ASSERT_NO_THROW(itemsManager->SetClientIDs(setDefs));
  CPPUNIT_ASSERT(setDefs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(itemsManager->GetItemAttributes(defs[0].ResultServerID).ClientID == 12345);
}

void ItemsManagerTestCase::TestSetDataType()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();

  const std::unique_ptr<OpcClient::ItemsManager> itemsManager = OpcClient::CreateItemsManager();
  std::vector<OpcClient::AddItemDef> defs;

  // valid items
  AddValidItemID(defs); 

  // Add items
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Add(defs));
  CPPUNIT_ASSERT(defs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(defs[0].ResultServerID != 0);
  CPPUNIT_ASSERT(itemsManager->GetItemsAttributes()[0].IsActive == true);

  OpcClient::SetDataTypeItemDef itemDef;
  itemDef.RequestedDataType = Common::INT_TYPE;
  itemDef.ServerID = defs[0].ResultServerID;
  std::vector<OpcClient::SetDataTypeItemDef> setDefs(1, itemDef);
  CPPUNIT_ASSERT_NO_THROW(itemsManager->SetDataType(setDefs));
  CPPUNIT_ASSERT(setDefs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(itemsManager->GetItemAttributes(defs[0].ResultServerID).RequestedDataType == itemDef.RequestedDataType);
}


void ItemsManagerTestCase::TestRemoveItem()
{
  std::shared_ptr<Common::AddonsManager> manager(Common::GetAddonsManager());
  PrepareTests();

  const std::unique_ptr<OpcClient::ItemsManager> itemsManager = OpcClient::CreateItemsManager();
  std::vector<OpcClient::AddItemDef> defs;

  // valid items
  AddValidItemID(defs); 

  // Add items
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Add(defs));
  CPPUNIT_ASSERT(defs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(defs[0].ResultServerID != 0);
  CPPUNIT_ASSERT(itemsManager->GetItemsAttributes()[0].IsActive == true);

  OpcClient::RemoveItemDef itemDef;
  itemDef.ServerID = defs[0].ResultServerID;
  std::vector<OpcClient::RemoveItemDef> removeDefs(1, itemDef);
  CPPUNIT_ASSERT(itemsManager->GetItemsAttributes().size() == 1);
  CPPUNIT_ASSERT_NO_THROW(itemsManager->Remove(removeDefs));
  CPPUNIT_ASSERT(removeDefs[0].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(itemsManager->GetItemsAttributes().size() == 0);
}
