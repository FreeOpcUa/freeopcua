/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary attributes.
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
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>

#include <algorithm>
#include <stdexcept>

//-------------------------------------------------------
// Serialization
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, DataValue_NULL)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DataValue data;

  GetStream() << data << flush;

  const std::vector<char> expectedData = {
  0
  };

  ASSERT_EQ(expectedData.size(), RawSize(data));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinarySerialization, DataValue_Value)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DataValue data;
  data.Encoding = DATA_VALUE;
  data.Value = true;

  GetStream() << data << flush;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN);
  const std::vector<char> expectedData = {
  1,
  encodingMask, 1
  };

  ASSERT_EQ(expectedData.size(), RawSize(data));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinarySerialization, DataValue_Full)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask =
     DATA_VALUE |
     DATA_VALUE_STATUS_CODE |
     DATA_VALUE_SOURCE_TIMESTAMP |
     DATA_VALUE_Server_TIMESTAMP |
     DATA_VALUE_SOURCE_PICOSECONDS |
     DATA_VALUE_Server_PICOSECONDS;

  DataValue data;
  data.Encoding = encodingMask;
  data.Value = QualifiedName(1, OpcUa::Names::Root);
  data.Status = static_cast<StatusCode>(1);
  data.SourceTimestamp.Value = 2;
  data.SourcePicoseconds = 3;
  data.ServerTimestamp.Value = 4;
  data.ServerPicoseconds = 5;

  GetStream() << data << flush;


  char variantMask = static_cast<uint8_t>(VariantType::QUALIFIED_NAME);
  const std::vector<char> expectedData = {
  encodingMask,
  variantMask,
  1, 0, // QualifiedName::NamespaceIndex
  4,0,0,0, // Qualified::Name.size()
  'R', 'o', 'o', 't', // QualifiedName::Name::c_str()
  1,0,0,0,
  2,0,0,0,0,0,0,0,
  3,0,
  4,0,0,0,0,0,0,0,
  5,0
  };

  ASSERT_EQ(expectedData.size(), RawSize(data));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}


//-------------------------------------------------------
// Deserialization
//-------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, DataValue_NUL)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0
  };

  GetChannel().SetData(expectedData);

  DataValue data;
  GetStream() >> data;

  ASSERT_EQ(data.Encoding, 0);
}

TEST_F(OpcUaBinaryDeserialization, DataValue_Value)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask = static_cast<uint8_t>(VariantType::BOOLEAN);
  const std::vector<char> expectedData = {
  1,
  encodingMask, 1
  };

  GetChannel().SetData(expectedData);

  DataValue data;
  GetStream() >> data;

  ASSERT_EQ(data.Encoding, DATA_VALUE);
  ASSERT_FALSE(data.Value.IsNul());
}

TEST_F(OpcUaBinaryDeserialization, DataValue_Full)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char encodingMask =
     DATA_VALUE |
     DATA_VALUE_STATUS_CODE |
     DATA_VALUE_SOURCE_TIMESTAMP |
     DATA_VALUE_Server_TIMESTAMP |
     DATA_VALUE_SOURCE_PICOSECONDS |
     DATA_VALUE_Server_PICOSECONDS;


  char variantMask = static_cast<uint8_t>(VariantType::BOOLEAN);
  const std::vector<char> expectedData = {
  encodingMask,
  variantMask, 1,
  1,0,0,0,
  2,0,0,0,0,0,0,0,
  3,0,
  4,0,0,0,0,0,0,0,
  5,0
  };

  GetChannel().SetData(expectedData);

  DataValue data;
  GetStream() >> data;

  ASSERT_EQ(data.Encoding, encodingMask);
  ASSERT_FALSE(data.Value.IsNul());
  ASSERT_EQ(data.Status, static_cast<StatusCode>(1));
  ASSERT_EQ(data.SourceTimestamp, 2);
  ASSERT_EQ(data.SourcePicoseconds, 3);
  ASSERT_EQ(data.ServerTimestamp, 4);
  ASSERT_EQ(data.ServerPicoseconds, 5);
}


TEST(DataValue, ConstructivbeFromDataValue)
{
  using namespace OpcUa;
  NodeId node(ObjectId::RootFolder);
  DataValue data;
  data = node;
  ASSERT_TRUE(data.Encoding && DATA_VALUE);
  ASSERT_TRUE(data.Value.Type() == VariantType::NODE_Id);

  DataValue newValue(data);
  ASSERT_TRUE(newValue.Encoding && DATA_VALUE);
  ASSERT_EQ(newValue.Value.Type(), VariantType::NODE_Id);
  ASSERT_NO_THROW(newValue.Value.As<NodeId>());
}

TEST(DataValue, ComparableByValue)
{
  using namespace OpcUa;
  DataValue data1(uint32_t(1));
  DataValue data2(uint32_t(1));

  ASSERT_EQ(data1, data2);
}
