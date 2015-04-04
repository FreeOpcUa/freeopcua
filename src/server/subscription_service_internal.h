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

    typedef std::map <IntegerId, std::shared_ptr<InternalSubscription>> SubscriptionsIdMap; // Map SubscptioinId, SubscriptionData


    class SubscriptionServiceInternal : public Server::SubscriptionService
    {
      public:
        SubscriptionServiceInternal(Server::AddressSpace::SharedPtr addressspace, boost::asio::io_service& io, bool debug);

       ~SubscriptionServiceInternal();

        virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerId>& subscriptions);
        virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback);
        virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& params);
        virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params);
        virtual void Publish(const PublishRequest& request);
        virtual RepublishResponse Republish(const RepublishParameters& request);

        void DeleteAllSubscriptions();
        boost::asio::io_service& GetIOService();
        bool PopPublishRequest(NodeId node);
        void TriggerEvent(NodeId node, Event event);
        Server::AddressSpace& GetAddressSpace();

      private:
        boost::asio::io_service& io;
        Server::AddressSpace::SharedPtr AddressSpace;
        bool Debug;
        mutable boost::shared_mutex DbMutex;
        SubscriptionsIdMap SubscriptionsMap; // Map SubscptioinId, SubscriptionData
        uint32_t LastSubscriptionId = 2;
        std::map<NodeId, uint32_t> PublishRequestQueues;
    };


  }

}

namespace 
{
  OpcUa::ByteString GenerateEventId()
  {
    //stupid id generator
    OpcUa::ByteString str;
    for (int i = 0; i < 8; i++)
      {
        int32_t val = rand() % std::numeric_limits<int32_t>::max(); 
        str.Data.push_back(val);
      }
    return str;
  }
}
