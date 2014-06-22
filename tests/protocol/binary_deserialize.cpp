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

//---------------------------------------------------------------------
// Integer types
//---------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, UInt8)
{
  std::vector<char> serializedData = {-1};
  GetChannel().SetData(serializedData);
  uint8_t byte = 0;
  GetStream() >> byte;
  ASSERT_EQ(byte, 0xff);
}

TEST_F(OpcUaBinaryDeserialization, Int8)
{
  std::vector<char> serializedData = {-1};
  GetChannel().SetData(serializedData);
  int8_t byte = 0;
  GetStream() >> byte;
  ASSERT_EQ(byte, -1);
}

TEST_F(OpcUaBinaryDeserialization, UInt16)
{
  std::vector<char> serializedData = {0, -1};
  GetChannel().SetData(serializedData);
  uint16_t word = 0;
  GetStream() >> word;
  ASSERT_EQ(word, 0xff00);
}

TEST_F(OpcUaBinaryDeserialization, Int16)
{
  std::vector<char> serializedData = {0, -1};
  GetChannel().SetData(serializedData);
  int16_t word = 0;
  GetStream() >> word;
  ASSERT_EQ(word, -256);
}

TEST_F(OpcUaBinaryDeserialization, UInt32)
{
  std::vector<char> serializedData = {0, -1, 0, -1};
  GetChannel().SetData(serializedData);
  uint32_t dword = 0;
  GetStream() >> dword;
  ASSERT_EQ(dword, 0xff00ff00);
}

TEST_F(OpcUaBinaryDeserialization, Int32)
{
  std::vector<char> serializedData = {0, -1, 0, -1};
  GetChannel().SetData(serializedData);
  int32_t sdword = 0;
  GetStream() >> sdword;
  ASSERT_EQ(sdword, (int32_t)0xff00ff00);
}

TEST_F(OpcUaBinaryDeserialization, UInt64)
{
  std::vector<char> serializedData = {0, -1, 0, -1, 0, 0, -1, -1};
  GetChannel().SetData(serializedData);
  uint64_t qword = 0;
  GetStream() >> qword;
  ASSERT_EQ(qword, 0xffff0000ff00ff00);
}

TEST_F(OpcUaBinaryDeserialization, Int64)
{
  std::vector<char> serializedData = {0, -1, 0, -1, 0, 0, -1, -1};
  GetChannel().SetData(serializedData);
  int64_t sqword = 0;
  GetStream() >> sqword;
  ASSERT_EQ(sqword, -281470698455296);
}

//-------------------------------------------------------------
// Floating point
//-------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Float)
{
  const std::vector<char> serializedData = {(char)0xC0, (char)0xD0, 0, 0};
  GetChannel().SetData(serializedData);
  float num = 0;
  GetStream() >> num;
  ASSERT_EQ(num, -6.5);
}

TEST_F(OpcUaBinaryDeserialization, Double)
{
  std::vector<char> serializedData = {0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41};
  GetChannel().SetData(serializedData);
  double num = 0;
  GetStream() >> num;
  ASSERT_EQ(num, 1200000);
}

//-------------------------------------------------------------
// String
//-------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, StdString)
{
  const std::vector<char> serializedData = {3, 0, 0, 0, 'u', 'r', 'l'};
  GetChannel().SetData(serializedData);

  std::string deserializedString;
  GetStream() >> deserializedString;
  
  std::string expectedString = {'u', 'r', 'l'};
  ASSERT_EQ(deserializedString, expectedString);
}
/*
// TODO 0xFF*4 seems to be an empty string.
TEST_F(OpcUaBinaryDeserialization, ZeroEndStdString)
{
  const std::vector<char> serializedData = {-1, -1, -1, -1, 'u', 'r', 'l',0};
  GetChannel().SetData(serializedData);

  std::string deserializedString;
  GetStream() >> deserializedString;
  
  std::string expectedString = {'u', 'r', 'l'};
  ASSERT_EQ(deserializedString, expectedString);
}
*/
TEST_F(OpcUaBinaryDeserialization, EmptyStdString)
{
  const std::vector<char> serializedData = {-1, -1, -1, -1};
  GetChannel().SetData(serializedData);

  std::string deserializedString;
  GetStream() >> deserializedString;
  
  std::string expectedString;
  ASSERT_EQ(deserializedString, expectedString);
}


