/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/protocol/datetime.h>
#include <opc/ua/protocol/guid.h>
#include <opc/ua/protocol/string_utils.h>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace testing;
using namespace OpcUa;

TEST(DateTime, ToTimeT_And_Back)
{
  time_t timet = time(0);
  const time_t t = ToTimeT(ToDateTime(timet));
  ASSERT_EQ(t, timet);
}

TEST(DateTime, FromTimeT)
{
  time_t t = 1;
  const time_t converted = OpcUa::ToTimeT(OpcUa::ToDateTime(t));
  ASSERT_EQ(t, converted);
}

TEST(DateTime, FromDateTime)
{
  OpcUa::DateTime t = OpcUa::CurrentDateTime();
  const OpcUa::DateTime converted = OpcUa::ToDateTime(OpcUa::ToTimeT(t));
  ASSERT_EQ(t/10000000LL*10000000LL, converted);
}

TEST(DateTime, ZeroToTimeT_ThrowInvalidArgument)
{
  DateTime t(0);
  ASSERT_THROW(OpcUa::ToTimeT(t), std::invalid_argument);
}

TEST(DateTime, ZeroTimeT)
{
  time_t t = 0;
  const DateTime converted = OpcUa::ToDateTime(t);
  ASSERT_EQ(converted/10000000, 11676096000);
}


TEST(Guid, ToString)
{
  OpcUa::Guid guid;
  guid.Data1 = 0x01020304;
  guid.Data2 = 0x0506;
  guid.Data3 = 0x0708;
  guid.Data4[0] = 0x09;
  guid.Data4[1] = 0x0A;
  guid.Data4[2] = 0x0B;
  guid.Data4[3] = 0x0C;
  guid.Data4[4] = 0x0D;
  guid.Data4[5] = 0x0E;
  guid.Data4[6] = 0x0F;
  guid.Data4[7] = 0x10;

  std::string converted = OpcUa::ToString(guid);
  EXPECT_EQ(converted, "01020304-0506-0708-090A0B0C0D0E0F10");
}

TEST(Guid, FromString)
{
  OpcUa::Guid guid;
  guid.Data1 = 0x01020304;
  guid.Data2 = 0x0506;
  guid.Data3 = 0x0708;
  guid.Data4[0] = 0x09;
  guid.Data4[1] = 0x0A;
  guid.Data4[2] = 0x0B;
  guid.Data4[3] = 0x0C;
  guid.Data4[4] = 0x0D;
  guid.Data4[5] = 0x0E;
  guid.Data4[6] = 0x0F;
  guid.Data4[7] = 0x10;

  const OpcUa::Guid converted = OpcUa::ToGuid("01020304-0506-0708-090A0B0C0D0E0F10");
  EXPECT_EQ(converted, guid);
}

TEST(Guid, InvalidString)
{
  EXPECT_EQ(OpcUa::ToGuid("01020304-0506-0708-090A0B0C0D0E0F10S"), OpcUa::Guid()); // 36 symbols
  EXPECT_EQ(OpcUa::ToGuid("0102030400506007080090A0B0C0D0E0F10"), OpcUa::Guid());
}

TEST(NodeID, NumericToString)
{
  OpcUa::NodeID id = OpcUa::NumericNodeID(1,2);
  std::string strId = OpcUa::ToString(id);
  ASSERT_EQ(strId, "ns=2;i=1;");
}

TEST(NodeID, NumericFromString)
{
  OpcUa::NodeID expected = OpcUa::NumericNodeID(1,2);
  OpcUa::NodeID converted = OpcUa::ToNodeID("ns=2;i=1;");
  ASSERT_EQ(expected, converted);
}

TEST(NodeID, StringToString)
{
  OpcUa::NodeID id = OpcUa::StringNodeID("string", 1);
  std::string strId = OpcUa::ToString(id);
  ASSERT_EQ(strId, "ns=1;s=string;");
}

TEST(NodeID, StringFromString)
{
  OpcUa::NodeID expected = OpcUa::StringNodeID("str",2);
  OpcUa::NodeID converted = OpcUa::ToNodeID("ns=2;s=str;");
  ASSERT_EQ(expected, converted);
}

