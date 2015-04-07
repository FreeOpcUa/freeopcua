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
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/protocol.h>

#include <algorithm>
#include <stdexcept>


//-------------------------------------------------------
// AttributeId
// //TODO Add check for ivalid attributeId deserialization. It must throw.
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, AttributeId)
{

  using namespace OpcUa::Binary;
  using OpcUa::AttributeId;

  GetStream() << AttributeId::Value << flush;

  const std::vector<char> expectedData = {
  13,0,0,0
  };

  ASSERT_EQ(expectedData.size(), RawSize(AttributeId::Value));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, AttributeId)
{
  using namespace OpcUa::Binary;
  using OpcUa::AttributeId;

  const std::vector<char> expectedData = {
  13,0,0,0
  };

  GetChannel().SetData(expectedData);

  AttributeId id;
  GetStream() >> id;

  ASSERT_EQ(id, AttributeId::Value);
}


//-------------------------------------------------------
// TimestampsToReturn
// TODO Add test for invalid Timestamps to return deserialization.
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, TimestampsToReturn)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  GetStream() << TimestampsToReturn::Neither << flush;

  const std::vector<char> expectedData = {
  3,0,0,0
  };

  ASSERT_EQ(expectedData.size(), RawSize(TimestampsToReturn::Neither));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, TimetampsToReturn)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  3,0,0,0
  };

  GetChannel().SetData(expectedData);

  TimestampsToReturn stamps;
  GetStream() >> stamps;

  ASSERT_EQ(stamps, TimestampsToReturn::Neither);
}

//-------------------------------------------------------
// ReadValueId
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ReadValueId)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using OpcUa::AttributeId;

  ReadValueId attr;

  attr.NodeId.Encoding = EV_TWO_BYTE;
  attr.NodeId.TwoByteData.Identifier = 1;
  attr.AttributeId = AttributeId::Value;
  attr.IndexRange = "1,2";
  attr.DataEncoding.NamespaceIndex = 2;
  attr.DataEncoding.Name = "test";

  GetStream() << attr << flush;

  const std::vector<char> expectedData = {
  0, 1,
  13,0,0,0,
  3,0,0,0, '1',',','2',
  2,0,
  4,0,0,0, 't','e','s','t'
  };

  ASSERT_EQ(expectedData.size(), RawSize(attr));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, ReadValueId)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using OpcUa::AttributeId;

  const std::vector<char> expectedData = {
  0, 1,
  13,0,0,0,
  3,0,0,0, '1',',','2',
  2,0,
  4,0,0,0, 't','e','s','t'
  };

  GetChannel().SetData(expectedData);

  ReadValueId attr;
  GetStream() >> attr;
  
  ASSERT_EQ(attr.NodeId.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(attr.NodeId.TwoByteData.Identifier, 1);
  ASSERT_EQ(attr.AttributeId, AttributeId::Value);
  ASSERT_EQ(attr.DataEncoding.NamespaceIndex, 2);
  ASSERT_EQ(attr.DataEncoding.Name, "test");
}

//-------------------------------------------------------
// ReadRequest
//-------------------------------------------------------

OpcUa::ReadValueId CreateReadValueId()
{
  using namespace OpcUa;
  OpcUa::ReadValueId attr;

  attr.NodeId.Encoding = OpcUa::EV_TWO_BYTE;
  attr.NodeId.TwoByteData.Identifier = 1;
  attr.AttributeId = OpcUa::AttributeId::Value;
  attr.IndexRange = "1,2";
  attr.DataEncoding.NamespaceIndex = 2;
  attr.DataEncoding.Name = "test";

  return attr;
}

