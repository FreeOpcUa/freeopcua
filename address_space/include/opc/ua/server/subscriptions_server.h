/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef SUBSCRIPTION_SERVER_H
#define SUBSCRIPTION_SERVER_H


#include <opc/ua/subscriptions.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/monitored_items.h>


namespace OpcUa
{
  struct MonitoredItemData
  {
    IntegerID SubscriptionID;
    MonitoringMode Mode;
    time_t LastTrigger;
    CreateMonitoredItemsResult Parameters;
  };

  class SubscriptionServicesServer: public Remote::SubscriptionServices
  {
    public:
      virtual std::vector<MonitoredItemData> PopItemsToPublish(const std::vector<IntegerID>& subscriptions) = 0;
  };

} // namespace OpcUa

#endif 
