/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_TESTS_COMMON_H__
#define __OPC_UA_TESTS_COMMON_H__

//#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <stdexcept>
#include <stdlib.h>
#include <string>

inline std::string PrintData(const std::vector<char>& vec)
{
  std::stringstream stream;
  for (std::size_t i = 0; i < vec.size(); ++i)
  {
    stream << "0x" << std::setfill('0') << std::setw(2) << std::hex << ((int)vec[i] & 0xff) << ' ';
    if ((i + 1) % 4 == 0)
    {
      stream << ' ';
    }

    if ((i + 1) % 32 == 0)
    {
      stream << std::endl;
    }
  }
  return stream.str();
}

inline std::string GetHost()
{
  if (const char* host = getenv("OPCUA_HOST"))
  {
    return host;
  }
  return "localhost";
}

inline int GetPort()
{
  if (char* port = getenv("OPCUA_PORT"))
  {
    return atoi(port);
  }
  return 4841;
}

inline std::string GetEndpoint()
{
  if (char* endpoint = getenv("OPCUA_ENDPOINT"))
  {
    return endpoint;
  }
  return "opc.tcp://localhost:4841";
}

class OutputChannel
{
public:
  virtual void Send(const char* data, std::size_t size)
  {
    SerializedData = std::vector<char>(data, data + size);
  }

  virtual ~OutputChannel()
  {
  }

  std::vector<char> SerializedData;
};

typedef OpcUa::Binary::OStream<OutputChannel> BinaryOStream;

class OpcUaBinarySerialization : public ::testing::Test
{
public:
  OpcUaBinarySerialization()
    : Channel(new OutputChannel)
    , Stream(new BinaryOStream(Channel))
  {
  }

protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }

  OutputChannel& GetChannel()
  {
    return *Channel;
  }

  BinaryOStream& GetStream()
  {
    return *Stream;
  }

private:
  std::shared_ptr<OutputChannel> Channel;
  std::unique_ptr<BinaryOStream> Stream;
};

class InputChannel
{
public:
  InputChannel()
  {
    CurPos = SerializedData.begin();
  }

  virtual ~InputChannel()
  {
  }

  virtual std::size_t Receive(char* data, std::size_t size)
  {
    if (CurPos == SerializedData.end())
    {
      return 0;
    }
    const std::size_t dist = static_cast<std::size_t>(std::distance(CurPos, SerializedData.end()));
    const std::size_t minSize = std::min(size, dist);
    std::copy(CurPos, CurPos + minSize, data);
    CurPos+=minSize;
    return minSize;
  }

  void SetData(const std::vector<char>& data)
  {
    SerializedData = data;
    CurPos = SerializedData.begin();
  }

  bool IsEmpty() const
  {
    return CurPos == SerializedData.end();
  }

private:
  std::vector<char> SerializedData;
  std::vector<char>::iterator CurPos;
};


typedef OpcUa::Binary::IStream<InputChannel> BinaryIStream;

class OpcUaBinaryDeserialization : public ::testing::Test
{
public:
  OpcUaBinaryDeserialization()
    : Channel(new InputChannel)
    , Stream(new BinaryIStream(Channel))
  {
  }

protected:
  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }

  InputChannel& GetChannel()
  {
    return *Channel;
  }

  BinaryIStream& GetStream()
  {
    return *Stream;
  }

protected:
  std::shared_ptr<InputChannel> Channel;
  std::unique_ptr<BinaryIStream> Stream;
};




#define TEST_RESPONSE_HEADER_BINARY_DATA \
  1,0,0,0,0,0,0,0, \
  2,0,0,0, \
  3,0,0,0, \
  static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  2,0,0,0,  3,0,0,0,  's','t','r',  3,0,0,0,  's','t','r', \
  EV_TWO_BYTE, 7, \
  8

