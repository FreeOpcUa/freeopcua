/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of class Server::CommandLine.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/server.h>
#include <src/server_options.h>
#include "test_addon.h"

#include <gtest/gtest.h>

TEST(ServerOptions, ParsesConfigurationFile)
{
  char* argv[2] = { "test.exe", "--config=./tests/configs/test.xml" };
  OpcUa::Server::CommandLine cmdline(2, argv);
  OpcUa::Server::ModulesConfiguration modules = cmdline.GetModules();
  ASSERT_EQ(modules.size(), 1);
  const Common::AddonConfiguration& module = modules.front();
  ASSERT_EQ(module.ID, "child_module");
  ASSERT_NE(module.Factory, Common::AddonFactory::SharedPtr());
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
