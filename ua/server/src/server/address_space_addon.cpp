/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "standard_namespace.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/view.h>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;
  using namespace OpcUa::Remote;


  class AddressSpaceAddon : public Common::Addon
  {
  public:
    AddressSpaceAddon()
      : Services(CreateStandardNamespace())
    {
    }

    virtual ~AddressSpaceAddon()
    {
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      InternalComputer = Common::GetAddon<InternalComputerAddon>(addons, InternalComputerAddonID);
      InternalComputer->RegisterViewServices(Services);
    }

    virtual void Stop()
    {
      InternalComputer->UnregisterViewServices();
      InternalComputer.reset();
    }

  private:
    std::shared_ptr<ViewServices> Services;
    std::shared_ptr<InternalComputerAddon> InternalComputer;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new AddressSpaceAddon());
}

