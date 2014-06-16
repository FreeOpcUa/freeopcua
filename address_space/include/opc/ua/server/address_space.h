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

#include <opc/ua/view.h>
#include <opc/ua/attributes.h>
#include <opc/ua/node_management.h>
#include <opc/ua/subscriptions.h>


namespace OpcUa
{
  namespace UaServer
  {

    class AddressSpace
      : public Remote::ViewServices
      , public Remote::AttributeServices
      , public Remote::NodeManagementServices
      , public Remote::SubscriptionServices 
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpace);
    };

    AddressSpace::UniquePtr CreateAddressSpace();

  } // namespace UaServer
} // nmespace OpcUa
