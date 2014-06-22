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
// MonitoredItemsData
//-------------------------------------------------------

TEST_F(MonitoredItemsSerialization, MonitoredItemsData)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateMonitoredItemsResult monitoringResult;
  monitoringResult.Status = StatusCode::BadNotImplemented;
  monitoringResult.MonitoredItemID = 1;
  monitoringResult.RevisedSamplingInterval = 1200000;
  monitoringResult.RevizedQueueSize = 3;
  //monitoringResult.ExtensionObjectHeader FilterResult;

  MonitoredItemsData data;
  data.Results.push_back(monitoringResult);

  GetStream() << data << flush;

  const std::vector<char> expectedData = {
    1,0,0,0,         // Results count
    0,0,(char)0x44,(char)0x80,   // StatusCode
    1,0,0,0,         // MonitoredItemID
    0,0,0,0,(char)0x80,(char)0x4f,(char)0x32,(char)0x41, // RevisedSamplingInterval
    3,0,0,0,         // RevizedQueueSize
    0,0,0,           // FilterResult (empty Extension object)

    0,0,0,0  // Diagnostics Count
  };

  EXPECT_EQ(expectedData, GetChannel().SerializedData) <<
      "Expected:" << std::endl <<
      PrintData(expectedData) << std::endl <<
      "Serialized:" << std::endl <<
      PrintData(GetChannel().SerializedData) <<
      std::endl;

  EXPECT_EQ(expectedData.size(), RawSize(data));
}

TEST_F(MonitoredItemsSerialization, CreateMonitoredItemsResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateMonitoredItemsResponse response;

  CreateMonitoredItemsResult monitoringResult;
  monitoringResult.Status = StatusCode::BadNotImplemented;
  monitoringResult.MonitoredItemID = 1;
  monitoringResult.RevisedSamplingInterval = 1200000;
  monitoringResult.RevizedQueueSize = 3;
  //monitoringResult.ExtensionObjectHeader FilterResult;

  response.Data.Results.push_back(monitoringResult);


  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CREATE_MONITORED_ITEMS_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0xF2, 0x2, // TypeID
    // RequestHeader
    TEST_RESPONSE_HEADER_BINARY_DATA,

    1,0,0,0,         // Results count
    0,0,(char)0x44,(char)0x80,   // StatusCode
    1,0,0,0,         // MonitoredItemID
    0,0,0,0,(char)0x80,(char)0x4f,(char)0x32,(char)0x41, // RevisedSamplingInterval
    3,0,0,0,         // RevizedQueueSize
    0,0,0,           // FilterResult (empty Extension object)

    0,0,0,0  // Diagnostics Count
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) <<
      "Expected: " << std::endl << PrintData(expectedData) << std::endl <<
      "Serialized: " << std::endl << PrintData(GetChannel().SerializedData) << std::endl;
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

