#include "internal_subscription.h"

#include <boost/thread/locks.hpp>

namespace OpcUa
{
namespace Internal
{

InternalSubscription::InternalSubscription(SubscriptionServiceInternal & service, const SubscriptionData & data, const NodeId & SessionAuthenticationToken, std::function<void (PublishResult)> callback, const Common::Logger::SharedPtr & logger)
  : Service(service)
  , AddressSpace(Service.GetAddressSpace())
  , Data(data)
  , CurrentSession(SessionAuthenticationToken)
  , Callback(callback)
  , io(service.GetIOService())
  , Timer(io, boost::posix_time::microseconds(static_cast<unsigned long>(1000 * data.RevisedPublishingInterval)))
  , LifeTimeCount(data.RevisedLifetimeCount)
  , Logger(logger)
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, create", Data.SubscriptionId);
}

void InternalSubscription::Start()
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, start", Data.SubscriptionId);
  std::shared_ptr<InternalSubscription> self = shared_from_this();
  Timer.async_wait([self](const boost::system::error_code & error) { self->PublishResults(error); });
}

InternalSubscription::~InternalSubscription()
{
  //Stop();
  LOG_DEBUG(Logger, "internal_subscription | id: {}, destroy", Data.SubscriptionId);
}

void InternalSubscription::Stop()
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, stop", Data.SubscriptionId);
  DeleteAllMonitoredItems();
  Timer.cancel();
}

void InternalSubscription::DeleteAllMonitoredItems()
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, DeleteAllMonitoredItems", Data.SubscriptionId);

  std::vector<uint32_t> handles;
  {
    boost::shared_lock<boost::shared_mutex> lock(DbMutex);

    for (auto pair : MonitoredDataChanges)
      {
        handles.push_back(pair.first);
      }
  }
  DeleteMonitoredItemsIds(handles);
}

bool InternalSubscription::HasExpired()
{
  bool expired = KeepAliveCount > LifeTimeCount ;

  if (expired)
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {} has expired: keep alive: {} > life time: {}", Data.SubscriptionId, KeepAliveCount, LifeTimeCount);
    }

  return expired;
}

void InternalSubscription::PublishResults(const boost::system::error_code & error)
{
  if (error)
    {
      LOG_WARN(Logger, "internal_subscription | id: {}, PublishResults: error: stopping subscription timer", Data.SubscriptionId);
      return;
    }

  if (HasExpired())
    {
      return;
    }

  if (HasPublishResult() && Service.PopPublishRequest(CurrentSession))   //Check we received a publishrequest before sending response
    {

      std::vector<PublishResult> results = PopPublishResult();

      if (results.size() > 0)
        {
          LOG_DEBUG(Logger, "internal_subscription | id: {}, have {} results", Data.SubscriptionId, results.size());

          if (Callback)
            {
              LOG_DEBUG(Logger, "internal_subscription | id: {}, calling callback", Data.SubscriptionId);
              Callback(results[0]);
            }

          else
            {
              LOG_DEBUG(Logger, "internal_subscription | id: {}, no callback defined for this subscription", Data.SubscriptionId);
            }
        }
    }

  TimerStopped = false;
  Timer.expires_at(Timer.expires_at() + boost::posix_time::microseconds(static_cast<unsigned long>(1000 * Data.RevisedPublishingInterval)));
  std::shared_ptr<InternalSubscription> self = shared_from_this();
  Timer.async_wait([self](const boost::system::error_code & error) { self->PublishResults(error); });
}


