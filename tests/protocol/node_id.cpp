/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

using namespace testing;
using namespace OpcUa;

class NodeDeserialization : public OpcUaBinaryDeserialization
{
};

class NodeSerialization : public OpcUaBinarySerialization
{
};

class NodeComparing : public OpcUaBinarySerialization
{
};


TEST(NodeID, DefaultConstructor)
{
  NodeID id;
  ASSERT_EQ(id.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(id.TwoByteData.Identifier, 0);
  ASSERT_EQ(id.ServerIndex, 0);
  ASSERT_EQ(id.NamespaceURI, std::string());
}

TEST(NodeID, NumericConstructor)
{
  NodeID id(99, 1);
  ASSERT_EQ(id.Encoding, EV_NUMERIC);
  ASSERT_EQ(id.NumericData.Identifier, 99);
  ASSERT_EQ(id.GetNamespaceIndex(), 1);
}

TEST(Node, StringConstructor)
{
  NodeID id("StrID", 10);
  ASSERT_EQ(id.Encoding, EV_STRING);
  ASSERT_EQ(id.GetStringIdentifier(), "StrID");
  ASSERT_EQ(id.GetNamespaceIndex(), 10);
}

TEST(Node, ConstructFromMessageID)
{
  NodeID id(ACTIVATE_SESSION_REQUEST);
  ASSERT_EQ(id.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(id.FourByteData.Identifier, ACTIVATE_SESSION_REQUEST);
  ASSERT_EQ(id.ServerIndex, 0);
  ASSERT_EQ(id.NamespaceURI, std::string());
}

TEST(Node, ConstructFromReferenceID)
{
  NodeID id(ReferenceID::HasChild);
  ASSERT_EQ(id.Encoding, EV_NUMERIC);
  ASSERT_EQ(id.NumericData.Identifier, static_cast<uint16_t>(ReferenceID::HasChild));
  ASSERT_EQ(id.ServerIndex, 0);
  ASSERT_EQ(id.NamespaceURI, std::string());
}

TEST(Node, EqualIfSameType)
{
  NodeID id1(ReferenceID::HasChild);
  NodeID id2(ReferenceID::HasChild);
  ASSERT_EQ(id1, id2);
}

TEST(Node, NotEqualByReferenceID)
{
  NodeID id1(ReferenceID::HasChild);
  NodeID id2(ReferenceID::Organizes);
  ASSERT_NE(id1, id2);
}

TEST(Node, NotEqualByObjectID)
{
  NodeID id(ReferenceID::Organizes);
  ASSERT_NE(id, ObjectID::RootFolder);
}

TEST(Node, EqualIfDifferentTypeButEqualIdentifier)
{
  NodeID id1;
  id1.Encoding = EV_TWO_BYTE;
  id1.TwoByteData.Identifier = 1;

  NodeID id2;
  id2.Encoding = EV_FOUR_BYTE;
  id2.FourByteData.Identifier = 1;
  ASSERT_EQ(id1, id2);
}

TEST(Node, DefferentIfDifferentNameSpace)
{
  NodeID id1;
  id1.Encoding = EV_FOUR_BYTE;
  id1.FourByteData.NamespaceIndex = 1;
  id1.FourByteData.Identifier = 1;

  NodeID id2;
  id2.Encoding = EV_FOUR_BYTE;
  id2.FourByteData.NamespaceIndex = 2;
  id2.FourByteData.Identifier = 1;

  ASSERT_NE(id1, id2);
}



//---------------------------------------------------------
// Deserialization
//---------------------------------------------------------

TEST_F(NodeDeserialization, TwoByte)
{
  const std::vector<char> expectedData = {
  EV_TWO_BYTE,
  1
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(id.TwoByteData.Identifier, 0x1);
}

TEST_F(NodeDeserialization, FourByte)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  EV_FOUR_BYTE,
  1,
  2, 0
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(id.FourByteData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.FourByteData.Identifier, 0x2);
}

TEST_F(NodeDeserialization, Numeric)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  EV_NUMERIC,
  1, 0,
  2, 0, 0, 0
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, EV_NUMERIC);
  ASSERT_EQ(id.NumericData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.NumericData.Identifier, 0x2);
}

TEST_F(NodeDeserialization, String)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  EV_STRING,
  1, 0,
  2, 0, 0, 0,
  'i', 'd'
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, EV_STRING);
  ASSERT_EQ(id.StringData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.StringData.Identifier, "id");
}

TEST_F(NodeDeserialization, Guid)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  EV_BYTE_STRING,
  1, 0,
  4, 0, 0, 0,
  1, 2, 3, 4
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, EV_BYTE_STRING);
  ASSERT_EQ(id.BinaryData.NamespaceIndex, 0x1);
  std::vector<uint8_t> expectedBytes = {1, 2, 3, 4};
  ASSERT_EQ(id.BinaryData.Identifier, expectedBytes);
}

