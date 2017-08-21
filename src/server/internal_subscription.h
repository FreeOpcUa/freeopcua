
#pragma once

//#include "address_space_internal.h"
#include "subscription_service_internal.h"

#include <opc/ua/event.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/services/attributes.h>

#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <chrono>
#include <iostream>
#include <list>
#include <vector>


namespace OpcUa
{
namespace Internal
{

class SubscriptionServiceInternal;

//Structure to store description of a MonitoredItems
struct MonitoredDataChange
{
  uint32_t MonitoredItemId;
  MonitoringMode Mode;
  time_t LastTrigger;
  uint32_t TriggerCount;
  MonitoredItemCreateResult Parameters;
  uint32_t ClientHandle;
  uint32_t CallbackHandle;
};

struct TriggeredDataChange
{
  uint32_t MonitoredItemId;
  MonitoredItems Data;
};

struct TriggeredEvent
{
  uint32_t MonitoredItemId;
  EventFieldList Data;
};

//typedef std::pair<NodeId, AttributeId> MonitoredItemsIndex;
typedef std::map<uint32_t, MonitoredDataChange> MonitoredDataChangeMap;
typedef std::map<NodeId, uint32_t> MonitoredEventsMap;

class AddressSpaceInMemory; //pre-declaration


class InternalSubscription : public std::enable_shared_from_this<InternalSubscription>
{
public:
  InternalSubscription(SubscriptionServiceInternal & service, const SubscriptionData & data, const NodeId & SessionAuthenticationToken, std::function<void (PublishResult)> Callback, const Common::Logger::SharedPtr & logger);
  ~InternalSubscription();
  void Start();
  void Stop();

  void NewAcknowlegment(const SubscriptionAcknowledgement & ack);
  std::vector<StatusCode> DeleteMonitoredItemsIds(const std::vector<uint32_t> & ids);
  bool EnqueueEvent(uint32_t monitoreditemid, const Event & event);
  bool EnqueueDataChange(uint32_t monitoreditemid, const DataValue & value);
  MonitoredItemCreateResult CreateMonitoredItem(const MonitoredItemCreateRequest & request);
  void DataChangeCallback(const uint32_t &, const DataValue & value);
  bool HasExpired();
  void TriggerEvent(NodeId node, Event event);
  RepublishResponse Republish(const RepublishParameters & params);
  ModifySubscriptionResult ModifySubscription(const ModifySubscriptionParameters & data);

private:
  void DeleteAllMonitoredItems();
  bool DeleteMonitoredEvent(uint32_t handle);
  bool DeleteMonitoredDataChange(uint32_t handle);
  std::vector<PublishResult> PopPublishResult();
  bool HasPublishResult();
  NotificationData GetNotificationData();
  void PublishResults(const boost::system::error_code & error);
  std::vector<Variant> GetEventFields(const EventFilter & filter, const Event & event);
  void TriggerDataChangeEvent(MonitoredDataChange monitoreditems, ReadValueId attrval);

private:
  SubscriptionServiceInternal & Service;
  Server::AddressSpace & AddressSpace;
  mutable boost::shared_mutex DbMutex;
  SubscriptionData Data;
  const NodeId CurrentSession;
  std::function<void (PublishResult)> Callback;

  uint32_t NotificationSequence = 1; //NotificationSequence start at 1! not 0
  uint32_t KeepAliveCount = 0;
  bool Startup = true; //To force specific behaviour at startup
  uint32_t LastMonitoredItemId = 100;
  MonitoredDataChangeMap MonitoredDataChanges;
  MonitoredEventsMap MonitoredEvents;
  std::list<PublishResult> NotAcknowledgedResults; //result that have not be acknowledeged and may have to be resent
  std::list<TriggeredDataChange> TriggeredDataChangeEvents;
  std::list<TriggeredEvent> TriggeredEvents;
  boost::asio::io_service & io;
  boost::asio::deadline_timer Timer;
  bool TimerStopped = false;
  uint32_t LifeTimeCount;
  Common::Logger::SharedPtr Logger;

};


}
}