bool InternalSubscription::HasPublishResult()
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  if (Startup || !TriggeredDataChangeEvents.empty() || !TriggeredEvents.empty())
    {
      LOG_TRACE(Logger, "internal_subscription | id: {}, HasPublishResult: all queues empty, should send publish event", Data.SubscriptionId);
      return true;
    }

  if (KeepAliveCount > Data.RevisedMaxKeepAliveCount)   //we need to send keepalive notification
    {
      LOG_TRACE(Logger, "internal_subscription | id: {}, HasPublishResult: KeepAliveCount: {} > MaxKeepAliveCount: {}, should send publish event", Data.SubscriptionId, KeepAliveCount, Data.RevisedMaxKeepAliveCount);
      return true;
    }

  LOG_TRACE(Logger, "internal_subscription | id: {}, HasPublishResult: KeepAliveCount: {}, MaxKeepAliveCount: {}", Data.SubscriptionId, KeepAliveCount, Data.RevisedMaxKeepAliveCount);
  ++KeepAliveCount;
  return false;

}

std::vector<PublishResult> InternalSubscription::PopPublishResult()
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  LOG_DEBUG(Logger, "internal_subscription | id: {}, PopPublishResult: {} queued items", Data.SubscriptionId, TriggeredDataChangeEvents.size());
  PublishResult result;
  result.SubscriptionId = Data.SubscriptionId;
  result.NotificationMessage.PublishTime = DateTime::Current();

  if (!TriggeredDataChangeEvents.empty())
    {
      NotificationData data = GetNotificationData();
      result.NotificationMessage.NotificationData.push_back(data);
      result.Results.push_back(StatusCode::Good);
    }

  if (!TriggeredEvents.empty())
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {}, PopPublishResult: {} events to send", Data.SubscriptionId, TriggeredEvents.size());

      EventNotificationList notif;

      for (TriggeredEvent ev : TriggeredEvents)
        {
          notif.Events.push_back(ev.Data);
        }

      TriggeredEvents.clear();
      NotificationData data(notif);
      result.NotificationMessage.NotificationData.push_back(data);
      result.Results.push_back(StatusCode::Good);
    }

  // clear TriggerCount to enable new events for next
  // publishing cycle
  for (auto & mdc : MonitoredDataChanges)
    {
        mdc.second.TriggerCount = 0;
    }

  // FIXME: also add statuschange notification since they can be send in same result

  KeepAliveCount = 0;
  Startup = false;

  result.NotificationMessage.SequenceNumber = NotificationSequence;
  ++NotificationSequence;
  result.MoreNotifications = false;

  for (const PublishResult & res : NotAcknowledgedResults)
    {
      result.AvailableSequenceNumbers.push_back(res.NotificationMessage.SequenceNumber);
    }

  NotAcknowledgedResults.push_back(result);

  LOG_DEBUG(Logger, "internal_subscription | id: {}, sending PublishResult with: {} notifications", Data.SubscriptionId, result.NotificationMessage.NotificationData.size());

  std::vector<PublishResult> resultlist;
  resultlist.push_back(result);

  return resultlist;
}

RepublishResponse InternalSubscription::Republish(const RepublishParameters & params)
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, Republish request for sequence: {}", Data.SubscriptionId, params.RetransmitSequenceNumber);

  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  RepublishResponse response;

  for (const PublishResult & res : NotAcknowledgedResults)
    {
      if (res.NotificationMessage.SequenceNumber == params.RetransmitSequenceNumber)
        {
          response.NotificationMessage = res.NotificationMessage;
          return response;
        }
    }

  response.Header.ServiceResult = StatusCode::BadMessageNotAvailable;
  return response;
}

ModifySubscriptionResult InternalSubscription::ModifySubscription(const ModifySubscriptionParameters & data)
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, ModifySubscription", Data.SubscriptionId);

  ModifySubscriptionResult result;

  if (data.RequestedLifetimeCount)
    {
      Data.RevisedLifetimeCount = data.RequestedLifetimeCount;
    }

  LifeTimeCount = result.RevisedLifetimeCount = Data.RevisedLifetimeCount;

  if (data.RequestedPublishingInterval)
    {
      Data.RevisedPublishingInterval = data.RequestedPublishingInterval;
    }

  result.RevisedPublishingInterval = Data.RevisedPublishingInterval;

  if (data.RequestedMaxKeepAliveCount)
    {
      Data.RevisedMaxKeepAliveCount = data.RequestedMaxKeepAliveCount;
    }

  result.RevisedMaxKeepAliveCount = Data.RevisedMaxKeepAliveCount;

  return result;
}

