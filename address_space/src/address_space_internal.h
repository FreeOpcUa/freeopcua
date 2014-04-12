/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Internal Address Space interfaces.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_SERVER_ADDRESS_SPACE_REGISTRY_H_
#define OPC_UA_SERVER_ADDRESS_SPACE_REGISTRY_H_

#include <opc/ua/attributes.h>
#include <opc/ua/server/addons/address_space_registry.h>
#include <opc/ua/view.h>
#include <opc/ua/addressspace.h>

namespace OpcUa
{
  namespace Internal
  {

    class AddressSpace
      : public Remote::ViewServices
      , public Remote::AttributeServices
      //, public Remote::AddressSpaceServices
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpace);
    };

    class AddressSpaceMultiplexor
      : public Server::AddressSpaceRegistry
      , public AddressSpace
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceMultiplexor);
    };


    AddressSpaceMultiplexor::UniquePtr CreateAddressSpaceMultiplexor();

  }
}

#endif // OPC_UA_SERVER_ADDRESS_SPACE_REGISTRY_H_
