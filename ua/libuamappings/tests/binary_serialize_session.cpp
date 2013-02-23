/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "common.h"

#include <opc/ua/extension_identifiers.h>
#include <opc/ua/message_identifiers.h>
#include <opc/ua/binary/stream.h>
#include <opc/ua/binary/types.h>
#include <opc/ua/binary/protocol/session.h>

#include <algorithm>
#include <stdexcept>



//-------------------------------------------------------
// CreateSessionRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CreateSessionRequest)
{

  using namespace OpcUa::Binary;

  CreateSessionRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::CREATE_SESSION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  FILL_APPLICATION_DESCRIPTION(request.ClientDescription);

  request.ServerURI = "su";
  request.EndpointURL = "eu";
  request.SessionName = "sn";
  request.ClientNonce =  {1,2,3,4};
  request.ClientCertificate = {5,6,7,8};
  request.RequestedSessionTimeout = 1;
  request.MaxResponseMessageSize = 2;

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xcd, 0x1, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  TEST_APPLICATION_DESCRIPTION_BINARY_DATA,
  2,0,0,0, 's','u',
  2,0,0,0, 'e','u',
  2,0,0,0, 's','n',
  4,0,0,0, 1,2,3,4,
  4,0,0,0, 5,6,7,8,
  1,0,0,0,0,0,0,0,
  2,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, CreateSessionRequest)
{
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xcd, 0x1, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  TEST_APPLICATION_DESCRIPTION_BINARY_DATA,
  2,0,0,0, 's','u',
  2,0,0,0, 'e','u',
  2,0,0,0, 's','n',
  4,0,0,0, 1,2,3,4,
  4,0,0,0, 5,6,7,8,
  1,0,0,0,0,0,0,0,
  2,0,0,0
  };

  GetChannel().SetData(expectedData);

  CreateSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::CREATE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);
  ASSERT_APPLICATION_DESCRIPTION_EQ(request.ClientDescription);

  ASSERT_EQ(request.ServerURI, "su");
  ASSERT_EQ(request.EndpointURL, "eu");
  ASSERT_EQ(request.SessionName, "sn");
  std::vector<uint8_t> clientNonce = {1,2,3,4};
  ASSERT_EQ(request.ClientNonce, clientNonce);
  CertificateData cert = {5,6,7,8};
  ASSERT_EQ(request.ClientCertificate, cert);
  ASSERT_EQ(request.RequestedSessionTimeout, 1);
  ASSERT_EQ(request.MaxResponseMessageSize, 2);
}


//----------------------------------------------------
// CreateSessionResponse
//----------------------------------------------------