//-------------------------------------------------------------
// ByteString
//-------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, ByteString)
{
  const std::vector<char> serializedData = {3, 0, 0, 0, 'u', 'r', 'l'};
  GetChannel().SetData(serializedData);

  std::vector<uint8_t> deserializedBytes;
  GetStream() >> deserializedBytes;
  
  std::vector<uint8_t> expectedBytes = {'u', 'r', 'l'};
  ASSERT_EQ(deserializedBytes, expectedBytes);
}

TEST_F(OpcUaBinaryDeserialization, AbsentBytes)
{
  const std::vector<char> serializedData = {-1, -1, -1, -1};
  GetChannel().SetData(serializedData);

  std::vector<uint8_t> deserializedBytes;
  GetStream() >> deserializedBytes;
  
  std::vector<uint8_t> expectedBytes;
  ASSERT_EQ(deserializedBytes, expectedBytes);
}

//-------------------------------------------------------------
// RawBuffer
//-------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, RawBuffer)
{
  const std::vector<char> serializedData = {3, 0, 0, 0, 'u', 'r', 'l'};
  GetChannel().SetData(serializedData);

  std::vector<char> deserializedBytes(serializedData.size());
  OpcUa::Binary::RawBuffer rawBuffer(&deserializedBytes[0], deserializedBytes.size());
  GetStream() >> rawBuffer;

  ASSERT_EQ(rawBuffer.Size,    serializedData.size());
  ASSERT_EQ(deserializedBytes, serializedData);
}

//-------------------------------------------------------------
// LocalizedText
//-------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, LocalizedText_Full)
{
  const std::vector<char> serializedData = {3, 2,0,0,0,'e','n', 4,0,0,0,'t','e','x','t'};
  GetChannel().SetData(serializedData);

  OpcUa::LocalizedText lt;
  GetStream() >> lt;
  
  ASSERT_EQ(lt.Locale, "en");
  ASSERT_EQ(lt.Text, "text");
}

TEST_F(OpcUaBinaryDeserialization, LocalizedText_Locale)
{
  const std::vector<char> serializedData = {1, 2,0,0,0,'e','n', 4,0,0,0,'t','e','x','t'};
  GetChannel().SetData(serializedData);

  OpcUa::LocalizedText lt;
  GetStream() >> lt;
  
  ASSERT_EQ(lt.Locale, "en");
  ASSERT_EQ(lt.Text, "");
}

TEST_F(OpcUaBinaryDeserialization, LocalizedText_Text)
{
  const std::vector<char> serializedData = {2, 2,0,0,0,'e','n', 4,0,0,0,'t','e','x','t'};
  GetChannel().SetData(serializedData);

  OpcUa::LocalizedText lt;
  GetStream() >> lt;
  
  ASSERT_EQ(lt.Locale, "");
  ASSERT_EQ(lt.Text, "en");
}


