/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/server/services_registry.h>
#include <opc/ua/view.h>
#include <opc/ua/subscriptions.h>

namespace OpcUa
{
  namespace Internal
  {

    class AddressSpaceAddon
      : public Common::Addon
      , public UaServer::AddressSpace
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceAddon);

    AddressSpaceAddon();
    virtual ~AddressSpaceAddon();

    public:
      virtual void Initialize(Common::AddonsManager& addons, const Common::AddonParameters& params);
      virtual void Stop();

    public: // NodeManagementServices
      virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items);
      virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items);
      virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value); //FIXME: remove
      virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference); //FIXME: remove

    public: // ViewServices
      virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const;
      virtual std::vector<ReferenceDescription> BrowseNext() const;
      virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const;

    public: // AttribueServices
      virtual std::vector<DataValue> Read(const OpcUa::ReadParameters& filter) const;
      virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter);

    public: // SubscriptionServices
      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters);
      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID> subscriptions);
      virtual std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIds);
      virtual void CreatePublishRequest(const std::vector<SubscriptionAcknowledgement>& acknowledgements);

    public: // MonitoredItemsServices
      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters);

    private:
      OpcUa::UaServer::AddressSpace::SharedPtr Registry;
      std::shared_ptr<OpcUa::UaServer::ServicesRegistry> InternalServer;
    };

  } // namespace UaServer
} // namespace OpcUa
