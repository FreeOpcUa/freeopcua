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

TEST(ModulesConfiguration, ParsesConfigurationFile)
{
  Common::ModulesConfiguration modules = Common::ParseConfiguration("./tests/core/configs/test.conf");
  ASSERT_EQ(modules.size(), 1);
  const Common::ModuleConfiguration module = modules.front();
  ASSERT_EQ(module.ID, "child_module");
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
  addon.ID = "test_addon";
  addon.Dependencies.push_back("id1");
  addon.Dependencies.push_back("id2");
  addon.Path = "path";
  Common::ParametersGroup group;
  group.Name = "group";
  group.Parameters.push_back(Common::Parameter("parameter", "value"));
  addon.Parameters.Parameters.push_back(Common::Parameter("parameter", "value"));
  addon.Parameters.Groups.push_back(group);

  Common::SaveConfiguration(Common::ModulesConfiguration({addon}), "test_config.config");
  const Common::ModulesConfiguration& modules = Common::ParseConfiguration("test_config.config");
  ASSERT_EQ(modules.size(), 1);
  const Common::ModuleConfiguration& config = modules[0];
  ASSERT_EQ(config.ID, "test_addon");
  ASSERT_EQ(config.Path, "path");
  ASSERT_EQ(config.Dependencies.size(), 2);
  ASSERT_EQ(config.Dependencies[0], "id1");
  ASSERT_EQ(config.Dependencies[1], "id2");
  ASSERT_EQ(config.Parameters.Parameters.size(), 1);
  ASSERT_EQ(config.Parameters.Parameters[0].Name, "parameter");
  ASSERT_EQ(config.Parameters.Parameters[0].Value, "value");
  ASSERT_EQ(config.Parameters.Groups.size(), 1);
  ASSERT_EQ(config.Parameters.Groups[0].Name, "group");
  ASSERT_EQ(config.Parameters.Groups[0].Parameters.size(), 1);
  ASSERT_EQ(config.Parameters.Groups[0].Parameters[0].Name, "parameter");
  ASSERT_EQ(config.Parameters.Groups[0].Parameters[0].Value, "value");
}

TEST(ModulesConfiguration, ParsesConfigurationFilesInDirectory)
{
  Common::ModulesConfiguration modules = Common::ParseConfigurationFiles("./tests/core/configs");
  ASSERT_EQ(modules.size(), 1);
  ASSERT_EQ(modules[0].ID, "child_module");
}