//-------------------------------------------------------------
// Message type
//-------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, MessageTypeHello)
{
  const std::vector<char> serializedData = {'H', 'E', 'L'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::MessageType msgType = OpcUa::Binary::MT_ERROR;
  GetStream() >> msgType;
  
  ASSERT_EQ(msgType, OpcUa::Binary::MT_HELLO);
}

TEST_F(OpcUaBinaryDeserialization, MessageTypeAcknowledge)
{
  const std::vector<char> serializedData = {'A', 'C', 'K'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::MessageType msgType = OpcUa::Binary::MT_ERROR;
  GetStream() >> msgType;
  
  ASSERT_EQ(msgType, OpcUa::Binary::MT_ACKNOWLEDGE);
}

TEST_F(OpcUaBinaryDeserialization, MessageTypeError)
{
  const std::vector<char> serializedData = {'E', 'R', 'R'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::MessageType msgType = OpcUa::Binary::MT_HELLO;
  GetStream() >> msgType;
  
  ASSERT_EQ(msgType, OpcUa::Binary::MT_ERROR);
}

TEST_F(OpcUaBinaryDeserialization, MessageTypeSecureOpen)
{
  const std::vector<char> serializedData = {'O', 'P', 'N'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::MessageType msgType = OpcUa::Binary::MT_ERROR;
  GetStream() >> msgType;
  
  ASSERT_EQ(msgType, OpcUa::Binary::MT_SECURE_OPEN);
}

TEST_F(OpcUaBinaryDeserialization, MessageTypeSecureClose)
{
  const std::vector<char> serializedData = {'C', 'L', 'O'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::MessageType msgType = OpcUa::Binary::MT_ERROR;
  GetStream() >> msgType;
  
  ASSERT_EQ(msgType, OpcUa::Binary::MT_SECURE_CLOSE);
}

TEST_F(OpcUaBinaryDeserialization, MessageTypeInvalid)
{
  const std::vector<char> serializedData = {'I', 'N', 'V'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::MessageType msgType = OpcUa::Binary::MT_ERROR;
  ASSERT_THROW(GetStream() >> msgType, std::logic_error);
}

//------------------------------------------------------------
// Chunk types
//------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, ChunkTypeSingle)
{
  const std::vector<char> serializedData = {'F'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::ChunkType chunkType = OpcUa::Binary::CHT_FINAL;
  GetStream() >> chunkType;
  
  ASSERT_EQ(chunkType, OpcUa::Binary::CHT_SINGLE);
}

TEST_F(OpcUaBinaryDeserialization, ChunkTypeIntermediate)
{
  const std::vector<char> serializedData = {'C'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::ChunkType chunkType = OpcUa::Binary::CHT_FINAL;
  GetStream() >> chunkType;
  
  ASSERT_EQ(chunkType, OpcUa::Binary::CHT_INTERMEDIATE);
}

TEST_F(OpcUaBinaryDeserialization, ChunkTypeFinal)
{
  const std::vector<char> serializedData = {'A'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::ChunkType chunkType = OpcUa::Binary::CHT_SINGLE;
  GetStream() >> chunkType;
  
  ASSERT_EQ(chunkType, OpcUa::Binary::CHT_FINAL);
}

TEST_F(OpcUaBinaryDeserialization, ChunkTypeInvalid)
{
  const std::vector<char> serializedData = {'I'};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::ChunkType chunkType = OpcUa::Binary::CHT_SINGLE;
  ASSERT_THROW(GetStream() >> chunkType, std::logic_error);
}


//---------------------------------------------------------
// Message Header
//---------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, MessageHeader)
{
  const std::vector<char> serializedData = {'H', 'E', 'L', 'F', 4, 3, 2, 1};
  GetChannel().SetData(serializedData);

  OpcUa::Binary::Header hdr;
  GetStream() >> hdr;
 
  ASSERT_EQ(hdr.Type,  OpcUa::Binary::MT_HELLO);
  ASSERT_EQ(hdr.Chunk, OpcUa::Binary::CHT_SINGLE);
  ASSERT_EQ(hdr.Size, (uint32_t)0x01020304);
}


//---------------------------------------------------------
// Message
//---------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Message)
{
  const std::vector<char> serializedData = {
  1, 0, 0, 0,      // ProtocolVersion
  2, 0, 0, 0,      // ReceiveBufferSize
  3, 0, 0, 0,      // SendBufferSize
  4, 0, 0, 0,      // MaxMessageSize
  5, 0, 0, 0,      // MaxChunkCount
  3, 0, 0, 0,      // string size
  'u', 'r', 'l'    // Endpoint
  };

  GetChannel().SetData(serializedData);

  OpcUa::Binary::Hello message;
  GetStream() >> message;
 
  ASSERT_EQ(message.ProtocolVersion,   1);
  ASSERT_EQ(message.ReceiveBufferSize, 2);
  ASSERT_EQ(message.SendBufferSize,    3);
  ASSERT_EQ(message.MaxMessageSize,    4);
  ASSERT_EQ(message.MaxChunkCount,     5);
  ASSERT_EQ(message.EndpointUrl, "url");
}

TEST_F(OpcUaBinaryDeserialization, InvalidMessage)
{
  const std::vector<char> serializedData = {
//  4, 3, 2, 1,      // ProtocolVersion
  4, 3, 2, 1,      // ReceiveBufferSize
  4, 3, 2, 1,      // SendBufferSize
  4, 3, 2, 1,      // MaxMessageSize
  4, 3, 2, 1,      // MaxChunkCount
  3, 0, 0, 0,      // string size
  'u', 'r', 'l'    // Endpoint
  };

  GetChannel().SetData(serializedData);

  OpcUa::Binary::Hello message;
  ASSERT_THROW(GetStream() >> message, std::logic_error);
}


//----------------------------------------------------------
// Acknowlefge
//----------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Acknowledge)
{
  std::vector<char> serializedData = {
  1, 0, 0, 0,      // ProtocolVersion
  2, 0, 0, 0,      // ReceiveBufferSize
  3, 0, 0, 0,      // SendBufferSize
  4, 0, 0, 0,      // MaxMessageSize
  5, 0, 0, 0,      // MaxChunkCount
  };
 
  GetChannel().SetData(serializedData);

  OpcUa::Binary::Acknowledge message;
  GetStream() >> message;

  ASSERT_EQ(message.ProtocolVersion,   1);
  ASSERT_EQ(message.ReceiveBufferSize, 2);
  ASSERT_EQ(message.SendBufferSize,    3);
  ASSERT_EQ(message.MaxMessageSize,    4);
  ASSERT_EQ(message.MaxChunkCount,     5);
}

TEST_F(OpcUaBinaryDeserialization, InvalidAcknowledge)
{
  std::vector<char> serializedData = {
//  4, 3, 2, 1,      // ProtocolVersion
  4, 3, 2, 1,      // ReceiveBufferSize
  4, 3, 2, 1,      // MaxMessageSize
  4, 3, 2, 1,      // MaxChunkCount
  };
 
  GetChannel().SetData(serializedData);

  OpcUa::Binary::Acknowledge message;
  ASSERT_THROW(GetStream() >> message, std::logic_error);
}

//----------------------------------------------------------
// Error
//----------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Error)
{
  std::vector<char> serializedData = {
  4, 3, 2, 1,      // Error code
  3, 0, 0, 0,      // string size
  'u', 'r', 'l'    // Endpoint
  };
 
  GetChannel().SetData(serializedData);

  OpcUa::Binary::Error message;
  ASSERT_NO_THROW(GetStream() >> message);

  uint32_t tmpInt = 0x01020304;
  ASSERT_EQ(message.Code, tmpInt);
  ASSERT_EQ(message.Reason, "url");
}

TEST_F(OpcUaBinaryDeserialization, InvalidError)
{
  std::vector<char> serializedData = {
//  4, 3, 2, 1,      // Error code
  3, 0, 0, 0,      // string size
  'u', 'r', 'l'    // Endpoint
  };
 
  GetChannel().SetData(serializedData);

  OpcUa::Binary::Error message;
  ASSERT_THROW(GetStream() >> message, std::logic_error);
}

//---------------------------------------------------------
// Secure Message Header
//---------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, SecureMessageHeader)
{
  const std::vector<char> serializedData = 
  {
    'H', 'E', 'L',
    'F',
    4, 3, 2, 1,
    1, 0, 0, 0
  };

  GetChannel().SetData(serializedData);

  OpcUa::Binary::SecureHeader hdr;
  GetStream() >> hdr;
 
  ASSERT_EQ(hdr.Type,  OpcUa::Binary::MT_HELLO);
  ASSERT_EQ(hdr.Chunk, OpcUa::Binary::CHT_SINGLE);
  ASSERT_EQ(hdr.Size, (uint32_t)0x01020304);
  ASSERT_EQ(hdr.ChannelID, (uint32_t)0x1);
}

//---------------------------------------------------------
// AsymmetricAlgorithmHeader
//---------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Asymmetric)
{
  const std::vector<char> serializedData = {
  3, 0, 0, 0,      
  'p', 'o', 'l',
  3, 0, 0, 0,
  1, 2, 3,
  3, 0, 0, 0,
  4, 5, 6
  };

  GetChannel().SetData(serializedData);
  
  OpcUa::Binary::AsymmetricAlgorithmHeader header;
  GetStream() >> header;

  ASSERT_EQ(header.SecurityPolicyURI, std::string("pol"));
  const std::vector<uint8_t> cert = {1, 2, 3};
  ASSERT_EQ(header.SenderCertificate, cert);
  const std::vector<uint8_t> thumb = {4, 5, 6};
  ASSERT_EQ(header.ReceiverCertificateThumbPrint, thumb);
}

//---------------------------------------------------------
// SymmetricAlgorithmHeader
//---------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, Symmetric)
{
  const std::vector<char> serializedData = {
  7, 6, 5, 4
  };

  GetChannel().SetData(serializedData);
  
  OpcUa::Binary::SymmetricAlgorithmHeader header;
  GetStream() >> header;
  ASSERT_EQ(header.TokenID, 0x04050607);
}

//---------------------------------------------------------
// SequenceHeader
//---------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, SequenceHeader)
{
  const std::vector<char> serializedData = {
  3, 0, 0, 0,      
  1, 0, 0, 0,
  };

  GetChannel().SetData(serializedData);
  
  OpcUa::Binary::SequenceHeader header;
  GetStream() >> header;

  ASSERT_EQ(header.SequenceNumber, 0x3);
  ASSERT_EQ(header.RequestID, 0x1);
}

//-------------------------------------------------------------------
// AdditionalHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, AdditionalHeader)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_SERVER_INDEX_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  3, 0, 0, 0,
  'u', 'r', 'i',
  1, 0, 0, 0,
  1
  };

  GetChannel().SetData(expectedData);

  AdditionalHeader header;
  GetStream() >> header;

  ASSERT_EQ(header.TypeID.Encoding, uint8_t(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_SERVER_INDEX_FLAG));
  ASSERT_EQ(header.TypeID.StringData.NamespaceIndex, 0x1);
  ASSERT_EQ(header.TypeID.StringData.Identifier, "id");
  ASSERT_EQ(header.TypeID.NamespaceURI, "uri");
  ASSERT_EQ(header.TypeID.ServerIndex, 1);
  ASSERT_EQ(header.Encoding, 1);

  ASSERT_EQ(expectedData.size(), Binary::RawSize(header));
}

//-------------------------------------------------------------------
// RequestHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, RequestHeader)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  EV_TWO_BYTE, 1,
  2, 0, 0, 0, 0, 0, 0, 0,
  3, 0, 0, 0,
  4, 0, 0, 0,
  5, 0, 0, 0,
  'a', 'u', 'd', 'i', 't',
  5, 0, 0, 0,
  0,
  6,
  8
  };

  GetChannel().SetData(expectedData);

  RequestHeader header;
  GetStream() >> header;

  ASSERT_EQ(header.SessionAuthenticationToken.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(header.SessionAuthenticationToken.TwoByteData.Identifier, 1);
  ASSERT_EQ(header.UtcTime, 2);
  ASSERT_EQ(header.RequestHandle, 3);
  ASSERT_EQ(header.ReturnDiagnostics, 4);
  ASSERT_EQ(header.AuditEntryID, "audit");
  ASSERT_EQ(header.Timeout, 5); // in miliseconds
  ASSERT_EQ(header.Additional.TypeID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(header.Additional.TypeID.TwoByteData.Identifier, 6);
  ASSERT_EQ(header.Additional.Encoding, 8);

  ASSERT_EQ(expectedData.size(), Binary::RawSize(header));
}

//-------------------------------------------------------------------
// DiagnosticInfo
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_Empty)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  0
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;
  ASSERT_EQ(info.EncodingMask, 0);
}

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_SymbolicID)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  DIM_SYMBOLIC_ID,
  2, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;

  ASSERT_EQ(info.EncodingMask, DIM_SYMBOLIC_ID);
  ASSERT_EQ(info.SymbolicID, 2);
  ASSERT_EQ(expectedData.size(), Binary::RawSize(info));
}

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_SymbolicID_Namespace)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  DIM_SYMBOLIC_ID | DIM_NAMESPACE,
  2, 0, 0, 0,
  3, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;

  ASSERT_EQ(info.EncodingMask, DIM_SYMBOLIC_ID | DIM_NAMESPACE);
  ASSERT_EQ(info.SymbolicID, 2);
  ASSERT_EQ(info.NamespaceURI, 3);
}

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_SymbolicID_LocalizedText)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  DIM_SYMBOLIC_ID | DIM_LOCALIZED_TEXT,
  2, 0, 0, 0,
  4, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;
  
  ASSERT_EQ(info.EncodingMask, DIM_SYMBOLIC_ID | DIM_LOCALIZED_TEXT);
  ASSERT_EQ(info.SymbolicID, 2);
  ASSERT_EQ(info.LocalizedText, 4);
}

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_LocalizedText_Locale)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  DIM_LOCALIZED_TEXT | DIM_LOCALE,
  2, 0, 0, 0,
  4, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;
 
  ASSERT_EQ(info.EncodingMask, DIM_LOCALIZED_TEXT | DIM_LOCALE);
  ASSERT_EQ(info.LocalizedText, 2);
  ASSERT_EQ(info.Locale, 4);
}

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_AdditionalInfo)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  DIM_ADDITIONAL_INFO,
  3,0,0,0,
  'a', 'd', 'd'
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;
 
  ASSERT_EQ(info.EncodingMask, DIM_ADDITIONAL_INFO);
  ASSERT_EQ(info.AdditionalInfo, "add");
}

