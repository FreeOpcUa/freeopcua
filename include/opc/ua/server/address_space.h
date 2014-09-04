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
#include <opc/ua/services/attributes.h>
#include <opc/ua/services/node_management.h>
#include <opc/ua/services/view.h>
#include <opc/ua/services/subscriptions.h>


namespace OpcUa
{
  namespace UaServer
  {

    class AddressSpace
      : public ViewServices
      , public AttributeServices
      , public NodeManagementServices
      , public SubscriptionServices
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpace);

      virtual void TriggerEvent(NodeID node, Event event) = 0;
    };

    AddressSpace::UniquePtr CreateAddressSpace(bool debug);

  } // namespace UaServer
} // nmespace OpcUa
