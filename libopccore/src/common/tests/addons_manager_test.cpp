/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Test of addons manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

#include <opccore/common/exception.h>
#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>

class AddonsManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(AddonsManagerTestCase);
  CPPUNIT_TEST(TestTwoManagers);
  CPPUNIT_TEST(TestOneManager);
  CPPUNIT_TEST_SUITE_END();

protected:
  void TestOneManager();
  void TestTwoManagers();
};

CPPUNIT_TEST_SUITE_REGISTRATION( AddonsManagerTestCase );

unsigned InitializedAddonsCount = 0;


class PropertyTreeAddon : public Common::Addon
{
private:
  bool Initialized;
  bool Stopped;

public:
  PropertyTreeAddon()
    : Initialized(false)
    , Stopped(false)
  {
  }

  virtual void Initialize()
  {
    CPPUNIT_ASSERT(InitializedAddonsCount == 0);
    InitializedAddonsCount++;
    Initialized = true;
  }
  
  virtual void Stop()
  {
    Stopped = true;
  }

  bool IsInitialized() const
  {
    return Initialized;
  }

  bool IsStopped() const
  {
    return Stopped;
  }
};

class PropertyTreeAddonFactory : public Common::AddonFactory
{
public:
  virtual Common::Addon::UniquePtr CreateAddon()
  {
    return  Common::Addon::UniquePtr(new PropertyTreeAddon());
  }
};

class DeviceIOManagerAddon : public Common::Addon
{
private:
  bool Initialized;
  bool Stopped;

public:
  DeviceIOManagerAddon()
    : Initialized(false)
    , Stopped(false)
  {
  }

  virtual void Initialize()
  {
    CPPUNIT_ASSERT(InitializedAddonsCount == 1);
    InitializedAddonsCount++;
    Initialized = true;
  }
  
  virtual void Stop()
  {
    Stopped = true;
  }

  bool IsInitialized() const
  {
    return Initialized;
  }
 
  bool IsStopped() const
  {
    return Stopped;
  }
};

class DeviceIOManagerFactory : public Common::AddonFactory
{
public:
  virtual Common::Addon::UniquePtr CreateAddon()
  {
    return  Common::Addon::UniquePtr(new DeviceIOManagerAddon());
  }
};
void AddonsManagerTestCase::TestOneManager()
{
  InitializedAddonsCount = 0;
  Common::AddonsManager::SharedPtr manager(Common::GetAddonsManager());
  CPPUNIT_ASSERT(manager);

  CPPUNIT_ASSERT_NO_THROW(manager->Register(Common::ADDON_ID_DEVICE_IO_MANAGER, Common::AddonFactory::UniquePtr(new DeviceIOManagerFactory), std::vector<Common::AddonID>(1, Common::ADDON_ID_PROPERTY_TREE)));

  CPPUNIT_ASSERT_NO_THROW(manager->Register(Common::ADDON_ID_PROPERTY_TREE, Common::AddonFactory::UniquePtr(new PropertyTreeAddonFactory)));

  CPPUNIT_ASSERT_NO_THROW(manager->Start(Common::AddonsConfiguration()));
  CPPUNIT_ASSERT_THROW(manager->Start(Common::AddonsConfiguration()), Common::Error);

  std::shared_ptr<PropertyTreeAddon> propertyTree;
  CPPUNIT_ASSERT_NO_THROW(propertyTree = Common::GetAddon<PropertyTreeAddon>(Common::ADDON_ID_PROPERTY_TREE));
  CPPUNIT_ASSERT(propertyTree);
  CPPUNIT_ASSERT(propertyTree->IsInitialized());
  
  CPPUNIT_ASSERT_NO_THROW(manager->Stop());
  CPPUNIT_ASSERT(propertyTree->IsStopped());

  CPPUNIT_ASSERT_THROW(manager->Stop(), Common::Error);
}
void AddonsManagerTestCase::TestTwoManagers()
{
  InitializedAddonsCount = 0;
  Common::AddonsManager::SharedPtr manager(Common::GetAddonsManager());
  CPPUNIT_ASSERT(manager);

  CPPUNIT_ASSERT_NO_THROW(manager->Register(Common::ADDON_ID_DEVICE_IO_MANAGER, Common::AddonFactory::UniquePtr(new DeviceIOManagerFactory), std::vector<Common::AddonID>(1, Common::ADDON_ID_PROPERTY_TREE)));
  CPPUNIT_ASSERT_THROW(manager->Register(Common::ADDON_ID_DEVICE_IO_MANAGER, Common::AddonFactory::UniquePtr(new DeviceIOManagerFactory), std::vector<Common::AddonID>(1, Common::ADDON_ID_PROPERTY_TREE)), Common::Error);
  CPPUNIT_ASSERT_THROW(manager->GetAddon(Common::ADDON_ID_DEVICE_IO_MANAGER), Common::Error);

  CPPUNIT_ASSERT_NO_THROW(manager->Register(Common::ADDON_ID_PROPERTY_TREE, Common::AddonFactory::UniquePtr(new PropertyTreeAddonFactory)));
  CPPUNIT_ASSERT_THROW(manager->GetAddon(Common::ADDON_ID_PROPERTY_TREE), Common::Error);

  CPPUNIT_ASSERT_NO_THROW(manager->Start(Common::AddonsConfiguration()));
  CPPUNIT_ASSERT_THROW(manager->Start(Common::AddonsConfiguration()), Common::Error);

  std::shared_ptr<PropertyTreeAddon> propertyTree;
  CPPUNIT_ASSERT_NO_THROW(propertyTree = Common::GetAddon<PropertyTreeAddon>(Common::ADDON_ID_PROPERTY_TREE));
  CPPUNIT_ASSERT(propertyTree);
  CPPUNIT_ASSERT(propertyTree->IsInitialized());
  
  std::shared_ptr<DeviceIOManagerAddon> deviceManager;
  CPPUNIT_ASSERT_NO_THROW(deviceManager = Common::GetAddon<DeviceIOManagerAddon>(Common::ADDON_ID_DEVICE_IO_MANAGER));
  CPPUNIT_ASSERT(deviceManager);
  CPPUNIT_ASSERT(deviceManager->IsInitialized());

  CPPUNIT_ASSERT_NO_THROW(manager->Stop());
  CPPUNIT_ASSERT(propertyTree->IsStopped());
  CPPUNIT_ASSERT(deviceManager->IsStopped());

  CPPUNIT_ASSERT_THROW(manager->Stop(), Common::Error);
}