TEST_F(OpcUaBinaryDeserialization, DiagnosticInfo_InnerStatusCode)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  DIM_INNER_STATUS_CODE,
  6,0,0,0,
  };

  GetChannel().SetData(expectedData);

  DiagnosticInfo info;
  GetStream() >> info;
 
  ASSERT_EQ(info.EncodingMask, DIM_INNER_STATUS_CODE);
  ASSERT_EQ(info.InnerStatusCode, static_cast<StatusCode>(6));
}

//-------------------------------------------------------------------
// ResponseHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, ResponseHeader)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  TEST_RESPONSE_HEADER_BINARY_DATA
  };

  GetChannel().SetData(expectedData);

  ResponseHeader header;
  GetStream() >> header;

  ASSERT_RESPONSE_HEADER_EQ(header);
}

//-------------------------------------------------------------------
// OpenSecureChannelRequset
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, OpenSequreChannelRequest)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  1, 0, (char)0xbe, 0x1, // TypeID

  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  1, 0, 0, 0,
  STR_RENEW,  0, 0, 0,
  MSM_SIGN, 0, 0, 0,
  1, 0, 0, 0,
  1,
  5, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  OpenSecureChannelRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.ClientProtocolVersion, 1);
  ASSERT_EQ(request.RequestType, STR_RENEW);
  ASSERT_EQ(request.SecurityMode, MSM_SIGN);
  ASSERT_EQ(request.ClientNonce, std::vector<uint8_t>(1, 1));
  ASSERT_EQ(request.RequestLifeTime, 5);

  ASSERT_REQUEST_HEADER_EQ(request.Header);
}

