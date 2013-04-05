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

#include <src/common/addons_core/dynamic_addon_factory.h>

#include <gtest/gtest.h>

TEST(DynamicAddonFactory, CanCreateAddons)
{
  Common::DynamicAddonFactory dynamicFactory("./libtest_dynamic_addon.so");
  ASSERT_TRUE(dynamicFactory.CreateAddon().get());
}

