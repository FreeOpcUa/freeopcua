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

#include <opc/ua/client/remote_server.h>
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
  std::unique_ptr<Server> Server;
  std::shared_ptr<AttributeServices> Service;
};



TEST_F(Attribute, Read)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));


  AttributeValueID id;
  id.Node.Encoding = EV_TWO_BYTE;
  id.Node.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::ObjectsFolder);
  id.Attribute = AttributeID::BROWSE_NAME;

  OpcUa::ReadParameters params;
  params.AttributesToRead.push_back(id);

  const std::vector<DataValue> values = Service->Read(params);
  ASSERT_EQ(values.size(), 1);
  EXPECT_TRUE(values.at(0).Encoding & DATA_VALUE);
}

TEST_F(Attribute, Write)
{
  using namespace OpcUa;
  ASSERT_TRUE(static_cast<bool>(Service));

  WriteValue value;
  value.Node.Encoding = EV_TWO_BYTE;
  value.Node.TwoByteData.Identifier = static_cast<uint8_t>(ObjectID::ObjectsFolder);
  value.Attribute = AttributeID::BROWSE_NAME;

  const std::vector<StatusCode> codes = Service->Write(std::vector<WriteValue>(1, value));
  ASSERT_EQ(codes.size(), 1);
  EXPECT_NE(codes[0], StatusCode::Good);
}