TEST_F(NodeDeserialization, ByteString)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
    EV_GUID,
    1, 0,
    4, 3, 2, 1,
    6, 5,
    8, 7,
    1, 2, 3, 4, 5, 6, 7, 8
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, EV_GUID);
  ASSERT_EQ(id.GuidData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.GuidData.Identifier.Data1, 0x01020304);
  ASSERT_EQ(id.GuidData.Identifier.Data2, 0x0506);
  ASSERT_EQ(id.GuidData.Identifier.Data3, 0x0708);
  ASSERT_EQ(id.GuidData.Identifier.Data4[0], 0x01);
  ASSERT_EQ(id.GuidData.Identifier.Data4[1], 0x02);
  ASSERT_EQ(id.GuidData.Identifier.Data4[2], 0x03);
  ASSERT_EQ(id.GuidData.Identifier.Data4[3], 0x04);
  ASSERT_EQ(id.GuidData.Identifier.Data4[4], 0x05);
  ASSERT_EQ(id.GuidData.Identifier.Data4[5], 0x06);
  ASSERT_EQ(id.GuidData.Identifier.Data4[6], 0x07);
  ASSERT_EQ(id.GuidData.Identifier.Data4[7], 0x08);
}

TEST_F(NodeDeserialization, NamespaceUri)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_NAMESPACE_URI_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  3, 0, 0, 0,
  'u', 'r', 'i'
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, uint8_t(EV_STRING | EV_NAMESPACE_URI_FLAG));
  ASSERT_EQ(id.StringData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.StringData.Identifier, "id");
  ASSERT_EQ(id.NamespaceURI, "uri");
}

TEST_F(NodeDeserialization, ServerIndexFlag)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_SERVER_INDEX_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  1, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, uint8_t(EV_STRING | EV_SERVER_INDEX_FLAG));
  ASSERT_EQ(id.StringData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.StringData.Identifier, "id");
  ASSERT_EQ(id.ServerIndex, 1);
}

TEST_F(NodeDeserialization, NamespaceUriAndServerIndex)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_SERVER_INDEX_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  3, 0, 0, 0,
  'u', 'r', 'i',
  1, 0, 0, 0
  };

  GetChannel().SetData(expectedData);

  NodeID id;
  GetStream() >> id;

  ASSERT_EQ(id.Encoding, uint8_t(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_SERVER_INDEX_FLAG));
  ASSERT_EQ(id.StringData.NamespaceIndex, 0x1);
  ASSERT_EQ(id.StringData.Identifier, "id");
  ASSERT_EQ(id.NamespaceURI, "uri");
  ASSERT_EQ(id.ServerIndex, 1);
}

//---------------------------------------------------------
// Serialization
//---------------------------------------------------------

TEST_F(NodeSerialization, ToByte)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = EV_TWO_BYTE;
  id.TwoByteData.Identifier = 0x1;

  const std::vector<char> expectedData = {
  EV_TWO_BYTE,
  1
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, FourByte)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = EV_FOUR_BYTE;
  id.FourByteData.NamespaceIndex = 0x1;
  id.FourByteData.Identifier = 0x2;

  const std::vector<char> expectedData = {
  EV_FOUR_BYTE,
  1,
  2, 0
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, Numeric)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = EV_NUMERIC;
  id.NumericData.NamespaceIndex = 0x1;
  id.NumericData.Identifier = 0x2;

  const std::vector<char> expectedData = {
  EV_NUMERIC,
  1, 0,
  2, 0, 0, 0
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, String)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = EV_STRING;
  id.StringData.NamespaceIndex = 0x1;
  id.StringData.Identifier = "id";

  const std::vector<char> expectedData = {
  EV_STRING,
  1, 0,
  2, 0, 0, 0,
  'i', 'd'
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, ByteString)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = EV_BYTE_STRING;
  id.BinaryData.NamespaceIndex = 0x1;
  id.BinaryData.Identifier = {1, 2, 3, 4};

  const std::vector<char> expectedData = {
  EV_BYTE_STRING,
  1, 0,
  4, 0, 0, 0,
  1, 2, 3, 4
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, Guid)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = EV_GUID;
  id.GuidData.NamespaceIndex = 0x1;
  id.GuidData.Identifier.Data1 = 0x01020304;
  id.GuidData.Identifier.Data2 = 0x0506;
  id.GuidData.Identifier.Data3 = 0x0708;
  id.GuidData.Identifier.Data4[0] = 0x01;
  id.GuidData.Identifier.Data4[1] = 0x02;
  id.GuidData.Identifier.Data4[2] = 0x03;
  id.GuidData.Identifier.Data4[3] = 0x04;
  id.GuidData.Identifier.Data4[4] = 0x05;
  id.GuidData.Identifier.Data4[5] = 0x06;
  id.GuidData.Identifier.Data4[6] = 0x07;
  id.GuidData.Identifier.Data4[7] = 0x08;

  const std::vector<char> expectedData = {
    EV_GUID,
    1, 0,
    4, 3, 2, 1,
    6, 5,
    8, 7,
    1, 2, 3, 4, 5, 6, 7, 8
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, NamespaceUri)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = static_cast<NodeIDEncoding>(EV_STRING | EV_NAMESPACE_URI_FLAG);
  id.StringData.NamespaceIndex = 0x1;
  id.StringData.Identifier = "id";
  id.NamespaceURI = "uri";

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_NAMESPACE_URI_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  3, 0, 0, 0,
  'u', 'r', 'i'
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, ServerIndexFlag)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = static_cast<NodeIDEncoding>(EV_STRING | EV_SERVER_INDEX_FLAG);
  id.StringData.NamespaceIndex = 0x1;
  id.StringData.Identifier = "id";
  id.ServerIndex = 1;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_SERVER_INDEX_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  1, 0, 0, 0,
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

