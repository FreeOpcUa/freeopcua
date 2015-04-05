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

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
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

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
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

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.Parameters.RequestedPublishingInterval = 1200000;
  request.Parameters.RequestedLifetimeCount = 2;
  request.Parameters.RequestedMaxKeepAliveCount = 3;
  request.Parameters.MaxNotificationsPerPublish = 4;
  request.Parameters.PublishingEnabled = true;
  request.Parameters.Priority = 5;

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x13, 0x3, // TypeId

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2,0,0,0,
    3,0,0,0,
    4,0,0,0,
    1,
    5
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(SubscriptionDeserialization, CreateSubscriptionRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x13, 0x3, // TypeId

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

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_REQUEST);

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
  data.Id = 2;
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

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
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

  ASSERT_EQ(data.Id, 2);
  ASSERT_EQ(data.RevisedPublishingInterval, 1200000);
  ASSERT_EQ(data.RevisedLifetimeCount, 3);
  ASSERT_EQ(data.RevizedMaxKeepAliveCount, 4);
}
//-------------------------------------------------------
// CreateSubscriptionResponse
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, CreateSubscriptionResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateSubscriptionResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.Data.Id = 2;
  response.Data.RevisedPublishingInterval = 1200000;
  response.Data.RevisedLifetimeCount = 3;
  response.Data.RevizedMaxKeepAliveCount = 4;

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x16, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    2,0,0,0,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    3,0,0,0,
    4,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(SubscriptionDeserialization, CreateSubscriptionResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x16, 0x3, // TypeId

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

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SUBSCRIPTION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Data.Id, 2);
  ASSERT_EQ(response.Data.RevisedPublishingInterval, 1200000);
  ASSERT_EQ(response.Data.RevisedLifetimeCount, 3);
  ASSERT_EQ(response.Data.RevizedMaxKeepAliveCount, 4);
}

//-------------------------------------------------------
// SubscriptionAcknowledgement
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, SubscriptionAcknowledgement)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SubscriptionAcknowledgement ack;
  ack.SubscriptionId = 1;
  ack.SequenceNumber = 2;

  GetStream() << ack << flush;

  const std::vector<char> expectedData = {
    1,0,0,0,
    2,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(ack));
}

TEST_F(SubscriptionDeserialization, SubscriptionAcknowledgement)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1,0,0,0,
    2,0,0,0,
  };

  GetChannel().SetData(expectedData);

  SubscriptionAcknowledgement ack;
  GetStream() >> ack;

  ASSERT_EQ(ack.SubscriptionId, 1);
  ASSERT_EQ(ack.SequenceNumber, 2);
}

//-------------------------------------------------------
// PublishParameters
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, PublishParameters)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SubscriptionAcknowledgement ack;
  ack.SubscriptionId = 1;
  ack.SequenceNumber = 2;

  PublishParameters params;
  params.Acknowledgements.push_back(ack);

  GetStream() << params << flush;

  const std::vector<char> expectedData = {
    1,0,0,0, // Count of acks
    1,0,0,0,
    2,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(params));
}

TEST_F(SubscriptionDeserialization, PublishParameters)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1,0,0,0, // Count of acks
    1,0,0,0,
    2,0,0,0,
  };

  GetChannel().SetData(expectedData);

  PublishParameters params;
  GetStream() >> params;

  ASSERT_EQ(params.Acknowledgements.size(), 1);
}

//-------------------------------------------------------
// CreateSubscriptionRequest
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, PublishRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::PUBLISH_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  SubscriptionAcknowledgement ack;
  ack.SubscriptionId = 1;
  ack.SequenceNumber = 2;

  request.Parameters.Acknowledgements.push_back(ack);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x3A, 0x3, // TypeId

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    // Parameters
    1,0,0,0, // Count of acks
    1,0,0,0,
    2,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(SubscriptionDeserialization, PublishRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x3A, 0x3, // TypeId

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    // Parameters
    1,0,0,0, // Count of acks
    1,0,0,0,
    2,0,0,0
  };

  GetChannel().SetData(expectedData);

  PublishRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::PUBLISH_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.Acknowledgements.size(), 1);
}

//-------------------------------------------------------
// NotificationMessage
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, NotificationMessage)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  NotificationMessage data;
  data.SequenceId = 1;
  data.PublishTime.Value = 2;
  data.Data.push_back(NotificationData());

  GetStream() << data << flush;

  const std::vector<char> expectedData = {
    1,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime

    1,0,0,0, // Count of data
    //Message.Header
    0,0, // TypeId
    0,   // Encoding
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(data));
}

TEST_F(SubscriptionDeserialization, NotificationMessage)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime
    // Data vector
    1,0,0,0, //Count of Data
    0,0, // Data.Header.TypeId
    0,   // Data.Header.Encoding
  };

  GetChannel().SetData(expectedData);

  NotificationMessage message;
  GetStream() >> message;

  ASSERT_EQ(message.SequenceId, IntegerId(1));
  ASSERT_EQ(message.PublishTime, 2);
}

