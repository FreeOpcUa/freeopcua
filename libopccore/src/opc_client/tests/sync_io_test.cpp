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

#include <opc/common/exception.h>
#include <opc/opc_client/errors.h>
#include <opc/opc_client/server.h>
#include <opc/opc_client/sync_io.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/managers/io_manager/manager.h>
#include <opc/managers/property_tree/item.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>
#include <iostream>
#include <tr1/functional>

#include "property_tree_builder.h"

class SyncIOTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(SyncIOTestCase);
  CPPUNIT_TEST(TestRightReadFromCache);
  CPPUNIT_TEST(TestWrongReadFromCache);
  CPPUNIT_TEST(TestReadFromDevice);
  CPPUNIT_TEST(TestWrongReadFromDevice);
  CPPUNIT_TEST(TestReadMaxAgeDevice);
  CPPUNIT_TEST(TestWriteDevice);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestRightReadFromCache();
  void TestWrongReadFromCache();
  void TestReadFromDevice();
  void TestWrongReadFromDevice();
  void TestReadMaxAgeDevice();
  void TestWriteDevice();
};

CPPUNIT_TEST_SUITE_REGISTRATION( SyncIOTestCase );

struct ServerData
{
  std::unique_ptr<OpcClient::Server> Server;
  std::shared_ptr<OpcClient::Group> Group;
  Common::ObjectID ServerID;
  Common::ObjectID ServerID2; // value of this item for read from device when reading with max age. Value == -1
  OpcClient::SyncIO::UniquePtr SyncIO;

  ServerData()
  {
    Server = OpcClient::CreateServer();
    PrepareTests(false);
    Group = Server->AddGroup("user_group");
    OpcClient::AddItemDef itemDef;
    itemDef.ItemID = "/Group1/Item1";
    std::vector<OpcClient::AddItemDef> itemsDef(1, itemDef);
    CPPUNIT_ASSERT_NO_THROW(Group->Add(itemsDef));
    CPPUNIT_ASSERT(itemsDef[0].ResultError.GetCode() == 0);
    const OpcClient::ItemAttributes& attrs = Group->GetItemAttributes(itemsDef[0].ResultServerID);
    CPPUNIT_ASSERT(attrs.Item->GetID() == itemsDef[0].ResultServerID);
    boost::posix_time::ptime current = boost::posix_time::microsec_clock::local_time();
    const Common::Value value(10, current);
    attrs.Item->SetValue(value);
    ServerID = itemsDef[0].ResultServerID;
    SyncIO = OpcClient::CreateSyncIO(Group);
 
    {
      itemDef.ItemID = "/Group1/Item3";
      std::vector<OpcClient::AddItemDef> itemsDef(1, itemDef);
      CPPUNIT_ASSERT_NO_THROW(Group->Add(itemsDef));
      ServerID2 = itemsDef[0].ResultServerID;
      CPPUNIT_ASSERT(itemsDef[0].ResultError.GetCode() == 0);
      const OpcClient::ItemAttributes& attrs = Group->GetItemAttributes(itemsDef[0].ResultServerID);
      CPPUNIT_ASSERT(attrs.Item->GetID() == itemsDef[0].ResultServerID);
    }
    CPPUNIT_ASSERT(ServerID != ServerID2);
  }
};
 
void SyncIOTestCase::TestRightReadFromCache()
{
  ServerData serverData;
  OpcClient::ReadItemDef readDef;
  readDef.ServerID = serverData.ServerID;
  std::vector<OpcClient::ReadItemDef> readDefs(1, readDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadFromCache(readDefs));
  const Common::Value& readValue =  readDefs[0].Data;
  CPPUNIT_ASSERT(readValue.Data == Common::VariantType(10));
}


void SyncIOTestCase::TestWrongReadFromCache()
{
  ServerData serverData;
  OpcClient::SyncIO::UniquePtr syncIO(OpcClient::CreateSyncIO(serverData.Group));
  OpcClient::ReadItemDef readDef;
  readDef.ServerID = serverData.ServerID + 1;
  std::vector<OpcClient::ReadItemDef> readDefs(1, readDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadFromCache(readDefs));
  CPPUNIT_ASSERT(readDefs[0].ResultError.GetCode() == NothingToRead.ErrorCode);
}

