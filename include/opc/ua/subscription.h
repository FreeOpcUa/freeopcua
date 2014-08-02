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
#include <map>


namespace OpcUa
{
  typedef std::map<uint32_t, AttributeValueID> AttValMap;
  typedef std::map<uint32_t, EventFilter> SimpleAttOpMap;

  class SubscriptionClient
  {
    public:
      //Called for each datachange events
      virtual void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeID attribute) {};
      //Called for every events receive from server
      // order and value of variants depend on event subscription and applied filter
      virtual void Event(uint32_t handle, std::vector<Variant> xx) {}; 
      //Called at server state changed
      virtual void StatusChange(StatusCode newstatus) {}; 
  };


  class Subscription
  {
    public:
      //Create a new subscription on server
      //methods of callback object will be called everytime an event is received from the server
      //FIXME: should we use interface or std::function for callback???? std::function syntax is ugly but is more flexible
      //Alternative could be
      //AddDataChangeCallback(std::function<const Node&, const Variuant& val, AttributeID> callback);
      //AddEventCallback(std::function<std::vector<Variant>> callback);
      Subscription(Remote::Server::SharedPtr server, const SubscriptionParameters& params, SubscriptionClient& callback); 
      //Delete the subscription from server
      void Delete();

      //Get information about the subscription
      uint32_t GetId() const { return Data.ID; } 
      double GetPeriode() const { return Data.RevisedPublishingInterval; } 

      //Subscribe to a Node attribute for its value to change
      // Subscribe to nodes for specified attribute change
      uint32_t SubscribeDataChange(const Node& node, AttributeID attr=AttributeID::VALUE);
      std::vector<uint32_t> SubscribeDataChange(const std::vector<AttributeValueID>& attributes);
      
      //Unsubscribe to datachange or events
      void UnSubscribe(uint32_t handle); 
      void UnSubscribe(std::vector<uint32_t> handles); 

      //Subscribe to Events for given node
      //As far as I remember the only allowed node is Server in most SDKs
      uint32_t SubscribeEvents(const Node& node, const EventFilter& eventfilter); 
      uint32_t SubscribeEvents(const Node& eventype); //subscribe to all variables og given event type at the server node

      //Subscribe to server status change
      // FIXME: Not sure we need to subscribe, maybe it is automatic .... so disabled for now
      //uint32_t SubscribeStatusChange(); 

      void PublishCallback(PublishResult); //Not sure it needs to be public

    private:
      //void Publish();

      Remote::Server::SharedPtr Server;
      SubscriptionData Data;
      SubscriptionClient& Client;
      uint32_t LastMonitoredItemHandle = 1;
      AttValMap AttributeValueMap; 
      SimpleAttOpMap SimpleAttributeOperandMap; //Not used currently
  };
}


