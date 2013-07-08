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
#include <opc/ua/protocol/subscriptions.h>

namespace OpcUa
{
  namespace Remote
  {

    class SubscriptionServices : private Common::Interface
    {
    public:
      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters) = 0;
    };

  }
}

#endif // __OPC_UA_CLIENT_SUBSCRIPTIONS_H
