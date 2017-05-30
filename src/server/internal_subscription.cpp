#include "internal_subscription.h"

#include <boost/thread/locks.hpp>

namespace OpcUa
{
namespace Internal
{

InternalSubscription::InternalSubscription(SubscriptionServiceInternal & service, const SubscriptionData & data, const NodeId & SessionAuthenticationToken, std::function<void (PublishResult)> callback, bool debug)
  : Service(service)
  , AddressSpace(Service.GetAddressSpace())
  , Data(data)
  , CurrentSession(SessionAuthenticationToken)
  , Callback(callback)
  , io(service.GetIOService())
  , Timer(io, boost::posix_time::milliseconds(data.RevisedPublishingInterval))
  , LifeTimeCount(data.RevisedLifetimeCount)
  , Debug(debug)
{
}

void InternalSubscription::Start()
{
  std::shared_ptr<InternalSubscription> self = shared_from_this();
  Timer.async_wait([self](const boost::system::error_code & error) { self->PublishResults(error); });
}

InternalSubscription::~InternalSubscription()
{
  //Stop();
}

void InternalSubscription::Stop()
{
  DeleteAllMonitoredItems();
  Timer.cancel();
}

void InternalSubscription::DeleteAllMonitoredItems()
{
  if (Debug) { std::cout << "InternalSubscription | Deleting all monitoreditems" << std::endl; }

  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<uint32_t> handles;

  for (auto pair : MonitoredDataChanges)
    {
      handles.push_back(pair.first);
    }

  lock.unlock();
  DeleteMonitoredItemsIds(handles);
}

bool InternalSubscription::HasExpired()
{
  bool expired = KeepAliveCount > LifeTimeCount ;

  if (expired)
    {
      if (Debug) { std::cout << "InternalSubscription | Subscription has expired " << KeepAliveCount << "  " << LifeTimeCount << std::endl; }
    }

  return expired;
}

void InternalSubscription::PublishResults(const boost::system::error_code & error)
{
  if (error)
    {
      if (Debug) { std::cout << "InternalSubscription | Stopping subscription timer" << std::endl; }

      return;
    }

  if (HasExpired())
    {
      if (Debug) { std::cout << "InternalSubscription | Subscription has expired" << std::endl; }

      return;
    }

  if (HasPublishResult() && Service.PopPublishRequest(CurrentSession))   //Check we received a publishrequest before sening respomse
    {

      std::vector<PublishResult> results = PopPublishResult();

      if (results.size() > 0)
        {
          if (Debug) { std::cout << "InternalSubscription | Subscription has " << results.size() << " results, calling callback" << std::endl; }

          if (Callback)
            {
              Callback(results[0]);
            }

          else
            {
              if (Debug) { std::cout << "InternalSubcsription | No callback defined for this subscription" << std::endl; }
            }
        }
    }

  TimerStopped = false;
  Timer.expires_at(Timer.expires_at() + boost::posix_time::milliseconds(Data.RevisedPublishingInterval));
  std::shared_ptr<InternalSubscription> self = shared_from_this();
  Timer.async_wait([self](const boost::system::error_code & error) { self->PublishResults(error); });
}


bool InternalSubscription::HasPublishResult()
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  if (Startup || ! TriggeredDataChangeEvents.empty() || ! TriggeredEvents.empty())
    {
      return true;
    }

  if (KeepAliveCount > Data.RevisedMaxKeepAliveCount)   //we need to send keepalive notification
    {
      if (Debug) { std::cout << "InternalSubscription | KeepAliveCount " << KeepAliveCount << " is > than MaxKeepAliveCount " <<  Data.RevisedMaxKeepAliveCount << " sending publish event" << std::endl; }

      return true;
    }

  ++KeepAliveCount;
  return false;

}

std::vector<PublishResult> InternalSubscription::PopPublishResult()
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  //std::cout << "PopPublishresult for subscription: " << Data.SubscriptionId << " with " << TriggeredDataChangeEvents.size() << " triggered items in queue" << std::endl;
  PublishResult result;
  result.SubscriptionId = Data.SubscriptionId;
  result.NotificationMessage.PublishTime = DateTime::Current();

  if (! TriggeredDataChangeEvents.empty())
    {
      NotificationData data = GetNotificationData();
      result.NotificationMessage.NotificationData.push_back(data);
      result.Results.push_back(StatusCode::Good);
    }

