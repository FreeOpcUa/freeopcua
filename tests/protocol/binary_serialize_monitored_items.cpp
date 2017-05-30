/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary monitored items.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <stdexcept>
class MonitoredItemsSerialization : public OpcUaBinarySerialization
{
};

class MonitoredItemsDeserialization : public OpcUaBinaryDeserialization
{
};


//-------------------------------------------------------
// std::vector<MonitoredItemCreateResult>
//-------------------------------------------------------

// TEST_F(MonitoredItemsSerialization, DISABLED_std::vector<MonitoredItemCreateResult>)
// {
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   MonitoredItemCreateResult monitoringResult;
//   monitoringResult.Status = StatusCode::BadNotImplemented;
//   monitoringResult.MonitoredItemId = 1;
//   monitoringResult.RevisedSamplingInterval = 1200000;
//   monitoringResult.RevisedQueueSize = 3;
//   //monitoringResult.ExtensionObjectHeader FilterResult;
//
//   std::vector<MonitoredItemCreateResult> data;
//   data.push_back(monitoringResult);
//
//   GetStream() << data << flush;
//
//   const std::vector<char> expectedData = {
//     1,0,0,0,         // Results count
//     0,0,(char)0x40,(char)0x80,   // StatusCode
//     1,0,0,0,         // MonitoredItemId
//     0,0,0,0,(char)0x80,(char)0x4f,(char)0x32,(char)0x41, // RevisedSamplingInterval
//     3,0,0,0,         // RevisedQueueSize
//     0,0,0,           // FilterResult (empty Extension object)
//
//     0,0,0,0  // Diagnostics Count
//   };
//
//   EXPECT_EQ(expectedData, GetChannel().SerializedData) <<
//       "Expected:" << std::endl <<
//       PrintData(expectedData) << std::endl <<
//       "Serialized:" << std::endl <<
//       PrintData(GetChannel().SerializedData) <<
//       std::endl;
//
//   EXPECT_EQ(expectedData.size(), RawSize(data));
// }

TEST_F(MonitoredItemsSerialization, DISABLED_CreateMonitoredItemsResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateMonitoredItemsResponse response;

  MonitoredItemCreateResult monitoringResult;
  monitoringResult.Status = StatusCode::BadNotImplemented;
  monitoringResult.MonitoredItemId = 1;
  monitoringResult.RevisedSamplingInterval = 1200000;
  monitoringResult.RevisedQueueSize = 3;
  //monitoringResult.ExtensionObjectHeader FilterResult;

  response.Results.push_back(monitoringResult);


  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_MONITORED_ITEMS_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  GetStream() << response << flush;

  const std::vector<char> expectedData =
  {
    1, 0, (char)0xF2, 0x2, // TypeId
    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1, 0, 0, 0,      // Results count
    0, 0, (char)0x40, (char)0x80, // StatusCode
    1, 0, 0, 0,      // MonitoredItemId
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41, // RevisedSamplingInterval
    3, 0, 0, 0,      // RevisedQueueSize
    0, 0, 0,         // FilterResult (empty Extension object)

    0, 0, 0, 0 // Diagnostics Count
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) <<
      "Expected: " << std::endl << PrintData(expectedData) << std::endl <<
      "Serialized: " << std::endl << PrintData(GetChannel().SerializedData) << std::endl;
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