//-------------------------------------------------------
// PublishResult
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, PublishResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishResult result;
  result.SubscriptionId = 1;
  result.AvailableSequenceNumber.push_back(2);
  result.MoreNotifications = true;

  result.Message.SequenceId = 1;
  result.Message.PublishTime.Value = 2;
  result.Message.Data.push_back(NotificationData());

  result.Statuses.push_back(StatusCode::Good);

  DiagnosticInfo diag;
  diag.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO);
  diag.LocalizedText = 4;
  diag.InnerDiagnostics.reset(new DiagnosticInfo());
  diag.InnerDiagnostics->EncodingMask = DIM_ADDITIONAL_INFO;
  diag.InnerDiagnostics->AdditionalInfo = "add";
  result.Diagnostics.push_back(diag);

  GetStream() << result << flush;

  const std::vector<char> expectedData = {
    1,0,0,0, // SubscriptionId
    //AvailableSequenceNumbers
    1,0,0,0, // count
    2,0,0,0,
    // MoreNotifications
    1,

    // NotificationData
    1,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime
    // Data vector
    1,0,0,0, //Count of Data
    0,0, // Data.Header.TypeId
    0,   // Data.Header.Encoding

    // Statuses
    1,0,0,0,
    0,0,0,0,
    // Diagnostics
    1,0,0,0, // Count
    static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
    DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(result));
}

TEST_F(SubscriptionDeserialization, PublishResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1,0,0,0, // SubscriptionId
    //AvailableSequenceNumbers
    1,0,0,0, // count
    2,0,0,0,
    // MoreNotifications
    1,

    // NotificationData
    1,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime
    // Data vector
    1,0,0,0, //Count of Data
    0,0, // Data.Header.TypeId
    0,   // Data.Header.Encoding

    // Statuses
    1,0,0,0,
    0,0,0,0,
    // Diagnostics
    1,0,0,0, // Count
    static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
    DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  GetChannel().SetData(expectedData);

  PublishResult result;
  GetStream() >> result;

  ASSERT_EQ(result.SubscriptionId, 1);
  ASSERT_EQ(result.AvailableSequenceNumber.size(), 1);
  ASSERT_EQ(result.MoreNotifications, true);
  ASSERT_EQ(result.Message.Data.size(), 1);
  ASSERT_EQ(result.Statuses.size(), 1);
  ASSERT_EQ(result.Diagnostics.size(), 1);
}

//-------------------------------------------------------
// PublishResponse
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, PublishResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::PUBLISH_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  PublishResult result;
  response.Result.SubscriptionId = 1;
  response.Result.AvailableSequenceNumber.push_back(2);
  response.Result.MoreNotifications = true;
  response.Result.Message.SequenceId = 1;
  response.Result.Message.PublishTime.Value = 2;
  response.Result.Message.Data.push_back(NotificationData());
  response.Result.Statuses.push_back(StatusCode::Good);

  DiagnosticInfo diag;
  diag.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO);
  diag.LocalizedText = 4;
  diag.InnerDiagnostics.reset(new DiagnosticInfo());
  diag.InnerDiagnostics->EncodingMask = DIM_ADDITIONAL_INFO;
  diag.InnerDiagnostics->AdditionalInfo = "add";
  response.Result.Diagnostics.push_back(diag);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x3D, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1,0,0,0, // SubscriptionId
    //AvailableSequenceNumbers
    1,0,0,0, // count
    2,0,0,0,
    // MoreNotifications
    1,

    // NotificationData
    1,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime
    // Data vector
    1,0,0,0, //Count of Data
    0,0, // Data.Header.TypeId
    0,   // Data.Header.Encoding

    // Statuses
    1,0,0,0,
    0,0,0,0,
    // Diagnostics
    1,0,0,0, // Count
    static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
    DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(SubscriptionSerialization, PublishResponse_Empty)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::PUBLISH_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  PublishResult result;
  response.Result.Message.PublishTime.Value = 2;

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x3D, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1,0,0,0, // SubscriptionId
    //AvailableSequenceNumbers
    0,0,0,0, // count
    // MoreNotifications
    0,

    // NotificationData
    0,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime
    // Data vector
    0,0,0,0, //Count

    // Statuses
    0,0,0,0, // Count
    // Diagnostics
    0,0,0,0, // Count
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(SubscriptionDeserialization, PublishResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x3D, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1,0,0,0, // SubscriptionId
    //AvailableSequenceNumbers
    1,0,0,0, // count
    2,0,0,0,
    // MoreNotifications
    1,

    // NotificationData
    1,0,0,0, // SequenceId
    2,0,0,0,0,0,0,0, // PublishTime
    // Data vector
    1,0,0,0, //Count of Data
    0,0, // Data.Header.TypeId
    0,   // Data.Header.Encoding

    // Statuses
    1,0,0,0,
    0,0,0,0,
    // Diagnostics
    1,0,0,0, // Count
    static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
    DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  GetChannel().SetData(expectedData);

  PublishResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::PUBLISH_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Result.Diagnostics.size(), 1);
}

