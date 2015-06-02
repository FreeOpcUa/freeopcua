#ifndef __OPC_UA_MAPPING_TYPES_MANUAL_H__
#define __OPC_UA_MAPPING_TYPES_MANUAL_H__

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/variant.h>

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>
#include <atomic>


namespace OpcUa
{
  /// Moved from session.h
  struct UserIdentifyToken
  {
    ExtensionObjectHeader Header;
    std::vector<uint8_t> PolicyId;

    struct UserNameStruct
    {
      std::string UserName;
      std::string Password;
      std::string EncryptionAlgorithm;
    } UserName;

    UserIdentifyToken();

    UserTokenType type() const;
    void setUser(const std::string &user, const std::string &password);
    void setPolicyId(const std::string &id);
  };

  // Moved from subscriptions.h
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

  struct MonitoredItems
  {
    IntegerId ClientHandle;
    DataValue Value;
  };

  struct EventNotificationList
  {
    std::vector<EventFieldList> Events;
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

} // namespace OpcUa

#endif // __OPC_UA_MAPPING_TYPES_H__