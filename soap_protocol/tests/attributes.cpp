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

#include <gtest/gtest.h>

TEST(Read, Request)
{
  OpcUa::ReadRequest opcua;
  opcua.Header = OpcUa::Test::CreateRequestHeader();
  opcua.Parameters.MaxAge = 1;
  opcua.Parameters.TimestampsType = OpcUa::TimestampsToReturn::BOTH;\

  OpcUa::AttributeValueID attr;
  attr.Attribute = OpcUa::AttributeID::VALUE;\
  attr.DataEncoding.NamespaceIndex = 1;
  attr.DataEncoding.Name = "name";
  attr.IndexRange = "range";
  attr.Node = OpcUa::NumericNodeID(84);
  opcua.Parameters.AttributesToRead.push_back(attr);

  soap service;
  ns3__ReadRequest* req = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(req, nullptr);
  OpcUa::Test::AssertRequestHeaderValid(req->RequestHeader);

  ASSERT_EQ(req->MaxAge, 1);
  ASSERT_EQ(req->TimestampsToReturn, ns3__TimestampsToReturn__Both_USCORE2);
  ASSERT_NE(req->NodesToRead, nullptr);
  ASSERT_EQ(req->NodesToRead->ReadValueId.size(), 1);
  ASSERT_NE(req->NodesToRead->ReadValueId[0], nullptr);
  ns3__ReadValueId* serializedId = req->NodesToRead->ReadValueId[0];
  ASSERT_EQ(serializedId->AttributeId, static_cast<unsigned>(OpcUa::AttributeID::VALUE));
  ASSERT_NE(serializedId->DataEncoding, nullptr);
  ASSERT_NE(serializedId->DataEncoding->Name, nullptr);
  ASSERT_NE(serializedId->DataEncoding->NamespaceIndex, nullptr);
  ASSERT_EQ(*serializedId->DataEncoding->Name, "name");
  ASSERT_EQ(*serializedId->DataEncoding->NamespaceIndex, 1);
  ASSERT_NE(serializedId->IndexRange, nullptr);
  ASSERT_EQ(*serializedId->IndexRange, "range");
  ASSERT_NODE_ID_EQ(serializedId->NodeId, "ns=0;i=84;");


  OpcUa::ReadRequest deserialized = OpcUa::Soap::Deserialize(req);
  OpcUa::Test::AssertRequestHeaderEq(deserialized.Header, opcua.Header);
  ASSERT_EQ(deserialized.Parameters.MaxAge, 1);
  ASSERT_EQ(deserialized.Parameters.TimestampsType, OpcUa::TimestampsToReturn::BOTH);
  ASSERT_EQ(deserialized.Parameters.AttributesToRead.size() ,1);
  const OpcUa::AttributeValueID deserializedValueId = deserialized.Parameters.AttributesToRead[0];
  ASSERT_EQ(deserializedValueId.Attribute, OpcUa::AttributeID::VALUE);
  ASSERT_EQ(deserializedValueId.DataEncoding.Name, "name");
  ASSERT_EQ(deserializedValueId.DataEncoding.NamespaceIndex, 1);
  ASSERT_EQ(deserializedValueId.IndexRange, "range");
  ASSERT_EQ(deserializedValueId.Node, OpcUa::NumericNodeID(84));
}

TEST(Read, Response)
{
  // Fill response
  OpcUa::ReadResponse opcua;
  opcua.Header = OpcUa::Test::CreateResponseHeader();
  opcua.Result.Diagnostics = OpcUa::Test::CreateDiagnosticInfoList();
  OpcUa::DataValue value;
  value.Encoding = OpcUa::DATA_VALUE_ALL;
  value.ServerPicoseconds = 1;
  value.ServerTimestamp = OpcUa::ToDateTime(2);
  value.SourcePicoseconds = 3;
  value.SourceTimestamp = OpcUa::ToDateTime(4);
  value.Value = (uint8_t)5;
  value.Status = OpcUa::StatusCode::BadAttributeIdInvalid;
  opcua.Result.Results.push_back(value);

  // serialize response
  soap service;
  ns3__ReadResponse* soapResponse = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(soapResponse, nullptr);
  OpcUa::Test::AssertResponseHeaderValid(soapResponse->ResponseHeader);
  OpcUa::Test::AssertDiagnosticInfoListValid(soapResponse->DiagnosticInfos);
  ASSERT_NE(soapResponse->Results, nullptr);
  ASSERT_EQ(soapResponse->Results->DataValue.size(), 1);
  const ns3__DataValue* serializedValue = soapResponse->Results->DataValue[0];
  ASSERT_NE(serializedValue, nullptr);
  ASSERT_NE(serializedValue->ServerTimestamp, nullptr);
  ASSERT_NE(serializedValue->SourceTimestamp, nullptr);
  ASSERT_NE(serializedValue->StatusCode, nullptr);
  ASSERT_NE(serializedValue->Value, nullptr);
  EXPECT_EQ(*serializedValue->ServerTimestamp, 2);
  EXPECT_EQ(*serializedValue->SourceTimestamp, 4);
  EXPECT_EQ(*serializedValue->StatusCode->Code, "0x80390000");
  ASSERT_NE(serializedValue->Value, nullptr);
  ASSERT_NE(serializedValue->Value->Byte, nullptr);
  ASSERT_EQ(*serializedValue->Value->Byte, 5);


  // deserialize response
  const OpcUa::ReadResponse deserialized = OpcUa::Soap::Deserialize(soapResponse);
  ASSERT_EQ(deserialized.TypeID, opcua.TypeID);
  ASSERT_RESPONSE_HEADER_EQ(deserialized.Header, opcua.Header);
  OpcUa::Test::AssertDiagnosticInfoListValid(deserialized.Result.Diagnostics);
  ASSERT_EQ(deserialized.Result.Results.size(), 1);
  const OpcUa::ReadResult deserializedResult = deserialized.Result;
  OpcUa::Test::AssertDiagnosticInfoListValid(deserializedResult.Diagnostics);
  ASSERT_EQ(deserializedResult.Results.size(), 1);
  const OpcUa::DataValue deserializedValue = deserializedResult.Results[0];
  const uint8_t mask =
      OpcUa::DATA_VALUE_SERVER_TIMESTAMP |
      OpcUa::DATA_VALUE_SOURCE_TIMESTAMP |
      OpcUa::DATA_VALUE |
      OpcUa::DATA_VALUE_STATUS_CODE;
  ASSERT_EQ(deserializedValue.Encoding, mask);
  ASSERT_EQ(deserializedValue.ServerPicoseconds, 0);
  ASSERT_EQ(deserializedValue.ServerTimestamp, OpcUa::ToDateTime(2));
  ASSERT_EQ(deserializedValue.SourcePicoseconds, 0);
  ASSERT_EQ(deserializedValue.SourceTimestamp, OpcUa::ToDateTime(4));
  ASSERT_EQ(deserializedValue.Status, OpcUa::StatusCode::BadAttributeIdInvalid);
  ASSERT_EQ(deserializedValue.Value, (uint8_t)5);
}




