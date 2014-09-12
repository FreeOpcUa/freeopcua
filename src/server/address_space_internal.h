/// @author Alexander Rykovanov 2013-2014
/// @author Olivier Roulet-dubonnet 2014
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include "address_space_addon.h"
#include "internal_subscription.h"

#include <opc/ua/event.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/view.h>
#include <opc/ua/services/attributes.h>
#include <opc/ua/services/node_management.h>

#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <ctime>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <deque>
#include <set>
#include <thread>



namespace OpcUa
{
  namespace Internal
  {

    using namespace OpcUa::Remote;

    class InternalSubscription;

    typedef std::map <IntegerID, std::shared_ptr<InternalSubscription>> SubscriptionsIDMap; // Map SubscptioinID, SubscriptionData
    
    //store subscription for one attribute
    struct AttSubscription
    {
      IntegerID SubscriptionId;
      IntegerID MonitoredItemId;
      MonitoringParameters Parameters;
    };

    //Store an attribute value together with a link to all its suscriptions
    struct AttributeValue
    {
      DataValue Value;
      std::list<AttSubscription> AttSubscriptions; // a pair is subscirotionID, monitoredItemID
    };

    typedef std::map<AttributeID, AttributeValue> AttributesMap;
    
    //Store all data related to a Node
    struct NodeStruct
    {
      AttributesMap Attributes;
      std::vector<ReferenceDescription> References;
    };

    typedef std::map<NodeID, NodeStruct> NodesMap;



    class AddressSpaceInMemory : public UaServer::AddressSpace
    {
      public:
        AddressSpaceInMemory(bool debug);

       ~AddressSpaceInMemory();

        //Services implementation
        virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions);
        virtual SubscriptionData CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback);
        virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& params);
        virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params);
        virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items);
        virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items);
        virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const;
        virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const;
        virtual std::vector<ReferenceDescription> BrowseNext() const;
        virtual std::vector<DataValue> Read(const ReadParameters& params) const;
        virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values);
        virtual void Publish(const PublishRequest& request);

        //Server side methods
        void DeleteAllSubscriptions();
        boost::asio::io_service& GetIOService();
        bool PopPublishRequest(NodeID node);
        void TriggerEvent(NodeID node, Event event);

      private:
        void EnqueueEvent(AttributeValue& attval, const Event& event);
        std::tuple<bool, NodeID> FindElementInNode(const NodeID& nodeid, const RelativePathElement& element) const;
        BrowsePathResult TranslateBrowsePath(const BrowsePath& browsepath) const;
        CreateMonitoredItemsResult CreateMonitoredItem( SubscriptionsIDMap::iterator& subscription_it,  const MonitoredItemRequest& request);
        DataValue GetValue(const NodeID& node, AttributeID attribute) const;
        StatusCode SetValue(const NodeID& node, AttributeID attribute, const Variant& data);
        void UpdateSubscriptions(AttributeValue& val);
        bool IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const;
        bool IsSuitableReferenceType(const ReferenceDescription& reference, const NodeID& typeID, bool includeSubtypes) const;
        std::vector<NodeID> SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const;
        AddNodesResult AddNode( const AddNodesItem& item );
        StatusCode AddReference(const AddReferencesItem& item);

      private:
        bool Debug = false;
        mutable boost::shared_mutex DbMutex;
        NodesMap Nodes;
        SubscriptionsIDMap SubscriptionsMap; // Map SubscptioinID, SubscriptionData
        uint32_t LastSubscriptionID = 2;
        std::map<NodeID, uint32_t> PublishRequestQueues;
        boost::asio::io_service io;
        std::shared_ptr<boost::asio::io_service::work> work; //work object prevent worker thread to exist even whenre there are no subsciptions
        std::thread service_thread;

    };
  }

  namespace UaServer
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug);
  }
}

