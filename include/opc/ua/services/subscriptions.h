/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Subscription services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_CLIENT_SUBSCRIPTIONS_H
#define __OPC_UA_CLIENT_SUBSCRIPTIONS_H

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/monitored_items.h>

#include <functional>

namespace OpcUa
{

  class SubscriptionServices : private Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(SubscriptionServices);

    public:
      virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest&, std::function<void (PublishResult)> callbackPublish) = 0; 
      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerId>& subscriptions) = 0;
      virtual void Publish(const PublishRequest& request) = 0;
      virtual RepublishResponse Republish(const RepublishParameters& params) = 0;

      //FIXME: Spec says MonitoredItems methods should be in their own service
      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters) = 0; 
      virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params) = 0; 
  };

}

#endif // __OPC_UA_CLIENT_SUBSCRIPTIONS_H
