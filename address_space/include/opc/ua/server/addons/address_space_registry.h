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
#include <opc/ua/addressspace.h>


namespace OpcUa
{
  namespace UaServer
  {

    /// @brief Basic address space registry.
    /// Instance will enumerate spaces and browse nodes in the every registered address space.
    //class AddressSpaceRegistry : Common::Interface
    class AddressSpaceRegistry : public Remote::AddressSpaceServices
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceRegistry);

    public:
      //virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value) = 0;
      //virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference) = 0;
    };

    const char AddressSpaceRegistryAddonID[] = "address_space_registry";

  } // namespace UaServer
} // nmespace OpcUa

#endif // opc_ua_address_space_addon_h