#define FILL_TEST_RESPONSE_HEADER(header) \
  header.Timestamp.Value = 1; \
  header.RequestHandle =  2; \
  header.ServiceResult = static_cast<StatusCode>(3); \
  header.InnerDiagnostics.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO); \
  header.InnerDiagnostics.LocalizedText = 4; \
  header.InnerDiagnostics.InnerDiagnostics.reset(new DiagnosticInfo()); \
  header.InnerDiagnostics.InnerDiagnostics->EncodingMask = DIM_ADDITIONAL_INFO; \
  header.InnerDiagnostics.InnerDiagnostics->AdditionalInfo = "add"; \
  header.StringTable = std::vector<std::string>(2, std::string("str")); \
  header.Additional.TypeID.Encoding = EV_TWO_BYTE; \
  header.Additional.TypeID.TwoByteData.Identifier = 7; \
  header.Additional.Encoding = 8;

#define ASSERT_RESPONSE_HEADER_EQ(header) \
  ASSERT_EQ(header.Timestamp.Value, 1); \
  ASSERT_EQ(header.RequestHandle, 2); \
  ASSERT_EQ(header.ServiceResult, static_cast<StatusCode>(3)); \
  ASSERT_EQ(header.InnerDiagnostics.EncodingMask, static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO)); \
  ASSERT_EQ(header.InnerDiagnostics.LocalizedText, 4); \
  ASSERT_TRUE(static_cast<bool>(header.InnerDiagnostics.InnerDiagnostics)); \
  ASSERT_EQ(header.InnerDiagnostics.InnerDiagnostics->EncodingMask, DIM_ADDITIONAL_INFO); \
  ASSERT_EQ(header.InnerDiagnostics.InnerDiagnostics->AdditionalInfo, "add"); \
  ASSERT_EQ(header.StringTable, std::vector<std::string>(2, std::string("str"))); \
  ASSERT_EQ(header.Additional.TypeID.Encoding, EV_TWO_BYTE); \
  ASSERT_EQ(header.Additional.TypeID.TwoByteData.Identifier, 7); \
  ASSERT_EQ(header.Additional.Encoding, 8);


#define TEST_REQUEST_HEADER_BINARY_DATA \
  EV_TWO_BYTE, 1, \
  2, 0, 0, 0, 0, 0, 0, 0, \
  3, 0, 0, 0, \
  4, 0, 0, 0, \
  5, 0, 0, 0, \
  'a', 'u', 'd', 'i', 't', \
  5, 0, 0, 0, \
  0, \
  6, \
  8

#define FILL_TEST_REQUEST_HEADER(header) \
  header.SessionAuthenticationToken.Encoding = EV_TWO_BYTE; \
  header.SessionAuthenticationToken.TwoByteData.Identifier = 1; \
  header.UtcTime.Value = 2; \
  header.RequestHandle = 3; \
  header.ReturnDiagnostics = 4; \
  header.AuditEntryID = "audit"; \
  header.Timeout = 5; \
  header.Additional.TypeID.Encoding = EV_TWO_BYTE; \
  header.Additional.TypeID.TwoByteData.Identifier = 6; \
  header.Additional.Encoding = 8;

#define ASSERT_REQUEST_HEADER_EQ(header) \
  ASSERT_EQ(header.SessionAuthenticationToken.Encoding, EV_TWO_BYTE); \
  ASSERT_EQ(header.SessionAuthenticationToken.TwoByteData.Identifier, 1); \
  ASSERT_EQ(header.UtcTime.Value, 2); \
  ASSERT_EQ(header.RequestHandle, 3); \
  ASSERT_EQ(header.ReturnDiagnostics, 4); \
  ASSERT_EQ(header.AuditEntryID, "audit"); \
  ASSERT_EQ(header.Timeout, 5); \
  ASSERT_EQ(header.Additional.TypeID.Encoding, EV_TWO_BYTE); \
  ASSERT_EQ(header.Additional.TypeID.TwoByteData.Identifier, 6); \
  ASSERT_EQ(header.Additional.Encoding, 8);


#define FILL_APPLICATION_DESCRIPTION(desc) \
  desc.URI = "u"; \
  desc.ProductURI = "pu"; \
  desc.Name.Encoding = HAS_LOCALE | HAS_TEXT; \
  desc.Name.Locale = "RU"; \
  desc.Name.Text = "text"; \
  desc.Type = ApplicationType::CLIENT; \
  desc.GatewayServerURI = "gw"; \
  desc.DiscoveryProfileURI = "dpu"; \
  desc.DiscoveryURLs.push_back("du");

