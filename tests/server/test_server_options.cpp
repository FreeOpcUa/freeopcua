/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of class UaServer::CommandLine.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/serverapp/server_options.h>
#include <opc/common/addons_core/config_file.h>

#include <algorithm>
#include <gtest/gtest.h>

#ifndef TEST_CORE_CONFIG_PATH
#define TEST_CORE_CONFIG_PATH "./tests/core/configs/"
#endif

const char* TestConfigFile = TEST_CORE_CONFIG_PATH "test.conf";
const char* TestConfigPath = TEST_CORE_CONFIG_PATH;

TEST(ServerOptions, ParsesCommandLine)
{
  const char* argv[4] = { "test.exe", "--config=" TEST_CORE_CONFIG_PATH, "--log-file=/path/to/log/server.log", "--daemon" };
  OpcUa::Server::CommandLine cmdline(4, argv);
  EXPECT_EQ(cmdline.GetLogFile(), "/path/to/log/server.log");
  EXPECT_EQ(cmdline.GetConfigDir(), TestConfigPath);
  EXPECT_TRUE(cmdline.IsDaemonMode());
}


TEST(ServerOptions, ParsesConfigurationFiles)
{
  Common::Configuration config = Common::ParseConfigurationFiles(TestConfigPath);
  std::vector<Common::AddonInformation> addons(config.Modules.size());
  std::transform(std::begin(config.Modules), std::end(config.Modules), std::begin(addons), [](const Common::ModuleConfiguration& module){
    return Common::GetAddonInfomation(module);
  });

  ASSERT_EQ(addons.size(), 1);
  const Common::AddonInformation& module = addons.front();
  ASSERT_EQ(module.Id, "child_module");
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

