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
  typedef std::map<IntegerID, AttributeValueID> AttValMap;

  class SubscriptionClient
  {
    public:
      virtual void DataChange(const Node& node, const Variant& val, AttributeID attribute) {};
      virtual void Event(std::vector<Variant> xx) {}; 
      virtual void StatusChange(StatusCode newstatus) {}; 
  };


  class Subscription
  {
    public:
      //Create a new subscription on server
      //callback will be called everytime an event is received from the server
      //FIXME: should we use interface or std::function for callback???? std::function syntax is ugly but is more flexible
      Subscription(Remote::Server::SharedPtr server, const SubscriptionParameters& params, SubscriptionClient& callback); 
      //Delete the subscription from server
      void Delete();

      //Get information about the subscription
      IntegerID GetId() const { return Data.ID; } 
      Duration GetPeriode() const { return Data.RevisedPublishingInterval; } 

      //Subscribe to a Node attribute for its value to change
      // Subscribe to nodes for specified attribute change
      uint32_t SubscribeDataChange(const Node& node, AttributeID attr=AttributeID::VALUE);
      std::vector<uint32_t> SubscribeDataChange(const std::vector<AttributeValueID>& attributes);
      void UnSubscribeDataChange(uint32_t handle); 
      void UnSubscribeDataChange(std::vector<uint32_t> handles); 

      //Subscribe to events 
      //FIXME: need to support filtering!!
      void SubscribeEvents(); //As far as I remember the only allowed node is Server in most SDKs
      //void SubscribeEvents(Node node); //As far as I remember the only allowed node is Server in most SDKs
      void UnSubscribeEvents();

      //Subscribe to server status change
      // FIXME: Not sure we need to subscribe, maybe it is automatic .... so disabled for now
      //void SubscribeStatusChange(); 
      //void UnSubscribeStatusChange(){}; 

      void PublishCallback(PublishResult); //Not sure it needs to be public

    private:
      void Publish();

      Remote::Server::SharedPtr Server;
      SubscriptionData Data;
      std::vector<uint32_t> Acknowledgments;
      SubscriptionClient& Client;
      uint32_t LastMonitoredItemHandle = 1;
      AttValMap Map; //I do not understand why I need this but event only send handles..
      uint32_t EventHandle = 0;
  };
}