TEST_F(OpcUaBinarySerialization, ReadRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ReadRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::READ_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.Parameters.MaxAge = 1200000;
  request.Parameters.TimestampsToReturn = TimestampsToReturn::Neither;

  request.Parameters.AttributesToRead.push_back(CreateReadValueId());

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x77, 0x2, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
  3,0,0,0,

  1,0,0,0,
  0, 1,
  13,0,0,0,
  3,0,0,0, '1',',','2',
  2,0,
  4,0,0,0, 't','e','s','t'

  };

  ASSERT_EQ(expectedData.size(), RawSize(request));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, ReadRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0x77, 0x2, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
  3,0,0,0,

  1,0,0,0,
  0, 1,
  13,0,0,0,
  3,0,0,0, '1',',','2',
  2,0,
  4,0,0,0, 't','e','s','t'

  };

  GetChannel().SetData(expectedData);
  ReadRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::READ_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.MaxAge, 1200000);
  ASSERT_EQ(request.Parameters.TimestampsToReturn, TimestampsToReturn::Neither);
  
  ASSERT_EQ(request.Parameters.AttributesToRead.size(), 1);

  ReadValueId attr = CreateReadValueId();

  ASSERT_EQ(request.Parameters.AttributesToRead[0].NodeId.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(request.Parameters.AttributesToRead[0].NodeId.TwoByteData.Identifier, 1);
  ASSERT_EQ(request.Parameters.AttributesToRead[0].AttributeId, OpcUa::AttributeId::Value);
  ASSERT_EQ(request.Parameters.AttributesToRead[0].DataEncoding.NamespaceIndex, 2);
  ASSERT_EQ(request.Parameters.AttributesToRead[0].DataEncoding.Name, "test");

}

//-------------------------------------------------------
// ReadResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ReadResponse)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ReadResponse resp;

  ASSERT_EQ(resp.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeId.FourByteData.Identifier, OpcUa::READ_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(resp.Header);

  resp.Results.push_back(OpcUa::DataValue(QualifiedName(1, OpcUa::Names::Root)));

  ASSERT_NO_THROW(GetStream() << resp << flush);

  char encodingMask = static_cast<char>(OpcUa::DATA_VALUE);
  char variantMask = static_cast<char>(OpcUa::VariantType::QUALIFIED_NAME);
  const std::vector<char> expectedData = {
  1, 0, (char)0x7A, 0x2, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,0,0,0,
  encodingMask,
  variantMask,
  1, 0,
  4, 0, 0, 0, 'R','o','o','t',
  (char)0xFF,(char)0xFF,(char)0xFF,(char)0xFF
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(resp));
}

TEST_F(OpcUaBinaryDeserialization, ReadResponse_with_QualifiedName_as_value)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char variantMask = static_cast<uint8_t>(VariantType::QUALIFIED_NAME);
  char encodingMask = DATA_VALUE;

  const std::vector<char> expectedData = {
      1, 0, (char)0x7A, 0x2, // TypeId
      // RequestHeader
      TEST_RESPONSE_HEADER_BINARY_DATA,
      1,0,0,0,
      encodingMask,
      variantMask,
      1, 0,
      4, 0, 0, 0, 'R','o','o','t',
      0,0,0,0
  };

  GetChannel().SetData(expectedData);
  ReadResponse resp;
  GetStream() >> resp;

  ASSERT_EQ(resp.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeId.FourByteData.Identifier, OpcUa::READ_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(resp.Header);
  ASSERT_EQ(resp.Results.size(), 1);
}


TEST_F(OpcUaBinaryDeserialization, ReadResponse)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  char variantMask = static_cast<uint8_t>(VariantType::BOOLEAN);
  char encodingMask = 
     DATA_VALUE |
     DATA_VALUE_STATUS_CODE |
     DATA_VALUE_SOURCE_TIMESTAMP |
     DATA_VALUE_Server_TIMESTAMP |
     DATA_VALUE_SOURCE_PICOSECONDS |
     DATA_VALUE_Server_PICOSECONDS;

  const std::vector<char> expectedData = {
  1, 0, (char)0x7A, 0x2, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // Results
  1,0,0,0,

  encodingMask,
  variantMask, 1,
  1,0,0,0,
  2,0,0,0,0,0,0,0,
  3,0,
  4,0,0,0,0,0,0,0,
  5,0,

  0,0,0,0
  };

  GetChannel().SetData(expectedData);
  ReadResponse resp;
  GetStream() >> resp;

  ASSERT_EQ(resp.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeId.FourByteData.Identifier, OpcUa::READ_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(resp.Header);
  ASSERT_EQ(resp.Results.size(), 1);
}

