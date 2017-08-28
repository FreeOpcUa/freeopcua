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

void AddressSpaceAddon::Initialize(Common::AddonsManager & addons, const Common::AddonParameters & params)
{
  Logger = addons.GetLogger();
  Registry = Server::CreateAddressSpace(Logger);
  InternalServer = addons.GetAddon<OpcUa::Server::ServicesRegistry>(OpcUa::Server::ServicesRegistryAddonId);
  InternalServer->RegisterViewServices(Registry);
  InternalServer->RegisterAttributeServices(Registry);
  InternalServer->RegisterNodeManagementServices(Registry);
  InternalServer->RegisterMethodServices(Registry);
}

void AddressSpaceAddon::Stop()
{
  InternalServer->UnregisterViewServices();
  InternalServer->UnregisterAttributeServices();
  InternalServer->UnregisterNodeManagementServices();
  InternalServer->UnregisterMethodServices();
  InternalServer.reset();
  Registry.reset();
}

std::vector<AddNodesResult> AddressSpaceAddon::AddNodes(const std::vector<AddNodesItem> & items)
{
  return Registry->AddNodes(items);
}

std::vector<StatusCode> AddressSpaceAddon::AddReferences(const std::vector<AddReferencesItem> & items)
{
  return Registry->AddReferences(items);
}

std::vector<BrowseResult> AddressSpaceAddon::Browse(const OpcUa::NodesQuery & query) const
{
  return Registry->Browse(query);
}
std::vector<BrowseResult> AddressSpaceAddon::BrowseNext() const
{
  return Registry->BrowseNext();
}

std::vector<BrowsePathResult> AddressSpaceAddon::TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters & params) const
{
  return Registry->TranslateBrowsePathsToNodeIds(params);
}

std::vector<NodeId> AddressSpaceAddon::RegisterNodes(const std::vector<NodeId> & params) const
{
  return Registry->RegisterNodes(params);
}

void AddressSpaceAddon::UnregisterNodes(const std::vector<NodeId> & params) const
{
  return Registry->UnregisterNodes(params);
}

std::vector<DataValue> AddressSpaceAddon::Read(const OpcUa::ReadParameters & filter) const
{
  return Registry->Read(filter);
}

std::vector<StatusCode> AddressSpaceAddon::Write(const std::vector<OpcUa::WriteValue> & filter)
{
  return Registry->Write(filter);
}

uint32_t AddressSpaceAddon::AddDataChangeCallback(const NodeId & node, AttributeId attribute, std::function<Server::DataChangeCallback> callback)
{
  return Registry->AddDataChangeCallback(node, attribute, callback);
}

void AddressSpaceAddon::DeleteDataChangeCallback(uint32_t clienthandle)
{
  return Registry->DeleteDataChangeCallback(clienthandle);
}

StatusCode AddressSpaceAddon::SetValueCallback(const NodeId & node, AttributeId attribute, std::function<DataValue(void)> callback)
{
  return Registry->SetValueCallback(node, attribute, callback);
}

void AddressSpaceAddon::SetMethod(const NodeId & node, std::function<std::vector<OpcUa::Variant> (NodeId context, std::vector<OpcUa::Variant> arguments)> callback)
{
  Registry->SetMethod(node, callback);
  return;
}

std::vector<CallMethodResult> AddressSpaceAddon::Call(const std::vector<CallMethodRequest> & methodsToCall)
{
  return Registry->Call(methodsToCall);
}


} // namespace Internal
} // namespace OpcUa

Common::Addon::UniquePtr OpcUa::Server::AddressSpaceAddonFactory::CreateAddon()
{
  return Common::Addon::UniquePtr(new Internal::AddressSpaceAddon());
}
