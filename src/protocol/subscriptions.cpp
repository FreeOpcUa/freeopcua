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
#include <opc/ua/protocol/monitored_items.h>

namespace OpcUa
{

////////////////////////////////////////////////////////
// SubscriptionData
////////////////////////////////////////////////////////

SubscriptionData::SubscriptionData()
  : RevisedPublishingInterval(100)
  , RevisedLifetimeCount(30) //Should be 3 times keepalive
  , RevisedMaxKeepAliveCount(10)
{
}

////////////////////////////////////////////////////////
// SubscriptionParameters
////////////////////////////////////////////////////////

CreateSubscriptionParameters::CreateSubscriptionParameters()
  : RequestedPublishingInterval(500)
  , RequestedLifetimeCount(3000)
  , RequestedMaxKeepAliveCount(10000)
  , MaxNotificationsPerPublish(0)
  , PublishingEnabled(true)
  , Priority(0)
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
// NotificationData
////////////////////////////////////////////////////////

NotificationData::NotificationData(DataChangeNotification notification) : DataChange(notification)
{
  //Header.TypeId  = ObjectId::DataChangeNotification;
  Header.TypeId  = ExpandedObjectId::DataChangeNotification;
  Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
}

NotificationData::NotificationData(EventNotificationList notification) : Events(notification)
{
  Header.TypeId  = ExpandedObjectId::EventNotificationList;
  Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
}

NotificationData::NotificationData(StatusChangeNotification notification) : StatusChange(notification)
{
  Header.TypeId  = ExpandedObjectId::StatusChangeNotification;
  Header.Encoding  = static_cast<ExtensionObjectEncoding>(Header.Encoding | ExtensionObjectEncoding::HAS_BINARY_BODY);
}


////////////////////////////////////////////////////////
// NotificationMessage
////////////////////////////////////////////////////////

NotificationMessage::NotificationMessage()
  : SequenceNumber(0)
  , PublishTime(DateTime::Current())
{
}

////////////////////////////////////////////////////////

namespace Binary
{
////////////////////////////////////////////////////////
// SubscriptionAcknowledgement -- to be removed
////////////////////////////////////////////////////////

template<>
std::size_t RawSize(const std::vector<SubscriptionAcknowledgement> & ack)
{
  return RawSizeContainer(ack);
}

template<>
void DataDeserializer::Deserialize<std::vector<SubscriptionAcknowledgement>>(std::vector<SubscriptionAcknowledgement> & ack)
{
  DeserializeContainer(*this, ack);
}

template<>
void DataSerializer::Serialize<std::vector<SubscriptionAcknowledgement>>(const std::vector<SubscriptionAcknowledgement> & ack)
{
  SerializeContainer(*this, ack);
}


////////////////////////////////////////////////////////
// MonitoredItems
////////////////////////////////////////////////////////

template<>
std::size_t RawSize(const MonitoredItems & request)
{
  return RawSize(request.ClientHandle) + RawSize(request.Value);
}

template<>
void DataDeserializer::Deserialize<MonitoredItems>(MonitoredItems & request)
{
  *this >> request.ClientHandle;
  *this >> request.Value;
}

template<>
void DataSerializer::Serialize<MonitoredItems>(const MonitoredItems & request)
{
  *this << request.ClientHandle;
  *this << request.Value;
}

template<>
void DataSerializer::Serialize<std::vector<MonitoredItems>>(const std::vector<MonitoredItems> & targets)
{
  SerializeContainer(*this, targets);
}

template<>
void DataDeserializer::Deserialize<std::vector<MonitoredItems>>(std::vector<MonitoredItems> & targets)
{
  DeserializeContainer(*this, targets);
}

////////////////////////////////////////////////////////
// StatusChangeNotification
////////////////////////////////////////////////////////

template<>
std::size_t RawSize(const StatusChangeNotification & request)
{
  return 4 + RawSize(request.Status) + RawSize(request.Diagnostic);
}

template<>
void DataDeserializer::Deserialize<StatusChangeNotification>(StatusChangeNotification & request)
{
  uint32_t tmp;
  *this >> tmp; //it seems we do not need the size
  *this >> request.Status;
  *this >> request.Diagnostic;
}

template<>
void DataSerializer::Serialize<StatusChangeNotification>(const StatusChangeNotification & request)
{
  *this << (uint32_t) RawSize(request);
  *this << request.Status;
  *this << request.Diagnostic;
}


////////////////////////////////////////////////////////
// EventNotificationList
////////////////////////////////////////////////////////

template<>
void DataSerializer::Serialize<std::vector<EventFieldList>>(const std::vector<EventFieldList> & targets)
{
  SerializeContainer(*this, targets);
}

template<>
void DataDeserializer::Deserialize<std::vector<EventFieldList>>(std::vector<EventFieldList> & targets)
{
  DeserializeContainer(*this, targets);
}

template<>
std::size_t RawSize(const EventFieldList & request)
{
  return RawSize(request.ClientHandle) + RawSizeContainer(request.EventFields);
}

template<>
void DataDeserializer::Deserialize<EventFieldList>(EventFieldList & request)
{
  *this >> request.ClientHandle;
  *this >> request.EventFields;
}

template<>
void DataSerializer::Serialize<EventFieldList>(const EventFieldList & request)
{
  *this << request.ClientHandle;
  *this << request.EventFields;
}

template<>
std::size_t RawSize(const EventNotificationList & request)
{
  return 4 + RawSizeContainer(request.Events);
}

template<>
void DataDeserializer::Deserialize<EventNotificationList>(EventNotificationList & request)
{
  uint32_t tmp;
  *this >> tmp; //it seems we do not need the size
  *this >> request.Events;
}

template<>
void DataSerializer::Serialize<EventNotificationList>(const EventNotificationList & request)
{
  *this << (uint32_t) RawSize(request);
  *this << request.Events;
}



////////////////////////////////////////////////////////
// DataChangeNotification
////////////////////////////////////////////////////////

template<>
std::size_t RawSize(const DataChangeNotification & request)
{
  return 4 + RawSizeContainer(request.Notification) + RawSize(request.Diagnostic);
}

template<>
void DataDeserializer::Deserialize<DataChangeNotification>(DataChangeNotification & request)
{
  uint32_t tmp;
  *this >> tmp; //it seems we do not need the size
  *this >> request.Notification;
  *this >> request.Diagnostic;
}

template<>
void DataSerializer::Serialize<DataChangeNotification>(const DataChangeNotification & request)
{
  *this << (uint32_t) RawSize(request);
  *this << request.Notification;
  *this << request.Diagnostic;
}


////////////////////////////////////////////////////////
// NotificationData
////////////////////////////////////////////////////////


template<>
std::size_t RawSize(const NotificationData & data)
{
  size_t total = 0;
  total += RawSize(data.Header);

  if (data.Header.TypeId == ExpandedObjectId::DataChangeNotification)
    {
      total += RawSize(data.DataChange);
    }

  else if (data.Header.TypeId == ExpandedObjectId::EventNotificationList)
    {
      total += RawSize(data.Events);
    }

  else if (data.Header.TypeId == ExpandedObjectId::StatusChangeNotification)
    {
      total += RawSize(data.StatusChange);
    }

  else
    {
      //Unknown type, we just ignore it
      //throw std::runtime_error("Uknown notificationData type" );
    }

  return total;
}

template<>
void DataDeserializer::Deserialize<NotificationData>(NotificationData & data)
{
  *this >> data.Header;

  if (data.Header.TypeId == ExpandedObjectId::DataChangeNotification)
    {
      *this >> data.DataChange;
    }

  else if (data.Header.TypeId == ExpandedObjectId::EventNotificationList)
    {
      *this >> data.Events;
    }

  else if (data.Header.TypeId == ExpandedObjectId::StatusChangeNotification)
    {
      *this >> data.StatusChange;
    }

  else
    {
      //Unknown type, we just ignore it
      //throw std::runtime_error("Uknown notification data type found in NotificationData");
    }
}

template<>
void DataSerializer::Serialize<NotificationData>(const NotificationData & data)
{
  *this << data.Header;

  if (data.Header.TypeId == ExpandedObjectId::DataChangeNotification)
    {
      *this << data.DataChange;
    }

  else if (data.Header.TypeId == ExpandedObjectId::EventNotificationList)
    {
      *this << data.Events;
    }

  else if (data.Header.TypeId == ExpandedObjectId::StatusChangeNotification)
    {
      *this << data.StatusChange;
    }

  else
    {
      //Unknown type, we just ignore it
      //throw std::runtime_error("Uknown notification data type found in NotificationData");// + itos(data.Header.TypeId.FourByteData.Identifier) );
    }
}

} // namespace Binary
} // namespace OpcUa
