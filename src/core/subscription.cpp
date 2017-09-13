/******************************************************************************
 *   Copyright (C) 2014-2014 Olivier Roulet-Dubonnet          *
 *   olivier.roulet@gmail.com          *
 *                      *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as      *
 *   published by the Free Software Foundation; version 3 of the License.   *
 *                      *
 *   This library is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 *   GNU Lesser General Public License for more details.        *
 *                      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the          *
 *   Free Software Foundation, Inc.,              *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.        *
 ******************************************************************************/


#include <opc/ua/subscription.h>
#include <opc/ua/protocol/string_utils.h>

#include <boost/asio.hpp>
#include <iostream>

namespace OpcUa
{
Subscription::Subscription(Services::SharedPtr server, const CreateSubscriptionParameters & params, SubscriptionHandler & callback, const Common::Logger::SharedPtr & logger)
  : Server(server), Client(callback), Logger(logger)
{
  CreateSubscriptionRequest request;
  request.Parameters = params;
  Services::SharedPtr serverptr = Server;
  Data = Server->Subscriptions()->CreateSubscription(request, [this, serverptr](PublishResult i) { this->PublishCallback(serverptr, i); });
  //After creating the subscription, it is expected to send at least one publish request
  Server->Subscriptions()->Publish(PublishRequest());
  Server->Subscriptions()->Publish(PublishRequest());
}

void Subscription::Delete()
{
  std::vector<StatusCode> results = Server->Subscriptions()->DeleteSubscriptions(std::vector<uint32_t> {Data.SubscriptionId});

  for (auto res : results)
    {
      CheckStatusCode(res);
    }
}

void Subscription::PublishCallback(Services::SharedPtr server, const PublishResult result)
{

  LOG_DEBUG(Logger, "subscription          | Suscription::PublishCallback called with {} notifications", result.NotificationMessage.NotificationData.size());

  for (const NotificationData & data : result.NotificationMessage.NotificationData)
    {
      if (data.Header.TypeId == ExpandedObjectId::DataChangeNotification)
        {
          LOG_DEBUG(Logger, "subscription          | notification is of type DataChange");

          CallDataChangeCallback(data);
        }

      else if (data.Header.TypeId == ExpandedObjectId::EventNotificationList)
        {
          LOG_DEBUG(Logger, "subscription          | notification is of type Event");

          CallEventCallback(data);
        }

      else if (data.Header.TypeId == ExpandedObjectId::StatusChangeNotification)
        {
          LOG_DEBUG(Logger, "subscription          | notification is of type StatusChange");

          CallStatusChangeCallback(data);
        }

      else
        {
          LOG_WARN(Logger, "subscription          | unknown notficiation type received: {}", data.Header.TypeId);
        }
    }

  OpcUa::SubscriptionAcknowledgement ack;
  ack.SubscriptionId = GetId();
  ack.SequenceNumber = result.NotificationMessage.SequenceNumber;
  PublishRequest request;
  request.SubscriptionAcknowledgements.push_back(ack);
  server->Subscriptions()->Publish(request);
}

void Subscription::CallDataChangeCallback(const NotificationData & data)
{
  for (const MonitoredItems & item : data.DataChange.Notification)
    {
      std::unique_lock<std::mutex> lock(Mutex); //could used boost::shared_lock to improve perf

      AttValMap::iterator mapit = AttributeValueMap.find(item.ClientHandle);

      if (mapit == AttributeValueMap.end())
        {
          LOG_WARN(Logger, "subscription          | got PublishResult for an unknown monitoreditem id: {}", item.ClientHandle);
        }

      else
        {
          AttributeId attr = mapit->second.Attribute;
          Node node = mapit->second.TargetNode;
          lock.unlock(); //unlock before calling client cades, you never know what they may do

          LOG_DEBUG(Logger, "subscription          | calling DataChange user callback: {} and node: {}", item.ClientHandle, mapit->second.TargetNode);

          Client.DataValueChange(mapit->second.MonitoredItemId, node, item.Value, attr);
          Client.DataChange(mapit->second.MonitoredItemId, node, item.Value.Value, attr);
        }
    }
}

void Subscription::CallStatusChangeCallback(const NotificationData & data)
{
  Client.StatusChange(data.StatusChange.Status);
}

void Subscription::CallEventCallback(const NotificationData & data)
{
  for (EventFieldList ef : data.Events.Events)
    {
      std::unique_lock<std::mutex> lock(Mutex); //could used boost::shared_lock to improve perf

      AttValMap::iterator mapit = AttributeValueMap.find(ef.ClientHandle);

      if (mapit == AttributeValueMap.end())
        {
          LOG_WARN(Logger, "subscription          | got PublishResult for an unknown MonitoredItem id: {}", ef.ClientHandle);
        }

      else
        {
          Event ev;
          uint32_t count = 0;

          if (mapit->second.Filter.Event.SelectClauses.size() != ef.EventFields.size())
            {
              throw std::runtime_error("subscription          | receive event format does not match requested filter");
            }

          for (SimpleAttributeOperand op : mapit->second.Filter.Event.SelectClauses)
            {
              auto & value = ef.EventFields[count];
              // add all fields as value
              ev.SetValue(op.BrowsePath, value);
              ++count;
              // server may send NULL fields - do not try to convert them to actual values
              if (value.IsNul())
                {
                  continue;
                }

              // set the default fields of events into their event attributes
              if (op.BrowsePath.size() == 1)
                {
                  auto & name = op.BrowsePath[0];

                  if (name == QualifiedName("EventId", 0))
                    {
                      ev.EventId = value.As<ByteString>();
                    }

                  else if (name == QualifiedName("EventType", 0))
                    {
                      ev.EventType = value.As<NodeId>();
                    }

                  else if (name == QualifiedName("SourceNode", 0))
                    {
                      ev.SourceNode = value.As<NodeId>();
                    }

                  else if (name == QualifiedName("SourceName", 0))
                    {
                      ev.SourceName = value.As<std::string>();
                    }

                  else if (name == QualifiedName("Message", 0))
                    {
                      ev.Message = value.As<LocalizedText>();
                    }

                  else if (name == QualifiedName("Severity", 0))
                    {
                      ev.Severity = value.As<uint16_t>();
                    }

                  else if (name == QualifiedName("LocalTime", 0))
                    {
                      ev.LocalTime = value.As<DateTime>();
                    }

                  else if (name == QualifiedName("ReceiveTime", 0))
                    {
                      ev.ReceiveTime = value.As<DateTime>();
                    }

                  else if (name == QualifiedName("Time", 0))
                    {
                      ev.Time = value.As<DateTime>();
                    }
                }
            }

          lock.unlock();

          LOG_DEBUG(Logger, "subscription          | calling client event callback");

          Client.Event(mapit->second.MonitoredItemId, ev);

          LOG_DEBUG(Logger, "subscription          | callback call finished");
        }
    }
}

RepublishResponse Subscription::Republish(uint32_t sequenceNumber)
{
  RepublishParameters params;
  params.SubscriptionId = Data.SubscriptionId;
  params.RetransmitSequenceNumber = sequenceNumber;
  RepublishResponse response = Server->Subscriptions()->Republish(params);
  return response;
}

uint32_t Subscription::SubscribeDataChange(const Node & node, AttributeId attr)
{
  ReadValueId avid;
  avid.NodeId = node.GetId();
  avid.AttributeId = attr;
  //avid.IndexRange //We leave it null, then the entire array is returned
  std::vector<uint32_t> results = SubscribeDataChange(std::vector<ReadValueId>({avid}));

  if (results.size() != 1) { throw std::runtime_error("subscription          | SubscribeDataChange should have returned exactly one result"); }

  return results.front();
}

std::vector<MonitoredItemCreateResult> Subscription::Subscribe(std::vector<MonitoredItemCreateRequest> request)
{
  std::unique_lock<std::mutex> lock(Mutex);

  MonitoredItemsParameters itemsParams;
  itemsParams.SubscriptionId = Data.SubscriptionId;
  itemsParams.TimestampsToReturn = TimestampsToReturn(2); // Don't know for better

  for (auto req : request)
    {
      itemsParams.ItemsToCreate.push_back(req);
    }

  return  Server->Subscriptions()->CreateMonitoredItems(itemsParams);
}

std::vector<uint32_t> Subscription::SubscribeDataChange(const std::vector<ReadValueId> & attributes)
{
  std::unique_lock<std::mutex> lock(Mutex);

  MonitoredItemsParameters itemsParams;
  itemsParams.SubscriptionId = Data.SubscriptionId;
  itemsParams.TimestampsToReturn = TimestampsToReturn(2); // Don't know for better

  for (ReadValueId attr : attributes)
    {
      MonitoredItemCreateRequest req;
      req.ItemToMonitor = attr;
      req.MonitoringMode = MonitoringMode::Reporting;
      MonitoringParameters params;
      params.SamplingInterval = Data.RevisedPublishingInterval;
      params.QueueSize = 1;
      params.DiscardOldest = true;
      params.ClientHandle = (uint32_t)++LastMonitoredItemHandle;
      req.RequestedParameters = params;
      itemsParams.ItemsToCreate.push_back(req);
    }

  std::vector<MonitoredItemCreateResult> results =  Server->Subscriptions()->CreateMonitoredItems(itemsParams);

  if (results.size() != attributes.size())
    {
      throw (std::runtime_error("subscription          | server did not send answer for all MonitoredItem requests"));
    }

  std::vector<uint32_t> monitoredItemsIds;
  unsigned int i = 0;

  for (const auto & res : results)
    {
      CheckStatusCode(res.Status);

      LOG_DEBUG(Logger, "subscription          | storing monitoreditem with handle: {} and id: {}", itemsParams.ItemsToCreate[i].RequestedParameters.ClientHandle, res.MonitoredItemId);

      MonitoredItemData mdata;
      mdata.MonitoredItemId = res.MonitoredItemId;
      mdata.Attribute =  attributes[i].AttributeId;
      mdata.TargetNode =  Node(Server, attributes[i].NodeId);
      AttributeValueMap[itemsParams.ItemsToCreate[i].RequestedParameters.ClientHandle] = mdata;
      monitoredItemsIds.push_back(res.MonitoredItemId);
      ++i;
    }

  return monitoredItemsIds;
}

void Subscription::setUsrPtr(uint32_t handle, UserData * usr)
{
  AttributeValueMap[handle].usrVar = usr;
}

UserData * Subscription::getUsrPtr(uint32_t handle)
{
  return AttributeValueMap[handle].usrVar;
}

void Subscription::UnSubscribe(uint32_t handle)
{
  return UnSubscribe(std::vector<uint32_t>(1, handle));
}

void Subscription::UnSubscribe(std::vector<uint32_t> handles)
{
  std::unique_lock<std::mutex> lock(Mutex);

  DeleteMonitoredItemsParameters params;
  params.SubscriptionId = Data.SubscriptionId;
  std::vector<uint32_t> mids;

  for (auto id : handles)
    {
      LOG_DEBUG(Logger, "subscription          | sending unsubscribe for MonitoredItem id: {}", id);
      mids.push_back(uint32_t(id));

      //Now trying to remove monitoreditem from our internal cache
      for (auto pair : AttributeValueMap)
        {
          if (pair.second.MonitoredItemId == id)
            {
              AttributeValueMap.erase(pair.first);
              break; //we modified our iterating object, so quit!!
            }
        }
    }

  params.MonitoredItemIds = mids;
  auto results = Server->Subscriptions()-> DeleteMonitoredItems(params);

  for (auto res : results)
    {
      CheckStatusCode(res);
    }
}

uint32_t Subscription::SubscribeEvents()
{
  return SubscribeEvents(Node(Server, ObjectId::Server), Node(Server, ObjectId::BaseEventType));
}

uint32_t Subscription::SubscribeEvents(const Node & node, const Node & eventtype)
{
  EventFilter filter;

  LOG_DEBUG(Logger, "subscription          | subscribing events with filter for:");

  for (Node & child : eventtype.GetProperties())
    {
      auto propertyName = child.GetBrowseName();
      LOG_DEBUG(Logger, "  property: {}", propertyName);

      SimpleAttributeOperand op;
      op.TypeId = eventtype.GetId();
      op.Attribute = AttributeId::Value;
      op.BrowsePath = std::vector<QualifiedName>({propertyName});
      filter.SelectClauses.push_back(op);
    }

  return SubscribeEvents(node, filter);
}

uint32_t Subscription::SubscribeEvents(const Node & node, const EventFilter & eventfilter)
{
  std::unique_lock<std::mutex> lock(Mutex);

  MonitoredItemsParameters itemsParams;
  itemsParams.SubscriptionId = Data.SubscriptionId;
  itemsParams.TimestampsToReturn = TimestampsToReturn(2); // Don't know for better

  ReadValueId avid;
  avid.NodeId = node.GetId();
  avid.AttributeId = AttributeId::EventNotifier;

  MonitoredItemCreateRequest req;
  req.ItemToMonitor = avid;
  req.MonitoringMode = MonitoringMode::Reporting;
  MonitoringParameters params;
  params.SamplingInterval = Data.RevisedPublishingInterval;
  params.QueueSize = std::numeric_limits<uint32_t>::max();
  params.DiscardOldest = true;
  params.ClientHandle = (uint32_t)++LastMonitoredItemHandle;

  MonitoringFilter filter(eventfilter);
  params.Filter = filter;
  req.RequestedParameters = params;
  itemsParams.ItemsToCreate.push_back(req);

  std::vector<MonitoredItemCreateResult> results =  Server->Subscriptions()->CreateMonitoredItems(itemsParams);

  if (results.size() != 1)
    {
      throw (std::runtime_error("subscription          | CreateMonitoredItems should return one result"));
    }

  auto result = results[0];
  // allow remote side to sloppily skip SelectClauses and WhereClause in its answer
  if (result.FilterResult.Event.SelectClauses.empty())
    {
      result.FilterResult.Event.SelectClauses = eventfilter.SelectClauses;
    }
  if (result.FilterResult.Event.WhereClause.empty())
    {
      result.FilterResult.Event.WhereClause = eventfilter.WhereClause;
    }
  MonitoredItemData mdata;
  mdata.TargetNode = Node(Server, avid.NodeId);
  mdata.Attribute = avid.AttributeId;
  mdata.MonitoredItemId = result.MonitoredItemId;
  mdata.Filter = result.FilterResult;
  AttributeValueMap[params.ClientHandle] = mdata;

  CheckStatusCode(result.Status);
  SimpleAttributeOperandMap[result.MonitoredItemId] = eventfilter; //Not used
  return result.MonitoredItemId;
}

}
