/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary secure channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/protocol.h>
#include <opc/ua/protocol/binary/stream.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
FindServersRequest::FindServersRequest()
  : TypeId(FIND_ServerS_REQUEST)
{

}

FindServersResponse::FindServersResponse()
  : TypeId(FIND_ServerS_RESPONSE)
{

}

namespace Binary
{

//-----------------------------------------------------
// FindServersParameters
//-----------------------------------------------------

template<>
std::size_t RawSize<FindServersParameters>(const FindServersParameters & params)
{
  return RawSize(params.EndpointUrl) +
         RawSizeContainer(params.Locales) +
         RawSizeContainer(params.ServersToReturn);
}

template<>
void DataSerializer::Serialize<FindServersParameters>(const FindServersParameters & params)
{
  *this << params.EndpointUrl;
  *this << params.Locales;
  *this << params.ServersToReturn;
}

template<>
void DataDeserializer::Deserialize<FindServersParameters>(FindServersParameters & params)
{
  *this >> params.EndpointUrl;
  *this >> params.Locales;
  *this >> params.ServersToReturn;
}

//---------------------------------------------------
// FindServersRequest
//---------------------------------------------------

template<>
std::size_t RawSize<FindServersRequest>(const FindServersRequest & request)
{
  return RawSize(request.TypeId) + RawSize(request.Header) + RawSize(request.Parameters);
}

template<>
void DataSerializer::Serialize<FindServersRequest>(const FindServersRequest & request)
{
  *this << request.TypeId;
  *this << request.Header;
  *this << request.Parameters;
}

template<>
void DataDeserializer::Deserialize<FindServersRequest>(FindServersRequest & request)
{
  *this >> request.TypeId;
  *this >> request.Header;
  *this >> request.Parameters;
}

//-----------------------------------------------------
// FindServersData
//-----------------------------------------------------

template<>
std::size_t RawSize<FindServersData>(const FindServersData & data)
{
  return RawSizeContainer(data.Descriptions);
}

template<>
void DataSerializer::Serialize<FindServersData>(const FindServersData & data)
{
  SerializeContainer(*this, data.Descriptions);
}

template<>
void DataDeserializer::Deserialize<FindServersData>(FindServersData & data)
{
  DeserializeContainer(*this, data.Descriptions);
}

//-----------------------------------------------------
// FindServersResponse
//-----------------------------------------------------

template<>
std::size_t RawSize<FindServersResponse>(const FindServersResponse & resp)
{
  return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Data);
}

template<>
void DataSerializer::Serialize<FindServersResponse>(const FindServersResponse & resp)
{
  *this << resp.TypeId;
  *this << resp.Header;
  *this << resp.Data;
}

template<>
void DataDeserializer::Deserialize<FindServersResponse>(FindServersResponse & resp)
{
  *this >> resp.TypeId;
  *this >> resp.Header;
  *this >> resp.Data;
}

}
}

