/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#ifndef ADDRESS_SPACE_ADDON_H_
#define ADDRESS_SPACE_ADDON_H_

#include "address_space_internal.h"

#include <opc/common/addons_core/addon.h>
#include <opc/ua/server/addons/services_registry.h>
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
      virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value);
      virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference);

    public: // ViewServices
      virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const;
      virtual std::vector<ReferenceDescription> BrowseNext() const;
      virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const;

    public: // AttribueServices
      virtual std::vector<DataValue> Read(const OpcUa::ReadParameters& filter) const;
      virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter);

    public:
      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters);
      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters);
      virtual std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIds);
      virtual void CreatePublishRequest(const std::vector<SubscriptionAcknowledgement>& acknowledgements);

 

    private:
      OpcUa::UaServer::AddressSpace::SharedPtr Registry;
      std::shared_ptr<OpcUa::UaServer::ServicesRegistryAddon> InternalServer;
    };

    class AddressSpaceAddonFactory : public Common::AddonFactory
    {
    public:
      DEFINE_CLASS_POINTERS(AddressSpaceAddonFactory);

    public:
      virtual Common::Addon::UniquePtr CreateAddon();
    };

  }
} // namespace OpcUa

#endif /* ADDRESS_SPACE_ADDON_H_ */
