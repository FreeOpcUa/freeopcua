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
    // UserTokenPolicy
    //-----------------------------------------------------
    template<>
    std::size_t RawSize<std::vector<UserTokenPolicy>>(const std::vector<UserTokenPolicy>& vec)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(vec.begin(), vec.end(), [&] (const UserTokenPolicy& policy) {totalSize += RawSize(policy);});
      return totalSize;
    }

    template<>
    void DataSerializer::Serialize<std::vector<UserTokenPolicy>>(const std::vector<UserTokenPolicy>& vec)
    {
      SerializeContainer(*this, vec);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<UserTokenPolicy>>(std::vector<UserTokenPolicy>& vec)
    {
      DeserializeContainer(*this, vec);
    }

    //-----------------------------------------------------
    // EndpointDescription
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<std::vector<EndpointDescription>>(const std::vector<EndpointDescription>& vec)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(vec.begin(), vec.end(), [&] (const EndpointDescription& desc) {totalSize += RawSize(desc);});
      return totalSize;
    }

    template<>
    void DataSerializer::Serialize<std::vector<EndpointDescription>>(const std::vector<EndpointDescription>& vec)
    {
      SerializeContainer(*this, vec);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<EndpointDescription>>(std::vector<EndpointDescription>& vec)
    {
      DeserializeContainer(*this, vec);
    }


    //-----------------------------------------------------
    // GetEndpointsResponse
    //-----------------------------------------------------

//     template<>
//     std::size_t RawSize<GetEndpointsResponse>(const GetEndpointsResponse& resp)
//     {
//       return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Endpoints);
//     }
//
//     template<>
//     void DataSerializer::Serialize<GetEndpointsResponse>(const GetEndpointsResponse& resp)
//     {
//       *this << resp.TypeId;
//       *this << resp.Header;
//       *this << resp.Endpoints;
//     }
//
//     template<>
//     void DataDeserializer::Deserialize<GetEndpointsResponse>(GetEndpointsResponse& resp)
//     {
//       *this >> resp.TypeId;
//       *this >> resp.Header;
//       *this >> resp.Endpoints;
//     }

    //-----------------------------------------------------
    // FindServersParameters
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<FindServersParameters>(const FindServersParameters& params)
    {
      return RawSize(params.EndpointURL) +
             RawSizeContainer(params.Locales) +
             RawSizeContainer(params.ServersToReturn);
    }

    template<>
    void DataSerializer::Serialize<FindServersParameters>(const FindServersParameters& params)
    {
      *this << params.EndpointURL;
      *this << params.Locales;
      *this << params.ServersToReturn;
    }

    template<>
    void DataDeserializer::Deserialize<FindServersParameters>(FindServersParameters& params)
    {
      *this >> params.EndpointURL;
      *this >> params.Locales;
      *this >> params.ServersToReturn;
    }

    //---------------------------------------------------
    // FindServersRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<FindServersRequest>(const FindServersRequest& request)
    {
      return RawSize(request.TypeId) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void DataSerializer::Serialize<FindServersRequest>(const FindServersRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<FindServersRequest>(FindServersRequest& request)
    {
      *this >> request.TypeId;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    //-----------------------------------------------------
    // FindServersData
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<FindServersData>(const FindServersData& data)
    {
      return RawSizeContainer(data.Descriptions);
    }

    template<>
    void DataSerializer::Serialize<FindServersData>(const FindServersData& data)
    {
      SerializeContainer(*this, data.Descriptions);
    }

    template<>
    void DataDeserializer::Deserialize<FindServersData>(FindServersData& data)
    {
      DeserializeContainer(*this, data.Descriptions);
    }

    //-----------------------------------------------------
    // FindServersResponse
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<FindServersResponse>(const FindServersResponse& resp)
    {
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Data);
    }

    template<>
    void DataSerializer::Serialize<FindServersResponse>(const FindServersResponse& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.Data;
    }

    template<>
    void DataDeserializer::Deserialize<FindServersResponse>(FindServersResponse& resp)
    {
      *this >> resp.TypeId;
      *this >> resp.Header;
      *this >> resp.Data;
    }

  }
}

