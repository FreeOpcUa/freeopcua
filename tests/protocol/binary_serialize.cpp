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
using namespace OpcUa::Binary;

TEST_F(OpcUaBinarySerialization, LeftShiftAddsData)
{
  uint8_t dataForSerialize = 1;
  const std::vector<char> expectedData = {1, 1};
  GetStream() << dataForSerialize;
  GetStream() << dataForSerialize;
  GetStream() << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, FlushResetsData)
{
  uint8_t dataForSerialize = 1;
  const std::vector<char> expectedData = {1};
  GetStream() << dataForSerialize << flush;
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

//---------------------------------------------------------------------
// Integer types
//---------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, UInt8)
{
  uint8_t dataForSerialize = 1;
  const std::vector<char> expectedData = { 1 };
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, Int8)
{
  int8_t dataForSerialize = 1;
  const std::vector<char> expectedData = { 1 };
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, UInt16)
{
  uint16_t dataForSerialize = 0x0102;
  const std::vector<char> expectedData = {2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, Int16)
{
  int16_t dataForSerialize = 0x0102;
  const std::vector<char> expectedData = {2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, UInt32)
{
  uint32_t dataForSerialize = 0x01020304;
  const std::vector<char> expectedData = {4, 3, 2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, Int32)
{
  int32_t dataForSerialize = 0x01020304;
  const std::vector<char> expectedData = {4, 3, 2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, UInt64)
{
  uint64_t dataForSerialize = 0x0102030405060708;
  const std::vector<char> expectedData = {8, 7, 6, 5, 4, 3, 2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, Int64)
{
  int64_t dataForSerialize = 0x0102030405060708;
  const std::vector<char> expectedData = {8, 7, 6, 5, 4, 3, 2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, DateTime)
{
  OpcUa::DateTime dataForSerialize(0x0102030405060708);
  const std::vector<char> expectedData = {8, 7, 6, 5, 4, 3, 2, 1};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, Guid)
{
  OpcUa::Guid dataForSerialize;
  dataForSerialize.Data1 = 0x01020304;
  dataForSerialize.Data2 = 0x0506;
  dataForSerialize.Data3 = 0x0708;

  for (unsigned i = 0; i < 8; ++i)
  {
    dataForSerialize.Data4[i] = i + 1;
  }
  
  const std::vector<char> expectedData = 
  {
    4, 3, 2, 1,
    6, 5,
    8, 7, 
    1, 2, 3, 4, 5, 6, 7, 8
  };
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}
//-------------------------------------------------------------
// Floating point
//-------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Float)
{
  float dataForSerialize = -6.5;
  const std::vector<char> expectedData = {0, 0, (char)0xD0, (char)0xC0};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, Double)
{
  double dataForSerialize = 1200000;
//  const std::vector<char> expectedData = {(char)0xC0, (char)0x1A, 0, 0, 0, 0, 0, 0};
  const std::vector<char> expectedData = {0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

//-------------------------------------------------------------
// String
//-------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, StdString)
{
  std::string dataForSerialize = {'u', 'r', 'l'};
  const std::vector<char> expectedData = {3, 0, 0, 0, 'u', 'r', 'l'};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, EmptyStdString)
{
  std::string dataForSerialize;
  const std::vector<char> expectedData(4, ~char());
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}


//-------------------------------------------------------------
// ByteString
//-------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ByteString)
{
  std::vector<uint8_t> dataForSerialize = {'u', 'r', 'l'};
  const std::vector<char> expectedData = {3, 0, 0, 0, 'u', 'r', 'l'};
  GetStream() << dataForSerialize << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, EmptyByteString)
{
  GetStream() << std::vector<uint8_t>() << flush;
  ASSERT_EQ(std::vector<char>(4, -1), GetChannel().SerializedData);
}


//-------------------------------------------------------------
// RawMessage
//-------------------------------------------------------------
TEST_F(OpcUaBinarySerialization, RawMessage)
{
  const std::vector<char> expectedData = {8, 7, 6, 5, 4, 3, 2, 1};
  GetStream() << OpcUa::Binary::RawMessage(&expectedData[0], expectedData.size()) << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

//-------------------------------------------------------------
// LocalizedText
//-------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, LocalizedText_Full)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  LocalizedText lt;
  lt.Encoding = HAS_LOCALE | HAS_TEXT;
  lt.Locale = "en";
  lt.Text = "text";
  
  GetStream() << lt << flush;
  
  const std::vector<char> expectedData = {3, 2,0,0,0,'e','n', 4,0,0,0,'t','e','x','t'};
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(RawSize(lt), expectedData.size());
}

TEST_F(OpcUaBinarySerialization, LocalizedText_Locale)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  LocalizedText lt;
  lt.Encoding = HAS_LOCALE;
  lt.Locale = "en";
  lt.Text = "text";
  
  GetStream() << lt << flush;
  
  const std::vector<char> expectedData = {1, 2,0,0,0,'e','n'};
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(RawSize(lt), expectedData.size());
}

TEST_F(OpcUaBinarySerialization, LocalizedText_Text)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  LocalizedText lt;
  lt.Encoding = HAS_TEXT;
  lt.Locale = "en";
  lt.Text = "text";
  
  GetStream() << lt << flush;
  
  const std::vector<char> expectedData = {2, 4,0,0,0,'t','e','x','t'};
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(RawSize(lt), expectedData.size());
}



//-------------------------------------------------------------
// Message type
//-------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, MessageHello)
{
  const std::vector<char> expectedData = {'H', 'E', 'L'};
  GetStream() << OpcUa::Binary::MT_HELLO << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, MessageAcknowledge)
{
  const std::vector<char> expectedData = {'A', 'C', 'K'};
  GetStream() << OpcUa::Binary::MT_ACKNOWLEDGE << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, MessageError)
{
  const std::vector<char> expectedData = {'E', 'R', 'R'};
  GetStream() << OpcUa::Binary::MT_ERROR << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, MessageOpen)
{
  const std::vector<char> expectedData = {'O', 'P', 'N'};
  GetStream() << OpcUa::Binary::MT_SECURE_OPEN << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, MessageClose)
{
  const std::vector<char> expectedData = {'C', 'L', 'O'};
  GetStream() << OpcUa::Binary::MT_SECURE_CLOSE << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, MessageInvalid)
{
  ASSERT_THROW(GetStream() << static_cast<OpcUa::Binary::MessageType>(-1), std::logic_error);
}

//------------------------------------------------------------
// Chunk types
//------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ChunkSingle)
{
  const std::vector<char> expectedData = {'F'};
  GetStream() << OpcUa::Binary::CHT_SINGLE << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, ChunkIntermediate)
{
  const std::vector<char> expectedData = {'C'};
  GetStream() << OpcUa::Binary::CHT_INTERMEDIATE << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, ChunkFinal)
{
  const std::vector<char> expectedData = {'A'};
  GetStream() << OpcUa::Binary::CHT_FINAL << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

TEST_F(OpcUaBinarySerialization, ChunkInvalid)
{
  ASSERT_THROW(GetStream() << static_cast<OpcUa::Binary::ChunkType>(-1), std::logic_error);
}

//---------------------------------------------------------
// Message Header
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, MessageHeader)
{
  OpcUa::Binary::Header hdr;
  hdr.Type  = OpcUa::Binary::MT_HELLO;
  hdr.Chunk = OpcUa::Binary::CHT_SINGLE;
  hdr.Size = 0x01020304;

  const std::vector<char> expectedData = {'H', 'E', 'L', 'F', 4, 3, 2, 1};
  GetStream() << hdr << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

//---------------------------------------------------------
// Message Header
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, SecureMessageHeader)
{
  OpcUa::Binary::SecureHeader hdr;
  hdr.Type  = OpcUa::Binary::MT_HELLO;
  hdr.Chunk = OpcUa::Binary::CHT_SINGLE;
  hdr.Size = 0x01020304;
  hdr.ChannelId = 0x1;

  const std::vector<char> expectedData = 
  {
    'H', 'E', 'L',
    'F',
    4, 3, 2, 1,
    1, 0, 0, 0
  };
  GetStream() << hdr << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(hdr));
}


//---------------------------------------------------------
// Hello
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Hello)
{
  OpcUa::Binary::Hello message;
  message.ProtocolVersion   = 1;
  message.ReceiveBufferSize = 2;
  message.SendBufferSize    = 3;
  message.MaxMessageSize    = 4;
  message.MaxChunkCount     = 5;
  message.EndpointUrl       = "url";

  const std::vector<char> expectedData = {
  1, 0, 0, 0,      // ProtocolVersion
  2, 0, 0, 0,      // ReceiveBufferSize
  3, 0, 0, 0,      // SendBufferSize
  4, 0, 0, 0,      // MaxMessageSize
  5, 0, 0, 0,      // MaxChunkCount
  3, 0, 0, 0,      // string size
  'u', 'r', 'l'    // Endpoint
  };

  GetStream() << message << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(message));
}

//---------------------------------------------------------
// Message
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Acknowledge)
{
  OpcUa::Binary::Acknowledge message;
  message.ProtocolVersion   = 1;
  message.ReceiveBufferSize = 2;
  message.SendBufferSize    = 3;
  message.MaxMessageSize    = 4;
  message.MaxChunkCount     = 5;
 
  const std::vector<char> expectedData = {
    1, 0, 0, 0,      // ProtocolVersion
    2, 0, 0, 0,      // ReceiveBufferSize
    3, 0, 0, 0,      // SendBufferSize
    4, 0, 0, 0,      // MaxMessageSize
    5, 0, 0, 0,      // MaxChunkCount
  };

  GetStream() << message << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(message));
}

//---------------------------------------------------------
// Error
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Error)
{
  OpcUa::Binary::Error message;
  message.Code           = 1;
  message.Reason         = "url";

  const std::vector<char> expectedData = {
  1, 0, 0, 0,      // Error code
  3, 0, 0, 0,      // string size
  'u', 'r', 'l'    // Endpoint
  };

  GetStream() << message << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(message));
}