TEST_F(OpcUaBinarySerialization, CreateSessionResponse)
{

  using namespace OpcUa::Binary;

  CreateSessionResponse response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.SessionID.Encoding = EV_FOUR_BYTE;
  response.SessionID.FourByteData.NamespaceIndex = 1;
  response.SessionID.FourByteData.Identifier = 2;

  response.AuthenticationToken.Encoding = EV_FOUR_BYTE;
  response.AuthenticationToken.FourByteData.NamespaceIndex = 1;
  response.AuthenticationToken.FourByteData.Identifier = 2;

  response.RevisedSessionTimeout = 3;
  response.ServerNonce = {1,2,3,4};
  response.ServerCertificate = {5,6,7,8};
  EndpointDescription e;
  FILL_TEST_ENDPOINT(e);
  response.ServerEndpoints.push_back(e);
  response.SignedServerCertificates.push_back({4,3,2,1});
  response.ServerSignature.Signature = {7,6,5,4};
  response.ServerSignature.Algorithm = "aes";
  
  response.MaxRequestMessageSize = 0x1000;

  GetStream() << response << flush;


  const std::vector<char> expectedData = {
  1, 0, (char)0xd0, 0x1, // TypeID
  // ResponseHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,1,2,0,
  1,1,2,0,
  3,0,0,0,0,0,0,0,
  4,0,0,0, 1,2,3,4,
  4,0,0,0, 5,6,7,8,
  1,0,0,0,
  TEST_ENDPOINT_BINARY_DATA,
  1,0,0,0, 4,0,0,0, 4,3,2,1,
  4,0,0,0, 7,6,5,4,
  3,0,0,0, 'a','e','s',
  0,0x10,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(OpcUaBinaryDeserialization, CreateSessionResponse)
{
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd0, 0x1, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,1,2,0,
  1,1,2,0,
  3,0,0,0,0,0,0,0,
  4,0,0,0, 1,2,3,4,
  4,0,0,0, 5,6,7,8,
  1,0,0,0,
  TEST_ENDPOINT_BINARY_DATA,
  1,0,0,0, 4,0,0,0, 4,3,2,1,
  4,0,0,0, 7,6,5,4,
  3,0,0,0, 'a','e','s',
  0,0x10,0,0
  };

  GetChannel().SetData(expectedData);

  CreateSessionResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.SessionID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.SessionID.FourByteData.NamespaceIndex, 1);
  ASSERT_EQ(response.SessionID.FourByteData.Identifier, 2);

  ASSERT_EQ(response.AuthenticationToken.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.AuthenticationToken.FourByteData.NamespaceIndex, 1);
  ASSERT_EQ(response.AuthenticationToken.FourByteData.Identifier, 2);

  ASSERT_EQ(response.RevisedSessionTimeout, 3);

  std::vector<uint8_t> serverNonce = {1,2,3,4};
  ASSERT_EQ(response.ServerNonce, serverNonce);

  std::vector<uint8_t> cert = {5,6,7,8};
  ASSERT_EQ(response.ServerCertificate, cert) ;

  ASSERT_EQ(response.ServerEndpoints.size(), 1);
  ASSERT_ENDPOINT_EQ(response.ServerEndpoints[0]);

  std::vector<CertificateData> certs = {{4,3,2,1}};
  ASSERT_EQ(response.SignedServerCertificates, certs);

  std::vector<uint8_t> signature = {7,6,5,4};
  ASSERT_EQ(response.ServerSignature.Signature, signature);
  ASSERT_EQ(response.ServerSignature.Algorithm, "aes");

  ASSERT_EQ(response.MaxRequestMessageSize, 0x1000);
}

//-------------------------------------------------------------------
// ExtensionObjectHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, UserIdentifyToken_Anonymous)
{
  using namespace OpcUa::Binary;

  UserIdentifyToken token;
/*
  ASSERT_EQ(token.Header.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(token.Header.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(token.Hader.TypeID.FourByteData.Identifier, USER_IDENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(token.Header.Encoding, HAS_BINARY_BODY);
  ASSERT_EQ(token.Anonymous.Data, std::vector{});
*/
  GetStream() << token << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x41, 0x1, // TypeID
  HAS_BINARY_BODY,
  0xd,0,0,0, 9,0,0,0,'A','n','o','n','y','m','o','u','s'
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(token));
}

