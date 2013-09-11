/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "test_addon.h"

namespace
{
  class TestAddonImpl : public OpcUa::Test::TestAddon
  {
  public:
    virtual void Initialize(Common::AddonsManager& manager, const Common::AddonParameters& parameters)
    {
      Parameters = parameters;
    }

    virtual void Stop()
    {
    }

  public:
    virtual Common::AddonParameters GetParameters() const
    {
      return Parameters;
    }

  private:
    Common::AddonParameters Parameters;
  };
}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new TestAddonImpl());
}
