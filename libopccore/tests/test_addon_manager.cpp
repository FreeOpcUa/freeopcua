/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "test_dynamic_addon_id.h"
#include "test_dynamic_addon.h"

#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/addons_core/dynamic_addon_factory.h>

#include <gtest/gtest.h>

using namespace Common;

namespace
{
  const char* modulePath = "./libtest_dynamic_addon.so";
}

TEST(AddonManager, CanCreateDynamicAddons)
{
  AddonsManager::UniquePtr addonsManager = CreateAddonsManager();
  addonsManager->Register(OpcCoreTests::TestDynamicAddonID, Common::CreateDynamicAddonFactory(modulePath));
  addonsManager->Start();
  Addon::SharedPtr addon = addonsManager->GetAddon(OpcCoreTests::TestDynamicAddonID);
  ASSERT_TRUE(static_cast<bool>(addon));

  OpcCoreTests::TestDynamicAddon::SharedPtr test = GetAddon<OpcCoreTests::TestDynamicAddon>(*addonsManager, OpcCoreTests::TestDynamicAddonID);
  ASSERT_TRUE(static_cast<bool>(test));
  ASSERT_EQ(test->GetStringWithHello(), "hello");
}