  if (! TriggeredEvents.empty())
    {
      if (Debug) { std::cout << "InternalSubcsription | Subscription " << Data.SubscriptionId << " has " << TriggeredEvents.size() << " events to send to client" << std::endl; }

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

  if (Debug) { std::cout << "InternalSubcsription | Sending Notification with " << result.NotificationMessage.NotificationData.size() << " notifications"  << std::endl; }

  std::vector<PublishResult> resultlist;
  resultlist.push_back(result);

  return resultlist;
}

RepublishResponse InternalSubscription::Republish(const RepublishParameters & params)
{
  if (Debug) { std::cout << "SubscriptionService| RepublishRequest for sequence: " << params.RetransmitSequenceNumber << std::endl; }

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
  if (Debug) { std::cout << "SubscriptionService| Creating monitored item." << std::endl; }

  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  MonitoredItemCreateResult result;
  uint32_t callbackHandle = 0;
  result.MonitoredItemId = ++LastMonitoredItemId;

  if (request.ItemToMonitor.AttributeId == AttributeId::EventNotifier)
    {
      if (Debug) { std::cout << "SubscriptionService| Subscribed o event notifier " << std::endl; }

      //client want to subscribe to events
      //FIXME: check attribute EVENT notifier is set for the node
      MonitoredEvents[request.ItemToMonitor.NodeId] = result.MonitoredItemId;
    }

  else
    {
      if (Debug) { std::cout << "SubscriptionService| Subscribing to data chanes in the address space." << std::endl; }

      uint32_t id = result.MonitoredItemId;
      callbackHandle = AddressSpace.AddDataChangeCallback(request.ItemToMonitor.NodeId, request.ItemToMonitor.AttributeId, [this, id](const OpcUa::NodeId & nodeId, OpcUa::AttributeId attr, const DataValue & value)
      {
        this->DataChangeCallback(id, value);
      });

      if (callbackHandle == 0)
        {
          if (Debug) { std::cout << "SubscriptionService| ERROR: address returned zero handle." << std::endl; }

          --LastMonitoredItemId; //revert increment
          result.Status = OpcUa::StatusCode::BadNodeAttributesInvalid;
          return result;
        }
    }

  result.Status = OpcUa::StatusCode::Good;
  result.RevisedSamplingInterval = Data.RevisedPublishingInterval; //Force our own rate
  result.RevisedQueueSize = request.RequestedParameters.QueueSize; // We should check that value, maybe set to a default...
  result.FilterResult = request.RequestedParameters.Filter; //We can omit that one if we do not change anything in filter
  MonitoredDataChange mdata;
  mdata.Parameters = result;
  mdata.Mode = request.MonitoringMode;
  mdata.ClientHandle = request.RequestedParameters.ClientHandle;
  mdata.CallbackHandle = callbackHandle;
  mdata.MonitoredItemId = result.MonitoredItemId;
  MonitoredDataChanges[result.MonitoredItemId] = mdata;

  if (Debug) { std::cout << "Created MonitoredItem with id: " << result.MonitoredItemId << " and client handle " << mdata.ClientHandle << std::endl; }

  //Forcing event,
  if (request.ItemToMonitor.AttributeId != AttributeId::EventNotifier)
    {
      TriggerDataChangeEvent(mdata, request.ItemToMonitor);
    }

  return result;
}

void InternalSubscription::TriggerDataChangeEvent(MonitoredDataChange monitoreditems, ReadValueId attrval)
{
  if (Debug) { std::cout << "InternalSubcsription | Manual Trigger of DataChangeEvent for sub: " << Data.SubscriptionId << " and clienthandle: " << monitoreditems.ClientHandle << std::endl; }

  ReadParameters params;
  params.AttributesToRead.push_back(attrval);
  std::vector<DataValue> vals = AddressSpace.Read(params);

  TriggeredDataChange event;
  event.MonitoredItemId = monitoreditems.MonitoredItemId;
  event.Data.ClientHandle = monitoreditems.ClientHandle;
  event.Data.Value = vals[0];
  TriggeredDataChangeEvents.push_back(event);
}

std::vector<StatusCode> InternalSubscription::DeleteMonitoredItemsIds(const std::vector<uint32_t> & monitoreditemsids)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<StatusCode> results;

