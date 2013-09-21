/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of class Server::CommandLine.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/application.h>
#include <src/server_options.h>
#include "test_addon.h"

#include <gtest/gtest.h>

TEST(LoadingAddon, TestAdon)
{
  char* argv[2] = { "test.exe", "--config=./tests/configs/test_addon.config" };
  OpcUa::Server::CommandLine cmdline(2, argv);
  OpcUa::Application::UniquePtr server = OpcUa::CreateApplication();
  ASSERT_NO_THROW(server->Start(cmdline.GetModules()));
  ASSERT_NO_THROW(server->GetAddonsManager());

  OpcUa::Test::TestAddon::SharedPtr testAddon;
  ASSERT_NO_THROW(testAddon = Common::GetAddon<OpcUa::Test::TestAddon>(server->GetAddonsManager(), OpcUa::Test::TestAddonID));
  ASSERT_NE(testAddon, OpcUa::Test::TestAddon::SharedPtr());
  const Common::AddonParameters params = testAddon->GetParameters();
  ASSERT_EQ(params.Parameters.size(), 1);
  ASSERT_EQ(params.Parameters[0].Name, "hello");
  ASSERT_EQ(params.Parameters[0].Value, "world");
}

