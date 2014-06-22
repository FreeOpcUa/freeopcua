/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Subscription services structs.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/monitored_items.h>

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

    NotificationData::NotificationData(DataChangeNotification notification)
    {
      //Header.TypeID  = ObjectID::DataChangeNotification; 
      Header.TypeID  = FourByteNodeID(811, 0); 
      Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
      DataChange = notification;
    }

    NotificationData::NotificationData(EventNotificationList notification)
    {
      Header.TypeID  = ObjectID::EventNotificationList; 
      Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
      Events = notification;
    }

    NotificationData::NotificationData(StatusChangeNotification notification)
    {
      Header.TypeID  = ObjectID::StatusChangeNotification; 
      Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
      StatusChange = notification;
    }



  ////////////////////////////////////////////////////////
  // NotificationMessage
  ////////////////////////////////////////////////////////

  NotificationMessage::NotificationMessage()
    : SequenceID(0)
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
    // MonitoredItems
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const MonitoredItems& request)
    {
      return RawSize(request.ClientHandle) + RawSize(request.Value);
    }

    template<>
    void DataDeserializer::Deserialize<MonitoredItems>(MonitoredItems& request)
    {
      *this >> request.ClientHandle;
      *this >> request.Value;
    }

    template<>
    void DataSerializer::Serialize<MonitoredItems>(const MonitoredItems& request)
    {
      *this << request.ClientHandle;
      *this << request.Value;
    }

    template<>
    void DataSerializer::Serialize<std::vector<MonitoredItems>>(const std::vector<MonitoredItems>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<MonitoredItems>>(std::vector<MonitoredItems>& targets)
    {
      DeserializeContainer(*this, targets);
    }



    ////////////////////////////////////////////////////////
    // DataChangeNotification
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize(const DataChangeNotification& request)
    {
      return 4 + RawSizeContainer(request.Notification) + RawSize(request.Diagnostic);
    }

    template<>
    void DataDeserializer::Deserialize<DataChangeNotification>(DataChangeNotification& request)
    {
      uint32_t tmp;
      *this >> tmp; //it seems we do not need the size
      *this >> request.Notification;
      *this >> request.Diagnostic;
    }

    template<>
    void DataSerializer::Serialize<DataChangeNotification>(const DataChangeNotification& request)
    {
      *this << (uint32_t) RawSize(request);
      *this << request.Notification;
      *this << request.Diagnostic;
    }


    ////////////////////////////////////////////////////////
    // NotificationData
    ////////////////////////////////////////////////////////


    template<>
    std::size_t RawSize(const NotificationData& data)
    {
      size_t total = 0;
      total += RawSize(data.Header);
      if ( data.Header.TypeID == FourByteNodeID(811, 0) )
      {
        total += RawSize(data.DataChange);
      }
      return total;
    }

    template<>
    void DataDeserializer::Deserialize<NotificationData>(NotificationData& data)
    {
      *this >> data.Header;
      if ( data.Header.TypeID == FourByteNodeID(811, 0) ) 
      {
          *this >> data.DataChange;
      }
    }

    template<>
    void DataSerializer::Serialize<NotificationData>(const NotificationData& data)
    {
      *this << data.Header;
      if (data.Header.TypeID == FourByteNodeID(811, 0) )
      {
        *this << data.DataChange;
      }
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
    ////////////////////////////////////////////////////////////////
    // FilterOperator
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<FilterOperator>(const FilterOperator&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<FilterOperator>(const FilterOperator& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<FilterOperator>(FilterOperator& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<FilterOperator>(tmp);
    }


    ////////////////////////////////////////////////////////////////
    // DeadbandType
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<DeadbandType>(const DeadbandType&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<DeadbandType>(const DeadbandType& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<DeadbandType>(DeadbandType& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<DeadbandType>(tmp);
    }


    ////////////////////////////////////////////////////////////////
    // DataChangeTrigger
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<DataChangeTrigger>(const DataChangeTrigger&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<DataChangeTrigger>(const DataChangeTrigger& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<DataChangeTrigger>(DataChangeTrigger& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<DataChangeTrigger>(tmp);
    }

    ////////////////////////////////////////////////////////////////
    // MonitoringMode
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoringMode>(const MonitoringMode&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<MonitoringMode>(const MonitoringMode& mode)
    {
      *this << static_cast<uint32_t>(mode);
    }

    template<>
    void DataDeserializer::Deserialize<MonitoringMode>(MonitoringMode& mode)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      mode = static_cast<MonitoringMode>(tmp);
    }
    ////////////////////////////////////////////////////////
    // AttributeOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<AttributeOperand>(const OpcUa::AttributeOperand& params)
    {
      return RawSize(params.Node) +
          RawSize(params.Alias) +
          RawSize(params.Path) +
          RawSize(params.AttributeID) +
          RawSizeContainer(params.IndexRange); 
    }

    template<>
    void DataDeserializer::Deserialize<AttributeOperand>(AttributeOperand& params)
    {
      *this >> params.Node;
      *this >> params.Alias;
      *this >> params.Path;
      *this >> params.AttributeID;
      *this >> params.IndexRange;
    }

    template<>
    void DataSerializer::Serialize<AttributeOperand>(const AttributeOperand& params)
    {
      *this << params.Node;
      *this << params.Alias;
      *this << params.Path;
      *this << params.AttributeID;
      *this << params.IndexRange;
    }

    ////////////////////////////////////////////////////////
    // SimpleAttributeOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<SimpleAttributeOperand>(const OpcUa::SimpleAttributeOperand& params)
    {
      return RawSize(params.TypeID) +
          RawSize(params.BrowsePath) +
          RawSize(params.AttributeID) +
          RawSizeContainer(params.IndexRange); 
    }

    template<>
    void DataDeserializer::Deserialize<SimpleAttributeOperand>(SimpleAttributeOperand& params)
    {
      *this >> params.TypeID;
      *this >> params.BrowsePath;
      *this >> params.AttributeID;
      *this >> params.IndexRange;
    }

    template<>
    void DataSerializer::Serialize<SimpleAttributeOperand>(const SimpleAttributeOperand& params)
    {
      *this << params.TypeID;
      *this << params.BrowsePath;
      *this << params.AttributeID;
      *this << params.IndexRange;
    }

    template<>
    void DataSerializer::Serialize<std::vector<SimpleAttributeOperand>>(const std::vector<SimpleAttributeOperand>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<SimpleAttributeOperand>>(std::vector<SimpleAttributeOperand>& targets)
    {
      DeserializeContainer(*this, targets);
    }


    ////////////////////////////////////////////////////////
    // ElementOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<ElementOperand>(const OpcUa::ElementOperand& params)
    {
      return RawSize(params.Index);
    }

    template<>
    void DataDeserializer::Deserialize<ElementOperand>(ElementOperand& params)
    {
      *this >> params.Index;
    }

    template<>
    void DataSerializer::Serialize<ElementOperand>(const ElementOperand& params)
    {
      *this << params.Index;
    }


    ////////////////////////////////////////////////////////
    // FilterOperand
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<FilterOperand>(const OpcUa::FilterOperand& params)
    {
      return RawSize(params.TypeID) +
          RawSize(params.Element) +
          RawSize(params.Attribute) +
          RawSize(params.SimpleAttribute); 
    }

    template<>
    void DataDeserializer::Deserialize<FilterOperand>(FilterOperand& params)
    {
      *this >> params.TypeID;
      *this >> params.Element;
      *this >> params.Attribute;
      *this >> params.SimpleAttribute;
    }

    template<>
    void DataSerializer::Serialize<FilterOperand>(const FilterOperand& params)
    {
      *this << params.TypeID;
      *this << params.Element;
      *this << params.Attribute;
      *this << params.SimpleAttribute;
    }
    
    template<>
    void DataSerializer::Serialize<std::vector<FilterOperand>>(const std::vector<FilterOperand>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<FilterOperand>>(std::vector<FilterOperand>& targets)
    {
      DeserializeContainer(*this, targets);
    }



    ////////////////////////////////////////////////////////
    // ContentFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<ContentFilter>(const OpcUa::ContentFilter& params)
    {
      return RawSize(params.Operator) +
          RawSizeContainer(params.FilterOperands); 
    }

    template<>
    void DataDeserializer::Deserialize<ContentFilter>(ContentFilter& params)
    {
      *this >> params.Operator;
      *this >> params.FilterOperands;
    }

    template<>
    void DataSerializer::Serialize<ContentFilter>(const ContentFilter& params)
    {
      *this << params.Operator;
      *this << params.FilterOperands;
    }




    ////////////////////////////////////////////////////////
    // AggregateFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<AggregateFilter>(const OpcUa::AggregateFilter& params)
    {
      return RawSize(params.StartTime) +
          RawSize(params.AggregateType) +
          RawSize(params.ProcessingInterval) +
          RawSize(params.UseServerCapabilitiesDefaults) +
          RawSize(params.TreatUncertainAsBad) +
          RawSize(params.PercentDataBad) +
          RawSize(params.PercentDataGood) +
          RawSize(params.SteppedSlopedExtrapolation);
    }

    template<>
    void DataDeserializer::Deserialize<AggregateFilter>(AggregateFilter& params)
    {
      *this >> params.StartTime;
      *this >> params.AggregateType;
      *this >> params.ProcessingInterval;
      *this >> params.UseServerCapabilitiesDefaults;
      *this >> params.TreatUncertainAsBad;
      *this >> params.PercentDataBad;
      *this >> params.PercentDataGood;
      *this >> params.SteppedSlopedExtrapolation;
    }

    template<>
    void DataSerializer::Serialize<AggregateFilter>(const AggregateFilter& params)
    {
      *this << params.StartTime;
      *this << params.AggregateType;
      *this << params.ProcessingInterval;
      *this << params.UseServerCapabilitiesDefaults;
      *this << params.TreatUncertainAsBad;
      *this << params.PercentDataBad;
      *this << params.PercentDataGood;
      *this << params.SteppedSlopedExtrapolation;
    }



    ////////////////////////////////////////////////////////
    // EventFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<EventFilter>(const OpcUa::EventFilter& params)
    {
      return RawSizeContainer(params.SelectClauses) +
          RawSize(params.WhereClause);
    }

    template<>
    void DataDeserializer::Deserialize<EventFilter>(EventFilter& params)
    {
      *this >> params.SelectClauses;
      *this >> params.WhereClause;
    }

    template<>
    void DataSerializer::Serialize<EventFilter>(const EventFilter& params)
    {
      *this << params.SelectClauses;
      *this << params.WhereClause;
    }


    ////////////////////////////////////////////////////////
    // DataChangeFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<DataChangeFilter>(const OpcUa::DataChangeFilter& params)
    {
      return RawSize(params.Trigger) +
          RawSize(params.Deadband) +
          RawSize(params.DeadbandValue);
    }

    template<>
    void DataDeserializer::Deserialize<DataChangeFilter>(DataChangeFilter& params)
    {
      *this >> params.Trigger;
      *this >> params.Deadband;
      *this >> params.DeadbandValue;
    }

    template<>
    void DataSerializer::Serialize<DataChangeFilter>(const DataChangeFilter& params)
    {
      *this << params.Trigger;
      *this << params.Deadband;
      *this << params.DeadbandValue;
    }


    ////////////////////////////////////////////////////////
    // ExtensionObjectMonitoringFilter
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<ExtensionObjectMonitoringFilter>(const OpcUa::ExtensionObjectMonitoringFilter& params)
    {
     const std::size_t sizeofEncoding = 1;
      return RawSize(params.typeID) +
          RawSize(sizeofEncoding); 
    }

    template<>
    void DataDeserializer::Deserialize<ExtensionObjectMonitoringFilter>(ExtensionObjectMonitoringFilter& params)
    {
      *this >> params.typeID;
      uint8_t tmp = 0;
      *this >> tmp;
      params.Encoding = static_cast<ExtensionObjectEncoding>(tmp);
    }

    template<>
    void DataSerializer::Serialize<ExtensionObjectMonitoringFilter>(const ExtensionObjectMonitoringFilter& params)
    {
      *this << params.typeID;
      *this << static_cast<uint8_t>(params.Encoding);
    }

 

    ////////////////////////////////////////////////////////
    // MonitoringParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoringParameters>(const OpcUa::MonitoringParameters& params)
    {
      return RawSize(params.ClientHandle) +
          RawSize(params.SamplingInterval) +
          RawSize(params.Filter) +
          RawSize(params.QueueSize) +
          RawSize(params.DiscardOldest); 
    }

    template<>
    void DataDeserializer::Deserialize<MonitoringParameters>(MonitoringParameters& params)
    {
      *this >> params.ClientHandle;
      *this >> params.SamplingInterval;
      *this >> params.Filter;
      *this >> params.QueueSize;
      *this >> params.DiscardOldest;
    }

    template<>
    void DataSerializer::Serialize<MonitoringParameters>(const MonitoringParameters& params)
    {
      *this << params.ClientHandle;
      *this << params.SamplingInterval;
      *this << params.Filter;
      *this << params.QueueSize;
      *this << params.DiscardOldest;
    }

    ////////////////////////////////////////////////////////
    // MonitoredItemRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoredItemRequest>(const OpcUa::MonitoredItemRequest& params)
    {
      return RawSize(params.ItemToMonitor) +
          RawSize(params.Mode) +
          RawSize(params.Parameters); 
    }

    template<>
    void DataDeserializer::Deserialize<MonitoredItemRequest>(MonitoredItemRequest& params)
    {
      *this >> params.ItemToMonitor;
      *this >> params.Mode;
      *this >> params.Parameters;
    }

    template<>
    void DataSerializer::Serialize<MonitoredItemRequest>(const MonitoredItemRequest& params)
    {
      *this << params.ItemToMonitor;
      *this << params.Mode;
      *this << params.Parameters;
    }

    template<>
    void DataSerializer::Serialize<std::vector<MonitoredItemRequest>>(const std::vector<MonitoredItemRequest>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<MonitoredItemRequest>>(std::vector<MonitoredItemRequest>& targets)
    {
      DeserializeContainer(*this, targets);
    }


    ////////////////////////////////////////////////////////
    // MonitoredItemsParameters
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<MonitoredItemsParameters>(const OpcUa::MonitoredItemsParameters& params)
    {
      return RawSize(params.SubscriptionID) +
          RawSize(params.Timestamps) +
          RawSizeContainer(params.ItemsToCreate); 
    }

    template<>
    void DataDeserializer::Deserialize<MonitoredItemsParameters>(MonitoredItemsParameters& params)
    {
      *this >> params.SubscriptionID;
      *this >> params.Timestamps;
      *this >> params.ItemsToCreate;
    }

    template<>
    void DataSerializer::Serialize<MonitoredItemsParameters>(const MonitoredItemsParameters& params)
    {
      *this << params.SubscriptionID;
      *this << params.Timestamps;
      *this << params.ItemsToCreate;
    }

    ////////////////////////////////////////////////////////
    // CreateMonitoredItemsRequest
    ////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<CreateMonitoredItemsRequest>(const OpcUa::CreateMonitoredItemsRequest& params)
    {
      return RawSize(params.TypeID) +
          RawSize(params.Header) +
          RawSize(params.Parameters); 
    }

    template<>
    void DataDeserializer::Deserialize<CreateMonitoredItemsRequest>(CreateMonitoredItemsRequest& params)
    {
      *this >> params.TypeID;
      *this >> params.Header;
      *this >> params.Parameters;
    }

    template<>
    void DataSerializer::Serialize<CreateMonitoredItemsRequest>(const CreateMonitoredItemsRequest& params)
    {
      *this << params.TypeID;
      *this << params.Header;
      *this << params.Parameters;
    }






  } // namespace Binary
} // namespace OpcUa