//-------------------------------------------------------
// PublishingModeParameters
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, PublishingModeParameters)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishingModeParameters params;
  params.Enabled = true;
  params.SubscriptionIds.push_back(IntegerId());

  GetStream() << params << flush;

  const std::vector<char> expectedData = {
    1,
    1,0,0,0, // SubscriptionId
    1,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(params));
}

TEST_F(SubscriptionSerialization, PublishingModeParameters_Empty)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishingModeParameters params;

  GetStream() << params << flush;

  const std::vector<char> expectedData = {
    0,
    0,0,0,0, // Count of subscriptions
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(params));
}


TEST_F(SubscriptionDeserialization, PublishingModeParameters)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1,
    1,0,0,0, // SubscriptionId
    1,0,0,0
  };

  GetChannel().SetData(expectedData);

  PublishingModeParameters params;
  GetStream() >> params;

  ASSERT_EQ(params.Enabled, true);
  ASSERT_EQ(params.SubscriptionIds.size(), 1);
  ASSERT_EQ(params.SubscriptionIds[0], 1);
}

//-------------------------------------------------------
// SetPublishingModeRequest
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, SetPublishingModeRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SetPublishingModeRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::SET_PUBLISHING_MODE_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.Parameters.Enabled = true;
  request.Parameters.SubscriptionIds.push_back(IntegerId());

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x1F, 0x3, // TypeId

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    // Parameters
    1, // Enabled
    1,0,0,0,
    1,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(SubscriptionDeserialization, SetPublishingModeRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x1F, 0x3, // TypeId

    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    // Parameters
    1, // Enabled
    1,0,0,0,
    1,0,0,0
  };

  GetChannel().SetData(expectedData);

  SetPublishingModeRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::SET_PUBLISHING_MODE_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.Enabled, true);
  ASSERT_EQ(request.Parameters.SubscriptionIds.size(), 1);
  ASSERT_EQ(request.Parameters.SubscriptionIds[0], 1);
}

//-------------------------------------------------------
// PublishingModeResult
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, PublishingModeResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishingModeResult result;
  result.Statuses.push_back(StatusCode::Good);

  GetStream() << result << flush;

  const std::vector<char> expectedData = {
    1,0,0,0,
    0,0,0,0, // StatusCode
    0,0,0,0  // Count of diagnostics
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(result));
}

TEST_F(SubscriptionSerialization, PublishingModeResult_Empty)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  PublishingModeResult result;

  GetStream() << result << flush;

  const std::vector<char> expectedData = {
    0,0,0,0,
    0,0,0,0, // Count of subscriptions
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(result));
}


TEST_F(SubscriptionDeserialization, PublishingModeResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1,0,0,0,
    0,0,0,0, // StatusCode
    0,0,0,0  // Count of diagnostics
  };

  GetChannel().SetData(expectedData);

  PublishingModeResult result;
  GetStream() >> result;

  ASSERT_EQ(result.Statuses.size(), 1);
  ASSERT_EQ(result.Statuses[0], StatusCode::Good);
  ASSERT_EQ(result.Diagnostics.size(), 0);
}

//-------------------------------------------------------
// SetPublishingModeResponse
//-------------------------------------------------------

TEST_F(SubscriptionSerialization, SetPublishingModeResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SetPublishingModeResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::SET_PUBLISHING_MODE_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.Result.Statuses.push_back(StatusCode::Good);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x22, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1,0,0,0,
    0,0,0,0, // StatusCode
    0,0,0,0  // Count of diagnostics
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(SubscriptionSerialization, SetPublishingModeResponse_Empty)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SetPublishingModeResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::SET_PUBLISHING_MODE_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x22, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    0,0,0,0, // StatusCode
    0,0,0,0  // Count of diagnostics
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << "Actual:" << std::endl << PrintData(GetChannel().SerializedData) << std::endl << "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(SubscriptionDeserialization, SetPublishingModeResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x22, 0x3, // TypeId

    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1,0,0,0,
    0,0,0,0, // StatusCode
    0,0,0,0  // Count of diagnostics
  };

  GetChannel().SetData(expectedData);

  SetPublishingModeResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::SET_PUBLISHING_MODE_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Result.Statuses.size(), 1);
  ASSERT_EQ(response.Result.Diagnostics.size(), 0);
}
