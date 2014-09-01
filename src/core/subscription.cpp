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


#include <opc/ua/event.h>
#include <opc/ua/subscription.h>
#include <opc/ua/protocol/string_utils.h>

#include <boost/asio.hpp>
#include <iostream>

namespace OpcUa
{
  Subscription::Subscription(Remote::Server::SharedPtr server, const SubscriptionParameters& params, SubscriptionClient& callback, bool debug)
    : Server(server), Client(callback), Debug(debug)
  {
    Data = Server->Subscriptions()->CreateSubscription(params, [&](PublishResult i){ this->PublishCallback(i); } );
    //After creating the subscription, it is expected to send at least one publish request
    Server->Subscriptions()->Publish(std::vector<SubscriptionAcknowledgement>());
    Server->Subscriptions()->Publish(std::vector<SubscriptionAcknowledgement>());
  }

  void Subscription::Delete()
  {
    std::vector<StatusCode> results = Server->Subscriptions()->DeleteSubscriptions(std::vector<IntegerID>({Data.ID}));
    for (auto res: results)
    {
      CheckStatusCode(res);
    }
  }

  void Subscription::PublishCallback(PublishResult result)
  {
    std::unique_lock<std::mutex> lock(Mutex); //To be finished
    //FIXME: finish to handle all types of publishresults!

    if (Debug){ std::cout << "Suscription::PublishCallback called" << std::endl; }
    for (const NotificationData& data: result.Message.Data )
    {
      if (Debug) { std::cout << "Notification is of type DataChange\n"; }
      if (data.Header.TypeID == ExpandedObjectID::DataChangeNotification)
      {
        for ( const MonitoredItems& item: data.DataChange.Notification)
        {
          AttValMap::iterator mapit = AttributeValueMap.find(item.ClientHandle);
          if ( mapit == AttributeValueMap.end() )
          {
            std::cout << "Server Error got publishresult for an unknown  monitoreditem id : "<< item.ClientHandle << std::endl; 
          }
          else
          {
            if (Debug) { std::cout << "Debug: Calling DataChange user callback " << item.ClientHandle << " and node: " << Node(Server, mapit->second.Node) << std::endl; }
            Client.DataChange( item.ClientHandle, Node(Server, mapit->second.Node, QualifiedName()), item.Value.Value, mapit->second.Attribute);
          }
        }
      }
      else if (data.Header.TypeID == ExpandedObjectID::EventNotificationList)
      {
        if (Debug) { std::cout << "Notification is of type Event\n"; }
        for ( EventFieldList ef :  data.Events.Events)
        {


          AttValMap::iterator mapit = AttributeValueMap.find(ef.ClientHandle);
          if ( mapit == AttributeValueMap.end() )
          {
            std::cout << "Server Error got publishresult for an unknown  monitoreditem id : "<< ef.ClientHandle << std::endl; 
          }
          else
          {
            //FIXME: it might be an idea to push the call to another thread to avoid hanging on user error
            //mapit->second.
            //FIXME: think about event format!! should we havae paires? or better create an event object
            Event ev;
            //ev.
            if (Debug) { std::cout << "Debug: Calling client callback\n"; }
            Client.Event(ef.ClientHandle, ef.EventFields);
            if (Debug) { std::cout << "Debug: callback call finished\n"; }
          }
        }
      }
      else if (data.Header.TypeID == ExpandedObjectID::StatusChangeNotification)
      {
        if (Debug) { std::cout << "Notification is of type StatusChange\n"; }
        Client.StatusChange(data.StatusChange.Status);
      }
      else
      {
        std::cout << "Error unknown notficiation type received: " << data.Header.TypeID <<std::endl;
      }
    }
    Server->Subscriptions()->Publish(std::vector<SubscriptionAcknowledgement>({result.Message.SequenceID}));
  }

  uint32_t Subscription::SubscribeDataChange(const Node& node, AttributeID attr)
  {
    AttributeValueID avid;
    avid.Node = node.GetId();
    avid.Attribute = attr;
    //avid.IndexRange //We leave it null, then the entire array is returned
    std::vector<uint32_t> results = SubscribeDataChange(std::vector<AttributeValueID>({avid}));
    if (results.size() == 0) { throw std::runtime_error("Protocol Error"); }
    return results.front();
  }

