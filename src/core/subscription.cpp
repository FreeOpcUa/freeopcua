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

#include <iostream>



namespace OpcUa
{
  Subscription::Subscription(Remote::Server::SharedPtr server, const SubscriptionParameters& params, SubscriptionClient& callback)
    : Server(server), Client(callback)
  {
    Data = Server->Subscriptions()->CreateSubscription(params, std::bind(&Subscription::PublishCallback, this, std::placeholders::_1));
    //After creating the subscription, it is expected to send at least one publish request
    Publish();
    Publish();
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
    for (NotificationData data: result.Message.Data )
    {
      if (data.Header.TypeID == ExpandedObjectID::DataChangeNotification)
      {
        for ( MonitoredItems item: data.DataChange.Notification)
        {
          AttValMap::iterator mapit = Map.find(item.ClientHandle);
          if ( mapit != Map.end() )
          {
            //FIXME: it might be an idea to push the call to another thread to avoid hanging on user error
            Client.DataChange( Node(Server, mapit->second.Node), item.Value, mapit->second.Attribute);
          }
        }
      }
      else
      {
        std::cout << "Error not implemented notification type (only DataChange currently)" << std::endl;
      }
    }
    Acknowledgments.push_back(result.Message.SequenceID);
    Publish(); //send new publish request to server, to its queue does not get empty
  }

  void Subscription::Publish()
  {
    std::vector<SubscriptionAcknowledgement> acknowledgements;
    for (uint32_t ackid: Acknowledgments)
    {
      SubscriptionAcknowledgement ack;
      ack.SubscriptionID = Data.ID;
      ack.SequenceNumber = ackid;
      acknowledgements.push_back(ack);
    }
    Acknowledgments.clear();
    Server->Subscriptions()->Publish(acknowledgements);
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

    for (auto attr : attributes)
    {
      MonitoredItemRequest req;
      req.ItemToMonitor = attr;
      req.Mode = MonitoringMode::Reporting;
      MonitoringParameters params;
      params.SamplingInterval = Data.RevisedPublishingInterval;
      params.QueueSize = 1;
      params.DiscardOldest = true;
      params.ClientHandle = IntegerID(++LastMonitoredItemHandle);
      Map[params.ClientHandle] = attr;
      req.Parameters = params;
      itemsParams.ItemsToCreate.push_back(req);
    }
    std::vector<CreateMonitoredItemsResult> results =  Server->Subscriptions()->CreateMonitoredItems(itemsParams).Results;
    std::vector<uint32_t> handles;
    for (auto res : results)
    {
      CheckStatusCode(res.Status);
      handles.push_back(res.MonitoredItemID);
    }
    return handles;
  }

  void Subscription::SubscribeEvents()
  {
    MonitoredItemsParameters itemsParams;
    itemsParams.SubscriptionID = Data.ID;

    AttributeValueID avid;
    avid.Node = ObjectID::Server;
    avid.Attribute = AttributeID::EVENT_NOTIFIER;

    MonitoredItemRequest req;
    req.ItemToMonitor = avid;
    req.Mode = MonitoringMode::Reporting;
    MonitoringParameters params;
    params.SamplingInterval = Data.RevisedPublishingInterval;
    params.QueueSize = std::numeric_limits<double>::max();
    params.DiscardOldest = true;
    params.ClientHandle = IntegerID(++LastMonitoredItemHandle);
    Map[params.ClientHandle] = avid;
    req.Parameters = params;
    itemsParams.ItemsToCreate.push_back(req);

    std::vector<CreateMonitoredItemsResult> results =  Server->Subscriptions()->CreateMonitoredItems(itemsParams).Results;
    for (auto res : results)
    {
      CheckStatusCode(res.Status);
    }
  }
  //void UnSubscribeEvents(){}; 

}