#define TEST_APPLICATION_DESCRIPTION_BINARY_DATA \
  1,0,0,0, 'u', \
  2,0,0,0, 'p','u', \
  3, \
  2,0,0,0, 'R','U', \
  4,0,0,0, 't','e','x','t', \
  1,0,0,0, \
  2,0,0,0, 'g','w', \
  3,0,0,0, 'd','p','u', \
  1,0,0,0, 2,0,0,0, 'd','u'

#define  ASSERT_APPLICATION_DESCRIPTION_EQ(desc) \
  ASSERT_EQ(desc.URI, "u"); \
  ASSERT_EQ(desc.ProductURI, "pu"); \
  ASSERT_EQ(desc.Name.Encoding, HAS_LOCALE | HAS_TEXT); \
  ASSERT_EQ(desc.Name.Locale, "RU"); \
  ASSERT_EQ(desc.Name.Text, "text"); \
  ASSERT_EQ(desc.Type, ApplicationType::CLIENT); \
  ASSERT_EQ(desc.GatewayServerURI, "gw"); \
  ASSERT_EQ(desc.DiscoveryProfileURI, "dpu"); \
  ASSERT_EQ(desc.DiscoveryURLs, std::vector<std::string>(1,"du"));

#define FILL_TEST_ENDPOINT(endpoint) \
  endpoint.EndpointURL = "eu"; \
  FILL_APPLICATION_DESCRIPTION(endpoint.ServerDescription); \
  endpoint.ServerCertificate = {1,2,3,4}; \
  endpoint.SecurityMode = MSM_NONE; \
  endpoint.SecurityPolicyURI = "spu"; \
  UserTokenPolicy token; \
  token.PolicyID = "pi"; \
  token.TokenType = UserIdentifyTokenType::USERNAME; \
  token.IssuedTokenType = "itt"; \
  token.IssuerEndpointURL = "ieu"; \
  token.SecurityPolicyURI = "spu"; \
  endpoint.UserIdentifyTokens.push_back(token); \
  endpoint.TransportProfileURI = "tpu"; \
  endpoint.SecurityLevel = 3;

#define  TEST_ENDPOINT_BINARY_DATA \
  2,0,0,0, 'e','u', \
  TEST_APPLICATION_DESCRIPTION_BINARY_DATA, \
  4,0,0,0, 1,2,3,4, \
  1,0,0,0, \
  3,0,0,0, 's','p','u', \
  1,0,0,0, \
  2,0,0,0, 'p','i', \
  1,0,0,0, \
  3,0,0,0, 'i','t','t', \
  3,0,0,0, 'i','e','u', \
  3,0,0,0, 's','p','u', \
  3,0,0,0, 't','p','u', \
  3

#define  ASSERT_ENDPOINT_EQ(e) \
  ASSERT_EQ(e.EndpointURL, "eu"); \
  ASSERT_APPLICATION_DESCRIPTION_EQ(e.ServerDescription); \
  const std::vector<uint8_t> certificate = {1,2,3,4}; \
  ASSERT_EQ(e.ServerCertificate, certificate); \
  ASSERT_EQ(e.SecurityMode, MSM_NONE); \
  ASSERT_EQ(e.SecurityPolicyURI, "spu"); \
  ASSERT_EQ(e.UserIdentifyTokens.size(), 1); \
  ASSERT_EQ(e.UserIdentifyTokens[0].PolicyID, "pi"); \
  ASSERT_EQ(e.UserIdentifyTokens[0].TokenType, UserIdentifyTokenType::USERNAME); \
  ASSERT_EQ(e.UserIdentifyTokens[0].IssuedTokenType, "itt"); \
  ASSERT_EQ(e.UserIdentifyTokens[0].IssuerEndpointURL, "ieu"); \
  ASSERT_EQ(e.UserIdentifyTokens[0].SecurityPolicyURI, "spu"); \
  ASSERT_EQ(e.TransportProfileURI, "tpu"); \
  ASSERT_EQ(e.SecurityLevel, 3);

#endif // __OPC_UA_TESTS_COMMON_H__

