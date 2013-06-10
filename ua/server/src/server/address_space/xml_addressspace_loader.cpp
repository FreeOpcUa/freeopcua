/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

// TODO Add EventNotifier Attribute to all requred nodes.

#include "xml_addressspace_loader.h"

#include <opc/ua/server/addons/address_space_registry.h>

namespace OpcUa
{
  namespace Internal
  {

    XmlAddressSpaceLoader::XmlAddressSpaceLoader(OpcUa::Server::AddressSpaceRegistry& registry)
      : Registry(registry)
    {
    }

    void XmlAddressSpaceLoader::Load(const char* fileName)
    {
    }

  } // namespace Internal
} // namespace OpcUa
