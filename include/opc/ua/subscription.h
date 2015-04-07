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

#include <opc/ua/global.h>
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
    IntegerId MonitoredItemId;
    Node TargetNode;
    AttributeId Attribute;
    MonitoringFilter Filter;
  };

  typedef std::map<IntegerId, MonitoredItemData> AttValMap;
  typedef std::map<IntegerId, EventFilter> SimpleAttOpMap;

  class SubscriptionHandler
  {
    public:
    virtual ~SubscriptionHandler() {}
    //Called for each datachange events
    virtual void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeId attribute) const
    {
      OPCUA_UNUSED(handle);
      OPCUA_UNUSED(node);
      OPCUA_UNUSED(val);
      OPCUA_UNUSED(attribute);
      std::cout << "default dc" << std::endl;
    }
    //Called for every events receive from server
    virtual void Event(uint32_t handle, const Event& event) const
    {
      OPCUA_UNUSED(handle);
      OPCUA_UNUSED(event);
      std::cout << "default c++ event callback has been called" << std::endl;
    }
    //Called at server state changed
    virtual void StatusChange(StatusCode status) const
    {
      OPCUA_UNUSED(status);
    }
  };


  class Subscription
  {
    public:
      //Create a new subscription on server
      //methods of callback object will be called everytime an event is received from the server
      //FIXME: should we use interface or std::function for callback???? std::function syntax is ugly but is more flexible
      //Alternative could be
      //AddDataChangeCallback(std::function<const Node&, const Variuant& val, AttributeId> callback);
      //AddEventCallback(std::function<std::vector<Variant>> callback);
      Subscription(Services::SharedPtr server, const SubscriptionParameters& params, SubscriptionHandler& callback, bool debug=false); 
      virtual ~Subscription() {}

      //Delete the subscription from server
      void Delete();

      //Get information about the subscription
      uint32_t GetId() const { return Data.Id; } 
      double GetPeriode() const { return Data.RevisedPublishingInterval; } 

      //Subscribe to a Node attribute for its value to change
      // Subscribe to nodes for specified attribute change
      uint32_t SubscribeDataChange(const Node& node, AttributeId attr=AttributeId::Value);
      std::vector<uint32_t> SubscribeDataChange(const std::vector<ReadValueId>& attributes);
      
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
      
      // Override this method if you want raw publish results from server
      // for example if you want to make sure you do not miss any packets, etc, ...
      virtual void PublishCallback( Services::SharedPtr serverLocalPtr, const PublishResult result); 

      //Request republish of a notification from server
      //SequenceNumber are send by server in PublishResult struct
      RepublishResponse Republish(uint32_t sequenceNumber);



    private:
      void CallDataChangeCallback(const NotificationData& data);
      void CallEventCallback(const NotificationData& data);
      void CallStatusChangeCallback(const NotificationData& data);

      Services::SharedPtr Server;
      SubscriptionData Data;
      SubscriptionHandler& Client;
      uint32_t LastMonitoredItemHandle = 1;
      AttValMap AttributeValueMap; 
      SimpleAttOpMap SimpleAttributeOperandMap; //Not used currently
      std::mutex Mutex;
      bool Debug;
  };
}