  std::vector<uint32_t> Subscription::SubscribeDataChange(const std::vector<AttributeValueID>& attributes)
  {
    MonitoredItemsParameters itemsParams;
    itemsParams.SubscriptionID = Data.ID;

    for (AttributeValueID attr : attributes)
    {
      MonitoredItemRequest req;
      req.ItemToMonitor = attr;
      req.Mode = MonitoringMode::Reporting;
      MonitoringParameters params;
      params.SamplingInterval = Data.RevisedPublishingInterval;
      params.QueueSize = 1;
      params.DiscardOldest = true;
      params.ClientHandle = IntegerID(++LastMonitoredItemHandle);
      req.Parameters = params;
      itemsParams.ItemsToCreate.push_back(req);
    }

    std::vector<CreateMonitoredItemsResult> results =  Server->Subscriptions()->CreateMonitoredItems(itemsParams).Results;

    if ( results.size() != attributes.size() ) 
    {
      throw(std::runtime_error("Error server did not send answer for all monitoreditem requessts"));
    }

    std::vector<uint32_t> handles;
    uint i = 0;
    for (const auto& res : results)
    {
      CheckStatusCode(res.Status);
      if (Debug ) { std::cout << "storing monitoreditem with handle " << itemsParams.ItemsToCreate[i].Parameters.ClientHandle << " and id " << res.MonitoredItemID << std::endl;  }
      MonitoredItemData mdata;
      mdata.MonitoredItemID = res.MonitoredItemID;
      mdata.Attribute =  attributes[i].Attribute;
      mdata.Node =  attributes[i].Node;
      AttributeValueMap[itemsParams.ItemsToCreate[i].Parameters.ClientHandle] = mdata;
      handles.push_back(itemsParams.ItemsToCreate[i].Parameters.ClientHandle);
      ++i;
    }
    return handles;
  }

  void Subscription::UnSubscribe(uint32_t handle)
  {
    return UnSubscribe(std::vector<uint32_t>({handle}));
  }

  void Subscription::UnSubscribe(std::vector<uint32_t> handles) 
  {
    DeleteMonitoredItemsParameters params;
    params.SubscriptionId = Data.ID;
    std::vector<IntegerID> mids;
    for (auto id : handles)
    {
      AttValMap::iterator mapit = AttributeValueMap.find(IntegerID(id));
      if ( mapit != AttributeValueMap.end() )
      {
        mids.push_back(mapit->second.MonitoredItemID);
      }
      else
      {
        //To no not confuse client we send to the server the last id it may use... but will this id one day exist?
        mids.push_back(IntegerID(std::numeric_limits<uint32_t>::max()));
      }

    }
    params.MonitoredItemsIds = mids;
    auto results = Server->Subscriptions()-> DeleteMonitoredItems(params);
    for (auto res : results)
    {
      CheckStatusCode(res);
    }
  }

  uint32_t Subscription::SubscribeEvents(const Node& eventtype)
  {
    EventFilter filter;
    //We only subscribe to variabes, since properties are supposed not to change
    //FIXME: order of variables might not be constant on all servers, we should order variables
    for ( const Node& child: eventtype.GetVariables() )
    {
      SimpleAttributeOperand op;
      op.TypeID = eventtype.GetId();
      op.Attribute = AttributeID::VALUE;
      op.BrowsePath = std::vector<QualifiedName>({child.GetName()});
      filter.SelectClauses.push_back(op);
    }
    return SubscribeEvents(Node(Server, ObjectID::Server), filter);
  }

  uint32_t Subscription::SubscribeEvents(const Node& node, const EventFilter& eventfilter)
  {
    MonitoredItemsParameters itemsParams;
    itemsParams.SubscriptionID = Data.ID;

    AttributeValueID avid;
    avid.Node = node.GetId();
    avid.Attribute = AttributeID::EVENT_NOTIFIER;

    MonitoredItemRequest req;
    req.ItemToMonitor = avid;
    req.Mode = MonitoringMode::Reporting;
    MonitoringParameters params;
    params.SamplingInterval = Data.RevisedPublishingInterval;
    params.QueueSize = std::numeric_limits<double>::max();
    params.DiscardOldest = true;
    params.ClientHandle = IntegerID(++LastMonitoredItemHandle);

    MonitoringFilter filter;
    filter.Event = eventfilter;
    params.Filter = filter;
    req.Parameters = params;
    itemsParams.ItemsToCreate.push_back(req);

    std::vector<CreateMonitoredItemsResult> results =  Server->Subscriptions()->CreateMonitoredItems(itemsParams).Results;
    if ( results.size()  != 1 )
    {
      throw(std::runtime_error("Protocol Error CreateMonitoredItems should return one result"));
    }

    MonitoredItemData mdata;
    mdata.Node = avid.Node;
    mdata.Attribute = avid.Attribute;
    mdata.MonitoredItemID = results[0].MonitoredItemID;
    AttributeValueMap[params.ClientHandle] = mdata;


    CreateMonitoredItemsResult res = results[0];
    CheckStatusCode(res.Status);
    SimpleAttributeOperandMap[res.MonitoredItemID] = eventfilter; //Not used
    return res.MonitoredItemID;
  }

}
