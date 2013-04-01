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

TEST(ServerOptions, DefaultPortIs4841)
{
  OpcUa::Server::CommandLine cmdline(0,0);
  ASSERT_EQ(cmdline.GetPort(), 4841);
}

TEST(ServerOptions, PortCanBeSet)
{
  char* argv[2] = { "test.exe", "--port=12345" };
  OpcUa::Server::CommandLine cmdline(2, argv);
  ASSERT_EQ(cmdline.GetPort(), 12345);
}

