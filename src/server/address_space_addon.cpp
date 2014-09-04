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

#include <opc/common/addons_core/addon_manager.h>
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/addons/services_registry.h>
#include <opc/ua/server/address_space.h>

#include <iostream>

namespace OpcUa
{
  namespace Internal
  {
    AddressSpaceAddon::AddressSpaceAddon()
    {
    }

    AddressSpaceAddon::~AddressSpaceAddon()
    {
    }

    AddressSpaceAddon::Options AddressSpaceAddon::GetOptions(const Common::AddonParameters& addonParams)
    {
      AddressSpaceAddon::Options options;
      for (const Common::Parameter& param : addonParams.Parameters)
      {
        if (param.Name == "debug" && !param.Value.empty() && param.Value != "0")
        {
          std::cout << "Enabled debug mode for address space addon." << std::endl;
          options.Debug = true;
        }
      }
      return options;
    }

    void AddressSpaceAddon::Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params)
    {
      Options options = GetOptions(params);
      Registry = Server::CreateAddressSpace(options.Debug);
      InternalServer = addons.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonID);
      InternalServer->RegisterViewServices(Registry);
      InternalServer->RegisterAttributeServices(Registry);
      InternalServer->RegisterNodeManagementServices(Registry);
      InternalServer->RegisterSubscriptionServices(Registry);
    }

    void AddressSpaceAddon::Stop()
    {
      InternalServer->UnregisterViewServices();
      InternalServer->UnregisterAttributeServices();
      InternalServer->UnregisterNodeManagementServices();
      InternalServer->UnregisterSubscriptionServices();
      InternalServer.reset();
      Registry.reset();
    }

    std::vector<AddNodesResult> AddressSpaceAddon::AddNodes(const std::vector<AddNodesItem>& items)
    {
      return Registry->AddNodes(items);
    }

    std::vector<StatusCode> AddressSpaceAddon::AddReferences(const std::vector<AddReferencesItem>& items)
    {
      return Registry->AddReferences(items);
    }

    void AddressSpaceAddon::AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
    {
      //Registry->AddAttribute(node, attribute, value);
    }

    void AddressSpaceAddon::AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
    {
      //Registry->AddReference(sourceNode, reference);
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

    SubscriptionData AddressSpaceAddon::CreateSubscription(const SubscriptionParameters& parameters, std::function<void (PublishResult)> callback)
    {
      return Registry->CreateSubscription(parameters, callback);
    }

    std::vector<StatusCode> AddressSpaceAddon::DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
    {
      return Registry->DeleteSubscriptions(subscriptions);
    }

    MonitoredItemsData AddressSpaceAddon::CreateMonitoredItems(const MonitoredItemsParameters& parameters)
    {
      return Registry->CreateMonitoredItems(parameters);
    }

    std::vector<StatusCode> AddressSpaceAddon::DeleteMonitoredItems(const DeleteMonitoredItemsParameters params)
    {
      return Registry->DeleteMonitoredItems(params);
    }

    void AddressSpaceAddon::Publish(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
    {
      return Registry->Publish(acknowledgements);
    }

    void AddressSpaceAddon::TriggerEvent(NodeID node, Event event)
    {
      return Registry->TriggerEvent(node, event);
    }
  } // namespace Internal
} // namespace OpcUa

Common::Addon::UniquePtr OpcUa::Server::AddressSpaceAddonFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new Internal::AddressSpaceAddon());
}
