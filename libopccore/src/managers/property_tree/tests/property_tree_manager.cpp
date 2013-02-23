#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/property_tree/manager.h>
#include "../manager/register.h"


class PropertyTreeManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(PropertyTreeManagerTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION(PropertyTreeManagerTestCase);


void PropertyTreeManagerTestCase::Test()
{
  PropertyTree::RegisterPropertyTreeAddon();
  Common::AddonsManager::SharedPtr manager = Common::GetAddonsManager();
  CPPUNIT_ASSERT_NO_THROW(manager->Start());
  PropertyTree::Manager::SharedPtr propertyTreeManager;
  CPPUNIT_ASSERT_NO_THROW(propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(Common::ADDON_ID_PROPERTY_TREE));
  CPPUNIT_ASSERT(propertyTreeManager);
  CPPUNIT_ASSERT(propertyTreeManager->GetPropertyTree());
}

