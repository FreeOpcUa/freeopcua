/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/common/exception.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/addon.h>
#include <opc/managers/io_manager/manager.h>
#include <src/managers/io_manager/manager_impl.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>
#include <tr1/functional>

class IoManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(IoManagerTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

 public:
  virtual void setUp()
  {
    Addons = Common::CreateAddonsManager();
  }

  virtual void tearDown()
  {
    Addons.reset();
  }
protected:
  Common::AddonsManager::UniquePtr Addons;

protected:
  void Test();
};


CPPUNIT_TEST_SUITE_REGISTRATION( IoManagerTestCase );

class DeviceManagerImpl 
    : public DeviceManager::Manager
{
public:
  explicit DeviceManagerImpl(const std::set<Common::ObjectID>& ids)
    : IDs(ids)
  {
  }

  std::set<Common::ObjectID> GetObjectIDs() const
  {
    return IDs;
  }


  virtual void Read(DeviceManager::ObjectDataMap& data)
  {
    Data = data;
  }

  virtual void Write(DeviceManager::ObjectDataMap& data)
  {
    Data = data;
  }

  void Reset()
  {
    Data.clear();
  }
 
  DeviceManager::ObjectDataMap GetData() const
  {
    return Data;
  }

private:
  DeviceManager::ObjectDataMap Data;
  const std::set<Common::ObjectID> IDs;
};

bool FindAbsentData(const DeviceManager::ObjectDataMap& map, const DeviceManager::ObjectDataMap::key_type& key)
{
  return map.find(key) == map.end();
}

void AddObjectDataPair(DeviceManager::ObjectDataMap& map, const Common::ObjectID& id)
{
  DeviceManager::ObjectData data;
  data.Value = Common::Value(id);
  map.insert(std::make_pair(id, data));
}

DeviceManager::ObjectDataMap CreateObjectDataMap(const std::set<Common::ObjectID> ids)
{
  DeviceManager::ObjectDataMap data;
  std::for_each(ids.begin(), ids.end(), 
    std::tr1::bind(&AddObjectDataPair, std::tr1::ref(data), std::tr1::placeholders::_1));
  return data;
}

void CheckBothManagers(const std::set<Common::ObjectID>& ids, DeviceManagerImpl& deviceManager1, DeviceManagerImpl& deviceManager2, InputOutputManager::RequestManager& ioManager)
{
  // read and write to one object for device manager 1
  std::unique_ptr<InputOutputManager::ReadRequest> readRequest;
  CPPUNIT_ASSERT_NO_THROW(readRequest.reset(ioManager.CreateReadRequest(ids).release()));
  CPPUNIT_ASSERT(readRequest.get());

  CPPUNIT_ASSERT_NO_THROW(readRequest->Perform());
  {
    const DeviceManager::ObjectDataMap& data1 = deviceManager1.GetData();
    const DeviceManager::ObjectDataMap& data2 = deviceManager2.GetData();
    CPPUNIT_ASSERT((data1.size() + data2.size()) == ids.size());
  }
  deviceManager1.Reset();
  deviceManager2.Reset();

  // write request
  std::unique_ptr<InputOutputManager::WriteRequest> writeRequest;
  CPPUNIT_ASSERT_NO_THROW(writeRequest.reset(ioManager.CreateWriteRequest(ids).release()));
  CPPUNIT_ASSERT(writeRequest.get());
  CPPUNIT_ASSERT_NO_THROW(writeRequest->Perform());
  {
    const DeviceManager::ObjectDataMap& data1 = deviceManager1.GetData();
    const DeviceManager::ObjectDataMap& data2 = deviceManager2.GetData();
    CPPUNIT_ASSERT((data1.size() + data2.size()) == ids.size());
  }
  deviceManager2.Reset();
  deviceManager1.Reset();
}

void CheckDeviceManager(const std::set<Common::ObjectID>& ids, DeviceManagerImpl& deviceManager, InputOutputManager::RequestManager& ioManager)
{
  // read and write to one object for device manager 1
  std::unique_ptr<InputOutputManager::ReadRequest> readRequest;
  CPPUNIT_ASSERT_NO_THROW(readRequest.reset(ioManager.CreateReadRequest(ids).release()));
  CPPUNIT_ASSERT(readRequest.get());

  CPPUNIT_ASSERT_NO_THROW(readRequest->Perform());
  {
    const DeviceManager::ObjectDataMap& data1 = deviceManager.GetData();
    CPPUNIT_ASSERT(ids.end() == std::find_if(ids.begin(), ids.end(), std::tr1::bind(FindAbsentData, std::tr1::cref(data1), std::tr1::placeholders::_1)));
  }
  deviceManager.Reset();

  // write request
  std::unique_ptr<InputOutputManager::WriteRequest> writeRequest;
  CPPUNIT_ASSERT_NO_THROW(writeRequest.reset(ioManager.CreateWriteRequest(ids).release()));
  CPPUNIT_ASSERT(writeRequest.get());
  CPPUNIT_ASSERT_NO_THROW(writeRequest->Perform());
  {
    const DeviceManager::ObjectDataMap& data1 = deviceManager.GetData();
    CPPUNIT_ASSERT(ids.end() == std::find_if(ids.begin(), ids.end(), std::tr1::bind(FindAbsentData, std::tr1::cref(data1), std::tr1::placeholders::_1)));
  }
  deviceManager.Reset();
}

void IoManagerTestCase::Test()
{
  std::unique_ptr<InputOutputManager::RequestManager> ioManager;
  try
  {
    ioManager = InputOutputManager::CreateRequestManager();
  }
  catch (const Common::Error& err)
  {
    CPPUNIT_ASSERT_MESSAGE(err.GetFullMessage(), false);
  }
  //Addon Manager should return valid object
  CPPUNIT_ASSERT(ioManager.get());
  
  // register device managers
  std::set<Common::ObjectID> ids1;
  ids1.insert(1);
  ids1.insert(2);
  std::shared_ptr<DeviceManagerImpl> deviceManager1(new DeviceManagerImpl(ids1));
  CPPUNIT_ASSERT_NO_THROW(ioManager->RegisterDeviceManager(deviceManager1));
  CPPUNIT_ASSERT_NO_THROW(ioManager->RegisterDeviceManager(deviceManager1));
  CheckDeviceManager(ids1, *deviceManager1, *ioManager);

  std::set<Common::ObjectID> invalidIds;
  invalidIds.insert(3);
  CPPUNIT_ASSERT_THROW(ioManager->CreateReadRequest(invalidIds), Common::Error);

  std::set<Common::ObjectID> ids2;
  ids2.insert(3);
  ids2.insert(4);
  std::shared_ptr<DeviceManagerImpl> deviceManager2(new DeviceManagerImpl(ids2));
  CPPUNIT_ASSERT_NO_THROW(ioManager->RegisterDeviceManager(deviceManager2));
  CheckDeviceManager(ids2, *deviceManager2, *ioManager);
  CheckDeviceManager(ids1, *deviceManager1, *ioManager);
  
  // check that both managers called
  ids1.insert(ids2.begin(), ids2.end());
  CheckBothManagers(ids2, *deviceManager1, *deviceManager2, *ioManager);
}
