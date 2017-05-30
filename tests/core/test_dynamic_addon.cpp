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
  virtual void Initialize(Common::AddonsManager &, const Common::AddonParameters & params)
  {
    Params = params;
  }

  virtual void Stop()
  {
  }

  virtual const char * GetStringWithHello() const
  {
    return "hello";
  }

  virtual Common::AddonParameters GetParameters() const
  {
    return Params;
  }
private:
  Common::AddonParameters Params;
};
}

extern "C"
#ifdef _WIN32
__declspec(dllexport)
#endif // _WIN32
Common::Addon * CreateAddon(const char * configuration)
{
  return new TestDynamicAddonImpl();
}

