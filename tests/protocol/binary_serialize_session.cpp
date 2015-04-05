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

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/session.h>

#include <algorithm>
#include <stdexcept>



//-------------------------------------------------------
// CreateSessionRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CreateSessionRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateSessionRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  FILL_APPLICATION_DESCRIPTION(request.Parameters.ClientDescription);

  request.Parameters.ServerURI = "su";
  request.Parameters.EndpointURL = "eu";
  request.Parameters.SessionName = "sn";
  request.Parameters.ClientNonce =  {1,2,3,4};
  request.Parameters.ClientCertificate = {5,6,7,8};
  request.Parameters.RequestedSessionTimeout = 1200000;
  request.Parameters.MaxResponseMessageSize = 2;

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xcd, 0x1, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  TEST_APPLICATION_DESCRIPTION_BINARY_DATA,
  2,0,0,0, 's','u',
  2,0,0,0, 'e','u',
  2,0,0,0, 's','n',
  4,0,0,0, 1,2,3,4,
  4,0,0,0, 5,6,7,8,
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
  2,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, CreateSessionRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xcd, 0x1, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  TEST_APPLICATION_DESCRIPTION_BINARY_DATA,
  2,0,0,0, 's','u',
  2,0,0,0, 'e','u',
  2,0,0,0, 's','n',
  4,0,0,0, 1,2,3,4,
  4,0,0,0, 5,6,7,8,
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
  2,0,0,0
  };

  GetChannel().SetData(expectedData);

  CreateSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);
  ASSERT_APPLICATION_DESCRIPTION_EQ(request.Parameters.ClientDescription);

  ASSERT_EQ(request.Parameters.ServerURI, "su");
  ASSERT_EQ(request.Parameters.EndpointURL, "eu");
  ASSERT_EQ(request.Parameters.SessionName, "sn");
  std::vector<uint8_t> clientNonce = {1,2,3,4};
  ASSERT_EQ(request.Parameters.ClientNonce, clientNonce);
  CertificateData cert = {5,6,7,8};
  ASSERT_EQ(request.Parameters.ClientCertificate, cert);
  ASSERT_EQ(request.Parameters.RequestedSessionTimeout, 1200000);
  ASSERT_EQ(request.Parameters.MaxResponseMessageSize, 2);
}


//----------------------------------------------------
// CreateSessionResponse
//----------------------------------------------------

TEST_F(OpcUaBinarySerialization, CreateSessionResponse)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CreateSessionResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.Session.SessionId.Encoding = EV_FOUR_BYTE;
  response.Session.SessionId.FourByteData.NamespaceIndex = 1;
  response.Session.SessionId.FourByteData.Identifier = 2;

  response.Session.AuthenticationToken.Encoding = EV_FOUR_BYTE;
  response.Session.AuthenticationToken.FourByteData.NamespaceIndex = 1;
  response.Session.AuthenticationToken.FourByteData.Identifier = 2;

  response.Session.RevisedSessionTimeout = 1200000;
  response.Session.ServerNonce = {1,2,3,4};
  response.Session.ServerCertificate = {5,6,7,8};
  EndpointDescription e;
  FILL_TEST_ENDPOINT(e);
  response.Session.ServerEndpoints.push_back(e);
  response.Session.SignedServerCertificates.push_back({4,3,2,1});
  response.Session.ServerSignature.Signature = {7,6,5,4};
  response.Session.ServerSignature.Algorithm = "aes";
  
  response.Session.MaxRequestMessageSize = 0x1000;

  GetStream() << response << flush;


  const std::vector<char> expectedData = {
  1, 0, (char)0xd0, 0x1, // TypeId
  // ResponseHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,1,2,0,
  1,1,2,0,
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
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
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd0, 0x1, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,1,2,0,
  1,1,2,0,
  0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
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

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Session.SessionId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.Session.SessionId.FourByteData.NamespaceIndex, 1);
  ASSERT_EQ(response.Session.SessionId.FourByteData.Identifier, 2);

  ASSERT_EQ(response.Session.AuthenticationToken.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.Session.AuthenticationToken.FourByteData.NamespaceIndex, 1);
  ASSERT_EQ(response.Session.AuthenticationToken.FourByteData.Identifier, 2);

  ASSERT_EQ(response.Session.RevisedSessionTimeout, 1200000);

  std::vector<uint8_t> serverNonce = {1,2,3,4};
  ASSERT_EQ(response.Session.ServerNonce, serverNonce);

  std::vector<uint8_t> cert = {5,6,7,8};
  ASSERT_EQ(response.Session.ServerCertificate, cert) ;

  ASSERT_EQ(response.Session.ServerEndpoints.size(), 1);
  ASSERT_ENDPOINT_EQ(response.Session.ServerEndpoints[0]);

  std::vector<CertificateData> certs = {{4,3,2,1}};
  ASSERT_EQ(response.Session.SignedServerCertificates, certs);

  std::vector<uint8_t> signature = {7,6,5,4};
  ASSERT_EQ(response.Session.ServerSignature.Signature, signature);
  ASSERT_EQ(response.Session.ServerSignature.Algorithm, "aes");

  ASSERT_EQ(response.Session.MaxRequestMessageSize, 0x1000);
}

