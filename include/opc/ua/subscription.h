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

#include <opc/ua/node.h>
#include <opc/ua/event.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/services/subscriptions.h>

#include <sstream>
#include <map>
#include <mutex>

#include <iostream> //debug

namespace OpcUa
{
  struct MonitoredItemData
  {
    IntegerID MonitoredItemID;
    Node TargetNode;
    AttributeID Attribute;
    MonitoringFilter Filter;
  };

  typedef std::map<IntegerID, MonitoredItemData> AttValMap;
  typedef std::map<IntegerID, EventFilter> SimpleAttOpMap;

  class SubscriptionClient
  {
    public:
      //Called for each datachange events
      virtual void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeID attribute) const {std::cout << "default dc" << std::endl;};
      //Called for every events receive from server
      virtual void Event(uint32_t handle, const Event& event) const {std::cout << "default c++ event callback has been called" << std::endl;}; 
      //Called at server state changed
      virtual void StatusChange(StatusCode status) const  {}; 
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
      Subscription(Services::SharedPtr server, const SubscriptionParameters& params, SubscriptionClient& callback, bool debug=false); 
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
      uint32_t SubscribeEvents(const Node& node, const Node& eventType); //subscribe to all variables og given event type 
      uint32_t SubscribeEvents(); //subscribe to variables of baseEventTypes and ServerNode 

      //Subscribe to server status change
      // FIXME: Not sure we need to subscribe, maybe it is automatic .... so disabled for now
      //uint32_t SubscribeStatusChange(); 

      void PublishCallback(PublishResult); //Not sure it needs to be public

    private:
      //void Publish();

      Services::SharedPtr Server;
      SubscriptionData Data;
      SubscriptionClient& Client;
      uint32_t LastMonitoredItemHandle = 1;
      AttValMap AttributeValueMap; 
      SimpleAttOpMap SimpleAttributeOperandMap; //Not used currently
      std::mutex Mutex;
      bool Debug;
  };
}