//-------------------------------------------------------------------
// SecurityToken
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, SecurityToken)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  1, 0, 0, 0,
  2, 0, 0, 0,
  3, 0, 0, 0, 0, 0, 0, 0,
  4, 0, 0, 0,
  };

  GetChannel().SetData(expectedData);

  SecurityToken token;
  GetStream() >> token;

  ASSERT_EQ(token.SecureChannelID, 1);
  ASSERT_EQ(token.TokenID, 2);
  ASSERT_EQ(token.CreatedAt, 3);
  ASSERT_EQ(token.RevisedLifetime, 4);
}

//-------------------------------------------------------------------
// OpenSecureChannelResponse
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, OpenSecureChannelResponse)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  1, 0, (char)0xC1, 0x1, // TypeID

  // ResponseHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  1, 0, 0, 0,
  2, 0, 0, 0,
  3,  0, 0, 0,
  4, 0, 0, 0, 0, 0, 0, 0,
  5, 0, 0, 0,
  1, 0, 0, 0,
  6
  };

  GetChannel().SetData(expectedData);

  OpenSecureChannelResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::OPEN_SECURE_CHANNEL_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.ServerProtocolVersion, 1);
  ASSERT_EQ(response.ChannelSecurityToken.SecureChannelID, 2);
  ASSERT_EQ(response.ChannelSecurityToken.TokenID, 3);
  ASSERT_EQ(response.ChannelSecurityToken.CreatedAt, 4);
  ASSERT_EQ(response.ChannelSecurityToken.RevisedLifetime, 5);
  ASSERT_EQ(response.ServerNonce, std::vector<uint8_t>(1, 6));

}