//-------------------------------------------------------
// WriteValue
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, WriteValue)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  WriteValue value;
  value.NodeId.Encoding = EV_FOUR_BYTE;
  value.NodeId.FourByteData.Identifier = 1;
  value.AttributeId = AttributeId::DisplayName;
  value.Value.Encoding = DATA_VALUE;
  value.Value.Value = true;
 
  GetStream() << value << flush;

  const std::vector<char> expectedData = {
  1,0,1,0, // Node
  4,0,0,0, // AttributeId
  -1,-1,-1,-1, // NumericRange  
  1, 1, 1 // NodesToWrite
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(value));
}

TEST_F(OpcUaBinaryDeserialization, WriteValue)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1,0,1,0, // Node
  4,0,0,0, // AttributeId
  -1,-1,-1,-1, // NumericRange  
  1, 1, 1 // NodesToWrite
  };


  GetChannel().SetData(expectedData);
  WriteValue value;
  GetStream() >> value;

  ASSERT_EQ(value.NodeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(value.NodeId.FourByteData.Identifier, 1);
  ASSERT_EQ(value.AttributeId, AttributeId::DisplayName);
  ASSERT_EQ(value.Value.Encoding, DATA_VALUE);
  ASSERT_EQ(value.Value.Value.Type(), VariantType::BOOLEAN);
  ASSERT_EQ(value.Value.Value.As<bool>(), true);
}


//-------------------------------------------------------
// WriteRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, WriteRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  WriteRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::WRITE_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  WriteValue value;
  value.NodeId.Encoding = EV_FOUR_BYTE;
  value.NodeId.FourByteData.Identifier = 1;
  value.AttributeId = AttributeId::DisplayName;
  value.Value.Encoding = DATA_VALUE;
  value.Value.Value = true;

  request.Parameters.NodesToWrite.push_back(value);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xA1, 0x2, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1,0,0,0,

  1,0,1,0, // Node
  4,0,0,0, // AttributeId
  -1,-1,-1,-1, // NumericRange  
  1, 1, 1 // NodesToWrite
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, WriteRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xA1, 0x2, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1,0,0,0,
  1,0,1,0, // Node
  4,0,0,0, // AttributeId
  -1,-1,-1,-1, // NumericRange  
  1, 1, 1 // Value
  };



  GetChannel().SetData(expectedData);
  WriteRequest request;
  GetStream() >> request;


  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::WRITE_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);


  ASSERT_EQ(request.Parameters.NodesToWrite.size(), 1);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].NodeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].NodeId.FourByteData.Identifier, 1);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].AttributeId, AttributeId::DisplayName);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Value.Encoding, DATA_VALUE);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Value.Value.Type(), VariantType::BOOLEAN);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Value.Value.As<bool>(), true);
}

//-------------------------------------------------------
// WriteResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, WriteResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  WriteResponse resp;

  ASSERT_EQ(resp.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeId.FourByteData.Identifier, OpcUa::WRITE_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(resp.Header);

  resp.Results.push_back(static_cast<StatusCode>(1));
 
  DiagnosticInfo info;
  info.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT);
  info.LocalizedText = 4;
  resp.DiagnosticInfos.push_back(info);
 
  GetStream() << resp << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xA4, 0x2, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  1,0,0,0, 1,0,0,0,
  1,0,0,0, static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT), 4,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(resp));
}

TEST_F(OpcUaBinaryDeserialization, WriteResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xA4, 0x2, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  1,0,0,0, 1,0,0,0,
  1,0,0,0, static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT), 4,0,0,0
  };

  GetChannel().SetData(expectedData);
  WriteResponse resp;
  GetStream() >> resp;


  ASSERT_EQ(resp.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeId.FourByteData.Identifier, OpcUa::WRITE_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(resp.Header);

  ASSERT_EQ(resp.Results.size(), 1);
  ASSERT_EQ(resp.Results[0], static_cast<OpcUa::StatusCode>(1));

  ASSERT_EQ(resp.DiagnosticInfos.size(), 1);
  ASSERT_EQ(resp.DiagnosticInfos[0].EncodingMask, static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT));
  ASSERT_EQ(resp.DiagnosticInfos[0].LocalizedText, 4);
}

