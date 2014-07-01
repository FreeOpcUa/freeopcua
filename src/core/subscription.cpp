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
  Subscription::Subscription(Remote::SubscriptionServices::SharedPtr service, const SubscriptionParameters& params, SubscriptionClient& callback): Service(service), Client(callback)
  {
    Data = Service->CreateSubscription(params, std::bind(&Subscription::PublishCallback, this, std::placeholders::_1));
    //After creating the subscription, it is expected to send a few publishRequests
    Publish();
    Publish();
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
            Client.DataChangeEvent(Node(mapit->second.Node, Server), item.Value, mapit->second.Attribute);
          }
        }
      }
      else
      {
        std::cout << "Error not implemented" << std::endl;
      }
    }
    Acknowledgments.push(result.Message.SequenceID);
    Publish();
  }

  void Subscription::Publish()
  {
    PublishParameters params;
    if (Acknowledgments.size() > 0 )
    {
      SubscriptionAcknowledgement ack;
      ack.SubscriptionID = Data.ID;
      ack.SequenceNumber = Acknowledgments.front();
      params.Acknowledgements.push_back(ack);
      Acknowledgments.pop();
    }
    Service->Publish(params);
  }

  void Subscription::Subscribe(Node node, AttributeID attr)
  {
    AttributeValueID avid;
    avid.Node = node.GetId();
    avid.Attribute = attr;
    //avid.IndexRange //We leave it null, then the entire array is returned
    return Subscribe(std::vector<AttributeValueID>({avid}));
  }

  void Subscription::Subscribe(std::vector<AttributeValueID> attributes)
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
    Service->CreateMonitoredItems(itemsParams);
  }

}