TEST(NodeID, GuidToString)
{
  OpcUa::Guid guid;
  guid.Data1 = 0x01020304;
  guid.Data2 = 0x0506;
  guid.Data3 = 0x0708;
  guid.Data4[0] = 0x09;
  guid.Data4[1] = 0x0A;
  guid.Data4[2] = 0x0B;
  guid.Data4[3] = 0x0C;
  guid.Data4[4] = 0x0D;
  guid.Data4[5] = 0x0E;
  guid.Data4[6] = 0x0F;
  guid.Data4[7] = 0x10;

  OpcUa::NodeID id = OpcUa::GuidNodeID(guid, 1);
  std::string strId = OpcUa::ToString(id);
  ASSERT_EQ(strId, "ns=1;g=01020304-0506-0708-090A0B0C0D0E0F10;");
}

TEST(NodeID, GuidFromString)
{
  OpcUa::Guid guid;
  guid.Data1 = 0x01020304;
  guid.Data2 = 0x0506;
  guid.Data3 = 0x0708;
  guid.Data4[0] = 0x09;
  guid.Data4[1] = 0x0A;
  guid.Data4[2] = 0x0B;
  guid.Data4[3] = 0x0C;
  guid.Data4[4] = 0x0D;
  guid.Data4[5] = 0x0E;
  guid.Data4[6] = 0x0F;
  guid.Data4[7] = 0x10;

  OpcUa::NodeID expected = OpcUa::GuidNodeID(guid,2);
  OpcUa::NodeID converted = OpcUa::ToNodeID("ns=1;g=01020304-0506-0708-090A0B0C0D0E0F10;");
  ASSERT_EQ(expected.Encoding, converted.Encoding);

  OpcUa::Guid expectedGuid = converted.GetGuidIdentifier();
  ASSERT_EQ(guid.Data1, expectedGuid.Data1);
}

TEST(NodeID, NamespaceUriToString)
{
  OpcUa::NodeID id = OpcUa::NumericNodeID(1,2);
  id.SetNamespaceURI("uri");

  std::string strId = OpcUa::ToString(id);
  ASSERT_EQ(strId, "nsu=uri;ns=2;i=1;");
}

TEST(NodeID, NamespaceUriFromString)
{
  OpcUa::NodeID expected = OpcUa::NumericNodeID(1,2);
  expected.SetNamespaceURI("uri");

  OpcUa::NodeID converted = OpcUa::ToNodeID("nsu=uri;ns=2;i=1;");
  ASSERT_EQ(converted, expected);
}

TEST(NodeID, ServerIndexToString)
{
  OpcUa::NodeID id = OpcUa::NumericNodeID(1,2);
  id.SetServerIndex(3);

  std::string strId = OpcUa::ToString(id);
  ASSERT_EQ(strId, "srv=3;ns=2;i=1;");
}

TEST(NodeID, ServerIndexFromString)
{
  OpcUa::NodeID expected = OpcUa::NumericNodeID(1,2);
  expected.SetServerIndex(3);

  OpcUa::NodeID converted = OpcUa::ToNodeID("srv=3;ns=2;i=1;");
  ASSERT_EQ(converted, expected);
}

TEST(NodeID, ServerIndexAndNamespaceUriToString)
{
  OpcUa::NodeID id = OpcUa::NumericNodeID(1,2);
  id.SetServerIndex(3);
  id.SetNamespaceURI("uri");

  std::string strId = OpcUa::ToString(id);
  ASSERT_EQ(strId, "srv=3;nsu=uri;ns=2;i=1;");
}

TEST(NodeID, ServerIndexAndNamespaceUriString)
{
  OpcUa::NodeID expected = OpcUa::NumericNodeID(1,2);
  expected.SetServerIndex(3);
  expected.SetNamespaceURI("uri");

  OpcUa::NodeID converted = OpcUa::ToNodeID("srv=3;nsu=uri;ns=2;i=1;");
  ASSERT_EQ(converted, expected);
}

TEST(NodeID, WithDefaultNamespace)
{
  OpcUa::NodeID expected = OpcUa::NumericNodeID(1, 2);
  OpcUa::NodeID converted = OpcUa::ToNodeID("i=1;", 2);
  ASSERT_EQ(expected, converted);
}
