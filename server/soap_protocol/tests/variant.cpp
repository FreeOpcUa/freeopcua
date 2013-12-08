/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "common.h"


TEST(Variant, bool)
{
  OpcUa::Variant var(true);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Boolean, nullptr);
  ASSERT_EQ(*serialized->Boolean, true);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::BOOLEAN);
  ASSERT_EQ(deserialize.Value, true);
}


TEST(Variant, bool_vector)
{
  std::vector<bool> data{false, true};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfBoolean, nullptr);
  ASSERT_EQ(serialized->ListOfBoolean->Boolean, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::BOOLEAN);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, byte)
{
  uint8_t data = 1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Byte, nullptr);
  ASSERT_EQ(*serialized->Byte, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::BYTE);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, byte_vector)
{
  std::vector<uint8_t> data{1, 2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfByte, nullptr);
  ASSERT_EQ(serialized->ListOfByte->Byte, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::BYTE);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, sbyte)
{
  int8_t data = -1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->SByte, nullptr);
  ASSERT_EQ(*serialized->SByte, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::SBYTE);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, sbyte_vector)
{
  std::vector<int8_t> data{-1, -2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfSByte, nullptr);
  ASSERT_EQ(serialized->ListOfSByte->SByte.size(), 2);
  ASSERT_EQ(serialized->ListOfSByte->SByte[0], -1);
  ASSERT_EQ(serialized->ListOfSByte->SByte[1], -2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::SBYTE);
  ASSERT_EQ(deserialize.Value, data);
}




TEST(Variant, int16)
{
  int16_t data = -1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Int16, nullptr);
  ASSERT_EQ(*serialized->Int16, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::INT16);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, int16_vector)
{
  std::vector<int16_t> data{-1, -2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfInt16, nullptr);
  ASSERT_EQ(serialized->ListOfInt16->Int16.size(), 2);
  ASSERT_EQ(serialized->ListOfInt16->Int16[0], -1);
  ASSERT_EQ(serialized->ListOfInt16->Int16[1], -2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::INT16);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, uint16)
{
  uint16_t data = 1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->UInt16, nullptr);
  ASSERT_EQ(*serialized->UInt16, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::UINT16);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, uint16_vector)
{
  std::vector<uint16_t> data{1, 2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfUInt16, nullptr);
  ASSERT_EQ(serialized->ListOfUInt16->UInt16.size(), 2);
  ASSERT_EQ(serialized->ListOfUInt16->UInt16[0], 1);
  ASSERT_EQ(serialized->ListOfUInt16->UInt16[1], 2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::UINT16);
  ASSERT_EQ(deserialize.Value, data);
}




TEST(Variant, int32)
{
  int32_t data = -1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Int32, nullptr);
  ASSERT_EQ(*serialized->Int32, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::INT32);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, int32_vector)
{
  std::vector<int32_t> data{-1, -2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfInt32, nullptr);
  ASSERT_EQ(serialized->ListOfInt32->Int32.size(), 2);
  ASSERT_EQ(serialized->ListOfInt32->Int32[0], -1);
  ASSERT_EQ(serialized->ListOfInt32->Int32[1], -2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::INT32);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, uint32)
{
  uint32_t data = 1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->UInt32, nullptr);
  ASSERT_EQ(*serialized->UInt32, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::UINT32);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, uint32_vector)
{
  std::vector<uint32_t> data{1, 2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfUInt32, nullptr);
  ASSERT_EQ(serialized->ListOfUInt32->UInt32.size(), 2);
  ASSERT_EQ(serialized->ListOfUInt32->UInt32[0], 1);
  ASSERT_EQ(serialized->ListOfUInt32->UInt32[1], 2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::UINT32);
  ASSERT_EQ(deserialize.Value, data);
}





