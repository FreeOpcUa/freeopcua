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

  struct NotificationMessage
  {
    ExtensionObjectHeader Header;
    // TODO Notification message content.
  };

  ////////////////////////////////////////////////////////
  // NotificationData
  ////////////////////////////////////////////////////////

  struct NotificationData
  {
    uint32_t SequenceID;
    DateTime PublishTime;
    NotificationMessage Message;

    NotificationData();
  };

  ////////////////////////////////////////////////////////
  // PublishResult
  ////////////////////////////////////////////////////////

  struct PublishResult
  {
    IntegerID SubscriptionID;
    std::vector<uint32_t> AvailableSequenceNumber;
    bool MoreNotifications;
    NotificationData Data;
    std::vector<StatusCode> Statuses;
    std::vector<DiagnosticInfo> Diagnostics;

    PublishResult();
  };

  ////////////////////////////////////////////////////////
  // PublishResponse
  ////////////////////////////////////////////////////////

  struct PublishResponse
  {
    NodeID TypeID;
    ResponseHeader Header;
    PublishResult Result;

    PublishResponse();
  };

} // namespace OpcUa

#endif /// OPC_UA_PROTOCOL_SUBSCRIPTIONS_H
