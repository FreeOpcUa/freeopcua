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

TEST(Session, Works)
{
  std::unique_ptr<OpcUa::Services> computer = OpcUa::Connect(GetEndpoint());

  OpcUa::RemoteSessionParameters session;
  session.ClientDescription.Name.Text = "opcua client";
  session.SessionName = "test";
  session.EndpointUrl = GetEndpoint();
  session.Timeout = 1000;

  computer->CreateSession(session);
  computer->ActivateSession();
  computer->CloseSession();
}