TEST(Variant, int64)
{
  int64_t data = -1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Int64, nullptr);
  ASSERT_EQ(*serialized->Int64, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::INT64);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, int64_vector)
{
  std::vector<int64_t> data{-1, -2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfInt64, nullptr);
  ASSERT_EQ(serialized->ListOfInt64->Int64.size(), 2);
  ASSERT_EQ(serialized->ListOfInt64->Int64[0], -1);
  ASSERT_EQ(serialized->ListOfInt64->Int64[1], -2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::INT64);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, uint64)
{
  uint64_t data = 1;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->UInt64, nullptr);
  ASSERT_EQ(*serialized->UInt64, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::UINT64);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, uint64_vector)
{
  std::vector<uint64_t> data{1, 2};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfUInt64, nullptr);
  ASSERT_EQ(serialized->ListOfUInt64->UInt64.size(), 2);
  ASSERT_EQ(serialized->ListOfUInt64->UInt64[0], 1);
  ASSERT_EQ(serialized->ListOfUInt64->UInt64[1], 2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::UINT64);
  ASSERT_EQ(deserialize.Value, data);
}





TEST(Variant, float)
{
  float data = 1.2;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Float, nullptr);
  ASSERT_EQ(*serialized->Float, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::FLOAT);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, float_vector)
{
  std::vector<float> data{1.2, 2.1};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfFloat, nullptr);
  ASSERT_EQ(serialized->ListOfFloat->Float.size(), 2);
  ASSERT_EQ(serialized->ListOfFloat->Float[0], 1.2f);
  ASSERT_EQ(serialized->ListOfFloat->Float[1], 2.1f);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::FLOAT);
  ASSERT_EQ(deserialize.Value, data);
}






TEST(Variant, double)
{
  double data = 1.2;

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Double, nullptr);
  ASSERT_EQ(*serialized->Double, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DOUBLE);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, double_vector)
{
  std::vector<double> data{1.2, 2.1};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfDouble, nullptr);
  ASSERT_EQ(serialized->ListOfDouble->Double.size(), 2);
  ASSERT_EQ(serialized->ListOfDouble->Double[0], 1.2);
  ASSERT_EQ(serialized->ListOfDouble->Double[1], 2.1);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DOUBLE);
  ASSERT_EQ(deserialize.Value, data);
}






TEST(Variant, string)
{
  std::string data = "hello";

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->String, nullptr);
  ASSERT_EQ(*serialized->String, data);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::STRING);
  ASSERT_EQ(deserialize.Value, data);
}


TEST(Variant, string_vector)
{
  std::vector<std::string> data{"1", "2"};

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfString, nullptr);
  ASSERT_EQ(serialized->ListOfString->String.size(), 2);
  ASSERT_EQ(serialized->ListOfString->String[0], "1");
  ASSERT_EQ(serialized->ListOfString->String[1], "2");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::STRING);
  ASSERT_EQ(deserialize.Value, data);
}







TEST(Variant, time)
{
  OpcUa::DateTime data = OpcUa::CurrentDateTime();

  OpcUa::Variant var(data);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->DateTime, nullptr);
  ASSERT_EQ(*serialized->DateTime, OpcUa::ToTimeT(data));

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DATE_TIME);
  ASSERT_EQ(OpcUa::ToTimeT(deserialize.Value.Time[0]), OpcUa::ToTimeT(data));
}


TEST(Variant, time_vector)
{
  std::vector<OpcUa::DateTime> data{OpcUa::CurrentDateTime(), OpcUa::CurrentDateTime()};

  OpcUa::Variant var(data);
  ASSERT_EQ(var.Type, OpcUa::VariantType::DATE_TIME);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfDateTime, nullptr);
  ASSERT_EQ(serialized->ListOfDateTime->DateTime.size(), 2);
  ASSERT_EQ(serialized->ListOfDateTime->DateTime[0], OpcUa::ToTimeT(data[0]));
  ASSERT_EQ(serialized->ListOfDateTime->DateTime[1], OpcUa::ToTimeT(data[1]));

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DATE_TIME);
  ASSERT_EQ(deserialize.Value.Time.size(), 2);
  std::vector<OpcUa::DateTime> deserialized = deserialize.Value.Time;
  ASSERT_EQ(OpcUa::ToTimeT(deserialized[0]), OpcUa::ToTimeT(data[0]));
  ASSERT_EQ(OpcUa::ToTimeT(deserialized[1]), OpcUa::ToTimeT(data[1]));
}




