/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/event.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/services/subscriptions.h>


namespace boost
{
  namespace asio
  {
    class io_service;
  }
}

namespace OpcUa
{
  namespace Server
  {

    class SubscriptionService : public SubscriptionServices
    {
    public:
      DEFINE_CLASS_POINTERS(SubscriptionService);

      virtual void TriggerEvent(NodeId node, Event event) = 0;
    };

    SubscriptionService::UniquePtr CreateSubscriptionService(std::shared_ptr<AddressSpace> addressspace, boost::asio::io_service& io, bool debug);

  } // namespace UaServer
} // nmespace OpcUa
