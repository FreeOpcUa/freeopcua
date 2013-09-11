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

#include "binary_serialization.h"

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

  ////////////////////////////////////////////////////////
  // SubscriptionAcknowledgement
  ////////////////////////////////////////////////////////

  SubscriptionAcknowledgement::SubscriptionAcknowledgement()
    : SequenceNumber(0)
  {
  }

  ////////////////////////////////////////////////////////
  // PublishRequest
  ////////////////////////////////////////////////////////

  PublishRequest::PublishRequest()
    : TypeID(PUBLISH_REQUEST)
  {
  }

  ////////////////////////////////////////////////////////
  // PublishResult
  ////////////////////////////////////////////////////////

  PublishResult::PublishResult()
    : MoreNotifications(false)
  {
  }

  ////////////////////////////////////////////////////////
  // PublishResponse
  ////////////////////////////////////////////////////////

  PublishResponse::PublishResponse()
    : TypeID(PUBLISH_RESPONSE)
  {
  }

  ////////////////////////////////////////////////////////
  // NotificationData
  ////////////////////////////////////////////////////////

  NotificationMessage::NotificationMessage()
    : SequenceID(1)
    , PublishTime(CurrentDateTime())
  {
  }

  ////////////////////////////////////////////////////////
  // PublishingModeParameters
  ////////////////////////////////////////////////////////

  PublishingModeParameters::PublishingModeParameters()
    : Enabled(false)
  {
  }

  ////////////////////////////////////////////////////////
  // SetPublishingModeRequest
  ////////////////////////////////////////////////////////

  SetPublishingModeRequest::SetPublishingModeRequest()
    : TypeID(SET_PUBLISHING_MODE_REQUEST)
  {
  }

  ////////////////////////////////////////////////////////
  // SetPublishingModeResponse
  ////////////////////////////////////////////////////////

  SetPublishingModeResponse::SetPublishingModeResponse()
    : TypeID(OpcUa::SET_PUBLISHING_MODE_RESPONSE)
  {
  }

  ////////////////////////////////////////////////////////

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
    void DataDeserializer::Deserialize<SubscriptionParameters>(SubscriptionParameters& params)
    {
      *this >> params.RequestedPublishingInterval;
      *this >> params.RequestedLifetimeCount;
      *this >> params.RequestedMaxKeepAliveCount;
      *this >> params.MaxNotificationsPerPublish;
      *this >> params.PublishingEnabled;
      *this >> params.Priority;
    }

    template<>
    void DataSerializer::Serialize<SubscriptionParameters>(const SubscriptionParameters& params)
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
    void DataDeserializer::Deserialize<CreateSubscriptionRequest>(CreateSubscriptionRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    template<>
    void DataSerializer::Serialize<CreateSubscriptionRequest>(const CreateSubscriptionRequest& request)
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
    void DataDeserializer::Deserialize<SubscriptionData>(SubscriptionData& data)
    {
      *this >> data.ID;
      *this >> data.RevisedPublishingInterval;
      *this >> data.RevisedLifetimeCount;
      *this >> data.RevizedMaxKeepAliveCount;
    }

    template<>
    void DataSerializer::Serialize<SubscriptionData>(const SubscriptionData& data)
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
    void DataDeserializer::Deserialize<CreateSubscriptionResponse>(CreateSubscriptionResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Data;
    }

    template<>
    void DataSerializer::Serialize<CreateSubscriptionResponse>(const CreateSubscriptionResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Data;
    }

    ////////////////////////////////////////////////////////
    // SubscriptionAcknowledgement
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const SubscriptionAcknowledgement& ack)
    {
      return RawSize(ack.SubscriptionID) + RawSize(ack.SequenceNumber);
    }

    template<>
    void DataDeserializer::Deserialize<SubscriptionAcknowledgement>(SubscriptionAcknowledgement& ack)
    {
      *this >> ack.SubscriptionID;
      *this >> ack.SequenceNumber;
    }

    template<>
    void DataSerializer::Serialize<SubscriptionAcknowledgement>(const SubscriptionAcknowledgement& ack)
    {
      *this << ack.SubscriptionID;
      *this << ack.SequenceNumber;
    }

    template<>
    std::size_t RawSize(const std::vector<SubscriptionAcknowledgement>& ack)
    {
      return RawSizeContainer(ack);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<SubscriptionAcknowledgement>>(std::vector<SubscriptionAcknowledgement>& ack)
    {
      DeserializeContainer(*this, ack);
    }

    template<>
    void DataSerializer::Serialize<std::vector<SubscriptionAcknowledgement>>(const std::vector<SubscriptionAcknowledgement>& ack)
    {
      SerializeContainer(*this, ack);
    }


    ////////////////////////////////////////////////////////
    // PublishParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const PublishParameters& params)
    {
      return RawSize(params.Acknowledgements);
    }

    template<>
    void DataDeserializer::Deserialize<PublishParameters>(PublishParameters& params)
    {
      *this >> params.Acknowledgements;
    }

    template<>
    void DataSerializer::Serialize<PublishParameters>(const PublishParameters& params)
    {
      *this << params.Acknowledgements;
    }

    ////////////////////////////////////////////////////////
    // PublishRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const PublishRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void DataDeserializer::Deserialize<PublishRequest>(PublishRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    template<>
    void DataSerializer::Serialize<PublishRequest>(const PublishRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    ////////////////////////////////////////////////////////
    // NotificationMessage
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const NotificationData& data)
    {
      return RawSize(data.Header);
    }

    template<>
    void DataDeserializer::Deserialize<NotificationData>(NotificationData& data)
    {
      *this >> data.Header;
    }

    template<>
    void DataSerializer::Serialize<NotificationData>(const NotificationData& data)
    {
      *this << data.Header;
    }

    ////////////////////////////////////////////////////////
    // NotificationMessage
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const NotificationMessage& message)
    {
      return RawSize(message.SequenceID) +
        RawSize(message.PublishTime) +
        RawSizeContainer(message.Data);
    }

    template<>
    void DataDeserializer::Deserialize<NotificationMessage>(NotificationMessage& message)
    {
      *this >> message.SequenceID;
      *this >> message.PublishTime;
      DeserializeContainer(*this, message.Data);
    }

    template<>
    void DataSerializer::Serialize<NotificationMessage>(const NotificationMessage& message)
    {
      *this << message.SequenceID;
      *this << message.PublishTime;
      SerializeContainer(*this, message.Data, 0);
    }

    ////////////////////////////////////////////////////////
    // PublishResult
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const PublishResult& result)
    {
      return RawSize(result.SubscriptionID) +
        RawSizeContainer(result.AvailableSequenceNumber) +
        RawSize(result.MoreNotifications) +
        RawSize(result.Message) +
        RawSizeContainer(result.Statuses) +
        RawSizeContainer(result.Diagnostics);
    }

    template<>
    void DataDeserializer::Deserialize<PublishResult>(PublishResult& result)
    {
      *this >> result.SubscriptionID;
      DeserializeContainer(*this, result.AvailableSequenceNumber);
      *this >> result.MoreNotifications;
      *this >> result.Message;
      DeserializeContainer(*this, result.Statuses);
      DeserializeContainer(*this, result.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<PublishResult>(const PublishResult& result)
    {
      *this << result.SubscriptionID;
      SerializeContainer(*this, result.AvailableSequenceNumber, 0);
      *this << result.MoreNotifications;
      *this << result.Message;
      SerializeContainer(*this, result.Statuses, 0);
      SerializeContainer(*this, result.Diagnostics, 0);
    }

    ////////////////////////////////////////////////////////
    // PublishResponse
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const PublishResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) + RawSize(response.Result);
    }

    template<>
    void DataDeserializer::Deserialize<PublishResponse>(PublishResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Result;
    }

    template<>
    void DataSerializer::Serialize<PublishResponse>(const PublishResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Result;
    }

    ////////////////////////////////////////////////////////
    // PublishingModeParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const PublishingModeParameters& params)
    {
      return RawSize(params.Enabled) + RawSizeContainer(params.SubscriptionIDs);
    }

    template<>
    void DataDeserializer::Deserialize<PublishingModeParameters>(PublishingModeParameters& params)
    {
      *this >> params.Enabled;
      DeserializeContainer(*this, params.SubscriptionIDs);
    }

    template<>
    void DataSerializer::Serialize<PublishingModeParameters>(const PublishingModeParameters& params)
    {
      *this << params.Enabled;
      SerializeContainer(*this, params.SubscriptionIDs, 0);
    }

    ////////////////////////////////////////////////////////
    // SetPublishingModeRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const SetPublishingModeRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void DataDeserializer::Deserialize<SetPublishingModeRequest>(SetPublishingModeRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    template<>
    void DataSerializer::Serialize<SetPublishingModeRequest>(const SetPublishingModeRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    ////////////////////////////////////////////////////////
    // SetPublishingModeResult
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const PublishingModeResult& result)
    {
      return RawSizeContainer(result.Statuses) + RawSizeContainer(result.Diagnostics);
    }

    template<>
    void DataDeserializer::Deserialize<PublishingModeResult>(PublishingModeResult& result)
    {
      DeserializeContainer(*this, result.Statuses);
      DeserializeContainer(*this, result.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<PublishingModeResult>(const PublishingModeResult& result)
    {
      SerializeContainer(*this, result.Statuses, 0);
      SerializeContainer(*this, result.Diagnostics, 0);
    }

    ////////////////////////////////////////////////////////
    // SetPublishingModeResponse
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const SetPublishingModeResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) + RawSize(response.Result);
    }

    template<>
    void DataDeserializer::Deserialize<SetPublishingModeResponse>(SetPublishingModeResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Result;
    }

    template<>
    void DataSerializer::Serialize<SetPublishingModeResponse>(const SetPublishingModeResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Result;
    }

  } // namespace Binary
} // namespace OpcUa
