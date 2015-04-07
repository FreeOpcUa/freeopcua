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

#include <gtest/gtest.h>
#include <opc/ua/attribute_ids.h>
#include <opc/ua/object_ids.h>
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/client/remote_connection.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/view.h>

namespace
{
  class OpcBinaryHandshake : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    } 
  };
}

using namespace OpcUa::Binary;

TEST_F(OpcBinaryHandshake, EstablishConnection)
{
  const std::string host = GetHost();
  const unsigned port = GetPort();
  std::unique_ptr<OpcUa::RemoteConnection> connect = OpcUa::Connect(host, port);
  ASSERT_EQ(connect->GetHost(), host);
  ASSERT_EQ(connect->GetPort(), port);
}

//-----------------------------------------------------------------------
// Hello
//-----------------------------------------------------------------------

TEST_F(OpcBinaryHandshake, SayingHello)
{
  std::shared_ptr<OpcUa::RemoteConnection> connection(OpcUa::Connect(GetHost(), GetPort()));
  OpcUa::Binary::OStream os(connection);
  OpcUa::Binary::IStream is(connection);

  OpcUa::Binary::Hello hello;
  hello.ProtocolVersion = 0;
  hello.ReceiveBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
  hello.SendBufferSize = OPCUA_DEFAULT_BUFFER_SIZE;
  hello.MaxMessageSize = OPCUA_DEFAULT_BUFFER_SIZE;
  hello.MaxChunkCount = 1;
  hello.EndpointUrl = GetEndpoint();

  OpcUa::Binary::Header hdr(OpcUa::Binary::MT_HELLO, OpcUa::Binary::CHT_SINGLE);
  hdr.AddSize(RawSize(hello));  

  os << hdr << hello << flush;

  OpcUa::Binary::Header ackHeader;
  is >> ackHeader;
  ASSERT_EQ(ackHeader.Type, OpcUa::Binary::MT_ACKNOWLEDGE);
  ASSERT_EQ(ackHeader.Chunk, OpcUa::Binary::CHT_SINGLE);

  OpcUa::Binary::Acknowledge ack;

  ASSERT_EQ(ackHeader.Size, RawSize(ack) + RawSize(ackHeader));

  ASSERT_NO_THROW(is >> ack);
}

//----------------------------------------------------------------------
// OpenSecureChannel
//----------------------------------------------------------------------

std::string GetEndpointUrl()
{
  std::stringstream s;
  s << "opc.tcp://" << GetHost() << ":" << GetPort();
  return s.str();
}

TEST_F(OpcBinaryHandshake, OpenSecureChannel_PolicyNone)
{
  std::shared_ptr<OpcUa::RemoteConnection> connection(OpcUa::Connect(GetHost(), GetPort()));

  OpcUa::Binary::SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::unique_ptr<OpcUa::IOChannel> secureChannel = OpcUa::Binary::CreateSecureChannel(connection, params);
}


//----------------------------------------------------------------------
// GetEndpoints
//----------------------------------------------------------------------

TEST_F(OpcBinaryHandshake, GetEndpoints)
{
  std::shared_ptr<OpcUa::RemoteConnection> connection(OpcUa::Connect(GetHost(), GetPort()));

  OpcUa::Binary::SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = OpcUa::Binary::CreateSecureChannel(connection, params);

  using OpcUa::Binary::flush;

  OpcUa::Binary::GetEndpointsRequest request;
  request.EndpointURL = params.EndpointUrl;
  OpcUa::Binary::IOStream io(secureChannel);
  io << request << flush;

  OpcUa::Binary::GetEndpointsResponse response;
  ASSERT_NO_THROW(io >> response);
}

//----------------------------------------------------------------------
// CreateSession
//----------------------------------------------------------------------

EndpointDescription GetEndpoint(OpcUa::Binary::IOStream& stream)
{
  OpcUa::Binary::GetEndpointsRequest request;
  request.EndpointURL = GetEndpointUrl();
  stream << request << flush;

  OpcUa::Binary::GetEndpointsResponse response;
  stream >> response;
  if (response.Endpoints.empty())
  {
    throw std::logic_error("Server returned empty list of endpoints");
  }
  return response.Endpoints.front();
}