TEST(Variant, guid)
{
  OpcUa::Guid guid;
  guid.Data1 = 1;
  guid.Data2 = 2;
  guid.Data3 = 3;
  memset(guid.Data4, 1, 8);

  OpcUa::Variant var(guid);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->Guid, nullptr);
  ASSERT_NE(serialized->Guid->String, nullptr);
  ASSERT_EQ(*serialized->Guid->String, OpcUa::ToString(guid));
  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::GUID);
  ASSERT_EQ(deserialize.Value.Guids.size(), 1);
  ASSERT_EQ(deserialize.Value.Guids[0], guid);
}

TEST(Variant, guid_vector)
{
  OpcUa::Guid guid;
  guid.Data1 = 1;
  guid.Data2 = 2;
  guid.Data3 = 3;
  memset(guid.Data4, 1, 8);

  std::vector<OpcUa::Guid> data{guid, guid};

  OpcUa::Variant var(data);
  ASSERT_EQ(var.Type, OpcUa::VariantType::GUID);

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfGuid, nullptr);
  ASSERT_EQ(serialized->ListOfGuid->Guid.size(), 2);
  ASSERT_NE(serialized->ListOfGuid->Guid[0]->String, nullptr);
  ASSERT_NE(serialized->ListOfGuid->Guid[1]->String, nullptr);

  ASSERT_EQ(*serialized->ListOfGuid->Guid[0]->String, OpcUa::ToString(guid));
  ASSERT_EQ(*serialized->ListOfGuid->Guid[1]->String, OpcUa::ToString(guid));

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::GUID);
  ASSERT_EQ(deserialize.Value.Guids.size(), 2);
  std::vector<OpcUa::Guid> deserialized = deserialize.Value.Guids;
  ASSERT_EQ(deserialized[0], guid);
  ASSERT_EQ(deserialized[1], guid);
}





TEST(Variant, ByteString)
{
  OpcUa::ByteString byteString(std::vector<uint8_t>{1, 2});

  OpcUa::Variant var(byteString);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ByteString, nullptr);
  ASSERT_NE(serialized->ByteString->__ptr, nullptr);
  ASSERT_EQ(serialized->ByteString->__size, 2);
  ASSERT_EQ(serialized->ByteString->__ptr[0], byteString.Data[0]);
  ASSERT_EQ(serialized->ByteString->__ptr[1], byteString.Data[1]);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::BYTE_STRING);
  ASSERT_EQ(deserialize.Value.ByteStrings.size(), 1);
  ASSERT_EQ(deserialize.Value.ByteStrings[0], byteString);
}


TEST(Variant, ByteString_vector)
{
  OpcUa::ByteString byteString(std::vector<uint8_t>{1, 2});
  OpcUa::Variant var(std::vector<OpcUa::ByteString>{byteString, byteString});

  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfByteString, nullptr);
  ASSERT_EQ(serialized->ListOfByteString->ByteString.size(), 2);
  ASSERT_EQ(serialized->ListOfByteString->ByteString[0].__size, 2);
  ASSERT_EQ(serialized->ListOfByteString->ByteString[0].__ptr[0], byteString.Data[0]);
  ASSERT_EQ(serialized->ListOfByteString->ByteString[0].__ptr[1], byteString.Data[1]);
  ASSERT_EQ(serialized->ListOfByteString->ByteString[1].__ptr[0], byteString.Data[0]);
  ASSERT_EQ(serialized->ListOfByteString->ByteString[1].__ptr[1], byteString.Data[1]);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::BYTE_STRING);
  ASSERT_EQ(deserialize.Value.ByteStrings.size(), 2);
  ASSERT_EQ(deserialize.Value.ByteStrings[0], byteString);
  ASSERT_EQ(deserialize.Value.ByteStrings[1], byteString);
}