NotificationData InternalSubscription::GetNotificationData()
{
  DataChangeNotification notification;

  for (const TriggeredDataChange & event : TriggeredDataChangeEvents)
    {
      notification.Notification.push_back(event.Data);
    }

  TriggeredDataChangeEvents.clear();
  NotificationData data(notification);
  return data;
}

void InternalSubscription::NewAcknowlegment(const SubscriptionAcknowledgement & ack)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  NotAcknowledgedResults.remove_if([&](PublishResult res) { return ack.SequenceNumber == res.NotificationMessage.SequenceNumber; });
}


MonitoredItemCreateResult InternalSubscription::CreateMonitoredItem(const MonitoredItemCreateRequest & request)
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, CreateMonitoredItem", Data.SubscriptionId);

  MonitoredItemCreateResult result;
  uint32_t callbackHandle = 0;
  {
    boost::unique_lock<boost::shared_mutex> lock(DbMutex);

    result.MonitoredItemId = ++LastMonitoredItemId;
    result.Status = OpcUa::StatusCode::Good;
    result.RevisedSamplingInterval = Data.RevisedPublishingInterval; // Force our own rate
    result.RevisedQueueSize = request.RequestedParameters.QueueSize; // We should check that value, maybe set to a default...
    result.FilterResult = request.RequestedParameters.Filter; // We can omit that one if we do not change anything in filter

    if (request.ItemToMonitor.AttributeId == AttributeId::EventNotifier)
      {
        LOG_DEBUG(Logger, "internal_subscription | id: {}, subscribe to event notifier", Data.SubscriptionId);
        LOG_TRACE(Logger, "internal_subscription | id: {}, {}", Data.SubscriptionId, result.FilterResult);

        // Client wants to subscribe to events
        // FIXME: check attribute EVENT notifier is set for the node
        MonitoredEvents[request.ItemToMonitor.NodeId] = result.MonitoredItemId;
      }
  }

  // Do not lock this part as it (indirectly) calls a locked AddressSpaceInMemory
  // function.
  // AddressSpaceInMemory functions call locked InternalSubscription functions
  // which will result in deadlocks when used from different threads
  if (request.ItemToMonitor.AttributeId != AttributeId::EventNotifier)
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {}, subscribe to data changes", Data.SubscriptionId);

      uint32_t id = result.MonitoredItemId;
      callbackHandle = AddressSpace.AddDataChangeCallback(request.ItemToMonitor.NodeId, request.ItemToMonitor.AttributeId, [this, id](const OpcUa::NodeId & nodeId, OpcUa::AttributeId attr, const DataValue & value)
      {
        this->DataChangeCallback(id, value);
      });
    }

  MonitoredDataChange mdata;
  {
    boost::unique_lock<boost::shared_mutex> lock(DbMutex);

    mdata.Parameters = result;
    mdata.Mode = request.MonitoringMode;
    mdata.TriggerCount = 0;
    mdata.ClientHandle = request.RequestedParameters.ClientHandle;
    mdata.CallbackHandle = callbackHandle;
    mdata.MonitoredItemId = result.MonitoredItemId;
    MonitoredDataChanges[result.MonitoredItemId] = mdata;
  }

  // Do not lock this part as it (indirectly) calls a locked AddressSpaceInMemory
  // function.
  LOG_DEBUG(Logger, "internal_subscription | id: {}, created MonitoredItem id: {}, ClientHandle: {}", Data.SubscriptionId, result.MonitoredItemId, mdata.ClientHandle);

  // Forcing event
  if (request.ItemToMonitor.AttributeId != AttributeId::EventNotifier)
    {
      TriggerDataChangeEvent(mdata, request.ItemToMonitor);
    }

  return result;
}