//---------------------------------------------------------
// AsymmetricAlgorithmHeader
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Asymmetric)
{
  OpcUa::Binary::AsymmetricAlgorithmHeader header;
  header.SecurityPolicyURI = "pol";
  header.SenderCertificate = {1, 2, 3};
  header.ReceiverCertificateThumbPrint = {4, 5, 6};

  const std::vector<char> expectedData = {
  3, 0, 0, 0,      
  'p', 'o', 'l',
  3, 0, 0, 0,
  1, 2, 3,
  3, 0, 0, 0,
  4, 5, 6
  };

  GetStream() << header << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//---------------------------------------------------------
// SymmetricAlgorithmHeader
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Symmetric)
{
  OpcUa::Binary::SymmetricAlgorithmHeader header;
  header.TokenId = 0x04050607;

  const std::vector<char> expectedData = {
  7, 6, 5, 4
  };

  GetStream() << header << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//---------------------------------------------------------
// SequenceHeader
//---------------------------------------------------------

TEST_F(OpcUaBinarySerialization, Sequence)
{
  OpcUa::Binary::SequenceHeader header;
  header.SequenceNumber = 0x04050607;
  header.RequestId = 0x1;

  const std::vector<char> expectedData = {
  7, 6, 5, 4,
  1, 0, 0, 0
  };

  GetStream() << header << flush;
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//-------------------------------------------------------------------
// AdditionalHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, AdditionalHeader)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  AdditionalHeader header;
  header.TypeId.Encoding = static_cast<NodeIdEncoding>(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_Server_INDEX_FLAG);
  header.TypeId.StringData.NamespaceIndex = 0x1;
  header.TypeId.StringData.Identifier = "id";
  header.TypeId.NamespaceURI = "uri";
  header.TypeId.ServerIndex = 1;
  header.Encoding = 1;

  const std::vector<char> expectedData = {
  int8_t(EV_STRING | EV_NAMESPACE_URI_FLAG | EV_Server_INDEX_FLAG),
  1, 0,
  2, 0, 0, 0,
  'i', 'd',
  3, 0, 0, 0,
  'u', 'r', 'i',
  1, 0, 0, 0,
  1
  };

  GetStream() << header << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//-------------------------------------------------------------------
// RequestHeader
//-------------------------------------------------------------------


TEST_F(OpcUaBinarySerialization, RequestHeader)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  RequestHeader header;
  FILL_TEST_REQUEST_HEADER(header);

  GetStream() << header << flush;

  const std::vector<char> expectedData = {
   TEST_REQUEST_HEADER_BINARY_DATA
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//-------------------------------------------------------------------
// DiagnosticInfo
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_Empty)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = DIM_NONE;

  GetStream() << info << flush;

  const std::vector<char> expectedData = {
  0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_SymbolicId)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = DIM_SYMBOLIC_Id;
  info.SymbolicId = 2;

  GetStream() << info << flush;

  const std::vector<char> expectedData = {
  DIM_SYMBOLIC_Id,
  2, 0, 0, 0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_SymbolicId_Namespace)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_SYMBOLIC_Id | DIM_NAMESPACE);
  info.SymbolicId = 2;
  info.NamespaceURI = 3;

  GetStream() << info << flush;

  const std::vector<char> expectedData = {
  DIM_SYMBOLIC_Id | DIM_NAMESPACE,
  2, 0, 0, 0,
  3, 0, 0, 0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_SymbolicId_LocalizedText)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_SYMBOLIC_Id | DIM_LOCALIZED_TEXT);
  info.SymbolicId = 2;
  info.LocalizedText = 4;

  GetStream() << info << flush;

  const std::vector<char> expectedData = {
  DIM_SYMBOLIC_Id | DIM_LOCALIZED_TEXT,
  2, 0, 0, 0,
  4, 0, 0, 0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_LocalizedText_Locale)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_LOCALE);
  info.LocalizedText = 2;
  info.Locale = 4;

  GetStream() << info <<flush;

  const std::vector<char> expectedData = {
  DIM_LOCALIZED_TEXT | DIM_LOCALE,
  2, 0, 0, 0,
  4, 0, 0, 0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_AdditionalInfo)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = DIM_ADDITIONAL_INFO;
  info.AdditionalInfo = "add";

  GetStream() << info << flush;

  const std::vector<char> expectedData = {
  DIM_ADDITIONAL_INFO,
  3,0,0,0,
  'a', 'd', 'd'
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

TEST_F(OpcUaBinarySerialization, DiagnosticInfo_InnerStatusCode)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  DiagnosticInfo info;
  info.EncodingMask = DIM_INNER_STATUS_CODE;
  info.InnerStatusCode = static_cast<StatusCode>(6);

  GetStream() << info << flush;

  const std::vector<char> expectedData = {
  DIM_INNER_STATUS_CODE,
  6,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(info));
}

