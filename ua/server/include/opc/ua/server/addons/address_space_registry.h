/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief View services addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_address_space_addon_h
#define opc_ua_address_space_addon_h

#include <opc/ua/attributes.h>
#include <opc/ua/view.h>


namespace OpcUa
{
  namespace Server
  {

    /// @brief Basic address space registry.
    /// Instance will enumerate spaces and browse nodes in the every registered address space.
    class AddressSpaceRegistry : Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceRegistry);

    public:
      virtual void Register(Remote::ViewServices::SharedPtr views) = 0;
      virtual void Unregister(Remote::ViewServices::SharedPtr views) = 0;

      virtual void Register(uint16_t namespaceIndex, Remote::AttributeServices::SharedPtr attributes) = 0;
      virtual void Unregister(uint16_t namespaceIndex) = 0;
    };

    const char AddressSpaceRegistryAddonID[] = "address_space_registry";

  } // namespace Server
} // nmespace OpcUa

#endif // opc_ua_address_space_addon_h
