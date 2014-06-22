/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of class UaServer::CommandLine.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/application.h>
#include <src/server/server_options.h>
#include "test_addon.h"

#include <gtest/gtest.h>

TEST(LoadingAddon, DISABLED_TestAdon)
{
  const char* argv[4] = { "test.exe", "--config=./tests/server/configs/", "--log-file=/path/to/log/server.log", "--daemon" };
  OpcUa::UaServer::CommandLine cmdline(4, argv);
  EXPECT_EQ(cmdline.GetLogFile(), "/path/to/log/server.log");
  EXPECT_TRUE(cmdline.IsDaemonMode());

  OpcUa::Application::UniquePtr server = OpcUa::CreateApplication();
  ASSERT_NO_THROW(server->Start(cmdline.GetModules()));
  ASSERT_NO_THROW(server->GetAddonsManager());

  OpcUa::Test::TestAddon::SharedPtr testAddon;
  ASSERT_NO_THROW(testAddon = server->GetAddonsManager().GetAddon<OpcUa::Test::TestAddon>(OpcUa::Test::TestAddonID));
  ASSERT_NE(testAddon, OpcUa::Test::TestAddon::SharedPtr());
  const Common::AddonParameters params = testAddon->GetParameters();
  ASSERT_EQ(params.Parameters.size(), 1);
  ASSERT_EQ(params.Parameters[0].Name, "hello");
  ASSERT_EQ(params.Parameters[0].Value, "world");
}


TEST(ServerOptions, DISABLED_ParsesConfigurationFile)
{
  const char* argv[2] = { "test.exe", "--config=./tests/server/configs" };
  OpcUa::UaServer::CommandLine cmdline(2, argv);
  std::vector<Common::AddonInformation> modules = cmdline.GetModules();
  ASSERT_EQ(modules.size(), 1);
  const Common::AddonInformation& module = modules.front();
  ASSERT_EQ(module.ID, "child_module");
  //ASSERT_EQ(module.Path, "child_module.so");
  ASSERT_EQ(module.Dependencies.size(), 2);
  ASSERT_EQ(module.Dependencies[0], "parent_module1");
  ASSERT_EQ(module.Dependencies[1], "parent_module2");

  ASSERT_EQ(module.Parameters.Parameters.size(), 5);
  ASSERT_EQ(module.Parameters.Parameters[0].Name, "application_name");
  ASSERT_EQ(module.Parameters.Parameters[0].Value, "Test OPC UA Server");

  ASSERT_EQ(module.Parameters.Groups.size(), 2);
  ASSERT_EQ(module.Parameters.Groups[0].Name, "user_token_policy");
  ASSERT_EQ(module.Parameters.Groups[0].Parameters.size(), 3);
  ASSERT_EQ(module.Parameters.Groups[0].Parameters[0].Name, "id");
  ASSERT_EQ(module.Parameters.Groups[0].Parameters[0].Value, "anonymous");
  ASSERT_EQ(module.Parameters.Groups[0].Parameters[1].Name, "type");
  ASSERT_EQ(module.Parameters.Groups[0].Parameters[1].Value, "anonymous");
}

