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
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
  ApplicationDescription::ApplicationDescription()
    : Type(ApplicationType::Client)
  {
  }

  GetEndpointsRequest::GetEndpointsRequest()
    : TypeId(GET_ENDPOINTS_REQUEST)
  {
  }

  GetEndpointsResponse::GetEndpointsResponse()
    : TypeId(GET_ENDPOINTS_RESPONSE)
  {
  }

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

    //---------------------------------------------------
    // EndpointsFilter
    //---------------------------------------------------

    template<>
    std::size_t RawSize<EndpointsFilter>(const EndpointsFilter& filter)
    {
      return RawSize(filter.EndpointURL) + RawSize(filter.LocaleIds) + RawSize(filter.ProfileUries);
    }

    template<>
    void DataSerializer::Serialize<EndpointsFilter>(const EndpointsFilter& filter)
    {
      *this << filter.EndpointURL;

      if (filter.LocaleIds.empty())
      {
        *this << uint32_t(0);
      }
      else
      {
        *this << filter.LocaleIds;
      }

      if (filter.ProfileUries.empty())
      {
        *this << uint32_t(0);
      }
      else
      {
        *this << filter.ProfileUries;
      }
    }

    template<>
    void DataDeserializer::Deserialize<EndpointsFilter>(EndpointsFilter& filter)
    {
      *this >> filter.EndpointURL;
      *this >> filter.LocaleIds;
      *this >> filter.ProfileUries;
    };

    //---------------------------------------------------
    // GetEndpointsRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<GetEndpointsRequest>(const GetEndpointsRequest& request)
    {
      return RawSize(request.TypeId) + RawSize(request.Header) + RawSize(request.Filter);
      //return RawSize(request.TypeId) + RawSize(request.Header) + RawSize(request.EndpointURL) + RawSize(request.LocaleIds) + RawSize(request.ProfileUries);
    }

    template<>
    void DataSerializer::Serialize<GetEndpointsRequest>(const GetEndpointsRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
      *this << request.Filter;
    }

    template<>
    void DataDeserializer::Deserialize<GetEndpointsRequest>(GetEndpointsRequest& request)
    {
      *this >> request.TypeId;
      *this >> request.Header;
      *this >> request.Filter;
    }

    //-----------------------------------------------------
    // UserIdentifyTokenType
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<UserIdentifyTokenType>(const UserIdentifyTokenType& type)
    {
      return 4;
    };

    template<>
    void DataSerializer::Serialize<UserIdentifyTokenType>(const UserIdentifyTokenType& type)
    {
      *this << static_cast<uint32_t>(type);
    }

    template<>
    void DataDeserializer::Deserialize<UserIdentifyTokenType>(UserIdentifyTokenType& type)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      type = static_cast<UserIdentifyTokenType>(tmp);
    };


    //-----------------------------------------------------
    // UserTokenPolicy
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<UserTokenPolicy>(const UserTokenPolicy& policy)
    {
      return RawSize(policy.PolicyId) +
             RawSize(policy.TokenType) +
             RawSize(policy.IssuedTokenType) +
             RawSize(policy.IssuerEndpointURL) +
             RawSize(policy.SecurityPolicyURI);
    };

    template<>
    std::size_t RawSize<std::vector<UserTokenPolicy>>(const std::vector<UserTokenPolicy>& vec)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(vec.begin(), vec.end(), [&] (const UserTokenPolicy& policy) {totalSize += RawSize(policy);});
      return totalSize;
    }

    template<>
    void DataSerializer::Serialize<UserTokenPolicy>(const UserTokenPolicy& policy)
    {
      *this << policy.PolicyId;
      *this << policy.TokenType;
      *this << policy.IssuedTokenType;
      *this << policy.IssuerEndpointURL;
      *this << policy.SecurityPolicyURI;
    }

    template<>
    void DataDeserializer::Deserialize<UserTokenPolicy>(UserTokenPolicy& policy)
    {
      *this >> policy.PolicyId;
      *this >> policy.TokenType;
      *this >> policy.IssuedTokenType;
      *this >> policy.IssuerEndpointURL;
      *this >> policy.SecurityPolicyURI;
    };

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
    // ApplicationDescription
    //-----------------------------------------------------

    template<>
    std::size_t RawSize<ApplicationDescription>(const ApplicationDescription& desc)
    {
      return RawSize(desc.URI) +
             RawSize(desc.ProductURI) +
             RawSize(desc.Name) +
             RawSize(desc.Type) +
             RawSize(desc.GatewayServerURI) +
             RawSize(desc.DiscoveryProfileURI) +
             RawSize(desc.DiscoveryURLs);
    };

    template<>
    void DataSerializer::Serialize<ApplicationDescription>(const ApplicationDescription& desc)
    {
      *this << desc.URI;
      *this << desc.ProductURI;
      *this << desc.Name;
      *this << desc.Type;
      *this << desc.GatewayServerURI;
      *this << desc.DiscoveryProfileURI;
      *this << desc.DiscoveryURLs;
    }

    template<>
    void DataDeserializer::Deserialize<ApplicationDescription>(ApplicationDescription& desc)
    {
      *this >> desc.URI;
      *this >> desc.ProductURI;
      *this >> desc.Name;
      *this >> desc.Type;
      *this >> desc.GatewayServerURI;
      *this >> desc.DiscoveryProfileURI;
      *this >> desc.DiscoveryURLs;
    }

    template<>
    void DataSerializer::Serialize<std::vector<ApplicationDescription>>(const std::vector<ApplicationDescription>& vec)
    {
      SerializeContainer(*this, vec);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<ApplicationDescription>>(std::vector<ApplicationDescription>& vec)
    {
      DeserializeContainer(*this, vec);
    }

    //-----------------------------------------------------
    // EndpointDescription
    //-----------------------------------------------------


    template<>
    std::size_t RawSize<EndpointDescription>(const EndpointDescription& desc)
    {
      return RawSize(desc.EndpointURL) +
             RawSize(desc.ServerDescription) +
             RawSize(desc.ServerCertificate) +
             RawSize(desc.SecurityMode) +
             RawSize(desc.SecurityPolicyURI) +
             RawSize(desc.UserIdentifyTokens) +
             RawSize(desc.TransportProfileURI) +
             sizeof(desc.SecurityLevel);
    }

    template<>
    std::size_t RawSize<std::vector<EndpointDescription>>(const std::vector<EndpointDescription>& vec)
    {
      const std::size_t headerSize = 4;
      std::size_t totalSize = headerSize;
      std::for_each(vec.begin(), vec.end(), [&] (const EndpointDescription& desc) {totalSize += RawSize(desc);});
      return totalSize;
    }

    template<>
    void DataSerializer::Serialize<EndpointDescription>(const EndpointDescription& desc)
    {
      *this << desc.EndpointURL;
      *this << desc.ServerDescription;
      *this << desc.ServerCertificate;
      *this << desc.SecurityMode;
      *this << desc.SecurityPolicyURI;
      *this << desc.UserIdentifyTokens;
      *this << desc.TransportProfileURI;
      *this << desc.SecurityLevel;
    }

    template<>
    void DataDeserializer::Deserialize<EndpointDescription>(EndpointDescription& desc)
    {
      *this >> desc.EndpointURL;
      *this >> desc.ServerDescription;
      *this >> desc.ServerCertificate;
      *this >> desc.SecurityMode;
      *this >> desc.SecurityPolicyURI;
      *this >> desc.UserIdentifyTokens;
      *this >> desc.TransportProfileURI;
      *this >> desc.SecurityLevel;
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

    template<>
    std::size_t RawSize<GetEndpointsResponse>(const GetEndpointsResponse& resp)
    {
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Endpoints);
    }

    template<>
    void DataSerializer::Serialize<GetEndpointsResponse>(const GetEndpointsResponse& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.Endpoints;
    }

    template<>
    void DataDeserializer::Deserialize<GetEndpointsResponse>(GetEndpointsResponse& resp)
    {
      *this >> resp.TypeId;
      *this >> resp.Header;
      *this >> resp.Endpoints;
    }

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

