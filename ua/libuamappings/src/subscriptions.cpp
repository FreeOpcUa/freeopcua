/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Subscription services structs.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/subscriptions.h>

namespace OpcUa
{
  ////////////////////////////////////////////////////////
  // SubscriptionParameters
  ////////////////////////////////////////////////////////

  SubscriptionParameters::SubscriptionParameters()
    : RequestedPublishingInterval(0)
    , RequestedLifetimeCount(1)
    , RequestedMaxKeepAliveCount(1)
    , MaxNotificationsPerPublish(1)
    , PublishingEnabled(true)
    , Priority(0)
  {
  }
  ////////////////////////////////////////////////////////
  // CreateSubscriptionRequest
  ////////////////////////////////////////////////////////

  CreateSubscriptionRequest::CreateSubscriptionRequest()
    : TypeID(CREATE_SUBSCRIPTION_REQUEST)
  {
  }

  ////////////////////////////////////////////////////////
  // SubscriptionData
  ////////////////////////////////////////////////////////

  SubscriptionData::SubscriptionData()
    : RevisedPublishingInterval(0)
    , RevisedLifetimeCount(1)
    , RevizedMaxKeepAliveCount(1)
  {
  }

  ////////////////////////////////////////////////////////
  // CreateSubscriptionResponse
  ////////////////////////////////////////////////////////

  CreateSubscriptionResponse::CreateSubscriptionResponse()
    : TypeID(CREATE_SUBSCRIPTION_RESPONSE)
  {
  }

  namespace Binary
  {
    ////////////////////////////////////////////////////////
    // SubscriptionParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<SubscriptionParameters>(const OpcUa::SubscriptionParameters& params)
    {
      return RawSize(params.RequestedPublishingInterval) +
          RawSize(params.RequestedLifetimeCount) +
          RawSize(params.RequestedMaxKeepAliveCount) +
          RawSize(params.MaxNotificationsPerPublish) +
          RawSize(params.PublishingEnabled) +
          RawSize(params.Priority);
    }

    template<>
    void IStream::Deserialize<SubscriptionParameters>(SubscriptionParameters& params)
    {
      *this >> params.RequestedPublishingInterval;
      *this >> params.RequestedLifetimeCount;
      *this >> params.RequestedMaxKeepAliveCount;
      *this >> params.MaxNotificationsPerPublish;
      *this >> params.PublishingEnabled;
      *this >> params.Priority;
    }

    template<>
    void OStream::Serialize<SubscriptionParameters>(const SubscriptionParameters& params)
    {
      *this << params.RequestedPublishingInterval;
      *this << params.RequestedLifetimeCount;
      *this << params.RequestedMaxKeepAliveCount;
      *this << params.MaxNotificationsPerPublish;
      *this << params.PublishingEnabled;
      *this << params.Priority;
    }

    ////////////////////////////////////////////////////////
    // CreateSubscriptionRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const CreateSubscriptionRequest& request)
    {
      return RawSize(request.TypeID) +
        RawSize(request.Header) +
        RawSize(request.Parameters);
    }

    template<>
    void IStream::Deserialize<CreateSubscriptionRequest>(CreateSubscriptionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    template<>
    void OStream::Serialize<CreateSubscriptionRequest>(const CreateSubscriptionRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    ////////////////////////////////////////////////////////
    // SubscriptionData
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const SubscriptionData& data)
    {
      return RawSize(data.ID) +
        RawSize(data.RevisedPublishingInterval) +
        RawSize(data.RevisedLifetimeCount) +
        RawSize(data.RevizedMaxKeepAliveCount);
    }

    template<>
    void IStream::Deserialize<SubscriptionData>(SubscriptionData& data)
    {
      *this >> data.ID;
      *this >> data.RevisedPublishingInterval;
      *this >> data.RevisedLifetimeCount;
      *this >> data.RevizedMaxKeepAliveCount;
    }

    template<>
    void OStream::Serialize<SubscriptionData>(const SubscriptionData& data)
    {
      *this << data.ID;
      *this << data.RevisedPublishingInterval;
      *this << data.RevisedLifetimeCount;
      *this << data.RevizedMaxKeepAliveCount;
    }

    ////////////////////////////////////////////////////////
    // CreateSubscriptionResponse
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const CreateSubscriptionResponse& response)
    {
      return RawSize(response.TypeID) +
        RawSize(response.Header) +
        RawSize(response.Data);
    }

    template<>
    void IStream::Deserialize<CreateSubscriptionResponse>(CreateSubscriptionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Data;
    }

    template<>
    void OStream::Serialize<CreateSubscriptionResponse>(const CreateSubscriptionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Data;
    }

  }
} // namespace OpcUa
