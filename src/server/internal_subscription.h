
#pragma once

//#include "address_space_internal.h"
#include "subscription_service_internal.h"

#include <opc/ua/event.h>
#include <opc/ua/server/address_space.h>
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

    class SubscriptionServiceInternal;

    //Structure to store description of a MonitoredItems
    struct MonitoredDataChange
    {
      IntegerID MonitoredItemId;
      MonitoringMode Mode;
      time_t LastTrigger;
      CreateMonitoredItemsResult Parameters;
      IntegerID ClientHandle;
      uint32_t CallbackHandle;
    };

    struct TriggeredDataChange
    {
      IntegerID MonitoredItemId;
      MonitoredItems Data;
    };

    struct TriggeredEvent
    {
      IntegerID MonitoredItemId;
      EventFieldList Data;
    };

    //typedef std::pair<NodeID, AttributeID> MonitoredItemsIndex;
    typedef std::map<IntegerID, MonitoredDataChange> MonitoredDataChangeMap;
    typedef std::map<NodeID, IntegerID> MonitoredEventsMap;

    class AddressSpaceInMemory; //pre-declaration

    
    class InternalSubscription : public std::enable_shared_from_this<InternalSubscription>
    {
      public:
        InternalSubscription(SubscriptionServiceInternal& service, const SubscriptionData& data, const NodeID& SessionAuthenticationToken, std::function<void (PublishResult)> Callback, bool debug=false);
        ~InternalSubscription();
        void Start();
        void Stop();

        void NewAcknowlegment(const SubscriptionAcknowledgement& ack);
        std::vector<StatusCode> DeleteMonitoredItemsIds(const std::vector<IntegerID>& ids);
        bool EnqueueEvent(IntegerID monitoreditemid, const Event& event);
        bool EnqueueDataChange(IntegerID monitoreditemid, const DataValue& value);
        CreateMonitoredItemsResult CreateMonitoredItem(const MonitoredItemRequest& request);
        void DataChangeCallback(const IntegerID&, const DataValue& value);
        bool HasExpired();
        void TriggerEvent(NodeID node, Event event);
        RepublishResponse Republish(const RepublishParameters& params);

      private:
        void DeleteAllMonitoredItems(); 
        bool DeleteMonitoredEvent(IntegerID handle);
        bool DeleteMonitoredDataChange(IntegerID handle);
        std::vector<PublishResult> PopPublishResult(); 
        bool HasPublishResult(); 
        NotificationData GetNotificationData();
        void PublishResults(const boost::system::error_code& error);
        std::vector<Variant> GetEventFields(const EventFilter& filter, const Event& event);
        void TriggerDataChangeEvent(MonitoredDataChange monitoreditems, AttributeValueID attrval);

      private:
        SubscriptionServiceInternal& Service;
        Server::AddressSpace& AddressSpace;
        mutable boost::shared_mutex DbMutex;
        SubscriptionData Data;
        const NodeID CurrentSession;
        std::function<void (PublishResult)> Callback;

        uint32_t NotificationSequence = 1; //NotificationSequence start at 1! not 0
        uint32_t KeepAliveCount = 0; 
        bool Startup = true; //To force specific behaviour at startup
        uint32_t LastMonitoredItemID = 100;
        MonitoredDataChangeMap MonitoredDataChanges; 
        MonitoredEventsMap MonitoredEvents;
        std::list<PublishResult> NotAcknowledgedResults; //result that have not be acknowledeged and may have to be resent
        std::list<TriggeredDataChange> TriggeredDataChangeEvents; 
        std::list<TriggeredEvent> TriggeredEvents; 
        boost::asio::io_service& io;
        boost::asio::deadline_timer Timer;
        bool TimerStopped = false;
        uint32_t LifeTimeCount;
        bool Debug = false;
         
    };


  }
}

 
