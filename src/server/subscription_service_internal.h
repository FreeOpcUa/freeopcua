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

typedef std::map <uint32_t, std::shared_ptr<InternalSubscription>> SubscriptionsIdMap; // Map SubscptioinId, SubscriptionData


class SubscriptionServiceInternal : public Server::SubscriptionService
{
public:
  SubscriptionServiceInternal(Server::AddressSpace::SharedPtr addressspace, boost::asio::io_service & io, const Common::Logger::SharedPtr & logger);

  ~SubscriptionServiceInternal();

  virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<uint32_t> & subscriptions);
  virtual ModifySubscriptionResponse ModifySubscription(const ModifySubscriptionParameters & parameters);
  virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest & request, std::function<void (PublishResult)> callback);
  virtual std::vector<MonitoredItemCreateResult> CreateMonitoredItems(const MonitoredItemsParameters & params);
  virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters & params);
  virtual void Publish(const PublishRequest & request);
  virtual RepublishResponse Republish(const RepublishParameters & request);

  void DeleteAllSubscriptions();
  boost::asio::io_service & GetIOService();
  bool PopPublishRequest(NodeId node);
  void TriggerEvent(NodeId node, Event event);
  Server::AddressSpace & GetAddressSpace();

private:
  boost::asio::io_service & io;
  Server::AddressSpace::SharedPtr AddressSpace;
  Common::Logger::SharedPtr Logger;
  mutable boost::shared_mutex DbMutex;
  SubscriptionsIdMap SubscriptionsMap; // Map SubscptioinId, SubscriptionData
  uint32_t LastSubscriptionId = 2;
  std::map<NodeId, uint32_t> PublishRequestQueues;
};


}

}

