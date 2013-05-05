/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Subscription service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/extension_identifiers.h>
#include <opc/ua/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/subscriptions.h>

#include <algorithm>
#include <stdexcept>

class SubscriptionSerialization : public OpcUaBinarySerialization
{
};

class SubscriptionDeserialization : public OpcUaBinaryDeserialization
{
};


//-------------------------------------------------------
// SubScriptionParameters
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, SubscriptionParameters)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SubscriptionParameters params;
  params.RequestedPublishingInterval = 1200000;
  params.RequestedLifetimeCount = 2;
  params.RequestedMaxKeepAliveCount = 3;
  params.MaxNotificationsPerPublish = 4;
  params.PublishingEnabled = true;
  params.Priority = 5;

  GetStream() << params << flush;

  const std::vector<char> expectedData = {
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2,0,0,0,
    3,0,0,0,
    4,0,0,0,
    1,
    5
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(params));
}

TEST_F(SubscriptionDeserialization, SubscriptionParameters)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2,0,0,0,
    3,0,0,0,
    4,0,0,0,
    1,
    5
  };

  GetChannel().SetData(expectedData);

  SubscriptionParameters params;
  GetStream() >> params;

  ASSERT_EQ(params.RequestedPublishingInterval, 1200000);
  ASSERT_EQ(params.RequestedLifetimeCount, 2);
  ASSERT_EQ(params.RequestedMaxKeepAliveCount, 3);
  ASSERT_EQ(params.MaxNotificationsPerPublish, 4);
  ASSERT_EQ(params.PublishingEnabled, true);
  ASSERT_EQ(params.Priority, 5);
}

//-------------------------------------------------------
// CreateSubscriptionRequest
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, CreateSubscriptionRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateSubscriptionRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.Parameters.RequestedPublishingInterval = 1200000;
  request.Parameters.RequestedLifetimeCount = 2;
  request.Parameters.RequestedMaxKeepAliveCount = 3;
  request.Parameters.MaxNotificationsPerPublish = 4;
  request.Parameters.PublishingEnabled = true;
  request.Parameters.Priority = 5;

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x13, 0x3, // TypeID

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2,0,0,0,
    3,0,0,0,
    4,0,0,0,
    1,
    5
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(SubscriptionDeserialization, CreateSubscriptionRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x13, 0x3, // TypeID

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2,0,0,0,
    3,0,0,0,
    4,0,0,0,
    1,
    5
  };

  GetChannel().SetData(expectedData);

  CreateSubscriptionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.RequestedPublishingInterval, 1200000);
  ASSERT_EQ(request.Parameters.RequestedLifetimeCount, 2);
  ASSERT_EQ(request.Parameters.RequestedMaxKeepAliveCount, 3);
  ASSERT_EQ(request.Parameters.MaxNotificationsPerPublish, 4);
  ASSERT_EQ(request.Parameters.PublishingEnabled, true);
  ASSERT_EQ(request.Parameters.Priority, 5);
}

//-------------------------------------------------------
// SubscriptionData
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, SubscriptionData)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SubscriptionData data;
  data.ID = 2;
  data.RevisedPublishingInterval = 1200000;
  data.RevisedLifetimeCount = 3;
  data.RevizedMaxKeepAliveCount = 4;

  GetStream() << data << flush;

  const std::vector<char> expectedData = {
    2,0,0,0,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    3,0,0,0,
    4,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(data));
}

TEST_F(SubscriptionDeserialization, SubscriptionData)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    2,0,0,0,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    3,0,0,0,
    4,0,0,0,
  };

  GetChannel().SetData(expectedData);

  SubscriptionData data;
  GetStream() >> data;

  ASSERT_EQ(data.ID, 2);
  ASSERT_EQ(data.RevisedPublishingInterval, 1200000);
  ASSERT_EQ(data.RevisedLifetimeCount, 3);
  ASSERT_EQ(data.RevizedMaxKeepAliveCount, 4);
}
//-------------------------------------------------------
// CreateSubscriptionRequest
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, CreateSubscriptionResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateSubscriptionResponse response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.Data.ID = 2;
  response.Data.RevisedPublishingInterval = 1200000;
  response.Data.RevisedLifetimeCount = 3;
  response.Data.RevizedMaxKeepAliveCount = 4;

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x16, 0x3, // TypeID

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    2,0,0,0,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    3,0,0,0,
    4,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(SubscriptionDeserialization, CreateSubscriptionResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x16, 0x3, // TypeID

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    2,0,0,0,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    3,0,0,0,
    4,0,0,0,
  };

  GetChannel().SetData(expectedData);

  CreateSubscriptionResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Data.ID, 2);
  ASSERT_EQ(response.Data.RevisedPublishingInterval, 1200000);
  ASSERT_EQ(response.Data.RevisedLifetimeCount, 3);
  ASSERT_EQ(response.Data.RevizedMaxKeepAliveCount, 4);
}
