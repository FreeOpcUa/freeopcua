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

#include <opc/ua/binary/protocol/common.h>

namespace OpcUa
{
  namespace Binary
  {
    //---------------------------------------------------
    // CreateSession
    //---------------------------------------------------

    struct CreateSessionRequest
    {
      NodeID TypeID;
      RequestHeader Header;

      ApplicationDescription ClientDescription;
      std::string ServerURI;
      std::string EndpointURL;
      std::string SessionName;
      std::vector<uint8_t> ClientNonce;
      CertificateData ClientCertificate;
      Duration RequestedSessionTimeout;
      uint32_t MaxResponseMessageSize;

      CreateSessionRequest();
    };


    struct CreateSessionResponse
    {
      NodeID TypeID;
      ResponseHeader Header;

      NodeID SessionID;
      NodeID AuthenticationToken;
      Duration RevisedSessionTimeout;
      std::vector<uint8_t> ServerNonce;
      CertificateData ServerCertificate;
      std::vector<EndpointDescription> ServerEndpoints;
      std::vector<CertificateData> SignedServerCertificates;
      SignatureData ServerSignature;
      uint32_t MaxRequestMessageSize;

      CreateSessionResponse();
    };

    //-------------------------------------------------
    // ActivateSessionRequest
    //-------------------------------------------------

    struct UserIdentifyToken
    {
      ExtensionObjectHeader Header;
      struct AnonymousStruct
      {
        std::vector<uint8_t> Data;
      } Anonymous;

      UserIdentifyToken();
    };

    struct ActivateSessionRequest
    {
      NodeID TypeID;
      RequestHeader Header;

      SignatureData ClientSignature;
      std::vector<CertificateData> ClientCertificates;
      std::vector<std::string> LocaleIDs;
      UserIdentifyToken IdentifyToken;
      SignatureData UserTokenSignature;

      ActivateSessionRequest();
    };

    struct ActivateSessionResponse
    {
      NodeID TypeID;
      ResponseHeader Header;
      std::vector<uint8_t> ServerNonce;
      std::vector<uint32_t>StatusCodes;
      std::vector<DiagnosticInfo> DiagnosticInfos;

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

  }
}

#endif // __OPC_UA_BINARY_SESSIONS_H__