TEST(Variant, NodeId)
{
  OpcUa::NodeID id = OpcUa::NumericNodeID(1, 1);
  id.SetNamespaceURI("uri");
  id.SetServerIndex(1);

  OpcUa::Variant var(id);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ExpandedNodeId, nullptr);
  ASSERT_NE(serialized->ExpandedNodeId->Identifier, nullptr);
  ASSERT_EQ(*serialized->ExpandedNodeId->Identifier, "srv=1;nsu=uri;ns=1;i=1;");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::NODE_ID);
  ASSERT_EQ(deserialize.Value.Node.size(), 1);
  ASSERT_EQ(deserialize.Value.Node[0], id);
}

TEST(Variant, NodeId_vector)
{
  OpcUa::NodeID id = OpcUa::NumericNodeID(1, 1);
  id.SetNamespaceURI("uri");
  id.SetServerIndex(1);

  OpcUa::Variant var(std::vector<OpcUa::NodeID>{id, id});
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfExpandedNodeId, nullptr);
  ASSERT_EQ(serialized->ListOfExpandedNodeId->ExpandedNodeId.size(), 2);
  ASSERT_NE(serialized->ListOfExpandedNodeId->ExpandedNodeId[0], nullptr);
  ASSERT_NE(serialized->ListOfExpandedNodeId->ExpandedNodeId[1], nullptr);
  ASSERT_NE(serialized->ListOfExpandedNodeId->ExpandedNodeId[0]->Identifier, nullptr);
  ASSERT_NE(serialized->ListOfExpandedNodeId->ExpandedNodeId[1]->Identifier, nullptr);
  ASSERT_EQ(*serialized->ListOfExpandedNodeId->ExpandedNodeId[0]->Identifier, "srv=1;nsu=uri;ns=1;i=1;");
  ASSERT_EQ(*serialized->ListOfExpandedNodeId->ExpandedNodeId[1]->Identifier, "srv=1;nsu=uri;ns=1;i=1;");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::NODE_ID);
  ASSERT_EQ(deserialize.Value.Node.size(), 2);
  ASSERT_EQ(deserialize.Value.Node[0], id);
  ASSERT_EQ(deserialize.Value.Node[1], id);
}






TEST(Variant, StatusCode)
{
  OpcUa::StatusCode code = OpcUa::StatusCode::BadNotImplemented;

  OpcUa::Variant var(code);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->StatusCode, nullptr);
  ASSERT_NE(serialized->StatusCode->Code, nullptr);
  ASSERT_EQ(*serialized->StatusCode->Code, "0x80440000");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::STATUS_CODE);
  ASSERT_EQ(deserialize.Value.Statuses.size(), 1);
  ASSERT_EQ(deserialize.Value.Statuses[0], OpcUa::StatusCode::BadNotImplemented);
}

TEST(Variant, StatusCode_vector)
{
  std::vector<OpcUa::StatusCode> codes = {OpcUa::StatusCode::BadNotImplemented, OpcUa::StatusCode::BadAttributeIdInvalid};

  OpcUa::Variant var(codes);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfStatusCode, nullptr);
  ASSERT_EQ(serialized->ListOfStatusCode->StatusCode.size(), 2);
  ASSERT_NE(serialized->ListOfStatusCode->StatusCode[0], nullptr);
  ASSERT_NE(serialized->ListOfStatusCode->StatusCode[0]->Code, nullptr);
  ASSERT_NE(serialized->ListOfStatusCode->StatusCode[1], nullptr);
  ASSERT_NE(serialized->ListOfStatusCode->StatusCode[1]->Code, nullptr);
  ASSERT_EQ(*serialized->ListOfStatusCode->StatusCode[0]->Code, "0x80440000");
  ASSERT_EQ(*serialized->ListOfStatusCode->StatusCode[1]->Code, "0x80390000");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::STATUS_CODE);
  ASSERT_EQ(deserialize.Value.Statuses.size(), 2);
  ASSERT_EQ(deserialize.Value.Statuses[0], OpcUa::StatusCode::BadNotImplemented);
  ASSERT_EQ(deserialize.Value.Statuses[1], OpcUa::StatusCode::BadAttributeIdInvalid);
}