TEST_F(OpcBinaryHandshake, GetCreateSession)
{
  using namespace OpcUa::Binary;
  std::shared_ptr<RemoteConnection> connection(Connect(GetHost(), GetPort()));

  SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = CreateSecureChannel(connection, params);

  IOStream stream(secureChannel);
  EndpointDescription endpoint = GetEndpoint(stream);

  CreateSessionRequest request;
  request.ClientDescription.URI = "http://treww.org/libopcua";
  request.ClientDescription.ProductURI = "http://treww.org/libopcua";
  request.ClientDescription.Name.Encoding = HAS_TEXT;
  request.ClientDescription.Name.Text = "libopcuamappings";
  request.ClientDescription.Type = ApplicationType::Client;

  request.ServerURI = endpoint.ServerDescription.URI;
  request.EndpointURL = endpoint.EndpointURL; // TODO make just endpoint.URL;
  request.SessionName = "libiocuamappings session test";
  request.ClientNonce = std::vector<uint8_t>(32,0);
//  request.ClientCertificate = GetCertificate();
  request.RequestedSessionTimeout = 3600000;
  request.MaxResponseMessageSize = 65536;

  stream << request << flush;

  OpcUa::Binary::CreateSessionResponse response;
  ASSERT_NO_THROW(stream >> response);

  ASSERT_EQ(response.TypeId.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeId.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeId.FourByteData.Identifier, OpcUa::CREATE_SESSION_RESPONSE);
}

//----------------------------------------------------------------------
// ActivateSession
//----------------------------------------------------------------------

OpcUa::Binary::CreateSessionResponse CreateSession(OpcUa::Binary::IOStream& stream)
{
  EndpointDescription endpoint = GetEndpoint(stream);

  CreateSessionRequest request;
  request.ClientDescription.URI = "http://treww.org/libopcua";
  request.ClientDescription.ProductURI = "http://treww.org/libopcua";
  request.ClientDescription.Name.Encoding = HAS_TEXT;
  request.ClientDescription.Name.Text = "libopcuamappings";
  request.ClientDescription.Type = ApplicationType::Client;

  request.ServerURI = endpoint.ServerDescription.URI;
  request.EndpointURL = endpoint.EndpointURL; // TODO make just endpoint.URL;
  request.SessionName = "libiocuamappings session test";
  request.ClientNonce = std::vector<uint8_t>(32,0);
//  request.ClientCertificate = GetCertificate();
  request.RequestedSessionTimeout = 3600000;
  request.MaxResponseMessageSize = 65536;

  stream << request << flush;

  OpcUa::Binary::CreateSessionResponse response;
  stream >> response;
  return response;
}

TEST_F(OpcBinaryHandshake, ActivateSession)
{
  using namespace OpcUa::Binary;
  std::shared_ptr<RemoteConnection> connection(Connect(GetHost(), GetPort()));

  SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = CreateSecureChannel(connection, params);

  IOStream stream(secureChannel);
  CreateSessionResponse session = CreateSession(stream);

  ActivateSessionRequest activate;
  activate.Header.SessionAuthenticationToken = session.AuthenticationToken;
  activate.LocaleIds.push_back("en");
  stream << activate << flush;

  ActivateSessionResponse response;
  stream >> response;

}
//----------------------------------------------------------------------
// CloseSession
//----------------------------------------------------------------------

TEST_F(OpcBinaryHandshake, CloseSession)
{
  using namespace OpcUa::Binary;
  std::shared_ptr<RemoteConnection> connection(Connect(GetHost(), GetPort()));

  SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = CreateSecureChannel(connection, params);

  IOStream stream(secureChannel);
  CreateSessionResponse session = CreateSession(stream);

  ActivateSessionRequest activate;
  activate.Header.SessionAuthenticationToken = session.AuthenticationToken;
  activate.LocaleIds.push_back("en");
  stream << activate << flush;

  ActivateSessionResponse response;
  stream >> response;

  CloseSessionRequest closeSession;
  closeSession.Header.SessionAuthenticationToken = session.AuthenticationToken;
  stream << closeSession << flush;

  CloseSessionResponse closeResponse;
  stream >> closeResponse;
}

//----------------------------------------------------------------------
// Browse
//----------------------------------------------------------------------

void CloseSession(OpcUa::Binary::IOStream& stream, const OpcUa::Binary::CreateSessionResponse& session)
{
  CloseSessionRequest closeSession;
  closeSession.Header.SessionAuthenticationToken = session.AuthenticationToken;
  stream << closeSession << flush;

  CloseSessionResponse closeResponse;
  stream >> closeResponse;
}

