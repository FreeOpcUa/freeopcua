/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Secure channel service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_SESSIONS_H__
#define __OPC_UA_BINARY_SESSIONS_H__

#include <opc/ua/protocol/types.h>

namespace OpcUa
{

  //---------------------------------------------------
  // CreateSession
  //---------------------------------------------------

  struct SessionParameters
  {
    ApplicationDescription ClientDescription;
    std::string ServerURI;
    std::string EndpointURL;
    std::string SessionName;
    std::vector<uint8_t> ClientNonce;
    CertificateData ClientCertificate;
    Duration RequestedSessionTimeout;
    uint32_t MaxResponseMessageSize;

    SessionParameters();
  };

  struct CreateSessionRequest
  {
    ExpandedNodeID TypeID;
    RequestHeader Header;
    SessionParameters Parameters;

    CreateSessionRequest();
  };


  struct SessionData
  {
    ExpandedNodeID SessionID;
    ExpandedNodeID AuthenticationToken;
    Duration RevisedSessionTimeout;
    std::vector<uint8_t> ServerNonce;
    CertificateData ServerCertificate;
    std::vector<EndpointDescription> ServerEndpoints;
    std::vector<CertificateData> SignedServerCertificates;
    SignatureData ServerSignature;
    uint32_t MaxRequestMessageSize;

    SessionData();
  };

  struct CreateSessionResponse
  {
    ExpandedNodeID TypeID;
    ResponseHeader Header;
    SessionData Session;

    CreateSessionResponse();
  };

  //-------------------------------------------------
  // ActivateSessionRequest
  //-------------------------------------------------

  struct UserIdentifyToken
  {
    ExtensionObjectHeader Header;
    std::vector<uint8_t> PolicyID;

    struct UserNameStruct
    {
      std::string UserName;
      std::string Password;
      std::string EncryptionAlgorithm;
    } UserName;

    UserIdentifyToken();

    UserIdentifyTokenType type() const;
    void setUser(const std::string &user, const std::string &password);
    void setPolicyID(const std::string &id);
  };

  struct UpdatedSessionParameters
  {
    SignatureData ClientSignature;
    std::vector<CertificateData> ClientCertificates;
    std::vector<std::string> LocaleIDs;
    UserIdentifyToken IdentifyToken;
    SignatureData UserTokenSignature;
  };

  struct ActivateSessionRequest
  {
    NodeID TypeID;
    RequestHeader Header;
    UpdatedSessionParameters Parameters;

    ActivateSessionRequest();
  };

  struct UpdatedSessionData
  {
    std::vector<uint8_t> ServerNonce;
    std::vector<uint32_t>StatusCodes;
    DiagnosticInfoList Diagnostics;
  };

  struct ActivateSessionResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    UpdatedSessionData Session;

    ActivateSessionResponse();
  };

  //-------------------------------------------------
  // CloseSessionRequest
  //-------------------------------------------------

  struct CloseSessionRequest
  {
    NodeID TypeID;
    RequestHeader Header;

    bool DeleteSubscriptions;

    CloseSessionRequest();
  };

  struct CloseSessionResponse
  {
    NodeID TypeID;
    ResponseHeader Header;

    CloseSessionResponse();
  };

  ExpandedNodeID GenerateSessionId();

  struct ServiceFaultResponse
  {
    NodeID TypeID;
    ResponseHeader Header;

    ServiceFaultResponse();
  };
  

} // namespace OpcUa

#endif // __OPC_UA_BINARY_SESSIONS_H__