TEST(Variant, DiagnosticInfo)
{
  OpcUa::DiagnosticInfo diag = OpcUa::Test::CreateDiagnosticInfo();

  OpcUa::Variant var(diag);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  OpcUa::Test::AssertDiagnosticInfoValid(serialized->DiagnosticInfo);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DIAGNOSTIC_INFO);
  ASSERT_EQ(deserialize.Value.Diagnostic.size(), 1);
  OpcUa::Test::AssertDiagnosticInfoValid(deserialize.Value.Diagnostic[0]);
}

TEST(Variant, DiagnosticInfo_vector)
{
  OpcUa::Variant var(OpcUa::Test::CreateDiagnosticInfoList());
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  OpcUa::Test::AssertDiagnosticInfoListValid(serialized->ListOfDiagnosticInfo);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DIAGNOSTIC_INFO);
  OpcUa::Test::AssertDiagnosticInfoListValid(deserialize.Value.Diagnostic);
}







TEST(Variant, QualifiedName)
{
  OpcUa::QualifiedName name(1, "name");

  OpcUa::Variant var(name);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->QualifiedName, nullptr);
  ASSERT_NE(serialized->QualifiedName->Name, nullptr);
  ASSERT_NE(serialized->QualifiedName->NamespaceIndex, nullptr);
  ASSERT_EQ(*serialized->QualifiedName->Name, "name");
  ASSERT_EQ(*serialized->QualifiedName->NamespaceIndex, 1);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::QUALIFIED_NAME);
  ASSERT_EQ(deserialize.Value.Name.size(), 1);
  ASSERT_EQ(deserialize.Value.Name[0].Name, "name");
  ASSERT_EQ(deserialize.Value.Name[0].NamespaceIndex, 1);
}
TEST(Variant, QualifiedName_vector)
{
  std::vector<OpcUa::QualifiedName> names = { OpcUa::QualifiedName(1, "name1"), OpcUa::QualifiedName(2, "name2") };

  OpcUa::Variant var(names);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfQualifiedName, nullptr);
  ASSERT_EQ(serialized->ListOfQualifiedName->QualifiedName.size(), 2);
  ASSERT_NE(serialized->ListOfQualifiedName->QualifiedName[0], nullptr);
  ASSERT_NE(serialized->ListOfQualifiedName->QualifiedName[1], nullptr);
  ASSERT_NE(serialized->ListOfQualifiedName->QualifiedName[0]->Name, nullptr);
  ASSERT_NE(serialized->ListOfQualifiedName->QualifiedName[0]->NamespaceIndex, nullptr);
  ASSERT_EQ(*serialized->ListOfQualifiedName->QualifiedName[0]->Name, "name1");
  ASSERT_EQ(*serialized->ListOfQualifiedName->QualifiedName[0]->NamespaceIndex, 1);
  ASSERT_NE(serialized->ListOfQualifiedName->QualifiedName[1]->Name, nullptr);
  ASSERT_NE(serialized->ListOfQualifiedName->QualifiedName[1]->NamespaceIndex, nullptr);
  ASSERT_EQ(*serialized->ListOfQualifiedName->QualifiedName[1]->Name, "name2");
  ASSERT_EQ(*serialized->ListOfQualifiedName->QualifiedName[1]->NamespaceIndex, 2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::QUALIFIED_NAME);
  ASSERT_EQ(deserialize.Value.Name.size(), 2);
  ASSERT_EQ(deserialize.Value.Name[0].Name, "name1");
  ASSERT_EQ(deserialize.Value.Name[0].NamespaceIndex, 1);
  ASSERT_EQ(deserialize.Value.Name[1].Name, "name2");
  ASSERT_EQ(deserialize.Value.Name[1].NamespaceIndex, 2);
}

