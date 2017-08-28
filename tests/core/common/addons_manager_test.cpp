/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Test of addons manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/exception.h>
#include <opc/common/logger.h>
#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/managers/device_manager/id.h>
#include <opc/managers/property_tree/id.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/config/SourcePrefix.h>

class AddonsManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(AddonsManagerTestCase);
  CPPUNIT_TEST(TestTwoManagers);
  CPPUNIT_TEST(TestOneManager);
  CPPUNIT_TEST_SUITE_END();

public:
  virtual void setUp()
  {
    spdlog::drop_all();
    Logger = spdlog::stderr_color_mt("test");
    Logger->set_level(spdlog::level::info);
    Addons = Common::CreateAddonsManager();
  }

  virtual void tearDown()
  {
    Addons.reset();
  }
protected:
  Common::Logger::SharedPtr Logger;
  Common::AddonsManager::UniquePtr Addons;


protected:
  void TestOneManager();
  void TestTwoManagers();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AddonsManagerTestCase);

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

  virtual void Initialize(Common::AddonsManager &, const Common::AddonParameters &)
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

  virtual void Initialize(Common::AddonsManager &, const Common::AddonParameters &)
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

  Common::AddonInformation dmConfig;
  dmConfig.Id = DeviceManager::ManagerId;
  dmConfig.Factory = Common::AddonFactory::UniquePtr(new DeviceIOManagerFactory);
  dmConfig.Dependencies = std::vector<Common::AddonId>(1, PropertyTree::ManagerId);
  CPPUNIT_ASSERT_NO_THROW(Addons->Register(dmConfig));

  Common::AddonInformation ptConfig;
  ptConfig.Id = PropertyTree::ManagerId;
  ptConfig.Factory = Common::AddonFactory::UniquePtr(new PropertyTreeAddonFactory);
  CPPUNIT_ASSERT_NO_THROW(Addons->Register(ptConfig));

  CPPUNIT_ASSERT_NO_THROW(Addons->Start(/*Common::AddonsConfiguration()*/));
  CPPUNIT_ASSERT_THROW(Addons->Start(/*Common::AddonsConfiguration()*/), Common::Error);

  std::shared_ptr<PropertyTreeAddon> propertyTree;
  CPPUNIT_ASSERT_NO_THROW(propertyTree = Common::GetAddon<PropertyTreeAddon>(*Addons, PropertyTree::ManagerId));
  CPPUNIT_ASSERT(propertyTree);
  CPPUNIT_ASSERT(propertyTree->IsInitialized());

  CPPUNIT_ASSERT_NO_THROW(Addons->Stop());
  CPPUNIT_ASSERT(propertyTree->IsStopped());

  CPPUNIT_ASSERT_THROW(Addons->Stop(), Common::Error);
}

void AddonsManagerTestCase::TestTwoManagers()
{
  InitializedAddonsCount = 0;

  Common::AddonInformation dmConfig;
  dmConfig.Id = DeviceManager::ManagerId;
  dmConfig.Factory = Common::AddonFactory::UniquePtr(new DeviceIOManagerFactory);
  dmConfig.Dependencies = std::vector<Common::AddonId>(1, PropertyTree::ManagerId);
  CPPUNIT_ASSERT_NO_THROW(Addons->Register(dmConfig));
  CPPUNIT_ASSERT_THROW(Addons->Register(dmConfig), Common::Error);
  CPPUNIT_ASSERT_THROW(Addons->GetAddon(DeviceManager::ManagerId), Common::Error);

  Common::AddonInformation ptConfig;
  ptConfig.Id = PropertyTree::ManagerId;
  ptConfig.Factory = Common::AddonFactory::UniquePtr(new PropertyTreeAddonFactory);
  CPPUNIT_ASSERT_NO_THROW(Addons->Register(ptConfig));
  CPPUNIT_ASSERT_THROW(Addons->GetAddon(PropertyTree::ManagerId), Common::Error);

  CPPUNIT_ASSERT_NO_THROW(Addons->Start(/*Common::AddonsConfiguration()*/));
  CPPUNIT_ASSERT_THROW(Addons->Start(/*Common::AddonsConfiguration()*/), Common::Error);

  std::shared_ptr<PropertyTreeAddon> propertyTree;
  CPPUNIT_ASSERT_NO_THROW(propertyTree = Common::GetAddon<PropertyTreeAddon>(*Addons, PropertyTree::ManagerId));
  CPPUNIT_ASSERT(propertyTree);
  CPPUNIT_ASSERT(propertyTree->IsInitialized());

  std::shared_ptr<DeviceIOManagerAddon> deviceManager;
  CPPUNIT_ASSERT_NO_THROW(deviceManager = Common::GetAddon<DeviceIOManagerAddon>(*Addons, DeviceManager::ManagerId));
  CPPUNIT_ASSERT(deviceManager);
  CPPUNIT_ASSERT(deviceManager->IsInitialized());

  CPPUNIT_ASSERT_NO_THROW(Addons->Stop());
  CPPUNIT_ASSERT(propertyTree->IsStopped());
  CPPUNIT_ASSERT(deviceManager->IsStopped());

  CPPUNIT_ASSERT_THROW(Addons->Stop(), Common::Error);
}