void InternalSubscription::TriggerDataChangeEvent(MonitoredDataChange monitoreditems, ReadValueId attrval)
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, TriggerDataChangeEvent: ClientHandle: {}", Data.SubscriptionId, monitoreditems.ClientHandle);

  ReadParameters params;
  params.AttributesToRead.push_back(attrval);
  std::vector<DataValue> vals = AddressSpace.Read(params);

  TriggeredDataChange event;
  event.MonitoredItemId = monitoreditems.MonitoredItemId;
  event.Data.ClientHandle = monitoreditems.ClientHandle;
  event.Data.Value = vals[0];
  {
    boost::unique_lock<boost::shared_mutex> lock(DbMutex);

    TriggeredDataChangeEvents.push_back(event);
  }
}

std::vector<StatusCode> InternalSubscription::DeleteMonitoredItemsIds(const std::vector<uint32_t> & monitoreditemsids)
{
  std::vector<StatusCode> results;

  for (const uint32_t & handle : monitoreditemsids)
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {}, DeletingMonitoredItemsIds: handle: {}", Data.SubscriptionId, handle);

      if (DeleteMonitoredEvent(handle))
        {
          results.push_back(StatusCode::Good);
          continue;
        }

      if (DeleteMonitoredDataChange(handle))
        {
          results.push_back(StatusCode::Good);
          continue;
        }

      results.push_back(StatusCode::BadMonitoredItemIdInvalid);

    }

  return results;
}

bool InternalSubscription::DeleteMonitoredDataChange(uint32_t handle)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  MonitoredDataChangeMap::iterator it = MonitoredDataChanges.find(handle);

  if (it == MonitoredDataChanges.end())
    {
      return false;
    }

  else
    {
      if (it->second.CallbackHandle != 0)  //if 0 this monitoreditem did not use callbacks
        {
          lock.unlock();
          // break deadlock condition: InternalSubscription <-> AddressSpace
          AddressSpace.DeleteDataChangeCallback(it->second.CallbackHandle);
          lock.lock();
        }

      MonitoredDataChanges.erase(handle);

      //We remove you our monitoreditem, now empty events which are already triggered
      for (auto ev = TriggeredDataChangeEvents.begin(); ev != TriggeredDataChangeEvents.end();)
        {
          if (ev->MonitoredItemId == handle)
            {
              LOG_DEBUG(Logger, "internal_subscription | id: {}, remove TriggeredDataChangeEvents of MonitoredItemId: {}", Data.SubscriptionId, handle);

              ev = TriggeredDataChangeEvents.erase(ev);
            }

          else
            {
              ++ev;
            }
        }

      return true;
    }
}

bool InternalSubscription::DeleteMonitoredEvent(uint32_t handle)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  for (auto pair : MonitoredEvents)
    {
      if (pair.second == handle)
        {
          MonitoredEvents.erase(pair.first);

          //We remove you our monitoreditem, now empty events which are already triggered
          for (auto ev = TriggeredEvents.begin(); ev != TriggeredEvents.end();)
            {
              if (ev->MonitoredItemId == handle)
                {
                  LOG_DEBUG(Logger, "internal_subscription | id: {}, remove TriggeredEvents of MonitoredItemId: {}", Data.SubscriptionId, handle);

                  ev = TriggeredEvents.erase(ev);
                }

              else
                {
                  ++ev;
                }
            }

          return true;
        }
    }

  return false;
}

void InternalSubscription::DataChangeCallback(const uint32_t & m_id, const DataValue & value)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  TriggeredDataChange event;
  MonitoredDataChangeMap::iterator it_monitoreditem = MonitoredDataChanges.find(m_id);

  if (it_monitoreditem == MonitoredDataChanges.end())
    {
      LOG_WARN(Logger, "internal_subscription | id: {}, DataChangeCallback called for unknown item: {}", Data.SubscriptionId, m_id);
      return ;
    }

  MonitoredDataChange& monitoredDataChange = it_monitoreditem->second;
  // spec says default sample interval for MonitoredItems is the same
  // as Subscription publishing interval, so bail out if event has been
  // triggered before
  if (monitoredDataChange.TriggerCount > 0)
    {
      return;
    }
  event.MonitoredItemId = it_monitoreditem->first;
  event.Data.ClientHandle = monitoredDataChange.ClientHandle;
  event.Data.Value = value;

  LOG_DEBUG(Logger, "internal_subscription | id: {}, enqueue TriggeredDataChange event: ClientHandle: {}", Data.SubscriptionId, event.Data.ClientHandle);

  ++monitoredDataChange.TriggerCount;
  TriggeredDataChangeEvents.push_back(event);
}