TEST_F(NodeSerialization, NamespaceUriAndServerIndex)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  NodeID id;
  id.Encoding = static_cast<NodeIDEncoding>(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_SERVER_INDEX_FLAG);
  id.StringData.NamespaceIndex = 0x1;
  id.StringData.Identifier = "id";
  id.NamespaceURI = "uri";
  id.ServerIndex = 1;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_SERVER_INDEX_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  3, 0, 0, 0,
  'u', 'r', 'i',
  1, 0, 0, 0
  };

  GetStream() << id << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(id));
}

//-----------------------------------------------------------------
// Comparing for less
//-----------------------------------------------------------------

TEST_F(NodeComparing, TwoByteForLess)
{
  EXPECT_TRUE(TwoByteNodeID(1) < TwoByteNodeID(2));
}

TEST_F(NodeComparing, TwoByteForNotLess)
{
  EXPECT_FALSE(TwoByteNodeID(2) < TwoByteNodeID(2));
}

TEST_F(NodeComparing, TwoByteForEqual)
{
  EXPECT_TRUE(TwoByteNodeID(2) == TwoByteNodeID(2));
}

TEST_F(NodeComparing, FourByteForLess)
{
  EXPECT_TRUE(FourByteNodeID(1) < FourByteNodeID(2));
}

TEST_F(NodeComparing, FourByteForNotLess)
{
  EXPECT_FALSE(FourByteNodeID(2) < FourByteNodeID(2));
}

TEST_F(NodeComparing, FourByteForEqual)
{
  EXPECT_TRUE(FourByteNodeID(2) == FourByteNodeID(2));
}

TEST_F(NodeComparing, NumericForLess)
{
  EXPECT_TRUE(NumericNodeID(1) < NumericNodeID(2));
}

TEST_F(NodeComparing, NumericNotLess)
{
  EXPECT_FALSE(NumericNodeID(2) < NumericNodeID(2));
}

TEST_F(NodeComparing, NumericEqual)
{
  EXPECT_TRUE(NumericNodeID(2) == NumericNodeID(2));
}

TEST_F(NodeComparing, StringForLess)
{
  EXPECT_TRUE(StringNodeID("1") < StringNodeID("2"));
}

TEST_F(NodeComparing, StringNotLess)
{
  EXPECT_FALSE(StringNodeID("2") < StringNodeID("2"));
}

TEST_F(NodeComparing, StringEqual)
{
  EXPECT_TRUE(StringNodeID("2") == StringNodeID("2"));
}

TEST_F(NodeComparing, BinaryForLess)
{
  EXPECT_TRUE(BinaryNodeID({1, 1}) < BinaryNodeID({2, 2}));
}

TEST_F(NodeComparing, BinaryNotLess)
{
  EXPECT_FALSE(BinaryNodeID({2, 3}) < BinaryNodeID({2, 3}));
}

TEST_F(NodeComparing, BinaryEqual)
{
  EXPECT_TRUE(BinaryNodeID({2, 2}) == BinaryNodeID({2,2}));
}

TEST_F(NodeComparing, GuidForLess)
{
  Guid l;
  l.Data1 = 1;
  Guid r;
  r.Data1 = 2;
  EXPECT_TRUE(GuidNodeID(l) < GuidNodeID(r));
}

TEST_F(NodeComparing, GuidNotLess)
{
  Guid l;
  l.Data1 = 1;
  Guid r;
  r.Data1 = 1;
  EXPECT_FALSE(GuidNodeID(l) < GuidNodeID(r));
}

TEST_F(NodeComparing, GuidEqual)
{
  Guid l;
  l.Data1 = 1;
  Guid r;
  r.Data1 = 1;
  EXPECT_TRUE(GuidNodeID(l) == GuidNodeID(r));
}

TEST_F(NodeComparing, NodesEqual)
{
  uint32_t id = 1;
  uint32_t ns = 2;

  NodeID node1 = NumericNodeID(id, ns);
  NodeID node2;
  node2 = node1;
  ASSERT_EQ(node1, node2);
  ASSERT_EQ(node1, NodeID(node1));
}

TEST(NodeID, SettingNamespaceUri_And_Index)
{
  uint32_t id = 1;
  uint32_t ns = 2;
  NodeID node = NumericNodeID(id, ns);
  node.SetNamespaceURI("uri");
  node.SetServerIndex(3);

  ASSERT_TRUE(node.HasNamespaceURI());
  ASSERT_TRUE(node.HasServerIndex());
}
