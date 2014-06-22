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
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <stdexcept>


//-------------------------------------------------------
// AttributeID
// //TODO Add check for ivalid attributeID deserialization. It must throw.
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, AttributeID)
{

  using namespace OpcUa::Binary;
  using OpcUa::AttributeID;

  GetStream() << AttributeID::VALUE << flush;

  const std::vector<char> expectedData = {
  13,0,0,0
  };

  ASSERT_EQ(expectedData.size(), RawSize(AttributeID::VALUE));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, AttributeID)
{
  using namespace OpcUa::Binary;
  using OpcUa::AttributeID;

  const std::vector<char> expectedData = {
  13,0,0,0
  };

  GetChannel().SetData(expectedData);

  AttributeID id;
  GetStream() >> id;

  ASSERT_EQ(id, AttributeID::VALUE);
}


//-------------------------------------------------------
// TimestampsToReturn
// TODO Add test for invalid Timestamps to return deserialization.
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, TimestampsToReturn)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  GetStream() << TimestampsToReturn::NEITHER << flush;

  const std::vector<char> expectedData = {
  3,0,0,0
  };

  ASSERT_EQ(expectedData.size(), RawSize(TimestampsToReturn::NEITHER));
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

  ASSERT_EQ(stamps, TimestampsToReturn::NEITHER);
}

//-------------------------------------------------------
// AttributeValueID
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, AttributeValueID)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using OpcUa::AttributeID;

  AttributeValueID attr;

  attr.Node.Encoding = EV_TWO_BYTE;
  attr.Node.TwoByteData.Identifier = 1;
  attr.Attribute = AttributeID::VALUE;
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

TEST_F(OpcUaBinaryDeserialization, AttributeValueID)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  using OpcUa::AttributeID;

  const std::vector<char> expectedData = {
  0, 1,
  13,0,0,0,
  3,0,0,0, '1',',','2',
  2,0,
  4,0,0,0, 't','e','s','t'
  };

  GetChannel().SetData(expectedData);

  AttributeValueID attr;
  GetStream() >> attr;
  
  ASSERT_EQ(attr.Node.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(attr.Node.TwoByteData.Identifier, 1);
  ASSERT_EQ(attr.Attribute, AttributeID::VALUE);
  ASSERT_EQ(attr.DataEncoding.NamespaceIndex, 2);
  ASSERT_EQ(attr.DataEncoding.Name, "test");
}

//-------------------------------------------------------
// ReadRequest
//-------------------------------------------------------