//-------------------------------------------------------------------
// ResponseHeader
//-------------------------------------------------------------------



TEST_F(OpcUaBinarySerialization, ResponseHeader)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ResponseHeader header;
  FILL_TEST_RESPONSE_HEADER(header);

  GetStream() << header << flush;

  const std::vector<char> expectedData = {
    TEST_RESPONSE_HEADER_BINARY_DATA
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//-------------------------------------------------------------------
// OpenSecureChannelRequset
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, OpenSequreChannelRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  OpenSecureChannelRequest request;
  FILL_TEST_REQUEST_HEADER(request.Header);
  
  request.Parameters.ClientProtocolVersion = 1;
  request.Parameters.RequestType = SecurityTokenRequestType::Renew;
  request.Parameters.SecurityMode = MessageSecurityMode::Sign;
  request.Parameters.ClientNonce = std::vector<uint8_t>(1, 1);
  request.Parameters.RequestLifeTime = 5;

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xbe, 0x1, // TypeId

  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1, 0, 0, 0,
  (uint32_t)SecurityTokenRequestType::Renew,  0, 0, 0,
  (uint32_t)MessageSecurityMode::Sign, 0, 0, 0,
  1, 0, 0, 0,
  1,
  5, 0, 0, 0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

//-------------------------------------------------------------------
// SecurityToken
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, SecurityToken)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  SecurityToken token;

  token.SecureChannelId = 1;
  token.TokenId = 2;
  token.CreatedAt.Value = 3;
  token.RevisedLifetime = 4;

  GetStream() << token << flush;

  const std::vector<char> expectedData = {
  1, 0, 0, 0,
  2,  0, 0, 0,
  3, 0, 0, 0, 0, 0, 0, 0,
  4, 0, 0, 0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(token));
}

