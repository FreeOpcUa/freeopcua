/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary session services.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/extension_identifiers.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
  SessionParameters::SessionParameters()
    : RequestedSessionTimeout(0)
    , MaxResponseMessageSize(0)
  {
  }


  CreateSessionRequest::CreateSessionRequest()
    : TypeID(CREATE_SESSION_REQUEST)
  {
  }

  SessionData::SessionData()
    : RevisedSessionTimeout(0)
    , MaxRequestMessageSize(0)
  {
  }

  CreateSessionResponse::CreateSessionResponse()
    : TypeID(CREATE_SESSION_RESPONSE)
  {
  }

  CloseSessionResponse::CloseSessionResponse()
    : TypeID(CLOSE_SESSION_RESPONSE)
  {
  }

  UserIdentifyToken::UserIdentifyToken()
    : Header(USER_IDENTIFY_TOKEN_ANONYMOUS, HAS_BINARY_BODY)
  {
    Anonymous.Data = {9,0,0,0,'A', 'n', 'o', 'n', 'y', 'm', 'o', 'u', 's'};
  }

  ActivateSessionRequest::ActivateSessionRequest()
    : TypeID(ACTIVATE_SESSION_REQUEST)
  {
  }

  ActivateSessionResponse::ActivateSessionResponse()
    : TypeID(ACTIVATE_SESSION_RESPONSE)
  {
  }

  CloseSessionRequest::CloseSessionRequest()
    : TypeID(CLOSE_SESSION_REQUEST)
    , DeleteSubscriptions(true)
  {
  }


  namespace Binary
  {

    //---------------------------------------------------
    // SessionParameters
    //---------------------------------------------------

    template<>
    std::size_t RawSize<SessionParameters>(const SessionParameters& params)
    {
      return RawSize(params.ClientDescription) +
        RawSize(params.ServerURI) +
        RawSize(params.EndpointURL) +
        RawSize(params.SessionName) +
        RawSize(params.ClientNonce) +
        RawSize(params.ClientCertificate) +
        sizeof(params.RequestedSessionTimeout) +
        sizeof(params.MaxResponseMessageSize);
    }

    template<>
    void DataSerializer::Serialize<SessionParameters>(const SessionParameters& params)
    {
      *this << params.ClientDescription;
      *this << params.ServerURI;
      *this << params.EndpointURL;
      *this << params.SessionName;
      *this << params.ClientNonce;
      *this << params.ClientCertificate;
      *this << params.RequestedSessionTimeout;
      *this << params.MaxResponseMessageSize;
    }

    template<>
    void DataDeserializer::Deserialize<SessionParameters>(SessionParameters& params)
    {
      *this >> params.ClientDescription;
      *this >> params.ServerURI;
      *this >> params.EndpointURL;
      *this >> params.SessionName;
      *this >> params.ClientNonce;
      *this >> params.ClientCertificate;
      *this >> params.RequestedSessionTimeout;
      *this >> params.MaxResponseMessageSize;
    }

    //---------------------------------------------------
    // CreateSessionRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CreateSessionRequest>(const CreateSessionRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void DataSerializer::Serialize<CreateSessionRequest>(const CreateSessionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<CreateSessionRequest>(CreateSessionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    //---------------------------------------------------
    // SessionData
    //---------------------------------------------------

    template<>
    std::size_t RawSize<SessionData>(const SessionData& data)
    {
      return RawSize(data.SessionID) +
      RawSize(data.AuthenticationToken) +
      sizeof(data.RevisedSessionTimeout) +
      RawSize(data.ServerNonce) +
      RawSize(data.ServerCertificate) +
      RawSize(data.ServerEndpoints) +
      RawSize(data.SignedServerCertificates) +
      RawSize(data.ServerSignature) +
      sizeof(data.MaxRequestMessageSize);
    }

    template<>
    void DataSerializer::Serialize<SessionData>(const SessionData& data)
    {
      *this << data.SessionID;
      *this << data.AuthenticationToken;
      *this << data.RevisedSessionTimeout;
      *this << data.ServerNonce;
      *this << data.ServerCertificate;
      *this << data.ServerEndpoints;
      *this << data.SignedServerCertificates;
      *this << data.ServerSignature;
      *this << data.MaxRequestMessageSize;
    }

    template<>
    void DataDeserializer::Deserialize<SessionData>(SessionData& data)
    {
      *this >> data.SessionID;
      *this >> data.AuthenticationToken;
      *this >> data.RevisedSessionTimeout;
      *this >> data.ServerNonce;
      *this >> data.ServerCertificate;
      *this >> data.ServerEndpoints;
      *this >> data.SignedServerCertificates;
      *this >> data.ServerSignature;
      *this >> data.MaxRequestMessageSize;
    }

    //---------------------------------------------------
    // CreateSessionResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<CreateSessionResponse>(const CreateSessionResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) + RawSize(response.Session);
    }

    template<>
    void DataSerializer::Serialize<CreateSessionResponse>(const CreateSessionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Session;
    }

    template<>
    void DataDeserializer::Deserialize<CreateSessionResponse>(CreateSessionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Session;
    }

    //---------------------------------------------------
    // UserIdentifyToken
    //---------------------------------------------------

    template<>
    std::size_t RawSize<UserIdentifyToken>(const UserIdentifyToken& token)
    {
      return RawSize(token.Header) + RawSize(token.Anonymous.Data);
    };

    template<>
    void DataSerializer::Serialize<UserIdentifyToken>(const UserIdentifyToken& token)
    {
      *this << token.Header;
      *this << token.Anonymous.Data;
    }

    template<>
    void DataDeserializer::Deserialize<UserIdentifyToken>(UserIdentifyToken& token)
    {
      *this >> token.Header;
      *this >> token.Anonymous.Data;
    }

    //---------------------------------------------------
    // UpdatedSessionParameters
    //---------------------------------------------------

    template<>
    std::size_t RawSize<UpdatedSessionParameters>(const UpdatedSessionParameters& params)
    {
      return RawSize(params.ClientSignature) +
             RawSizeContainer(params.ClientCertificates) +
             RawSizeContainer(params.LocaleIDs) +
             RawSize(params.IdentifyToken) + 
             RawSize(params.UserTokenSignature);
    };

    template<>
    void DataSerializer::Serialize<UpdatedSessionParameters>(const UpdatedSessionParameters& params)
    {
      *this << params.ClientSignature;
      if (!params.ClientCertificates.empty())
      {
        *this << params.ClientCertificates;
      }
      else
      {
        *this << (uint32_t)0;
      }
      if (!params.LocaleIDs.empty())
      {
        *this << params.LocaleIDs;
      }
      else
      {
        *this << (uint32_t)0;
      }

      *this << params.IdentifyToken;
      *this << params.UserTokenSignature;
    }

    template<>
    void DataDeserializer::Deserialize<UpdatedSessionParameters>(UpdatedSessionParameters& params)
    {
      *this >> params.ClientSignature;
      *this >> params.ClientCertificates;
      *this >> params.LocaleIDs;
      *this >> params.IdentifyToken;
      *this >> params.UserTokenSignature;
    }


    //---------------------------------------------------
    // ActivateSessionRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<ActivateSessionRequest>(const ActivateSessionRequest& request)
    {
      return RawSize(request.TypeID) +
             RawSize(request.Header) +
             RawSize(request.Parameters);
    };

    template<>
    void DataSerializer::Serialize<ActivateSessionRequest>(const ActivateSessionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<ActivateSessionRequest>(ActivateSessionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    //---------------------------------------------------
    // UpdatedSessionData
    //---------------------------------------------------

    template<>
    std::size_t RawSize<UpdatedSessionData>(const UpdatedSessionData& data)
    {
      return RawSize(data.ServerNonce) +
             RawSizeContainer(data.StatusCodes) +
             RawSizeContainer(data.Diagnostics);
    };

    template<>
    void DataSerializer::Serialize<UpdatedSessionData>(const UpdatedSessionData& data)
    {
      *this << data.ServerNonce;
      if (!data.StatusCodes.empty())
      {
        SerializeContainer(*this, data.StatusCodes);
      }
      else
      {
        *this << (uint32_t)0;
      }

      if (!data.Diagnostics.empty())
      {
        SerializeContainer(*this, data.Diagnostics, 0);
      }
      else
      {
        *this << (uint32_t)0;
      }
    }

    template<>
    void DataDeserializer::Deserialize<UpdatedSessionData>(UpdatedSessionData& data)
    {
      *this >> data.ServerNonce;
      DeserializeContainer(*this, data.StatusCodes);
      DeserializeContainer(*this, data.Diagnostics);
    }


    //---------------------------------------------------
    // ActivateSessionResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<ActivateSessionResponse>(const ActivateSessionResponse& response)
    {
      return RawSize(response.TypeID) +
             RawSize(response.Header) +
             RawSize(response.Session);
    };

    template<>
    void DataSerializer::Serialize<ActivateSessionResponse>(const ActivateSessionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Session;
    }

    template<>
    void DataDeserializer::Deserialize<ActivateSessionResponse>(ActivateSessionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Session;
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

    template<>
    void DataSerializer::Serialize<CloseSessionRequest>(const CloseSessionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      *this << request.DeleteSubscriptions;
    }

    template<>
    void DataDeserializer::Deserialize<CloseSessionRequest>(CloseSessionRequest& request)
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

    template<>
    void DataSerializer::Serialize<CloseSessionResponse>(const CloseSessionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
    }

    template<>
    void DataDeserializer::Deserialize<CloseSessionResponse>(CloseSessionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
    }


  } // namespace Binary
} // namespace OpcUa

