/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/server/addons/endpoints.h>

#include <opccore/common/addons_core/addon_manager.h>

namespace
{

  using namespace OpcUa::Server;

  class Endpoints : public EndpointsAddon
  {
  public:
    Endpoints()
    {
    }

    virtual std::vector<Endpoint> GetEndpoints() const
    {
      return std::vector<Endpoint>();
    }

  public:
    virtual void Initialize(Common::AddonsManager& manager)
    {
    }

    virtual void Stop()
    {
    }
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new ::Endpoints());
}

