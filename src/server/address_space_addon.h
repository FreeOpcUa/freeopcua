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
#include <opc/ua/event.h>
#include <opc/ua/server/address_space.h>
#include <opc/ua/server/services_registry.h>
#include <opc/ua/services/view.h>
#include <opc/ua/services/subscriptions.h>

namespace OpcUa
{
  namespace Internal
  {

    class AddressSpaceAddon
      : public Common::Addon
      , public Server::AddressSpace
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

    public: // ViewServices
      virtual std::vector<BrowseResult> Browse(const OpcUa::NodesQuery& query) const;
      virtual std::vector<BrowseResult> BrowseNext() const;
      virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const;

    public: // AttribueServices
      virtual std::vector<DataValue> Read(const OpcUa::ReadParameters& filter) const;
      virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter);

    public: // Server internal methods
      virtual uint32_t AddDataChangeCallback(const NodeId& node, AttributeId attribute, std::function<Server::DataChangeCallback> callback);
      virtual void DeleteDataChangeCallback(uint32_t clienthandle);
      virtual StatusCode SetValueCallback(const NodeId& node, AttributeId attribute, std::function<DataValue(void)> callback);

    private:
      struct Options
      {
        bool Debug = false;
      };

    private:
      Options GetOptions(const Common::AddonParameters& addonParams);

    private:
      OpcUa::Server::AddressSpace::SharedPtr Registry;
      std::shared_ptr<OpcUa::Server::ServicesRegistry> InternalServer;
    };

  } // namespace UaServer
} // namespace OpcUa
