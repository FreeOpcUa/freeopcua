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

  request.Parameters.ServerUri = "su";
  request.Parameters.EndpointUrl = "eu";
  request.Parameters.SessionName = "sn";
  request.Parameters.ClientNonce =  ByteString(std::vector<uint8_t> {1, 2, 3, 4});
  request.Parameters.ClientCertificate = ByteString(std::vector<uint8_t> {5, 6, 7, 8});
  request.Parameters.RequestedSessionTimeout = 1200000;
  request.Parameters.MaxResponseMessageSize = 2;

  GetStream() << request << flush;

  const std::vector<char> expectedData =
  {
    1, 0, (char)0xcd, 0x1, // TypeId
    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,
    TEST_APPLICATION_DESCRIPTION_BINARY_DATA,
    2, 0, 0, 0, 's', 'u',
    2, 0, 0, 0, 'e', 'u',
    2, 0, 0, 0, 's', 'n',
    4, 0, 0, 0, 1, 2, 3, 4,
    4, 0, 0, 0, 5, 6, 7, 8,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2, 0, 0, 0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(OpcUaBinaryDeserialization, CreateSessionRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData =
  {
    1, 0, (char)0xcd, 0x1, // TypeId
    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,
    TEST_APPLICATION_DESCRIPTION_BINARY_DATA,
    2, 0, 0, 0, 's', 'u',
    2, 0, 0, 0, 'e', 'u',
    2, 0, 0, 0, 's', 'n',
    4, 0, 0, 0, 1, 2, 3, 4,
    4, 0, 0, 0, 5, 6, 7, 8,
    0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
    2, 0, 0, 0
  };

  GetChannel().SetData(expectedData);

  CreateSessionRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);
  ASSERT_APPLICATION_DESCRIPTION_EQ(request.Parameters.ClientDescription);

  ASSERT_EQ(request.Parameters.ServerUri, "su");
  ASSERT_EQ(request.Parameters.EndpointUrl, "eu");
  ASSERT_EQ(request.Parameters.SessionName, "sn");
  ByteString clientNonce = ByteString(std::vector<uint8_t> {1, 2, 3, 4});
  ASSERT_EQ(request.Parameters.ClientNonce, clientNonce);
  ByteString cert = ByteString(std::vector<uint8_t> {5, 6, 7, 8});
  ASSERT_EQ(request.Parameters.ClientCertificate, cert);
  ASSERT_EQ(request.Parameters.RequestedSessionTimeout, 1200000);
  ASSERT_EQ(request.Parameters.MaxResponseMessageSize, 2);
}


//----------------------------------------------------
// CreateSessionResponse
//----------------------------------------------------

