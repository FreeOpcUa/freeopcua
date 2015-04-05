/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of class Server::CommandLine.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/addons_core/config_file.h>
#include <opc/common/addons_core/addon.h>

#include <gtest/gtest.h>

using namespace testing;

#ifndef TEST_CORE_CONFIG_PATH
#define TEST_CORE_CONFIG_PATH "./tests/core/configs/"
#endif

const char* TestConfigFile = TEST_CORE_CONFIG_PATH "test.conf";
const char* TestConfigPath = TEST_CORE_CONFIG_PATH;

TEST(ModulesConfiguration, ParsesConfigurationFile)
{
  Common::Configuration config = Common::ParseConfiguration(TestConfigFile);
  ASSERT_EQ(config.Modules.size(), 1);

  ASSERT_EQ(config.Parameters.Parameters.size(), 2);
  const std::vector<Common::Parameter> standaloneParams = config.Parameters.Parameters;
  EXPECT_EQ(standaloneParams[0].Name, "param1");
  EXPECT_EQ(standaloneParams[0].Value, "value1");
  EXPECT_EQ(standaloneParams[1].Name, "param2");
  EXPECT_EQ(standaloneParams[1].Value, "value2");

  const std::vector<Common::ParametersGroup> standaloneGroups = config.Parameters.Groups;
  ASSERT_EQ(standaloneGroups.size(), 2);
  ASSERT_EQ(standaloneGroups[0].Name, "group1");
  const std::vector<Common::Parameter> group1Params = standaloneGroups[0].Parameters;
  ASSERT_EQ(group1Params.size(), 2);
  ASSERT_EQ(group1Params[0].Name, "param1");
  ASSERT_EQ(group1Params[0].Value, "value1");
  ASSERT_EQ(group1Params[1].Name, "param2");
  ASSERT_EQ(group1Params[1].Value, "value2");

  ASSERT_EQ(standaloneGroups[1].Name, "group2");
  const std::vector<Common::Parameter> group2Params = standaloneGroups[1].Parameters;
  ASSERT_EQ(group2Params.size(), 2);
  ASSERT_EQ(group2Params[0].Name, "param1");
  ASSERT_EQ(group2Params[0].Value, "value1");
  ASSERT_EQ(group2Params[1].Name, "param2");
  ASSERT_EQ(group2Params[1].Value, "value2");


  const Common::ModuleConfiguration module = config.Modules.front();
  ASSERT_EQ(module.Id, "child_module");
  ASSERT_EQ(module.Path, "child_module.so");
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

TEST(ModulesConfiguration, SavesConfigurationFile)
{
  Common::ModuleConfiguration addon;
  addon.Id = "test_addon";
  addon.Dependencies.push_back("id1");
  addon.Dependencies.push_back("id2");
  addon.Path = "path";
  Common::ParametersGroup group;
  group.Name = "group";
  group.Parameters.push_back(Common::Parameter("parameter", "value"));
  addon.Parameters.Parameters.push_back(Common::Parameter("parameter", "value"));
  addon.Parameters.Groups.push_back(group);

  Common::SaveConfiguration(Common::ModulesConfiguration({addon}), "test_config.config");
  const Common::Configuration& config = Common::ParseConfiguration("test_config.config");
  ASSERT_EQ(config.Modules.size(), 1);
  const Common::ModuleConfiguration& module = config.Modules[0];
  ASSERT_EQ(module.Id, "test_addon");
  ASSERT_EQ(module.Path, "path");
  ASSERT_EQ(module.Dependencies.size(), 2);
  ASSERT_EQ(module.Dependencies[0], "id1");
  ASSERT_EQ(module.Dependencies[1], "id2");
  ASSERT_EQ(module.Parameters.Parameters.size(), 1);
  ASSERT_EQ(module.Parameters.Parameters[0].Name, "parameter");
  ASSERT_EQ(module.Parameters.Parameters[0].Value, "value");
  ASSERT_EQ(module.Parameters.Groups.size(), 1);
  ASSERT_EQ(module.Parameters.Groups[0].Name, "group");
  ASSERT_EQ(module.Parameters.Groups[0].Parameters.size(), 1);
  ASSERT_EQ(module.Parameters.Groups[0].Parameters[0].Name, "parameter");
  ASSERT_EQ(module.Parameters.Groups[0].Parameters[0].Value, "value");
}

TEST(ModulesConfiguration, ParsesConfigurationFilesInDirectory)
{
  Common::Configuration config = Common::ParseConfigurationFiles(TestConfigPath);
  ASSERT_EQ(config.Modules.size(), 1);
  ASSERT_EQ(config.Modules[0].Id, "child_module");
}

