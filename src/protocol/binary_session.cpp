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
CloseSessionResponse::CloseSessionResponse()
  : TypeId(CLOSE_SESSION_RESPONSE)
{
}

UserIdentifyToken::UserIdentifyToken()
  : Header(USER_IdENTIFY_TOKEN_ANONYMOUS, HAS_BINARY_BODY)
{
}

UserTokenType UserIdentifyToken::type() const
{
  UserTokenType type = UserTokenType::Anonymous;

  if (Header.TypeId.FourByteData.Identifier == USER_IdENTIFY_TOKEN_USERNAME)
    { type = UserTokenType::UserName; }

  return type;
}

void UserIdentifyToken::setUser(const std::string & user, const std::string & password)
{
  Header.TypeId.FourByteData.Identifier = USER_IdENTIFY_TOKEN_USERNAME;
  UserName.UserName = user;
  UserName.Password = password;
  //UserName.EncryptionAlgorithm = "http://www.w3.org/2001/04/xmlenc#rsa-oaep";
}

void UserIdentifyToken::setPolicyId(const std::string & id)
{
  int sz = id.length();
  PolicyId.resize(sz);

  for (int i = 0; i < sz; i++)
    {
      PolicyId[i] = id[i];
    }
}

CloseSessionRequest::CloseSessionRequest()
  : TypeId(CLOSE_SESSION_REQUEST)
  , DeleteSubscriptions(true)
{
}

ServiceFaultResponse::ServiceFaultResponse()
  : TypeId(SERVICE_FAULT)
{
}

namespace Binary
{
//---------------------------------------------------
// UserIdentifyToken
//---------------------------------------------------

template<>
std::size_t RawSize<UserIdentifyToken::UserNameStruct>(const UserIdentifyToken::UserNameStruct & uname)
{
  return RawSize(uname.UserName) + RawSize(uname.Password) + RawSize(uname.EncryptionAlgorithm);
}

template<>
void DataSerializer::Serialize<UserIdentifyToken::UserNameStruct>(const UserIdentifyToken::UserNameStruct & uname)
{
  *this << uname.UserName;
  *this << uname.Password;
  *this << uname.EncryptionAlgorithm;
}

template<>
void DataDeserializer::Deserialize<UserIdentifyToken::UserNameStruct>(UserIdentifyToken::UserNameStruct & uname)
{
  *this >> uname.UserName;
  *this >> uname.Password;
  *this >> uname.EncryptionAlgorithm;
}

template<>
std::size_t RawSize<UserIdentifyToken>(const UserIdentifyToken & token)
{
  std::size_t ret = RawSize(token.Header) + RawSize(token.PolicyId) + RawSize((uint32_t)0);

  if (token.type() == UserTokenType::UserName)
    { ret += RawSize(token.UserName); }

  return ret;
}

template<>
void DataSerializer::Serialize<UserIdentifyToken>(const UserIdentifyToken & token)
{
  *this << token.Header;
  *this << int32_t(RawSize(token) - RawSize(token.Header) - RawSize((uint32_t)0));
  *this << token.PolicyId;

  if (token.type() == UserTokenType::UserName)
    { *this << token.UserName; }
}

template<>
void DataDeserializer::Deserialize<UserIdentifyToken>(UserIdentifyToken & token)
{
  *this >> token.Header;
  int32_t tmp;
  *this >> tmp;
  *this >> token.PolicyId;

  if (token.type() == UserTokenType::UserName)
    { *this >> token.UserName; }
}

//---------------------------------------------------
// CloseSessionRequest
//---------------------------------------------------

template<>
std::size_t RawSize<CloseSessionRequest>(const CloseSessionRequest & request)
{
  return RawSize(request.TypeId) + RawSize(request.Header) +
         RawSize(request.DeleteSubscriptions);
}

template<>
void DataSerializer::Serialize<CloseSessionRequest>(const CloseSessionRequest & request)
{
  *this << request.TypeId;
  *this << request.Header;

  *this << request.DeleteSubscriptions;
}

template<>
void DataDeserializer::Deserialize<CloseSessionRequest>(CloseSessionRequest & request)
{
  *this >> request.TypeId;
  *this >> request.Header;

  *this >> request.DeleteSubscriptions;
}

//---------------------------------------------------
// CloseSessionResponse
//---------------------------------------------------

template<>
std::size_t RawSize<CloseSessionResponse>(const CloseSessionResponse & response)
{
  return RawSize(response.TypeId) + RawSize(response.Header);
}

template<>
void DataSerializer::Serialize<CloseSessionResponse>(const CloseSessionResponse & response)
{
  *this << response.TypeId;
  *this << response.Header;
}

template<>
void DataDeserializer::Deserialize<CloseSessionResponse>(CloseSessionResponse & response)
{
  *this >> response.TypeId;
  *this >> response.Header;
}

//---------------------------------------------------
// ServiceFaultResponse
//---------------------------------------------------

template<>
std::size_t RawSize<ServiceFaultResponse>(const ServiceFaultResponse & request)
{
  return RawSize(request.TypeId) + RawSize(request.Header);
}

template<>
void DataSerializer::Serialize<ServiceFaultResponse>(const ServiceFaultResponse & request)
{
  *this << request.TypeId;
  *this << request.Header;
}

template<>
void DataDeserializer::Deserialize<ServiceFaultResponse>(ServiceFaultResponse & request)
{
  *this >> request.TypeId;
  *this >> request.Header;
}


} // namespace Binary
} // namespace OpcUa