// TEST_F(OpcUaBinarySerialization, CreateSessionResponse)
// {
//
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   CreateSessionResponse response;
//
//   ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);
//
//   FILL_TEST_RESPONSE_HEADER(response.Header);
//
//   response.Parameters.SessionId.Encoding = EV_FOUR_BYTE;
//   response.Parameters.SessionId.FourByteData.NamespaceIndex = 1;
//   response.Parameters.SessionId.FourByteData.Identifier = 2;
//
//   response.Parameters.AuthenticationToken.Encoding = EV_FOUR_BYTE;
//   response.Parameters.AuthenticationToken.FourByteData.NamespaceIndex = 1;
//   response.Parameters.AuthenticationToken.FourByteData.Identifier = 2;
//
//   response.Parameters.RevisedSessionTimeout = 1200000;
//   response.Parameters.ServerNonce = ByteString(std::vector<uint8_t>{1,2,3,4});
//   response.Parameters.ServerCertificate = ByteString(std::vector<uint8_t>{5,6,7,8});
//   EndpointDescription e;
//   FILL_TEST_ENDPOINT(e);
//   response.Parameters.ServerEndpoints.push_back(e);
//   SignedSoftwareCertificate cert;
//   cert.CertificateData = ByteString(std::vector<uint8_t>{4,3,2,1});
//   cert.Signature = ByteString(std::vector<uint8_t>{8,7,6,5});
//   response.Parameters.ServerSoftwareCertificates.push_back(cert);
//   response.Parameters.ServerSignature.Signature = ByteString(std::vector<uint8_t>{7,6,5,4});
//   response.Parameters.ServerSignature.Algorithm = "aes";
//
//   response.Parameters.MaxRequestMessageSize = 0x1000;
//
//   GetStream() << response << flush;
//
//
//   const std::vector<char> expectedData = {
//   1, 0, (char)0xd0, 0x1, // TypeId
//   // ResponseHeader
//   TEST_RESPONSE_HEADER_BINARY_DATA,
//   1,1,2,0,
//   1,1,2,0,
//   0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
//   4,0,0,0, 1,2,3,4,
//   4,0,0,0, 5,6,7,8,
//   1,0,0,0,
//   TEST_ENDPOINT_BINARY_DATA,
//   1,0,0,0, 4,0,0,0, 4,3,2,1, 4,0,0,0, 8,7,6,5,
//   4,0,0,0, 7,6,5,4,
//   3,0,0,0, 'a','e','s',
//   0,0x10,0,0
//   };
//
//   ASSERT_EQ(expectedData, GetChannel().SerializedData);
//   ASSERT_EQ(expectedData.size(), RawSize(response));
// }
//
// TEST_F(OpcUaBinaryDeserialization, CreateSessionResponse)
// {
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   const std::vector<char> expectedData = {
//   1, 0, (char)0xd0, 0x1, // TypeId
//   // RequestHeader
//   TEST_RESPONSE_HEADER_BINARY_DATA,
//   1,1,2,0,
//   1,1,2,0,
//   0, 0, 0, 0, (char)0x80, (char)0x4f, (char)0x32, (char)0x41,
//   4,0,0,0, 1,2,3,4,
//   4,0,0,0, 5,6,7,8,
//   1,0,0,0,
//   TEST_ENDPOINT_BINARY_DATA,
//   1,0,0,0, 4,0,0,0, 4,3,2,1, 4,0,0,0, 8,7,6,5,
//   4,0,0,0, 7,6,5,4,
//   3,0,0,0, 'a','e','s',
//   0,0x10,0,0
//   };
//
//   GetChannel().SetData(expectedData);
//
//   CreateSessionResponse response;
//   GetStream() >> response;
//
//   ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);
//
//   ASSERT_RESPONSE_HEADER_EQ(response.Header);
//
//   ASSERT_EQ(response.Parameters.SessionId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(response.Parameters.SessionId.FourByteData.NamespaceIndex, 1);
//   ASSERT_EQ(response.Parameters.SessionId.FourByteData.Identifier, 2);
//
//   ASSERT_EQ(response.Parameters.AuthenticationToken.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(response.Parameters.AuthenticationToken.FourByteData.NamespaceIndex, 1);
//   ASSERT_EQ(response.Parameters.AuthenticationToken.FourByteData.Identifier, 2);
//
//   ASSERT_EQ(response.Parameters.RevisedSessionTimeout, 1200000);
//
//   ByteString serverNonce = ByteString(std::vector<uint8_t>{1,2,3,4});
//   ASSERT_EQ(response.Parameters.ServerNonce, serverNonce);
//
//   ByteString serverCert = ByteString(std::vector<uint8_t>{5,6,7,8});
//   ASSERT_EQ(response.Parameters.ServerCertificate, serverCert);
//
//   ASSERT_EQ(response.Parameters.ServerEndpoints.size(), 1);
//   ASSERT_ENDPOINT_EQ(response.Parameters.ServerEndpoints[0]);
//
//   SignedSoftwareCertificate cert;
//   cert.CertificateData = ByteString(std::vector<uint8_t>{4,3,2,1});
//   cert.Signature = ByteString(std::vector<uint8_t>{8,7,6,5});
//   std::vector<SignedSoftwareCertificate> certs = {cert};
// //   ASSERT_EQ(response.Parameters.ServerSoftwareCertificates, certs);
//
//   ByteString signature = ByteString(std::vector<uint8_t>{7,6,5,4});
//   ASSERT_EQ(response.Parameters.ServerSignature.Signature, signature);
//   ASSERT_EQ(response.Parameters.ServerSignature.Algorithm, "aes");
//
//   ASSERT_EQ(response.Parameters.MaxRequestMessageSize, 0x1000);
// }

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

  const std::vector<char> expectedData =
  {
    1, 0, (char)0x41, 0x1, // TypeId
    HAS_BINARY_BODY,
    0x5, 0, 0, 0, 1, 0, 0, 0, '0'
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(token));
}

