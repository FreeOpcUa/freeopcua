/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "test_dynamic_addon.h"

namespace
{
  class TestDynamicAddonImpl : public OpcCoreTests::TestDynamicAddon
  {
  public:
    virtual void Initialize()
    {
    }

    virtual void Stop()
    {
    }

    virtual void DoNothing()
    {
    }
  };
}

extern "C" Common::Addon::UniquePtr CreateAddon(const char* configuration)
{
  return Common::Addon::UniquePtr(new TestDynamicAddonImpl());
}

