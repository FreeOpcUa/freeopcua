/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Test of Gps::DeviceMananager class
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../addon.h"
#include "../gps_device_manager.h"
#include <common/addons_core/addon_ids.h>
#include <common/addons_core/addon_manager.h>
#include <managers/device_manager/manager.h>
#include <managers/property_tree/group.h>
#include <managers/property_tree/manager/manager.h>
#include <managers/property_tree/manager/register.h>
#include <managers/property_tree/property_tree.h>

class GpsDeviceManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(GpsDeviceManagerTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION( GpsDeviceManagerTestCase );

void GpsDeviceManagerTestCase::Test()
{
  std::shared_ptr<Common::AddonsManager> manager = Common::GetAddonsManager();
  std::shared_ptr<PropertyTree::Manager> propertyTreeManager;
  propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE);
  std::shared_ptr<Gefest::PropertyTree> propertyTree = propertyTreeManager->GetPropertyTree();
  std::shared_ptr<Gefest::Group> rootGroup = propertyTree->GetRootGroup();
  CPPUNIT_ASSERT(rootGroup->ItemExists("latitude"));
  CPPUNIT_ASSERT(rootGroup->ItemExists("longitude"));
  CPPUNIT_ASSERT(rootGroup->ItemExists("altitude"));
  CPPUNIT_ASSERT(rootGroup->ItemExists("gps_mode"));
  std::shared_ptr<DeviceManager::Manager> deviceManager = Common::GetAddon<DeviceManager::Manager>(Common::ADDON_ID_DEVICE_IO_MANAGER);
  CPPUNIT_ASSERT(deviceManager);
  std::set<Common::ObjectID> ids = deviceManager->GetObjectIDs();
  CPPUNIT_ASSERT(!ids.empty());
  DeviceManager::ObjectDataMap data;
  data[*ids.begin()];
  CPPUNIT_ASSERT_THROW(deviceManager->Write(data), Common::Error);
  CPPUNIT_ASSERT_NO_THROW(deviceManager->Read(data));
  CPPUNIT_ASSERT(data.begin()->second.Error.GetCode() == 0);
  manager->Stop();
}
