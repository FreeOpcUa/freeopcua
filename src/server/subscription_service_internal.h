/// @author Olivier Roulet-dubonnet 2014
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include "address_space_addon.h"
#include "internal_subscription.h"


#include <opc/ua/server/subscription_service.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/event.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>

#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <ctime>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <deque>
#include <set>
#include <thread>

namespace OpcUa
{
  namespace Internal
  {

    class InternalSubscription;

    typedef std::map <IntegerID, std::shared_ptr<InternalSubscription>> SubscriptionsIDMap; // Map SubscptioinID, SubscriptionData


    class SubscriptionServiceInternal : public UaServer::SubscriptionService
    {
      public:
        SubscriptionServiceInternal(std::shared_ptr<UaServer::AddressSpace> addressspace, bool debug);

       ~SubscriptionServiceInternal();

        virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions);
        virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback);
        virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& params);
        virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params);
        virtual void Publish(const PublishRequest& request);

        void DeleteAllSubscriptions();
        boost::asio::io_service& GetIOService();
        bool PopPublishRequest(NodeID node);
        void TriggerEvent(NodeID node, Event event);
        std::shared_ptr<UaServer::AddressSpace> GetAddressSpace();

      private:
        //CreateMonitoredItemsResult CreateMonitoredItem( SubscriptionsIDMap::iterator& subscription_it,  const MonitoredItemRequest& request);
        //void UpdateSubscriptions(AttributeValue& val);

      private:
        std::shared_ptr<UaServer::AddressSpace> AddressSpace;
        bool Debug;
        mutable boost::shared_mutex DbMutex;
        SubscriptionsIDMap SubscriptionsMap; // Map SubscptioinID, SubscriptionData
        uint32_t LastSubscriptionID = 2;
        std::map<NodeID, uint32_t> PublishRequestQueues;
        boost::asio::io_service io;
        std::shared_ptr<boost::asio::io_service::work> work; //work object prevent worker thread to exist even whenre there are no subsciptions
        std::thread service_thread;
    };


  }

  namespace UaServer
  {
    SubscriptionService::UniquePtr CreateSubscriptionService(std::shared_ptr<UaServer::AddressSpace> addressspace, bool debug);
  }


}