TEST_F(OpcUaBinaryDeserialization, UserIdentifyToken_Anonymous)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  const std::vector<char> expectedData =
  {
    1, 0, (char)0x41, 0x1, // TypeId
    HAS_BINARY_BODY,
    0x5, 0, 0, 0, 1, 0, 0, 0, '0'
  };

  GetChannel().SetData(expectedData);

  UserIdentifyToken token;
  GetStream() >> token;

  ASSERT_EQ(token.Header.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(token.Header.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(token.Header.TypeId.FourByteData.Identifier, OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS);
  ASSERT_EQ(token.Header.Encoding, HAS_BINARY_BODY);
  std::vector<uint8_t> policy_id = {'0'};
  ASSERT_EQ(token.PolicyId, policy_id);
}


//-------------------------------------------------------
// ActivateSessionRequest
//-------------------------------------------------------

// TEST_F(OpcUaBinarySerialization, ActivateSessionRequest)
// {
//
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   ActivateSessionRequest request;
//   request.Parameters.UserIdentityToken.setPolicyId("0");
//
//   ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_REQUEST);
//
//   FILL_TEST_REQUEST_HEADER(request.Header);
//
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.TypeId.FourByteData.Identifier, OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS);
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.Encoding, HAS_BINARY_BODY);
//   std::vector<uint8_t> policy_id = {1,0,0,0,'0'};
//   ASSERT_EQ(request.Parameters.UserIdentityToken.PolicyId, policy_id);
//
//   GetStream() << request << flush;
//
//   const std::vector<char> expectedData = {
//   1, 0, (char)0xd3, 0x1, // TypeId
//   // RequestHeader
//   TEST_REQUEST_HEADER_BINARY_DATA,
//   -1,-1,-1,-1,
//   -1,-1,-1,-1,
//
//   0,0,0,0,
//   0,0,0,0,
//
//   1, 0, (char)0x41, 0x1, // TypeId
//   1,
//   0x5,0,0,0, 1,0,0,0,'0',
//   -1,-1,-1,-1,
//   -1,-1,-1,-1
//   };
//
//   ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
//   ASSERT_EQ(expectedData.size(), RawSize(request));
// }
//
// TEST_F(OpcUaBinaryDeserialization, ActivateSessionRequest)
// {
//
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   const std::vector<char> expectedData = {
//   1, 0, (char)0xd3, 0x1, // TypeId
//   // RequestHeader
//   TEST_REQUEST_HEADER_BINARY_DATA,
//   -1,-1,-1,-1,
//   -1,-1,-1,-1,
//
//   1,0,0,0, 1,0,0,0, 1,
//   0,0,0,0,
//
//   1, 0, (char)0x41, 0x1, // TypeId
//   1,
//   0x5,0,0,0, 1,0,0,0,'0',
//   -1,-1,-1,-1,
//   -1,-1,-1,-1
//   };
//
//   GetChannel().SetData(expectedData);
//
//   ActivateSessionRequest request;
//   GetStream() >> request;
//
//   ASSERT_EQ(request.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(request.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(request.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_REQUEST);
//
//   ASSERT_REQUEST_HEADER_EQ(request.Header);
//
//   ASSERT_EQ(request.Parameters.ClientSoftwareCertificates.size(), 1);
//   ASSERT_EQ(request.Parameters.ClientSoftwareCertificates[0].CertificateData, ByteString(std::vector<uint8_t>{1}));
//
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.TypeId.FourByteData.Identifier, OpcUa::USER_IdENTIFY_TOKEN_ANONYMOUS);
//   ASSERT_EQ(request.Parameters.UserIdentityToken.Header.Encoding, HAS_BINARY_BODY);
//   std::vector<uint8_t> policy_id = {1,0,0,0,'0'};
//   ASSERT_EQ(request.Parameters.UserIdentityToken.PolicyId, policy_id);
// }

//-------------------------------------------------------
// ActivateSessionResponse
//-------------------------------------------------------

// TEST_F(OpcUaBinarySerialization, ActivateSessionResponse)
// {
//
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   ActivateSessionResponse response;
//
//   ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_RESPONSE);
//
//   FILL_TEST_RESPONSE_HEADER(response.Header);
//   response.Parameters.ServerNonce = ByteString(std::vector<uint8_t>{1,1});
//   std::vector<StatusCode> results;
//   results.push_back(StatusCode(1));
//   results.push_back(StatusCode(2));
//   response.Parameters.Results = results;
//
//   GetStream() << response << flush;
//
//   const std::vector<char> expectedData = {
//   1, 0, (char)0xd6, 0x1, // TypeId
//   // RequestHeader
//   TEST_RESPONSE_HEADER_BINARY_DATA,
//   1,0,0,0, 1,
//   2,0,0,0, 1,0,0,0, 1,0,0,0,
//   0,0,0,0,
//   };
//
//   ASSERT_EQ(expectedData.size(), RawSize(response));
//   ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
// }
//
// TEST_F(OpcUaBinaryDeserialization, ActivateSessionResponse)
// {
//   using namespace OpcUa;
//   using namespace OpcUa::Binary;
//
//   const std::vector<char> expectedData = {
//   1, 0, (char)0xd6, 0x1, // TypeId
//   // RequestHeader
//   TEST_RESPONSE_HEADER_BINARY_DATA,
//   1,0,0,0, 1,
//   2,0,0,0, 1,0,0,0, 1,0,0,0,
//   0,0,0,0,
//   };
//
//   GetChannel().SetData(expectedData);
//
//   ActivateSessionResponse response;
//   GetStream() >> response;
//
//   ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
//   ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
//   ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::ACTIVATE_SESSION_RESPONSE);
//
//   ASSERT_RESPONSE_HEADER_EQ(response.Header);
//
//   ASSERT_EQ(response.Parameters.ServerNonce, ByteString(std::vector<uint8_t>{1,1}));
//   std::vector<StatusCode> results;
//   results.push_back(StatusCode(1));
//   results.push_back(StatusCode(2));
//   ASSERT_EQ(response.Parameters.Results, results);
//   ASSERT_TRUE(response.Parameters.DiagnosticInfos.empty());
// }

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

  const std::vector<char> expectedData =
  {
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

  const std::vector<char> expectedData =
  {
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

  const std::vector<char> expectedData =
  {
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

  const std::vector<char> expectedData =
  {
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


