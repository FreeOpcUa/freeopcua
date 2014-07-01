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

#pragma once

#include <opc/ua/subscriptions.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/node.h>

#include <boost/asio.hpp>
#include <sstream>
#include <queue>
#include <map>


namespace OpcUa
{
  typedef std::map<IntegerID, AttributeValueID> AttValMap;

  class SubscriptionClient
  {
    public:
      virtual void DataChangeEvent(const Node& node, const Variant& val, AttributeID attribute);
      //virtual void Event(Event result); FIXME: Not implemented
      //virtual void StatusChange(Event result); FIXME: Not implemented
  };


  class Subscription
  {
    public:
      Subscription(Remote::SubscriptionServices::SharedPtr service, const SubscriptionParameters& params, SubscriptionClient& callback);
      //OpcUa::IntegerID GetId() { return Data.ID; }
      SubscriptionData GetData() { return Data; }

      //Monitor node for DataChange events
      void Subscribe(Node node, AttributeID attr=AttributeID::VALUE);
      void Subscribe(std::vector<AttributeValueID> attributes);
      void UnSubscribe(Node node, AttributeID attr=AttributeID::VALUE);
      void UnSubscribe(std::vector<AttributeValueID> attributes);
      //Monitor for events FIXME: Event support not implemented
      //void SubscribeEvents(Node node); //As far as I remember the only allowed node is Server in most SDKs
      //void Unsubscribe(Node node);
      void PublishCallback(PublishResult); //Not sure it needs to be public

    private:
      void Publish();

      Remote::SubscriptionServices::SharedPtr Service;
      SubscriptionData Data;
      std::queue<uint32_t> Acknowledgments;
      SubscriptionClient& Client;
      uint32_t LastMonitoredItemHandle = 1;
      AttValMap Map; //I do not understand why I need this but event only send handles..
  };
}


