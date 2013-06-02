/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "standard_namespace.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space_registry.h>
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/view.h>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;
  using namespace OpcUa::Remote;

  class StandardAddressSpaceAddon
    : public Common::Addon
  {
  public:
      StandardAddressSpaceAddon()
      : Space(Internal::CreateAddressSpaceMultiplexor())
    {
    }

    virtual ~StandardAddressSpaceAddon()
    {
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Registry = Common::GetAddon<AddressSpaceRegistry>(addons, OpcUa::Server::AddressSpaceRegistryAddonID);
      Registry->Register(Space);
      Registry->Register(0, Space);
    }

    virtual void Stop()
    {
      Registry->Unregister(Space);
      Registry->Unregister(0);
    }

  private:
    std::shared_ptr<OpcUa::Internal::AddressSpace> Space;
    std::shared_ptr<AddressSpaceRegistry> Registry;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new StandardAddressSpaceAddon());
}

