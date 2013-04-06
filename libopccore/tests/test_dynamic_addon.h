/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef TEST_DYNAMIC_ADDON_H
#define TEST_DYNAMIC_ADDON_H

#include <opccore/common/addons_core/addon.h>

#include <string>

namespace OpcCoreTests
{
  class TestDynamicAddon : public Common::Addon
  {
  public:
    virtual std::string GetStringWithHello() const = 0;
  };

}

#endif // TEST_DYNAMIC_ADDON_H
