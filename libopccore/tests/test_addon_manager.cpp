/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/common/addons_core/addon_manager.h>

#include <gtest/gtest.h>

const char* modulePath = "./libtest_dynamic_addon.so";

TEST(AddonManager, CanCreateDynamicAddons)
{
  AddonsConfiguration dynamicAddonsCinfig;
  dynamicAddonsConfig.DynamicModules.push_back(modulePath); 
  AddonsManager::UniquePtr addonsManager = CreateAddonsManager();
  addonsManager->Start(dynamicAddonsConfig);
}

