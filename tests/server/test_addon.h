/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Test
  {
    class TestAddon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(TestAddon);

    public:
      virtual Common::AddonParameters GetParameters() const = 0;
    };

    const char TestAddonID[] = "TestAddonID";
  }
}
