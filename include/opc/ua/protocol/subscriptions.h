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
  // DeleteSubscriptionRequest
  ////////////////////////////////////////////////////////

  struct DeleteSubscriptionRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    std::vector<IntegerId> SubscriptionsIds;

    DeleteSubscriptionRequest();
  };

  struct DeleteSubscriptionResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    std::vector<StatusCode> Results;
    DiagnosticInfoList Diagnostic;

    DeleteSubscriptionResponse();
  };

  ////////////////////////////////////////////////////////
  // CreateSubscriptionRequest
  ////////////////////////////////////////////////////////

  struct CreateSubscriptionRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    SubscriptionParameters Parameters;

    CreateSubscriptionRequest();
  };

  ////////////////////////////////////////////////////////
  // SubscriptionData
  ////////////////////////////////////////////////////////

  struct SubscriptionData
  {
    IntegerId Id;
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
    NodeId TypeId;
    ResponseHeader Header;
    SubscriptionData Data;

    CreateSubscriptionResponse();
  };

  ////////////////////////////////////////////////////////
  // SubscriptionAcknowledgement
  ////////////////////////////////////////////////////////

  struct SubscriptionAcknowledgement
  {
    IntegerId SubscriptionId;
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

  struct RepublishParameters
  {
    IntegerId Subscription;
    uint32_t Counter = 0;
  };


  ////////////////////////////////////////////////////////
  // PublishRequest
  ////////////////////////////////////////////////////////

  struct PublishRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    PublishParameters Parameters;

    PublishRequest();
  };

  struct RepublishRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    RepublishParameters Parameters;

    RepublishRequest();
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
    IntegerId ClientHandle;
    std::vector<Variant> EventFields; 
  };

  struct EventNotificationList
  {
    std::vector<EventFieldList> Events;
  };

  struct MonitoredItems
  {
    IntegerId ClientHandle;
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
    uint32_t SequenceId;
    DateTime PublishTime;
    std::vector<NotificationData> Data;

    NotificationMessage();
  };

  ////////////////////////////////////////////////////////
  // PublishResult
  ////////////////////////////////////////////////////////

  struct PublishResult
  {
    IntegerId SubscriptionId;
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
    NodeId TypeId;
    ResponseHeader Header;
    PublishResult Result;

    PublishResponse();
  };

  struct RepublishResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    NotificationMessage Message;

    RepublishResponse();
  };


  struct PublishingModeParameters
  {
    bool Enabled;
    std::vector<IntegerId> SubscriptionIds;

    PublishingModeParameters();
  };

  struct SetPublishingModeRequest
  {
    NodeId TypeId;
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
    NodeId TypeId;
    ResponseHeader Header;
    PublishingModeResult Result;

    SetPublishingModeResponse();
  };

} // namespace OpcUa

#endif /// OPC_UA_PROTOCOL_SUBSCRIPTIONS_H
