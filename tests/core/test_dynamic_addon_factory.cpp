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

const char* modulePath = "./libtest_dynamic_addon.so";

TEST(DynamicAddonFactory, CanCreateAddons)
{
  Common::AddonFactory::UniquePtr dynamicFactory = Common::CreateDynamicAddonFactory(modulePath);
  ASSERT_TRUE(dynamicFactory->CreateAddon().get());
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
  ASSERT_EQ(testAddon->GetStringWithHello(), "hello");
}

