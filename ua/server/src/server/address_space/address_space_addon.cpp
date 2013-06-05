/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "address_space_internal.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space_registry.h>
#include <opc/ua/server/addons/internal_computer.h>
#include <opc/ua/view.h>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Server;
  using namespace OpcUa::Remote;



  class AddressSpaceAddon
    : public Common::Addon
    , public AddressSpaceRegistry
  {
  public:
    AddressSpaceAddon()
      : Registry(Internal::CreateAddressSpaceMultiplexor())
    {
    }

    virtual ~AddressSpaceAddon()
    {
    }

  public:
    virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      InternalComputer = Common::GetAddon<InternalComputerAddon>(addons, InternalComputerAddonID);
      InternalComputer->RegisterViewServices(Registry);
      InternalComputer->RegisterAttributeServices(Registry);
    }

    virtual void Stop()
    {
      InternalComputer->UnregisterViewServices();
      InternalComputer->UnregisterAttributeServices();
      InternalComputer.reset();
    }

  public: // AddressSpaceRegistry
    virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
    {
      Registry->AddAttribute(node, attribute, value);
    }

    virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
    {
      Registry->AddReference(sourceNode, reference);
    }

  private:
    std::shared_ptr<OpcUa::Internal::AddressSpaceMultiplexor> Registry;
    std::shared_ptr<InternalComputerAddon> InternalComputer;
  };

}


extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new AddressSpaceAddon());
}
