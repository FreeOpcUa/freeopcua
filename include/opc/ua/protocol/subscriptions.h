/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Secure channel service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_PROTOCOL_SUBSCRIPTIONS_H
#define OPC_UA_PROTOCOL_SUBSCRIPTIONS_H

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/data_value.h>

namespace OpcUa
{
  ////////////////////////////////////////////////////////
  // SubscriptionData
  ////////////////////////////////////////////////////////

  struct SubscriptionParameters
  {
    Duration RequestedPublishingInterval;
    uint32_t RequestedLifetimeCount;
    uint32_t RequestedMaxKeepAliveCount;
    uint32_t MaxNotificationsPerPublish;
    bool PublishingEnabled;
    uint8_t Priority;

    SubscriptionParameters();
  };

  ////////////////////////////////////////////////////////
  // CreateSubscriptionRequest
  ////////////////////////////////////////////////////////

  struct CreateSubscriptionRequest
  {
    NodeID TypeID;
    RequestHeader Header;
    SubscriptionParameters Parameters;

    CreateSubscriptionRequest();
  };

  ////////////////////////////////////////////////////////
  // SubscriptionData
  ////////////////////////////////////////////////////////

  struct SubscriptionData
  {
    IntegerID ID;
    Duration RevisedPublishingInterval;
    uint32_t RevisedLifetimeCount;
    uint32_t RevizedMaxKeepAliveCount;

    SubscriptionData();
  };

  ////////////////////////////////////////////////////////
  // CreateSubscriptionResponse
  ////////////////////////////////////////////////////////

  struct CreateSubscriptionResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    SubscriptionData Data;

    CreateSubscriptionResponse();
  };

  ////////////////////////////////////////////////////////
  // SubscriptionAcknowledgement
  ////////////////////////////////////////////////////////

  struct SubscriptionAcknowledgement
  {
    IntegerID SubscriptionID;
    uint32_t SequenceNumber;

    SubscriptionAcknowledgement();
  };

  ////////////////////////////////////////////////////////
  // PublishParameters
  ////////////////////////////////////////////////////////

  struct PublishParameters
  {
    std::vector<SubscriptionAcknowledgement> Acknowledgements;
  };

  ////////////////////////////////////////////////////////
  // PublishRequest
  ////////////////////////////////////////////////////////

  struct PublishRequest
  {
    NodeID TypeID;
    RequestHeader Header;
    PublishParameters Parameters;

    PublishRequest();
  };

  ////////////////////////////////////////////////////////
  // NotificationMessage
  ////////////////////////////////////////////////////////

  struct StatusChangeNotification
  {
    StatusCode Status;
    DiagnosticInfo Diagnostic;
  };

  struct EventFieldList
  {
    IntegerID ClientHandle;
    //std::vector<BaseDataType> EventFields; FIXME
  };

  struct EventNotificationList
  {
    std::vector<EventFieldList> Events;
  };

  struct MonitoredItems
  {
    IntegerID ClientHandle;
    DataValue Value;
  };

  struct DataChangeNotification
  {
    std::vector<MonitoredItems> Notification;
    DiagnosticInfoList Diagnostic;
  };


  struct NotificationData
  {
    ExtensionObjectHeader Header;
    DataChangeNotification DataChange;
    EventNotificationList Events;
    StatusChangeNotification StatusChange;

    NotificationData(){}
    NotificationData(DataChangeNotification notification);
    NotificationData(EventNotificationList notification);
    NotificationData(StatusChangeNotification notification);
  };


  ////////////////////////////////////////////////////////
  // NotificationMessage
  ////////////////////////////////////////////////////////

  struct NotificationMessage
  {
    uint32_t SequenceID;
    DateTime PublishTime;
    std::vector<NotificationData> Data;

    NotificationMessage();
  };

  ////////////////////////////////////////////////////////
  // PublishResult
  ////////////////////////////////////////////////////////

  struct PublishResult
  {
    IntegerID SubscriptionID;
    std::vector<uint32_t> AvailableSequenceNumber;
    bool MoreNotifications;
    NotificationMessage Message;
    std::vector<StatusCode> Statuses;
    DiagnosticInfoList Diagnostics;

    PublishResult();
  };

  ////////////////////////////////////////////////////////
  // SetPublishingMode
  ////////////////////////////////////////////////////////

  struct PublishResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    PublishResult Result;

    PublishResponse();
  };

  struct PublishingModeParameters
  {
    bool Enabled;
    std::vector<IntegerID> SubscriptionIDs;

    PublishingModeParameters();
  };

  struct SetPublishingModeRequest
  {
    NodeID TypeID;
    RequestHeader Header;
    PublishingModeParameters Parameters;

    SetPublishingModeRequest();
  };

  struct PublishingModeResult
  {
    std::vector<StatusCode> Statuses;
    DiagnosticInfoList Diagnostics;
  };

  struct SetPublishingModeResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    PublishingModeResult Result;

    SetPublishingModeResponse();
  };

} // namespace OpcUa

#endif /// OPC_UA_PROTOCOL_SUBSCRIPTIONS_H