TEST_F(OpcUaBinaryDeserialization, UserIdentifyToken_Anonymous)
{
  using namespace OpcUa::Binary;
  const std::vector<char> expectedData = {
  1, 0, (char)0x41, 0x1, // TypeID
  HAS_BINARY_BODY,
  0xd,0,0,0, 9,0,0,0,'A','n','o','n','y','m','o','u','s'
  };

  GetChannel().SetData(expectedData);

  UserIdentifyToken token;
  GetStream() >> token;

  ASSERT_EQ(token.Header.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(token.Header.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(token.Header.TypeID.FourByteData.Identifier, OpcUa::USER_IDENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(token.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> anonymous = {9,0,0,0,'A','n','o','n','y','m','o','u','s'};
  ASSERT_EQ(token.Anonymous.Data, anonymous);
}


//-------------------------------------------------------
// ActivateSessionRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ActivateSessionRequest)
{

  using namespace OpcUa::Binary;

  ActivateSessionRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  ASSERT_EQ(request.IdentifyToken.Header.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.IdentifyToken.Header.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.IdentifyToken.Header.TypeID.FourByteData.Identifier, OpcUa::USER_IDENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(request.IdentifyToken.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> anonymous = {9,0,0,0,'A','n','o','n','y','m','o','u','s'};
  ASSERT_EQ(request.IdentifyToken.Anonymous.Data, anonymous);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd3, 0x1, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  -1,-1,-1,-1,
  -1,-1,-1,-1,

  0,0,0,0,
  0,0,0,0,

  1, 0, (char)0x41, 0x1, // TypeID
  1,
  0x0d,0,0,0, 9,0,0,0,'A','n','o','n','y','m','o','u','s',
  -1,-1,-1,-1,
  -1,-1,-1,-1
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, ActivateSessionRequest)
{

  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd3, 0x1, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  -1,-1,-1,-1,
  -1,-1,-1,-1,

  1,0,0,0, 1,0,0,0, 1,
  0,0,0,0,

  1, 0, (char)0x41, 0x1, // TypeID
  1,
  0xd,0,0,0, 9,0,0,0,'A','n','o','n','y','m','o','u','s',
  -1,-1,-1,-1,
  -1,-1,-1,-1
  };

  GetChannel().SetData(expectedData);

  ActivateSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.ClientCertificates.size(), 1);
  ASSERT_EQ(request.ClientCertificates[0].size(), 1);
  ASSERT_EQ(request.ClientCertificates[0][0], 1);

  ASSERT_EQ(request.IdentifyToken.Header.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.IdentifyToken.Header.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.IdentifyToken.Header.TypeID.FourByteData.Identifier, OpcUa::USER_IDENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(request.IdentifyToken.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> anonymous = {9,0,0,0,'A','n','o','n','y','m','o','u','s'};
  ASSERT_EQ(request.IdentifyToken.Anonymous.Data, anonymous);
}

//-------------------------------------------------------
// ActivateSessionResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ActivateSessionResponse)
{

  using namespace OpcUa::Binary;

  ActivateSessionResponse response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);
  response.ServerNonce = std::vector<uint8_t>(1,1);
  response.StatusCodes = std::vector<uint32_t>(2,1);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd6, 0x1, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,0,0,0, 1,
  2,0,0,0, 1,0,0,0, 1,0,0,0,
  0,0,0,0,
  };

  ASSERT_EQ(expectedData.size(), RawSize(response));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, ActivateSessionResponse)
{
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd6, 0x1, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,0,0,0, 1,
  2,0,0,0, 1,0,0,0, 1,0,0,0,
  0,0,0,0,
  };

  GetChannel().SetData(expectedData);

  ActivateSessionResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.ServerNonce, std::vector<uint8_t>(1,1));
  ASSERT_EQ(response.StatusCodes, std::vector<uint32_t>(2,1));
  ASSERT_TRUE(response.DiagnosticInfos.empty());
}

//-------------------------------------------------------
// CloseSessionRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CloseSessionRequest)
{

  using namespace OpcUa::Binary;

  CloseSessionRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::CLOSE_SESSION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd9, 0x1, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  1
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, CloseSessionRequest)
{

  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd9, 0x1, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  1,
  };

  GetChannel().SetData(expectedData);

  CloseSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::CLOSE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.DeleteSubscriptions, true);
}

//-------------------------------------------------------
// CloseSessionResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CloseSessionResponse)
{

  using namespace OpcUa::Binary;

  CloseSessionResponse response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CLOSE_SESSION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xdc, 0x1, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  };

  ASSERT_EQ(expectedData.size(), RawSize(response));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, CloseSessionResponse)
{

  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xdc, 0x1, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,
  };

  GetChannel().SetData(expectedData);

  CloseSessionResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::CLOSE_SESSION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);
}