void InternalSubscription::TriggerEvent(NodeId node, Event event)
{
  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  MonitoredEventsMap::iterator it = MonitoredEvents.find(node);

  if (it == MonitoredEvents.end())
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {} does not monitor NodeId: {}", Data.SubscriptionId, node);

      return;
    }

  lock.unlock();//Enqueue vill need to set a unique lock
  EnqueueEvent(it->second, event);
}

bool InternalSubscription::EnqueueEvent(uint32_t monitoredItemId, const Event & event)
{
  LOG_DEBUG(Logger, "internal_subscription | id: {}, EnqueEvent: {}", Data.SubscriptionId, event);

  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  //Find monitoredItem
  std::map<uint32_t, MonitoredDataChange>::iterator mii_it =  MonitoredDataChanges.find(monitoredItemId);

  if (mii_it == MonitoredDataChanges.end())
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {}, MonitoredItemId: {} is already deleted", Data.SubscriptionId, monitoredItemId);

      return false;
    }

  //Check filter against event data and create EventFieldList to send
  //FIXME: Here we should also check event agains WhereClause of filter
  EventFieldList fieldlist;
  fieldlist.ClientHandle = mii_it->second.ClientHandle;
  fieldlist.EventFields = GetEventFields(mii_it->second.Parameters.FilterResult.Event, event);
  TriggeredEvent ev;
  ev.Data = fieldlist;
  ev.MonitoredItemId = monitoredItemId;
  TriggeredEvents.push_back(ev);
  return true;
}

std::vector<Variant> InternalSubscription::GetEventFields(const EventFilter & filter, const Event & event)
{
  //Go through filter and add value og matches as in spec
  std::vector<Variant> fields;

  LOG_DEBUG(Logger, "internal_subscription | id: {}, GetEventFields: filter size: {}", Data.SubscriptionId, filter.SelectClauses.size());

  for (SimpleAttributeOperand sattr : filter.SelectClauses)
    {
      LOG_DEBUG(Logger, "internal_subscription | id: {}, BrowsePath size: {}", Data.SubscriptionId, sattr.BrowsePath.size());

      if (sattr.BrowsePath.size() == 0)
        {
          fields.push_back(event.GetValue(sattr.Attribute));
        }

      else
        {
          LOG_DEBUG(Logger, "internal_subscription | id: {}, send value for: {}", Data.SubscriptionId, sattr.BrowsePath[0]);

          if (sattr.BrowsePath[0] == QualifiedName("EventId", 0))
            {
              fields.push_back(event.EventId);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("EventType", 0))
            {
              fields.push_back(event.EventType);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("SourceNode", 0))
            {
              fields.push_back(event.SourceNode);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("SourceName", 0))
            {
              fields.push_back(event.SourceName);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("Message", 0))
            {
              LOG_DEBUG(Logger, "internal_subscription | message is: {}", event.Message);

              fields.push_back(event.Message);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("Severity", 0))
            {
              fields.push_back(event.Severity);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("LocalTime", 0))
            {
              fields.push_back(event.LocalTime);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("ReceiveTime", 0))
            {
              fields.push_back(event.ReceiveTime);
            }

          else if (sattr.BrowsePath[0] == QualifiedName("Time", 0))
            {
              fields.push_back(event.Time);
            }

          else
            {
              fields.push_back(event.GetValue(sattr.BrowsePath));
            }
        }
    }

  return fields;
}


}
}


