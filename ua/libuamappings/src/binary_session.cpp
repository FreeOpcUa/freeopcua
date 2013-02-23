/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary session services.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "binary_serialization.h"

#include <opc/ua/binary/protocol/session.h>
#include <opc/ua/binary/stream.h>
#include <opc/ua/binary/types.h>
#include <opc/ua/extension_identifiers.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
  namespace Binary
  {

    //---------------------------------------------------
    // CreateSessionRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CreateSessionRequest>(const CreateSessionRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) +
        RawSize(request.ClientDescription) +
        RawSize(request.ServerURI) +
        RawSize(request.EndpointURL) +
        RawSize(request.SessionName) +
        RawSize(request.ClientNonce) +
        RawSize(request.ClientCertificate) +
        sizeof(request.RequestedSessionTimeout) +
        sizeof(request.MaxResponseMessageSize);
    }

    CreateSessionRequest::CreateSessionRequest()
      : TypeID(CREATE_SESSION_REQUEST)
      , RequestedSessionTimeout(0)
      , MaxResponseMessageSize(0)
    {
    }

    template<>
    void OStream::Serialize<CreateSessionRequest>(const CreateSessionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      *this << request.ClientDescription;
      *this << request.ServerURI;
      *this << request.EndpointURL;
      *this << request.SessionName;
      *this << request.ClientNonce;
      *this << request.ClientCertificate;
      *this << request.RequestedSessionTimeout;
      *this << request.MaxResponseMessageSize;
    }

    template<>
    void IStream::Deserialize<CreateSessionRequest>(CreateSessionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;

      *this >> request.ClientDescription;
      *this >> request.ServerURI;
      *this >> request.EndpointURL;
      *this >> request.SessionName;
      *this >> request.ClientNonce;
      *this >> request.ClientCertificate;
      *this >> request.RequestedSessionTimeout;
      *this >> request.MaxResponseMessageSize;
    }

    //---------------------------------------------------
    // CreateSessionResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CreateSessionResponse>(const CreateSessionResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) +
      RawSize(response.SessionID) +
      RawSize(response.AuthenticationToken) +
      sizeof(response.RevisedSessionTimeout) +
      RawSize(response.ServerNonce) +
      RawSize(response.ServerCertificate) +
      RawSize(response.ServerEndpoints) +
      RawSize(response.SignedServerCertificates) +
      RawSize(response.ServerSignature) +
      sizeof(response.MaxRequestMessageSize);
    }

    CreateSessionResponse::CreateSessionResponse()
      : TypeID(CREATE_SESSION_RESPONSE)
      , RevisedSessionTimeout(0)
      , MaxRequestMessageSize(0)
    {
    }

    template<>
    void OStream::Serialize<CreateSessionResponse>(const CreateSessionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.SessionID;
      *this << response.AuthenticationToken;
      *this << response.RevisedSessionTimeout;
      *this << response.ServerNonce;
      *this << response.ServerCertificate;
      *this << response.ServerEndpoints;
      *this << response.SignedServerCertificates;
      *this << response.ServerSignature;
      *this << response.MaxRequestMessageSize;
    }

    template<>
    void IStream::Deserialize<CreateSessionResponse>(CreateSessionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.SessionID;
      *this >> response.AuthenticationToken;
      *this >> response.RevisedSessionTimeout;
      *this >> response.ServerNonce;
      *this >> response.ServerCertificate;
      *this >> response.ServerEndpoints;
      *this >> response.SignedServerCertificates;
      *this >> response.ServerSignature;
      *this >> response.MaxRequestMessageSize;
    }

    //---------------------------------------------------
    // ActivateSessionRequest
    //---------------------------------------------------
    UserIdentifyToken::UserIdentifyToken()
      : Header(USER_IDENTIFY_TOKEN_ANONYMOUS, HAS_BINARY_BODY)
    {
      Anonymous.Data = {9,0,0,0,'A', 'n', 'o', 'n', 'y', 'm', 'o', 'u', 's'};
    }

    template<>
    std::size_t RawSize<UserIdentifyToken>(const UserIdentifyToken& token)
    {
      return RawSize(token.Header) + RawSize(token.Anonymous.Data);
    };

    template<>
    void OStream::Serialize<UserIdentifyToken>(const UserIdentifyToken& token)
    {
      *this << token.Header;
      *this << token.Anonymous.Data;
    }

    template<>
    void IStream::Deserialize<UserIdentifyToken>(UserIdentifyToken& token)
    {
      *this >> token.Header;
      *this >> token.Anonymous.Data;
    }



    ActivateSessionRequest::ActivateSessionRequest()
      : TypeID(ACTIVATE_SESSION_REQUEST)
    {
    }

    template<>
    std::size_t RawSize<ActivateSessionRequest>(const ActivateSessionRequest& request)
    {
      return RawSize(request.TypeID) +
             RawSize(request.Header) +
             RawSize(request.ClientSignature) +
             RawSizeContainer(request.ClientCertificates) +
             RawSizeContainer(request.LocaleIDs) +
             RawSize(request.IdentifyToken) + 
             RawSize(request.UserTokenSignature);
    };

    template<>
    void OStream::Serialize<ActivateSessionRequest>(const ActivateSessionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.ClientSignature;
      if (!request.ClientCertificates.empty())
      {
        *this << request.ClientCertificates;
      }
      else
      {
        *this << (uint32_t)0;
      }
      if (!request.LocaleIDs.empty())
      {
        *this << request.LocaleIDs;
      }
      else
      {
        *this << (uint32_t)0;
      }

      *this << request.IdentifyToken;
      *this << request.UserTokenSignature;
    }

    template<>
    void IStream::Deserialize<ActivateSessionRequest>(ActivateSessionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.ClientSignature;
      *this >> request.ClientCertificates;
      *this >> request.LocaleIDs;
      *this >> request.IdentifyToken;
      *this >> request.UserTokenSignature;
    }


    //---------------------------------------------------
    // ActivateSessionResponse
    //---------------------------------------------------

    ActivateSessionResponse::ActivateSessionResponse()
      : TypeID(ACTIVATE_SESSION_RESPONSE)
    {
    }

    template<>
    std::size_t RawSize<ActivateSessionResponse>(const ActivateSessionResponse& response)
    {
      return RawSize(response.TypeID) +
             RawSize(response.Header) +
             RawSize(response.ServerNonce) +
             RawSizeContainer(response.StatusCodes) +
             RawSizeContainer(response.DiagnosticInfos);
    };

    template<>
    void OStream::Serialize<ActivateSessionResponse>(const ActivateSessionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.ServerNonce;
      if (!response.StatusCodes.empty())
      {
        SerializeContainer(*this, response.StatusCodes);
      }
      else
      {
        *this << (uint32_t)0;
      }

      if (!response.DiagnosticInfos.empty())
      {
        SerializeContainer(*this, response.DiagnosticInfos);
      }
      else
      {
        *this << (uint32_t)0;
      }
    }

    template<>
    void IStream::Deserialize<ActivateSessionResponse>(ActivateSessionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.ServerNonce;
      DeserializeContainer(*this, response.StatusCodes);
      DeserializeContainer(*this, response.DiagnosticInfos);
    }

    //---------------------------------------------------
    // CloseSessionRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CloseSessionRequest>(const CloseSessionRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) +
        RawSize(request.DeleteSubscriptions);
    }

    CloseSessionRequest::CloseSessionRequest()
      : TypeID(CLOSE_SESSION_REQUEST)
      , DeleteSubscriptions(true)
    {
    }

    template<>
    void OStream::Serialize<CloseSessionRequest>(const CloseSessionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      *this << request.DeleteSubscriptions;
    }

    template<>
    void IStream::Deserialize<CloseSessionRequest>(CloseSessionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;

      *this >> request.DeleteSubscriptions;
    }

    //---------------------------------------------------
    // CloseSessionResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CloseSessionResponse>(const CloseSessionResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header);
    }

    CloseSessionResponse::CloseSessionResponse()
      : TypeID(CLOSE_SESSION_RESPONSE)
    {
    }

    template<>
    void OStream::Serialize<CloseSessionResponse>(const CloseSessionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
    }

    template<>
    void IStream::Deserialize<CloseSessionResponse>(CloseSessionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
    }


  } // namespace Binary
} // namespace OpcUa