//-------------------------------------------------------------------
// ExtensionObjectHeader
//-------------------------------------------------------------------

TEST_F(OpcUaBinarySerialization, UserIdentifyToken_Anonymous)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  UserIdentifyToken token;
  token.setPolicyId("0");
/*
  ASSERT_EQ(token.Header.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(token.Header.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(token.Hader.TypeId.FourByteData.Identifier, USER_IdENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(token.Header.Encoding, HAS_BINARY_BODY);
  ASSERT_EQ(token.Anonymous.Data, std::vector{});
*/
  GetStream() << token << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x41, 0x1, // TypeId
  HAS_BINARY_BODY,
  0x5,0,0,0, 1,0,0,0,'0'
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(token));
}

TEST_F(OpcUaBinaryDeserialization, UserIdentifyToken_Anonymous)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  const std::vector<char> expectedData = {
  1, 0, (char)0x41, 0x1, // TypeId
  HAS_BINARY_BODY,
  0x5,0,0,0, 1,0,0,0,'0'
  };

  GetChannel().SetData(expectedData);

  UserIdentifyToken token;
  GetStream() >> token;

  ASSERT_EQ(token.Header.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(token.Header.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(token.Header.TypeId.FourByteData.Identifier, OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(token.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> policy_id = {1,0,0,0,'0'};
  ASSERT_EQ(token.PolicyId, policy_id);
}


//-------------------------------------------------------
// ActivateSessionRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ActivateSessionRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ActivateSessionRequest request;
  request.Parameters.IdentifyToken.setPolicyId("0");

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  ASSERT_EQ(request.Parameters.IdentifyToken.Header.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.Parameters.IdentifyToken.Header.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.Parameters.IdentifyToken.Header.TypeId.FourByteData.Identifier, OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(request.Parameters.IdentifyToken.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> policy_id = {1,0,0,0,'0'};
  ASSERT_EQ(request.Parameters.IdentifyToken.PolicyId, policy_id);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd3, 0x1, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  -1,-1,-1,-1,
  -1,-1,-1,-1,

  0,0,0,0,
  0,0,0,0,

  1, 0, (char)0x41, 0x1, // TypeId
  1,
  0x5,0,0,0, 1,0,0,0,'0',
  -1,-1,-1,-1,
  -1,-1,-1,-1
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, ActivateSessionRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd3, 0x1, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  -1,-1,-1,-1,
  -1,-1,-1,-1,

  1,0,0,0, 1,0,0,0, 1,
  0,0,0,0,

  1, 0, (char)0x41, 0x1, // TypeId
  1,
  0x5,0,0,0, 1,0,0,0,'0',
  -1,-1,-1,-1,
  -1,-1,-1,-1
  };

  GetChannel().SetData(expectedData);

  ActivateSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.ClientCertificates.size(), 1);
  ASSERT_EQ(request.Parameters.ClientCertificates[0].size(), 1);
  ASSERT_EQ(request.Parameters.ClientCertificates[0][0], 1);

  ASSERT_EQ(request.Parameters.IdentifyToken.Header.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.Parameters.IdentifyToken.Header.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.Parameters.IdentifyToken.Header.TypeId.FourByteData.Identifier, OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(request.Parameters.IdentifyToken.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> policy_id = {1,0,0,0,'0'};
  ASSERT_EQ(request.Parameters.IdentifyToken.PolicyId, policy_id);
}

//-------------------------------------------------------
// ActivateSessionResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, ActivateSessionResponse)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ActivateSessionResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);
  response.Session.ServerNonce = std::vector<uint8_t>(1,1);
  response.Session.StatusCodes = std::vector<uint32_t>(2,1);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd6, 0x1, // TypeId
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
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd6, 0x1, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,0,0,0, 1,
  2,0,0,0, 1,0,0,0, 1,0,0,0,
  0,0,0,0,
  };

  GetChannel().SetData(expectedData);

  ActivateSessionResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Session.ServerNonce, std::vector<uint8_t>(1,1));
  ASSERT_EQ(response.Session.StatusCodes, std::vector<uint32_t>(2,1));
  ASSERT_TRUE(response.Session.Diagnostics.empty());
}

//-------------------------------------------------------
// CloseSessionRequest
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CloseSessionRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CloseSessionRequest request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CLOSE_SESSION_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd9, 0x1, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  1
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, CloseSessionRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xd9, 0x1, // TypeId
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,
  1,
  };

  GetChannel().SetData(expectedData);

  CloseSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CLOSE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.DeleteSubscriptions, true);
}

//-------------------------------------------------------
// CloseSessionResponse
//-------------------------------------------------------

TEST_F(OpcUaBinarySerialization, CloseSessionResponse)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  CloseSessionResponse response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CLOSE_SESSION_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0xdc, 0x1, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  };

  ASSERT_EQ(expectedData.size(), RawSize(response));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(OpcUaBinaryDeserialization, CloseSessionResponse)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0xdc, 0x1, // TypeId
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,
  1,
  };

  GetChannel().SetData(expectedData);

  CloseSessionResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CLOSE_SESSION_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);
}


