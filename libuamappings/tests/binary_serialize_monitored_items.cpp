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

#include <opc/ua/extension_identifiers.h>
#include <opc/ua/message_identifiers.h>
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

  MonitoredItemsData data;

  GetStream() << data << flush;

  const std::vector<char> expectedData = {
    1,0,0,0,         // Results count
    0,0,(char)0x44,(char)0x80,   // StatusCode
    0,0,0,0,         // MonitoredItemID
    0,0,0,0,0,0,0,0, // RevisedSamplingInterval
    0,0,0,0,         // RevizedQueueSize
    0,0,0,           // FilterResult (empty Extension object)

    0,0,0,0  // Diagnostics Count
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(data));
}

TEST_F(MonitoredItemsSerialization, CreateMonitoredItemsResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateMonitoredItemsResponse response;

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
    0,0,0,0,         // MonitoredItemID
    0,0,0,0,0,0,0,0, // RevisedSamplingInterval
    0,0,0,0,         // RevizedQueueSize
    0,0,0,           // FilterResult (empty Extension object)

    0,0,0,0  // Diagnostics Count
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