//-------------------------------------------------------------------
// CloseSecureChannelRequset
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, CloseSequreChannelRequest)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  1, 0, (char)0xc4, 0x1, // TypeID

  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  };

  GetChannel().SetData(expectedData);

  CloseSecureChannelRequest request;
  GetStream() >> request;

  ASSERT_REQUEST_HEADER_EQ(request.Header);
}

//----------------------------------------------------------------
// SignatureData
//----------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, SignatureData)
{

  using namespace OpcUa;

  const std::vector<char> expectedData = {
    4, 0, 0, 0, 1,2,3,4,      // Signature
    3, 0, 0, 0, 'a','e','s'   // Algorithm
  };

  GetChannel().SetData(expectedData);

  OpcUa::SignatureData s;
  GetStream() >> s;

  std::vector<uint8_t> signature = {1,2,3,4};
  ASSERT_EQ(s.Signature, signature); 
  ASSERT_EQ(s.Algorithm, "aes");
}

//-------------------------------------------------------------------
// ExtensionObjectHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, ExtensionObjectHeader)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  1, 0, (char)0x41, 0x1, // TypeID
  HAS_BINARY_BODY
  };

  GetChannel().SetData(expectedData);
  ExtensionObjectHeader header(OpcUa::USER_IDENTIFY_TOKEN_ANONYMOUS, HAS_BINARY_BODY);
  GetStream() >> header;
  
  ASSERT_EQ(header.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(header.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(header.TypeID.FourByteData.Identifier, OpcUa::USER_IDENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(header.Encoding, HAS_BINARY_BODY);
}

//-------------------------------------------------------------------
// QualifiedName
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, QualifiedName)
{
  using namespace OpcUa;

  const std::vector<char> expectedData = {
  1, 0,
  4, 0, 0, 0, 'n','a','m','e'
  };

  GetChannel().SetData(expectedData);

  QualifiedName name;
  GetStream() >> name;

  ASSERT_EQ(name.NamespaceIndex, 1);
  ASSERT_EQ(name.Name, "name");
}

//-------------------------------------------------------------------
// IntegerID
//-------------------------------------------------------------------

TEST_F(OpcUaBinaryDeserialization, IntegerID)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  5, 0, 0, 0
  };

  GetChannel().SetData(expectedData);

  IntegerID id;
  GetStream() >> id;

  ASSERT_EQ(id, 5);
}

