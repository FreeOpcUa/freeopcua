/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of class Server::CommandLine.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <server/server_options.h>

#include <gtest/gtest.h>

TEST(ServerOptions, ParsesConfigurationFile)
{
  char* argv[2] = { "test.exe", "--config=./tests/configs/test.xml" };
  OpcUa::Server::CommandLine cmdline(2, argv);
  OpcUa::Server::ModulesConfiguration modules = cmdline.GetModules();
  ASSERT_EQ(modules.size(), 1);
  const OpcUa::Server::ModuleConfig& module = modules.front();
  ASSERT_EQ(module.ID, "child_module");
  ASSERT_EQ(module.Path, "child_module.so");
  ASSERT_EQ(module.DependsOn.size(), 2);
  ASSERT_EQ(module.DependsOn[0], "parent_module1");
  ASSERT_EQ(module.DependsOn[1], "parent_module2");
}

