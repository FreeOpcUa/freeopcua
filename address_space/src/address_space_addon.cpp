/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "address_space_addon.h"
#include "address_space_internal.h"

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space_registry.h>

namespace OpcUa
{
  namespace Internal
  {
    AddressSpaceAddon::AddressSpaceAddon()
      : Registry(Internal::CreateAddressSpaceMultiplexor())
    {
    }

    AddressSpaceAddon::~AddressSpaceAddon()
    {
    }

    void AddressSpaceAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      InternalServer = addons.GetAddon<OpcUa::UaServer::ServicesRegistryAddon>(OpcUa::UaServer::ServicesRegistryAddonID);
      InternalServer->RegisterViewServices(Registry);
      InternalServer->RegisterAttributeServices(Registry);
      InternalServer->RegisterAddressSpaceServices(Registry);
    }

    void AddressSpaceAddon::Stop()
    {
      InternalServer->UnregisterViewServices();
      InternalServer->UnregisterAttributeServices();
      InternalServer->UnregisterAttributeServices();
      InternalServer.reset();
    }

    void AddressSpaceAddon::AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
    {
      Registry->AddAttribute(node, attribute, value);
    }

    void AddressSpaceAddon::AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
    {
      Registry->AddReference(sourceNode, reference);
    }

    std::vector<ReferenceDescription> AddressSpaceAddon::Browse(const OpcUa::NodesQuery& query) const
    {
      return Registry->Browse(query);
    }
    std::vector<ReferenceDescription> AddressSpaceAddon::BrowseNext() const
    {
      return Registry->BrowseNext();
    }

    std::vector<BrowsePathResult> AddressSpaceAddon::TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const 
    {
      return Registry->TranslateBrowsePathsToNodeIds(params);
    }

    std::vector<DataValue> AddressSpaceAddon::Read(const OpcUa::ReadParameters& filter) const
    {
      return Registry->Read(filter);
    }

    std::vector<StatusCode> AddressSpaceAddon::Write(const std::vector<OpcUa::WriteValue>& filter)
    {
      return Registry->Write(filter);
    }

  } // namespace Internal
} // namespace OpcUa

Common::Addon::UniquePtr OpcUa::Internal::AddressSpaceAddonFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new AddressSpaceAddon());
}