TEST(Variant, LocalizedText)
{
  OpcUa::LocalizedText text("text", "en");
  OpcUa::Variant var(text);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->LocalizedText, nullptr);
  ASSERT_NE(serialized->LocalizedText->Locale, nullptr);
  ASSERT_NE(serialized->LocalizedText->Text, nullptr);
  ASSERT_EQ(*serialized->LocalizedText->Locale, "en");
  ASSERT_EQ(*serialized->LocalizedText->Text, "text");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(deserialize.Value.Text.size(), 1);
  ASSERT_EQ(deserialize.Value.Text[0].Locale, "en");
  ASSERT_EQ(deserialize.Value.Text[0].Text, "text");
}

TEST(Variant, LocalizedText_vector)
{
  std::vector<OpcUa::LocalizedText> texts = {OpcUa::LocalizedText("text", "en"), OpcUa::LocalizedText("text1", "en") };
  OpcUa::Variant var(texts);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);

  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfLocalizedText, nullptr);
  ASSERT_EQ(serialized->ListOfLocalizedText->LocalizedText.size(), 2);
  ASSERT_NE(serialized->ListOfLocalizedText->LocalizedText[0], nullptr);
  ASSERT_NE(serialized->ListOfLocalizedText->LocalizedText[0]->Locale, nullptr);
  ASSERT_NE(serialized->ListOfLocalizedText->LocalizedText[0]->Text, nullptr);
  ASSERT_NE(serialized->ListOfLocalizedText->LocalizedText[1], nullptr);
  ASSERT_NE(serialized->ListOfLocalizedText->LocalizedText[1]->Locale, nullptr);
  ASSERT_NE(serialized->ListOfLocalizedText->LocalizedText[1]->Text, nullptr);

  ASSERT_EQ(*serialized->ListOfLocalizedText->LocalizedText[0]->Locale, "en");
  ASSERT_EQ(*serialized->ListOfLocalizedText->LocalizedText[0]->Text, "text");
  ASSERT_EQ(*serialized->ListOfLocalizedText->LocalizedText[1]->Locale, "en");


  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(deserialize.Value.Text.size(), 2);
  ASSERT_EQ(deserialize.Value.Text[0].Locale, "en");
  ASSERT_EQ(deserialize.Value.Text[0].Text, "text");
  ASSERT_EQ(deserialize.Value.Text[1].Locale, "en");
  ASSERT_EQ(deserialize.Value.Text[1].Text, "text1");
}







TEST(Variant, DataValue)
{
  OpcUa::LocalizedText text("text", "en");
  OpcUa::DataValue value;
  value = OpcUa::Variant(text);

  OpcUa::Variant var(value);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->DataValue, nullptr);
  ASSERT_EQ(serialized->DataValue->ServerTimestamp, nullptr);
  ASSERT_EQ(serialized->DataValue->SourceTimestamp, nullptr);
  ASSERT_EQ(serialized->DataValue->StatusCode, nullptr);
  ASSERT_NE(serialized->DataValue->Value, nullptr);
  ASSERT_NE(serialized->DataValue->Value->LocalizedText, nullptr);
  ASSERT_NE(serialized->DataValue->Value->LocalizedText->Locale, nullptr);
  ASSERT_NE(serialized->DataValue->Value->LocalizedText->Text, nullptr);
  ASSERT_EQ(*serialized->DataValue->Value->LocalizedText->Locale, "en");
  ASSERT_EQ(*serialized->DataValue->Value->LocalizedText->Text, "text");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DATA_VALUE);
  ASSERT_EQ(deserialize.Value.Value.size(), 1);
  const OpcUa::DataValue& deserializedValue = deserialize.Value.Value[0];
  ASSERT_TRUE(deserializedValue.Encoding & OpcUa::DATA_VALUE);
  ASSERT_EQ(deserializedValue.Value.Type, OpcUa::VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(deserializedValue.Value.Value.Text[0].Text, "text");
  ASSERT_EQ(deserializedValue.Value.Value.Text[0].Locale, "en");
}

