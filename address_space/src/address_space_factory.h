/// @author Olivier Roulet-Dubonnet 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#ifndef ADDRESS_SPACE_FACTORY_H
#define ADDRESS_SPACE_FACTORY_H


#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace UaServer
  {

    class AddressSpaceAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  }
} // namespace OpcUa
#endif /* ADDRESS_SPACE_FACTORY_H */

