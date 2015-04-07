
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
      IntegerId MonitoredItemId;
      MonitoringMode Mode;
      time_t LastTrigger;
      CreateMonitoredItemsResult Parameters;
      IntegerId ClientHandle;
      uint32_t CallbackHandle;
    };

    struct TriggeredDataChange
    {
      IntegerId MonitoredItemId;
      MonitoredItems Data;
    };

    struct TriggeredEvent
    {
      IntegerId MonitoredItemId;
      EventFieldList Data;
    };

    //typedef std::pair<NodeId, AttributeId> MonitoredItemsIndex;
    typedef std::map<IntegerId, MonitoredDataChange> MonitoredDataChangeMap;
    typedef std::map<NodeId, IntegerId> MonitoredEventsMap;

    class AddressSpaceInMemory; //pre-declaration

    
    class InternalSubscription : public std::enable_shared_from_this<InternalSubscription>
    {
      public:
        InternalSubscription(SubscriptionServiceInternal& service, const SubscriptionData& data, const NodeId& SessionAuthenticationToken, std::function<void (PublishResult)> Callback, bool debug=false);
        ~InternalSubscription();
        void Start();
        void Stop();

        void NewAcknowlegment(const SubscriptionAcknowledgement& ack);
        std::vector<StatusCode> DeleteMonitoredItemsIds(const std::vector<IntegerId>& ids);
        bool EnqueueEvent(IntegerId monitoreditemid, const Event& event);
        bool EnqueueDataChange(IntegerId monitoreditemid, const DataValue& value);
        CreateMonitoredItemsResult CreateMonitoredItem(const MonitoredItemRequest& request);
        void DataChangeCallback(const IntegerId&, const DataValue& value);
        bool HasExpired();
        void TriggerEvent(NodeId node, Event event);
        RepublishResponse Republish(const RepublishParameters& params);

      private:
        void DeleteAllMonitoredItems(); 
        bool DeleteMonitoredEvent(IntegerId handle);
        bool DeleteMonitoredDataChange(IntegerId handle);
        std::vector<PublishResult> PopPublishResult(); 
        bool HasPublishResult(); 
        NotificationData GetNotificationData();
        void PublishResults(const boost::system::error_code& error);
        std::vector<Variant> GetEventFields(const EventFilter& filter, const Event& event);
        void TriggerDataChangeEvent(MonitoredDataChange monitoreditems, ReadValueId attrval);

      private:
        SubscriptionServiceInternal& Service;
        Server::AddressSpace& AddressSpace;
        mutable boost::shared_mutex DbMutex;
        SubscriptionData Data;
        const NodeId CurrentSession;
        std::function<void (PublishResult)> Callback;

        uint32_t NotificationSequence = 1; //NotificationSequence start at 1! not 0
        uint32_t KeepAliveCount = 0; 
        bool Startup = true; //To force specific behaviour at startup
        uint32_t LastMonitoredItemId = 100;
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

 
