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
#include <opc/ua/protocol/protocol.h>

namespace OpcUa
{
  //-------------------------------------------------
  // ActivateSessionRequest
  //-------------------------------------------------

  struct UserIdentifyToken
  {
    ExtensionObjectHeader Header;
    std::vector<uint8_t> PolicyId;

    struct UserNameStruct
    {
      std::string UserName;
      std::string Password;
      std::string EncryptionAlgorithm;
    } UserName;

    UserIdentifyToken();

    UserTokenType type() const;
    void setUser(const std::string &user, const std::string &password);
    void setPolicyId(const std::string &id);
  };

  struct UpdatedSessionParameters
  {
    SignatureData ClientSignature;
    std::vector<CertificateData> ClientCertificates;
    std::vector<std::string> LocaleIds;
    UserIdentifyToken IdentifyToken;
    SignatureData UserTokenSignature;
  };

  struct ActivateSessionRequest
  {
    NodeId TypeId;
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
    NodeId TypeId;
    ResponseHeader Header;
    UpdatedSessionData Session;

    ActivateSessionResponse();
  };

  //-------------------------------------------------
  // CloseSessionRequest
  //-------------------------------------------------

  struct CloseSessionRequest
  {
    NodeId TypeId;
    RequestHeader Header;

    bool DeleteSubscriptions;

    CloseSessionRequest();
  };

  struct CloseSessionResponse
  {
    NodeId TypeId;
    ResponseHeader Header;

    CloseSessionResponse();
  };

  ExpandedNodeId GenerateSessionId();

  struct ServiceFaultResponse
  {
    NodeId TypeId;
    ResponseHeader Header;

    ServiceFaultResponse();
  };
  

} // namespace OpcUa

#endif // __OPC_UA_BINARY_SESSIONS_H__