TEST(Write, Request)
{
  OpcUa::WriteRequest opcua;
  opcua.Header = OpcUa::Test::CreateRequestHeader();

  OpcUa::WriteValue value;
  value.Attribute = OpcUa::AttributeID::VALUE;
  value.Data = OpcUa::DataValue(uint32_t(1));
  value.Node = OpcUa::NumericNodeID(1,2);
  value.NumericRange = "1,2";

  opcua.Parameters.NodesToWrite.push_back(value);

  soap service;
  ns3__WriteRequest* req = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(req, nullptr);
  OpcUa::Test::AssertRequestHeaderValid(req->RequestHeader);
  ASSERT_NE(req->NodesToWrite, nullptr);
  ASSERT_EQ(req->NodesToWrite->WriteValue.size(), 1);

  const ns3__WriteValue* serializedValue = req->NodesToWrite->WriteValue[0];
  ASSERT_NE(serializedValue, nullptr);
  ASSERT_EQ(serializedValue->AttributeId, static_cast<unsigned>(OpcUa::AttributeID::VALUE));
  ASSERT_NE(serializedValue->IndexRange, nullptr);
  ASSERT_EQ(*serializedValue->IndexRange, std::string("1,2"));
  ASSERT_NE(serializedValue->NodeId, nullptr);
  ASSERT_NE(serializedValue->NodeId->Identifier, nullptr);
  ASSERT_EQ(*serializedValue->NodeId->Identifier, std::string("ns=2;i=1;"));
  ASSERT_NE(serializedValue->Value, nullptr);
  ASSERT_NE(serializedValue->Value->Value, nullptr);
  ASSERT_NE(serializedValue->Value->Value->UInt32, nullptr);
  ASSERT_EQ(*serializedValue->Value->Value->UInt32, 1);

  OpcUa::WriteRequest deserialized = OpcUa::Soap::Deserialize(req);
  OpcUa::Test::AssertRequestHeaderEq(deserialized.Header, opcua.Header);
  ASSERT_EQ(deserialized.Parameters.NodesToWrite.size(), 1);
  OpcUa::WriteValue deserializedValue = deserialized.Parameters.NodesToWrite[0];
  ASSERT_EQ(deserializedValue.Attribute, OpcUa::AttributeID::VALUE);
  ASSERT_EQ(deserializedValue.Node, OpcUa::NumericNodeID(1,2));
  ASSERT_EQ(deserializedValue.NumericRange, std::string("1,2"));
  ASSERT_TRUE(deserializedValue.Data == OpcUa::DataValue(uint32_t(1)));
}

TEST(Write, Response)
{
  OpcUa::WriteResponse opcua;
  opcua.Header = OpcUa::Test::CreateResponseHeader();
  opcua.Result.Diagnostics = OpcUa::Test::CreateDiagnosticInfoList();
  opcua.Result.StatusCodes = {OpcUa::StatusCode::BadNotReadable};

  soap service;
  ns3__WriteResponse* req = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(req, nullptr);
  OpcUa::Test::AssertResponseHeaderValid(req->ResponseHeader);
  OpcUa::Test::AssertDiagnosticInfoListValid(req->DiagnosticInfos);
  ASSERT_NE(req->Results, nullptr);
  ASSERT_EQ(req->Results->StatusCode.size(), 1);
  ns3__StatusCode* code = req->Results->StatusCode[0];
  ASSERT_NE(code, nullptr);
  ASSERT_EQ(*code->Code, std::string("0x806f0000"));

  OpcUa::WriteResponse deserialized = OpcUa::Soap::Deserialize(req);
  ASSERT_RESPONSE_HEADER_EQ(deserialized.Header, opcua.Header);
  OpcUa::Test::AssertDiagnosticInfoListValid(deserialized.Result.Diagnostics);
  ASSERT_EQ(deserialized.Result.StatusCodes.size(), 1);
  ASSERT_EQ(deserialized.Result.StatusCodes[0], OpcUa::StatusCode::BadNotReadable);
}

