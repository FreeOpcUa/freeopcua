#ifndef __OPC_UA_MAPPING_TYPES_MANUAL_H__
#define __OPC_UA_MAPPING_TYPES_MANUAL_H__

#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/attribute_ids.h>

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
  void setUser(const std::string & user, const std::string & password);
  void setPolicyId(const std::string & id);
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

  NotificationData() {}
  NotificationData(DataChangeNotification notification);
  NotificationData(EventNotificationList notification);
  NotificationData(StatusChangeNotification notification);
};


// moved from monitored_items.h

struct DataChangeFilter
{
  DataChangeTrigger Trigger;
  DeadbandType Deadband;
  double DeadbandValue;
};

struct SimpleAttributeOperand
{
  NodeId TypeId;
  std::vector<QualifiedName> BrowsePath;
  AttributeId Attribute;
  std::vector<std::string> IndexRange;
};

struct ElementOperand
{
  uint32_t Index;
};

struct LiteralOperand
{
  // BaseDataType Value; // TODO
  Variant Value;
};

struct AttributeOperand
{
  NodeId Node;
  std::string Alias;
  RelativePath Path;
  IntegerId AttributeId;
  std::vector<std::string> IndexRange;
};

struct FilterOperand
{
  ExtensionObjectHeader Header;
  ElementOperand Element;
  LiteralOperand Literal;
  AttributeOperand Attribute;
  SimpleAttributeOperand SimpleAttribute;
};

struct ContentFilterElement
{
  FilterOperator Operator;
  std::vector<FilterOperand> FilterOperands;
};

struct EventFilter
{
  std::vector<SimpleAttributeOperand> SelectClauses;
  std::vector<ContentFilterElement> WhereClause;
};

struct AggregateFilter
{
  DateTime StartTime;
  NodeId AggregateType;
  Duration ProcessingInterval;
  //AggregateConfiguration Configuration; //aggregate conf is in fact the following parameters
  bool UseServerCapabilitiesDefaults;
  bool TreatUncertainAsBad;
  uint8_t PercentDataBad;
  uint8_t PercentDataGood;
  bool SteppedSlopedExtrapolation;
};

struct MonitoringFilter
{
  ExtensionObjectHeader Header;
  DataChangeFilter DataChange;
  EventFilter Event;
  AggregateFilter Aggregate;
  MonitoringFilter() {}
  MonitoringFilter(DataChangeFilter filter);
  MonitoringFilter(EventFilter filter);
  MonitoringFilter(AggregateFilter filter);
};

} // namespace OpcUa

#endif // __OPC_UA_MAPPING_TYPES_H__