OpcUa::AttributeValueID CreateAttributeValueID()
{
  using namespace OpcUa;
  OpcUa::AttributeValueID attr;

  attr.Node.Encoding = OpcUa::EV_TWO_BYTE;
  attr.Node.TwoByteData.Identifier = 1;
  attr.Attribute = OpcUa::AttributeID::VALUE;
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

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::READ_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.Parameters.MaxAge = 1200000;
  request.Parameters.TimestampsType = TimestampsToReturn::NEITHER;

  request.Parameters.AttributesToRead.push_back(CreateAttributeValueID());

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x77, 0x2, // TypeID
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
  1, 0, (char)0x77, 0x2, // TypeID
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

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::READ_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.MaxAge, 1200000);
  ASSERT_EQ(request.Parameters.TimestampsType, TimestampsToReturn::NEITHER);
  
  ASSERT_EQ(request.Parameters.AttributesToRead.size(), 1);

  AttributeValueID attr = CreateAttributeValueID();

  ASSERT_EQ(request.Parameters.AttributesToRead[0].Node.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(request.Parameters.AttributesToRead[0].Node.TwoByteData.Identifier, 1);
  ASSERT_EQ(request.Parameters.AttributesToRead[0].Attribute, OpcUa::AttributeID::VALUE);
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

  ASSERT_EQ(resp.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeID.FourByteData.Identifier, OpcUa::READ_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(resp.Header);

  ASSERT_NO_THROW(GetStream() << resp << flush);

  const std::vector<char> expectedData = {
  1, 0, (char)0x7A, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  0,0,0,0,
  0,0,0,0,
  };

  ASSERT_EQ(expectedData.size(), RawSize(resp));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
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
     DATA_VALUE_SERVER_TIMESTAMP |
     DATA_VALUE_SOURCE_PICOSECONDS |
     DATA_VALUE_SERVER_PICOSECONDS;

  const std::vector<char> expectedData = {
  1, 0, (char)0x7A, 0x2, // TypeID
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

  ASSERT_EQ(resp.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeID.FourByteData.Identifier, OpcUa::READ_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(resp.Header);
  ASSERT_EQ(resp.Result.Results.size(), 1);
}

//-------------------------------------------------------
// WriteValue
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, WriteValue)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  WriteValue value;
  value.Node.Encoding = EV_FOUR_BYTE;
  value.Node.FourByteData.Identifier = 1;
  value.Attribute = AttributeID::DISPLAY_NAME;
  value.Data.Encoding = DATA_VALUE;
  value.Data.Value.Type = VariantType::BOOLEAN;
  value.Data.Value.Value.Boolean = std::vector<bool>{true};
 
  GetStream() << value << flush;

  const std::vector<char> expectedData = {
  1,0,1,0, // Node
  4,0,0,0, // AttributeID
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
  4,0,0,0, // AttributeID
  -1,-1,-1,-1, // NumericRange  
  1, 1, 1 // NodesToWrite
  };


  GetChannel().SetData(expectedData);
  WriteValue value;
  GetStream() >> value;

  ASSERT_EQ(value.Node.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(value.Node.FourByteData.Identifier, 1);
  ASSERT_EQ(value.Attribute, AttributeID::DISPLAY_NAME);
  ASSERT_EQ(value.Data.Encoding, DATA_VALUE);
  ASSERT_EQ(value.Data.Value.Type, VariantType::BOOLEAN);
  ASSERT_EQ(value.Data.Value.Value.Boolean.size(), 1);
  ASSERT_EQ(value.Data.Value.Value.Boolean[0], true);
}


//-------------------------------------------------------
// WriteRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, WriteRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  WriteRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::WRITE_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  WriteValue value;
  value.Node.Encoding = EV_FOUR_BYTE;
  value.Node.FourByteData.Identifier = 1;
  value.Attribute = AttributeID::DISPLAY_NAME;
  value.Data.Encoding = DATA_VALUE;
  value.Data.Value.Type = VariantType::BOOLEAN;
  value.Data.Value.Value.Boolean = std::vector<bool>{true};

  request.Parameters.NodesToWrite.push_back(value);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xA1, 0x2, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1,0,0,0,

  1,0,1,0, // Node
  4,0,0,0, // AttributeID
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
  1, 0, (char)0xA1, 0x2, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1,0,0,0,
  1,0,1,0, // Node
  4,0,0,0, // AttributeID
  -1,-1,-1,-1, // NumericRange  
  1, 1, 1 // Value
  };



  GetChannel().SetData(expectedData);
  WriteRequest request;
  GetStream() >> request;


  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::WRITE_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);


  ASSERT_EQ(request.Parameters.NodesToWrite.size(), 1);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Node.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Node.FourByteData.Identifier, 1);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Attribute, AttributeID::DISPLAY_NAME);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Data.Encoding, DATA_VALUE);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Data.Value.Type, VariantType::BOOLEAN);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Data.Value.Value.Boolean.size(), 1);
  ASSERT_EQ(request.Parameters.NodesToWrite[0].Data.Value.Value.Boolean[0], true);
}

//-------------------------------------------------------
// WriteResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, WriteResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  WriteResponse resp;

  ASSERT_EQ(resp.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeID.FourByteData.Identifier, OpcUa::WRITE_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(resp.Header);

  resp.Result.StatusCodes.push_back(static_cast<StatusCode>(1));
 
  DiagnosticInfo info;
  info.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT);
  info.LocalizedText = 4;
  resp.Result.Diagnostics.push_back(info);
 
  GetStream() << resp << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xA4, 0x2, // TypeID
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
  1, 0, (char)0xA4, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  1,0,0,0, 1,0,0,0,
  1,0,0,0, static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT), 4,0,0,0
  };

  GetChannel().SetData(expectedData);
  WriteResponse resp;
  GetStream() >> resp;


  ASSERT_EQ(resp.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(resp.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(resp.TypeID.FourByteData.Identifier, OpcUa::WRITE_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(resp.Header);

  ASSERT_EQ(resp.Result.StatusCodes.size(), 1);
  ASSERT_EQ(resp.Result.StatusCodes[0], static_cast<OpcUa::StatusCode>(1));

  ASSERT_EQ(resp.Result.Diagnostics.size(), 1);
  ASSERT_EQ(resp.Result.Diagnostics[0].EncodingMask, static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT));
  ASSERT_EQ(resp.Result.Diagnostics[0].LocalizedText, 4);
}