void ActivateSession(OpcUa::Binary::IOStream& stream, const OpcUa::Binary::CreateSessionResponse& session)
{
  using namespace OpcUa::Binary;
  ActivateSessionRequest activate;
  activate.Header.SessionAuthenticationToken = session.AuthenticationToken;
  activate.LocaleIds.push_back("en");
  stream << activate << flush;

  ActivateSessionResponse response;
  stream >> response;
}

TEST_F(OpcBinaryHandshake, Browse)
{
  using namespace OpcUa::Binary;
  std::shared_ptr<RemoteConnection> connection(Connect(GetHost(), GetPort()));

  SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = CreateSecureChannel(connection, params);

  IOStream stream(secureChannel);
  CreateSessionResponse session = CreateSession(stream);
  ActivateSession(stream, session);

  OpcUa::Binary::BrowseRequest browse;
  browse.Header.SessionAuthenticationToken = session.AuthenticationToken;
  browse.MaxReferenciesPerNode = 2;

  BrowseDescription desc;
  desc.NodeToBrowse.Encoding = EV_TWO_BYTE;
  desc.NodeToBrowse.TwoByteData.Identifier = 84; // root node.
  desc.Direction = BrowseDirection::FORWARD;
  desc.ReferenceTypeId.Encoding = EV_TWO_BYTE;
  desc.ReferenceTypeId.TwoByteData.Identifier = 33;
  desc.IncludeSubtypes = true;
  desc.NodeClasses = NodeClass::Unspecified;
  desc.ResultMask = BrowseResultMask::All;
  browse.NodesToBrowse.push_back(desc);
  
  stream << browse << flush;

  BrowseResponse response;
  stream >> response;

  EXPECT_TRUE(!response.Results.empty());

  BrowseNextRequest browseNext;
  browseNext.Header.SessionAuthenticationToken = session.AuthenticationToken;
  browseNext.ReleaseContinuationPoints= false;
  browseNext.ContinuationPoints.push_back(response.Results[0].ContinuationPoint);  

  stream << browseNext << flush;

  BrowseNextResponse resp;
  stream >> resp;

  EXPECT_TRUE(!response.Results.empty());

  CloseSession(stream, session);
}

//----------------------------------------------------------------------
// Read
//----------------------------------------------------------------------

TEST_F(OpcBinaryHandshake, Read)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  std::shared_ptr<RemoteConnection> connection(Connect(GetHost(), GetPort()));

  SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = CreateSecureChannel(connection, params);

  IOStream stream(secureChannel);
  CreateSessionResponse session = CreateSession(stream);
  ActivateSession(stream, session);



  ReadValueId value;
  value.Node.Encoding = EV_FOUR_BYTE;
  value.Node.FourByteData.Identifier = static_cast<uint8_t>(ObjectId::RootFolder);
  value.Attribute = AttributeId::DisplayName;


  OpcUa::Binary::ReadRequest request;
  request.Header.SessionAuthenticationToken = session.AuthenticationToken;
  request.MaxAge = 0;
  request.TimestampsType = TimestampsToReturn::Neither;
  request.AttributesToRead.push_back(value);
 
  stream << request << flush;

  ReadResponse response;
  stream >> response;

  EXPECT_TRUE(!response.Results.empty());

  CloseSession(stream, session);
}

//----------------------------------------------------------------------
// Write
//----------------------------------------------------------------------

TEST_F(OpcBinaryHandshake, Write)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  std::shared_ptr<RemoteConnection> connection(Connect(GetHost(), GetPort()));

  SecureConnectionParams params;
  params.EndpointUrl = GetEndpointUrl();
  params.SecurePolicy = "http://opcfoundation.org/UA/SecurityPolicy#None";

  std::shared_ptr<OpcUa::IOChannel> secureChannel = CreateSecureChannel(connection, params);

  IOStream stream(secureChannel);
  CreateSessionResponse session = CreateSession(stream);
  ActivateSession(stream, session);

  OpcUa::Binary::WriteRequest request;
  request.Header.SessionAuthenticationToken = session.AuthenticationToken;

  WriteValue value;
  value.Node.Encoding = EV_FOUR_BYTE;
  value.Node.FourByteData.Identifier = static_cast<uint8_t>(ObjectId::RootFolder); 
  value.Attribute = AttributeId::DisplayName;
  value.Data.Encoding = DATA_VALUE;
  value.Data.Value.Type = VariantType::STRING;
  value.Data.Value.Value.String.push_back("root");

  request.NodesToWrite.push_back(value);

  stream << request << flush;

  WriteResponse response;
  stream >> response;

  EXPECT_TRUE(!response.StatusCodes.empty());

  CloseSession(stream, session);
}