void SyncIOTestCase::TestReadFromDevice()
{
  ServerData serverData;
  OpcClient::SyncIO::UniquePtr syncIO(OpcClient::CreateSyncIO(serverData.Group));
  OpcClient::ReadItemDef readDef;
  std::vector<OpcClient::ReadItemDef> readDefs;
  CPPUNIT_ASSERT_THROW(serverData.SyncIO->ReadFromDevice(readDefs), Common::Error);

  readDef.ServerID = ~Common::ObjectID();
  readDefs.push_back(readDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadFromDevice(readDefs));
  CPPUNIT_ASSERT(readDefs[0].ResultError.GetCode() == InvalidServerID.ErrorCode);

  readDef.ServerID = serverData.ServerID;
  readDefs.push_back(readDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadFromDevice(readDefs));
  CPPUNIT_ASSERT(readDefs[1].ResultError.GetCode() == 0);
}

void SyncIOTestCase::TestWrongReadFromDevice()
{
  ServerData serverData;
  OpcClient::SyncIO::UniquePtr syncIO(OpcClient::CreateSyncIO(serverData.Group));
  std::vector<OpcClient::ReadItemDef> readDefs;
  // exception due to passed invalid readDefs
  CPPUNIT_ASSERT_THROW(serverData.SyncIO->ReadFromDevice(readDefs), Common::Error);

  OpcClient::ReadItemDef readDef;
  readDef.ServerID = ~Common::ObjectID();
  readDefs.push_back(readDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadFromDevice(readDefs));
  
  readDef.ServerID = serverData.ServerID;
  readDefs.push_back(readDef);
  // no exception due to readDefs partialy incorrect.
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadFromDevice(readDefs));
  CPPUNIT_ASSERT(readDefs[0].ResultError.GetCode() == InvalidServerID.ErrorCode);
  CPPUNIT_ASSERT(readDefs[1].ResultError.GetCode() == 0);
}

void SyncIOTestCase::TestReadMaxAgeDevice()
{
  ServerData serverData;
  OpcClient::SyncIO::UniquePtr syncIO(OpcClient::CreateSyncIO(serverData.Group));
  std::vector<OpcClient::ReadItemDef> readDefs;
  // Nothing to read
  CPPUNIT_ASSERT_THROW(serverData.SyncIO->ReadMaxAge(readDefs), Common::Error);
  OpcClient::ReadItemDef readDef;
   // error in read def
  readDef.ServerID = ~Common::ObjectID();
  readDef.MaxAge = 10000;
  readDefs.push_back(readDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadMaxAge(readDefs));
 
  // this should be read from cache: value should be 10
  readDef.ServerID = serverData.ServerID;
  readDef.MaxAge = 10000;
  readDefs.push_back(readDef);
  // this should be read from device: value should be -1
  readDef.ServerID = serverData.ServerID2;
  readDef.MaxAge = 10000;
  readDefs.push_back(readDef);
  // no exception.
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->ReadMaxAge(readDefs));
  CPPUNIT_ASSERT(readDefs[0].ResultError.GetCode() == InvalidServerID.ErrorCode);
  CPPUNIT_ASSERT(readDefs[1].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(readDefs[1].Data.Data == Common::VariantType(10));
  CPPUNIT_ASSERT(readDefs[2].ResultError.GetCode() == 0);
  CPPUNIT_ASSERT(readDefs[2].Data.Data == Common::VariantType(-1));
}

void SyncIOTestCase::TestWriteDevice()
{
  ServerData serverData;
  OpcClient::SyncIO::UniquePtr syncIO(OpcClient::CreateSyncIO(serverData.Group));
  std::vector<OpcClient::WriteItemDef> writeDefs;
  CPPUNIT_ASSERT_THROW(serverData.SyncIO->Write(writeDefs), Common::Error);

  OpcClient::WriteItemDef writeDef;
  writeDef.ServerID = ~Common::ObjectID();
  writeDefs.push_back(writeDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->Write(writeDefs));
  CPPUNIT_ASSERT(writeDefs[0].ResultError.GetCode() == InvalidServerID.ErrorCode);

  writeDef.ServerID = serverData.ServerID;
  writeDefs.push_back(writeDef);
  CPPUNIT_ASSERT_NO_THROW(serverData.SyncIO->Write(writeDefs));
  CPPUNIT_ASSERT(writeDefs[0].ResultError.GetCode() == InvalidServerID.ErrorCode);
  CPPUNIT_ASSERT(writeDefs[1].ResultError.GetCode() == 0);
}