//-------------------------------------------------------------------
// OpenSecureChannelResponse
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, OpenSecureChannelResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  OpenSecureChannelResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::OPEN_SECURE_CHANNEL_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.ServerProtocolVersion = 1;
  response.ChannelSecurityToken.SecureChannelId = 2;
  response.ChannelSecurityToken.TokenId = 3;
  response.ChannelSecurityToken.CreatedAt.Value = 4;
  response.ChannelSecurityToken.RevisedLifetime = 5;
  response.ServerNonce = std::vector<uint8_t>(1, 6);
  
  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xC1, 0x1, // TypeId

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

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

//-------------------------------------------------------------------
// CloseSecureChannelRequset
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CloseSequreChannelRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CloseSecureChannelRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CLOSE_SECURE_CHANNEL_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);
  
  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xc4, 0x1, // TypeId

  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

//----------------------------------------------------------------
// SignatureData
//----------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, SignatureData)
{
  OpcUa::SignatureData s;
  s.Signature = {1,2,3,4};
  s.Algorithm = "aes";
 
  const std::vector<char> expectedData = {
    4, 0, 0, 0, 1,2,3,4,      // Signature
    3, 0, 0, 0, 'a','e','s'   // Algorithm
  };

  GetStream() << s << flush;

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(s));
}

//-------------------------------------------------------------------
// ExtensionObjectHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ExtensionObjectHeader)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ExtensionObjectHeader header(OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS, HAS_BINARY_BODY);

  GetStream() << header << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x41, 0x1, // TypeId
  HAS_BINARY_BODY
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(header));
}

//-------------------------------------------------------------------
// QualifiedName
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, QualifiedName)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  QualifiedName name;

  name.NamespaceIndex = 1;
  name.Name = "name";

  GetStream() << name << flush;

  const std::vector<char> expectedData = {
  1, 0,
  4, 0, 0, 0, 'n','a','m','e'
  };

  ASSERT_EQ(expectedData.size(), RawSize(name));
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}

//-------------------------------------------------------------------
// IntegerId
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, IntegerId)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  IntegerId id(5);

  GetStream() << id << flush;

  const std::vector<char> expectedData = {
  5, 0, 0, 0
  };

  ASSERT_EQ(expectedData.size(), RawSize(id));
  ASSERT_EQ(expectedData, GetChannel().SerializedData);
}
