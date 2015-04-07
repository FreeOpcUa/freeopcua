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
  namespace Server
  {

    typedef void DataChangeCallback(const NodeId& node, AttributeId attribute, DataValue);

    class AddressSpace
      : public ViewServices
      , public AttributeServices
      , public NodeManagementServices
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpace);
      
      //Server side methods
      virtual uint32_t AddDataChangeCallback(const NodeId& node, AttributeId attribute, std::function<DataChangeCallback> callback) = 0;
      virtual void DeleteDataChangeCallback(uint32_t clienthandle) = 0;
      virtual StatusCode SetValueCallback(const NodeId& node, AttributeId attribute, std::function<DataValue(void)> callback) = 0;
      //FIXME : SHould we also expose SetValue and GetValue on server side? then we need to lock them ...
    };

    AddressSpace::UniquePtr CreateAddressSpace(bool debug);

  } // namespace UaServer
} // nmespace OpcUa