  for (const uint32_t & handle : monitoreditemsids)
    {
      if (Debug) { std::cout << "InternalSubcsription | Deleting Monitoreditemsid: " << handle << std::endl; }

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
  MonitoredDataChangeMap::iterator it = MonitoredDataChanges.find(handle);

  if (it == MonitoredDataChanges.end())
    {
      return false;
    }

  else
    {
      if (it->second.CallbackHandle != 0)  //if 0 this monitoreditem did not use callbacks
        {
          AddressSpace.DeleteDataChangeCallback(it->second.CallbackHandle);
        }

      MonitoredDataChanges.erase(handle);

      //We remove you our monitoreditem, now empty events which are already triggered
      for (auto ev = TriggeredDataChangeEvents.begin(); ev != TriggeredDataChangeEvents.end();)
        {
          if (ev->MonitoredItemId == handle)
            {
              if (Debug) { std::cout << "InternalSubscription | Remove triggeredEvent for monitoreditemid " << handle << std::endl; }

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
                  if (Debug) { std::cout << "InternalSubscription | Remove triggeredEvent for monitoreditemid " << handle << std::endl; }

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
      std::cout << "InternalSubcsription | DataChangeCallback called for unknown item" << std::endl;
      return ;
    }

  event.MonitoredItemId = it_monitoreditem->first;
  event.Data.ClientHandle = it_monitoreditem->second.ClientHandle;
  event.Data.Value = value;

  if (Debug) { std::cout << "InternalSubcsription | Enqueued DataChange triggered item for sub: " << Data.SubscriptionId << " and clienthandle: " << event.Data.ClientHandle << std::endl; }

  TriggeredDataChangeEvents.push_back(event);
}

void InternalSubscription::TriggerEvent(NodeId node, Event event)
{
  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  MonitoredEventsMap::iterator it = MonitoredEvents.find(node);

  if (it == MonitoredEvents.end())
    {
      if (Debug) { std::cout << "InternalSubcsription | Subscription: " << Data.SubscriptionId << " has no subcsription for this event" << std::endl; }

      return;
    }

  lock.unlock();//Enqueue vill need to set a unique lock
  EnqueueEvent(it->second, event);
}

bool InternalSubscription::EnqueueEvent(uint32_t monitoreditemid, const Event & event)
{
  if (Debug) { std::cout << "InternalSubcsription | Enqueing event to be send" << std::endl; }

  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  if (Debug) { std::cout << "enqueueing event: " << event << std::endl;}

  //Find monitoredItem
  std::map<uint32_t, MonitoredDataChange>::iterator mii_it =  MonitoredDataChanges.find(monitoreditemid);

  if (mii_it == MonitoredDataChanges.end())
    {
      if (Debug) { std::cout << "InternalSubcsription | monitoreditem " << monitoreditemid << " is already deleted" << std::endl; }

      return false;
    }

  //Check filter against event data and create EventFieldList to send
  //FIXME: Here we should also check event agains WhereClause of filter
  EventFieldList fieldlist;
  fieldlist.ClientHandle = mii_it->second.ClientHandle;
  fieldlist.EventFields = GetEventFields(mii_it->second.Parameters.FilterResult.Event, event);
  TriggeredEvent ev;
  ev.Data = fieldlist;
  ev.MonitoredItemId = monitoreditemid;
  TriggeredEvents.push_back(ev);
  return true;
}

std::vector<Variant> InternalSubscription::GetEventFields(const EventFilter & filter, const Event & event)
{
  //Go through filter and add value og matches as in spec
  std::vector<Variant> fields;

  if (Debug) { std::cout << "InternalSubscription | InternalGetEventField " << filter.SelectClauses.size() << std::endl; }

  for (SimpleAttributeOperand sattr : filter.SelectClauses)
    {
      if (Debug) { std::cout << "InternalSubscription | BrowsePath size " << sattr.BrowsePath.size() << std::endl; }

      if (sattr.BrowsePath.size() == 0)
        {
          fields.push_back(event.GetValue(sattr.Attribute));
        }

      else
        {
          if (Debug) { std::cout << "InternalSubscription | sending value for : " << sattr.BrowsePath[0] << std::endl; }

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
              if (Debug) { std::cout << "msg is: " << event.Message << std::endl; }

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


