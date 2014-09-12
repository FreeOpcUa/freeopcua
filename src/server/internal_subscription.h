
#pragma once

#include "address_space_internal.h"

#include <opc/ua/event.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/services/attributes.h>

#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <chrono>
#include <iostream>
#include <list>
#include <vector>


namespace OpcUa
{
  namespace Internal
  {
    //Structure to store description of a MonitoredItems
    struct DataMonitoredItems
    {
      IntegerID SubscriptionID; //see if we can remove that one
      MonitoringMode Mode;
      time_t LastTrigger;
      CreateMonitoredItemsResult Parameters;
      IntegerID ClientHandle;
    };

    class AddressSpaceInMemory; //pre-declaration

    
    class InternalSubscription
    {
      public:
        InternalSubscription(const SubscriptionData& data, const NodeID& SessionAuthenticationToken, AddressSpaceInMemory& AddressSpace, std::function<void (PublishResult)> Callback);
        ~InternalSubscription();

        void NewAcknowlegment(const SubscriptionAcknowledgement& ack);
        std::vector<StatusCode> DeleteMonitoredItemsIds(const std::vector<IntegerID>& ids);
        bool EnqueueEvent(IntegerID monitoreditemid, const Event& event);
        bool EnqueueDataChange(IntegerID monitoreditemid, const DataValue& value);
        CreateMonitoredItemsResult AddMonitoredItem(const MonitoredItemRequest& request);
        bool HasExpired();
        void Stop();

      private:
        std::vector<PublishResult> PopPublishResult(); 
        bool HasPublishResult(); 

        mutable boost::shared_mutex DbMutex;
        SubscriptionData Data;
        AddressSpaceInMemory& AddressSpace;
        NodeID CurrentSession;
        std::function<void (PublishResult)> Callback;
        uint32_t NotificationSequence = 1; //NotificationSequence start at 1! not 0
        uint32_t KeepAliveCount = 0; 
        bool Startup = true; //To force specific behaviour at startup
        uint32_t LastMonitoredItemID = 2;
        std::map<IntegerID, DataMonitoredItems> MonitoredItemsMap; //Map MonitoredItemID, DataMonitoredItems
        std::list<PublishResult> NotAcknowledgedResults; //result that have not be acknowledeged and may have to be resent
        std::list<MonitoredItems> MonitoredItemsTriggered; 
        std::list<EventFieldList> EventTriggered; 
        boost::asio::io_service& io;
        boost::asio::deadline_timer timer;
        uint32_t LifeTimeCount;


        NotificationData GetNotificationData();
        void PublishResults(const boost::system::error_code& error);
        std::vector<Variant> GetEventFields(const EventFilter& filter, const Event& event);

        bool Debug = true;
         
    };


  }
}

 
