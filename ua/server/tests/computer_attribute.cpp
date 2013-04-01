/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test browseing with computer.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/client/remote_computer.h>
#include <opc/ua/object_ids.h>

#include <stdexcept>

using namespace OpcUa;
using namespace OpcUa::Remote;


class Attribute : public ::testing::Test
{
protected:
  Attribute()
  {
  }
 
  virtual void SetUp()
  {
    Server = Connect(GetEndpoint());
    if (!Server)
    {
      return;
    }

    SessionParameters session;
    session.ClientDescription.Name.Text = "opcua view service test";
    session.SessionName = "opua test";
    session.EndpointURL = GetEndpoint();
    session.Timeout = 1000;

    Server->CreateSession(session);
    Server->ActivateSession();

    Service = Server->Attributes();
  }

  virtual void TearDown()
  {
    Service = std::unique_ptr<AttributeServices>();
    if (Server)
    {
      Server->CloseSession();
      Server.reset();
    }
  }

protected:
  BrowseParameters Params;
  std::unique_ptr<Computer> Server;
  std::shared_ptr<AttributeServices> Service;
};



TEST_F(Attribute, Read)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));


  Remote::ReadParameters params;
  params.Node.Encoding = EV_TWO_BYTE;
  params.Node.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::ObjectsFolder);
  params.Attribute = AttributeID::BROWSE_NAME;

  const DataValue value = Service->Read(params);
  ASSERT_TRUE(value.Encoding & DATA_VALUE);
}

TEST_F(Attribute, Write)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));


  Remote::WriteParameters params;
  params.Node.Encoding = EV_TWO_BYTE;
  params.Node.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::ObjectsFolder);
  params.Attribute = AttributeID::BROWSE_NAME;

  const StatusCode code = Service->Write(params);
  ASSERT_NE(code, 0);
}