TEST(Variant, DataValue_vector)
{
  OpcUa::LocalizedText text("text", "en");
  std::vector<OpcUa::DataValue> value;
  value.push_back(OpcUa::DataValue(OpcUa::Variant(text)));
  value.push_back(OpcUa::DataValue(OpcUa::Variant(text)));

  OpcUa::Variant var(value);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfDataValue, nullptr);
  ASSERT_EQ(serialized->ListOfDataValue->DataValue.size(), 2);
  const ns3__DataValue* val1 = serialized->ListOfDataValue->DataValue[0];
  ASSERT_EQ(val1->ServerTimestamp, nullptr);
  ASSERT_EQ(val1->SourceTimestamp, nullptr);
  ASSERT_EQ(val1->StatusCode, nullptr);
  ASSERT_NE(val1->Value, nullptr);
  ASSERT_NE(val1->Value->LocalizedText, nullptr);
  ASSERT_NE(val1->Value->LocalizedText->Locale, nullptr);
  ASSERT_NE(val1->Value->LocalizedText->Text, nullptr);
  ASSERT_EQ(*val1->Value->LocalizedText->Locale, "en");
  ASSERT_EQ(*val1->Value->LocalizedText->Text, "text");

  const ns3__DataValue* val2 = serialized->ListOfDataValue->DataValue[1];
  ASSERT_EQ(val2->ServerTimestamp, nullptr);
  ASSERT_EQ(val2->SourceTimestamp, nullptr);
  ASSERT_EQ(val2->StatusCode, nullptr);
  ASSERT_NE(val2->Value, nullptr);
  ASSERT_NE(val2->Value->LocalizedText, nullptr);
  ASSERT_NE(val2->Value->LocalizedText->Locale, nullptr);
  ASSERT_NE(val2->Value->LocalizedText->Text, nullptr);
  ASSERT_EQ(*val2->Value->LocalizedText->Locale, "en");
  ASSERT_EQ(*val2->Value->LocalizedText->Text, "text");

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::DATA_VALUE);
  ASSERT_EQ(deserialize.Value.Value.size(), 2);
  const OpcUa::DataValue& deserializedValue1 = deserialize.Value.Value[0];
  ASSERT_TRUE(deserializedValue1.Encoding & OpcUa::DATA_VALUE);
  ASSERT_EQ(deserializedValue1.Value.Type, OpcUa::VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(deserializedValue1.Value.Value.Text[0].Text, "text");
  ASSERT_EQ(deserializedValue1.Value.Value.Text[0].Locale, "en");

  const OpcUa::DataValue& deserializedValue2 = deserialize.Value.Value[1];
  ASSERT_TRUE(deserializedValue2.Encoding & OpcUa::DATA_VALUE);
  ASSERT_EQ(deserializedValue2.Value.Type, OpcUa::VariantType::LOCALIZED_TEXT);
  ASSERT_EQ(deserializedValue2.Value.Value.Text[0].Text, "text");
  ASSERT_EQ(deserializedValue2.Value.Value.Text[0].Locale, "en");
}





TEST(Variant, Variant_vector)
{
  std::vector<OpcUa::Variant> value;
  value.push_back(OpcUa::Variant(uint8_t(1)));
  value.push_back(OpcUa::Variant(uint8_t(2)));

  OpcUa::Variant var(value);
  soap s;
  ns3__Variant* serialized = OpcUa::Soap::Serialize(&s, var);
  ASSERT_NE(serialized, nullptr);
  ASSERT_NE(serialized->ListOfVariant, nullptr);
  ASSERT_EQ(serialized->ListOfVariant->Variant.size(), 2);
  const ns3__Variant* val1 = serialized->ListOfVariant->Variant[0];
  ASSERT_NE(val1->Byte, nullptr);
  ASSERT_EQ(*val1->Byte, 1);

  const ns3__Variant* val2 = serialized->ListOfVariant->Variant[1];
  ASSERT_NE(val2->Byte, nullptr);
  ASSERT_EQ(*val2->Byte, 2);

  OpcUa::Variant deserialize = OpcUa::Soap::Deserialize(serialized);
  ASSERT_EQ(deserialize.Type, OpcUa::VariantType::VARIANT);
  ASSERT_EQ(deserialize.Value.Variants.size(), 2);
  const OpcUa::Variant& deserializedValue1 = deserialize.Value.Variants[0];
}

