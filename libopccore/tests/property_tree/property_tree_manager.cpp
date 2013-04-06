/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/property_tree/manager.h>
#include <opccore/managers/property_tree/register.h>


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
  Common::AddonsConfiguration addonsConfig;
  addonsConfig.StaticAddonsInitializers.push_back(std::bind(PropertyTree::RegisterPropertyTreeAddon, std::placeholders::_1));

  Common::AddonsManager::UniquePtr addons = Common::CreateAddonsManager();
  CPPUNIT_ASSERT_NO_THROW(addons->Start(addonsConfig));
  PropertyTree::Manager::SharedPtr propertyTreeManager;
  CPPUNIT_ASSERT_NO_THROW(propertyTreeManager = Common::GetAddon<PropertyTree::Manager>(*addons, "property_tree"));
  CPPUNIT_ASSERT(propertyTreeManager);
  CPPUNIT_ASSERT(propertyTreeManager->GetPropertyTree());
}

