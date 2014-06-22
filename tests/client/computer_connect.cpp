/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test Remote computer connection.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/client/remote_server.h>

#include <stdexcept>

TEST(Server, EstablishValidConnection)
{
  const std::string endpoint = GetEndpoint();
  std::unique_ptr<OpcUa::Remote::Server> computer;
  ASSERT_NO_THROW(computer = OpcUa::Remote::Connect(endpoint));
  ASSERT_TRUE(computer.get());
}

TEST(Server, ErrorIfInvalidUri)
{
  const std::string endpoint = "invalid uri";
  ASSERT_THROW(OpcUa::Remote::Connect(endpoint), std::logic_error);
}

TEST(Server, ErrorIdUnknownHost)
{
  const std::string endpoint = "opc.tcp://host.at.tne.mars:4840";
  ASSERT_THROW(OpcUa::Remote::Connect(endpoint), std::logic_error);
}

