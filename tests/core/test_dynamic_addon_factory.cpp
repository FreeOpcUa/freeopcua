/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "test_dynamic_addon.h"

#include <opc/common/addons_core/dynamic_addon_factory.h>

#include <gtest/gtest.h>


#ifndef DYNAMIC_ADDON_PATH
#define DYNAMIC_ADDON_PATH "./libtest_dynamic_addon.so"
#endif

const char * modulePath = DYNAMIC_ADDON_PATH;

TEST(DynamicAddonFactory, CanCreateAddons)
{
  Common::AddonFactory::UniquePtr dynamicFactory = Common::CreateDynamicAddonFactory(modulePath);
  ASSERT_TRUE(static_cast<bool>(dynamicFactory->CreateAddon()));
}

TEST(DynamicAddonFactory, AddonInterfaceCastsToManagerInterface)
{
  Common::AddonFactory::UniquePtr dynamicFactory = Common::CreateDynamicAddonFactory(modulePath);
  std::shared_ptr<Common::Addon> addon(dynamicFactory->CreateAddon());
  std::shared_ptr<OpcCoreTests::TestDynamicAddon> testAddon = std::dynamic_pointer_cast<OpcCoreTests::TestDynamicAddon>(addon);
  ASSERT_TRUE(static_cast<bool>(testAddon));
}

TEST(DynamicAddonFactory, CanCallMethodsOfAddon)
{
  Common::AddonFactory::UniquePtr dynamicFactory = Common::CreateDynamicAddonFactory(modulePath);
  std::shared_ptr<Common::Addon> addon(dynamicFactory->CreateAddon());
  std::shared_ptr<OpcCoreTests::TestDynamicAddon> testAddon = std::dynamic_pointer_cast<OpcCoreTests::TestDynamicAddon>(addon);
  ASSERT_EQ(testAddon->GetStringWithHello(), std::string("hello"));
}

