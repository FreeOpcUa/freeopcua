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

#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/view.h>
#include <opc/ua/services/attributes.h>
#include <opc/ua/services/node_management.h>

#include <boost/thread/locks.hpp>
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

    class InternalSubscription;

    struct NodeAttribute
    {
      NodeId Node;
      AttributeId Attribute;
      NodeAttribute(NodeId node, AttributeId attribute) : Node(node), Attribute(attribute) {}
      NodeAttribute() {} //seems compiler wants this one
    };

    typedef std::map<uint32_t, NodeAttribute> ClientIdToAttributeMapType;
    
    struct DataChangeCallbackData
    {
      std::function<Server::DataChangeCallback> Callback;
      IntegerId ClientHandle;
    };

    typedef std::map<uint32_t, DataChangeCallbackData> DataChangeCallbackMap;

    //Store an attribute value together with a link to all its suscriptions
    struct AttributeValue
    {
      DataValue Value;
      DataChangeCallbackMap DataChangeCallbacks;
      std::function<DataValue(void)> GetValueCallback;
    };

    typedef std::map<AttributeId, AttributeValue> AttributesMap;
    
    //Store all data related to a Node
    struct NodeStruct
    {
      AttributesMap Attributes;
      std::vector<ReferenceDescription> References;
    };

    typedef std::map<NodeId, NodeStruct> NodesMap;

    //In memory storage of server opc-ua data model
    class AddressSpaceInMemory : public Server::AddressSpace
    {
      public:
        AddressSpaceInMemory(bool debug);

       ~AddressSpaceInMemory();

        //Services implementation
        virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items);
        virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items);
        virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const;
        virtual std::vector<BrowseResult> Browse(const OpcUa::NodesQuery& query) const;
        virtual std::vector<BrowseResult> BrowseNext() const;
        virtual std::vector<DataValue> Read(const ReadParameters& params) const;
        virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values);

        //Server side methods

        /// @brief Add callback which will be called when values of attribute is changed.
        /// @return handle of a callback which should be passed to the DeletDataChangeCallabck
        uint32_t AddDataChangeCallback(const NodeId& node, AttributeId attribute, std::function<Server::DataChangeCallback> callback);

        /// @bried Delete data change callback assosioated with handle.
        void DeleteDataChangeCallback(uint32_t serverhandle);

        /// @brief Set callback which will be called to read new value of the attribue.
        StatusCode SetValueCallback(const NodeId& node, AttributeId attribute, std::function<DataValue(void)> callback);

      private:
        std::tuple<bool, NodeId> FindElementInNode(const NodeId& nodeid, const RelativePathElement& element) const;
        BrowsePathResult TranslateBrowsePath(const BrowsePath& browsepath) const;
        DataValue GetValue(const NodeId& node, AttributeId attribute) const;
        StatusCode SetValue(const NodeId& node, AttributeId attribute, const DataValue& data);
        bool IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const;
        bool IsSuitableReferenceType(const ReferenceDescription& reference, const NodeId& typeId, bool includeSubtypes) const;
        std::vector<NodeId> SelectNodesHierarchy(std::vector<NodeId> sourceNodes) const;
        AddNodesResult AddNode( const AddNodesItem& item );
        StatusCode AddReference(const AddReferencesItem& item);
        NodeId GetNewNodeId(const NodeId& id);

      private:
        bool Debug = false;
        mutable boost::shared_mutex DbMutex;
        NodesMap Nodes;
        ClientIdToAttributeMapType ClientIdToAttributeMap; //Use to find callback using callback subcsriptionid
        uint32_t MaxNodeIdNum = 0;
        std::atomic<uint32_t> DataChangeCallbackHandle;
    };
  }

  namespace Server
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug);
  }
}

