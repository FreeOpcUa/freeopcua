/// @author Alexander Rykovanov 2013-2014
/// @author Olivier Roulet-dubonnet 2014
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "address_space_internal.h"


namespace OpcUa
{
  namespace Internal
  {
    typedef std::map <IntegerId, std::shared_ptr<InternalSubscription>> SubscriptionsIdMap; // Map SubscptioinId, SubscriptionData
    
    //store subscription for one attribute
    struct AttSubscription
    {
      IntegerId SubscriptionId;
      IntegerId MonitoredItemId;
      MonitoringParameters Parameters;
    };

    AddressSpaceInMemory::AddressSpaceInMemory(bool debug)
        : Debug(debug)
        , DataChangeCallbackHandle(0)
    {
      /*
      ObjectAttributes attrs;
      attrs.Description = LocalizedText(OpcUa::Names::Root);
      attrs.DisplayName = LocalizedText(OpcUa::Names::Root);

      AddNodesItem rootNode;
      rootNode.BrowseName = QualifiedName(0, OpcUa::Names::Root);
      rootNode.Class = NodeClass::Object;
      rootNode.RequestedNewNodeId = ObjectId::RootFolder;
      rootNode.TypeDefinition = ObjectId::FolderType;
      rootNode.Attributes = attrs;
      AddNode(rootNode);
      */
    }

    AddressSpaceInMemory::~AddressSpaceInMemory()
    {
    }

    std::vector<AddNodesResult> AddressSpaceInMemory::AddNodes(const std::vector<AddNodesItem>& items)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<AddNodesResult> results;
      for (const AddNodesItem& item: items)
      {
        results.push_back(AddNode(item));
      }
      return results;
    }

    std::vector<StatusCode> AddressSpaceInMemory::AddReferences(const std::vector<AddReferencesItem>& items)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<StatusCode> results;
      for (const auto& item : items)
      {
        results.push_back(AddReference(item));
      }
      return results;
    }

    std::vector<BrowsePathResult> AddressSpaceInMemory::TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<BrowsePathResult> results;
      for (BrowsePath browsepath : params.BrowsePaths )
      {
        BrowsePathResult result = TranslateBrowsePath(browsepath);
        results.push_back(result);
      }
      return results;
    }

    std::vector<BrowseResult> AddressSpaceInMemory::Browse(const OpcUa::NodesQuery& query) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      if (Debug) std::cout << "AddressSpaceInternal | Browsing." << std::endl;
      std::vector<BrowseResult> results;
      for ( BrowseDescription browseDescription: query.NodesToBrowse)
      {
        BrowseResult result;
        if(Debug)
        {
          std::cout << "AddressSpaceInternal | Browsing ";
          std::cout << " NodeId: '" << browseDescription.NodeToBrowse << "'";
          std::cout << ", ReferenceId: '" << browseDescription.ReferenceTypeId << "'";
          std::cout << ", Direction: " << browseDescription.Direction;
          std::cout << ", NodeClasses: 0x" << std::hex << (unsigned)browseDescription.NodeClasses;
          std::cout << ", ResultMask: '0x" << std::hex << (unsigned)browseDescription.ResultMask << std::endl;
        }

        NodesMap::const_iterator node_it = Nodes.find(browseDescription.NodeToBrowse);
        if ( node_it == Nodes.end() )
        {
          if (Debug) std::cout << "AddressSpaceInternal | Node '" << OpcUa::ToString(browseDescription.NodeToBrowse) << "' not found in the address space." << std::endl;
          continue;
        }

        std::copy_if(node_it->second.References.begin(), node_it->second.References.end(), std::back_inserter(result.Referencies),
            std::bind(&AddressSpaceInMemory::IsSuitableReference, this, std::cref(browseDescription), std::placeholders::_1)
        );
        results.push_back(result);
      }
      return results;
    }

    std::vector<BrowseResult> AddressSpaceInMemory::BrowseNext() const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      return std::vector<BrowseResult>();
    }

    std::vector<DataValue> AddressSpaceInMemory::Read(const ReadParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<DataValue> values;
      for (const ReadValueId& attribute : params.AttributesToRead)
      {
        values.push_back(GetValue(attribute.NodeId, attribute.AttributeId));
      }
      return values;
    }

    std::vector<StatusCode> AddressSpaceInMemory::Write(const std::vector<OpcUa::WriteValue>& values)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<StatusCode> statuses;
      for (WriteValue value : values)
      {
        if (value.Value.Encoding & DATA_VALUE)
        {
          statuses.push_back(SetValue(value.NodeId, value.AttributeId, value.Value));
          continue;
        }
        statuses.push_back(StatusCode::BadNotWritable);
      }
      return statuses;
    }

    std::tuple<bool, NodeId> AddressSpaceInMemory::FindElementInNode(const NodeId& nodeid, const RelativePathElement& element) const
    {
      NodesMap::const_iterator nodeit = Nodes.find(nodeid);
      if ( nodeit != Nodes.end() )
      {
        for (auto reference : nodeit->second.References)
        {
          //if (reference.first == current) { std::cout <<   reference.second.BrowseName.NamespaceIndex << reference.second.BrowseName.Name << " to " << element.TargetName.NamespaceIndex << element.TargetName.Name <<std::endl; }
          if (reference.BrowseName == element.TargetName)
          {
            return std::make_tuple(true, reference.TargetNodeId);
          }
        }
      }
      return std::make_tuple(false, NodeId());
    }

    BrowsePathResult AddressSpaceInMemory::TranslateBrowsePath(const BrowsePath& browsepath) const
    {
      NodeId current = browsepath.StartingNode;
      BrowsePathResult result;

      for (RelativePathElement element : browsepath.Path.Elements)
      {
        auto res = FindElementInNode(current, element);
        if ( std::get<0>(res) == false )
        {
          result.Status = OpcUa::StatusCode::BadNoMatch;
          return result;
        }
        current = std::get<1>(res);
      }

      result.Status = OpcUa::StatusCode::Good;
      std::vector<BrowsePathTarget> targets;
      BrowsePathTarget target;
      target.Node = current;
      target.RemainingPathIndex = std::numeric_limits<uint32_t>::max();
      targets.push_back(target);
      result.Targets = targets;
      return result;
    }

    DataValue AddressSpaceInMemory::GetValue(const NodeId& node, AttributeId attribute) const
    {
      NodesMap::const_iterator nodeit = Nodes.find(node);
      if ( nodeit == Nodes.end() )
      {
        if (Debug) std::cout << "AddressSpaceInternal | Bad node not found: " << node << std::endl;
      }
      else
      {
        AttributesMap::const_iterator attrit = nodeit->second.Attributes.find(attribute);
        if ( attrit == nodeit->second.Attributes.end() )
        {
          if (Debug) std::cout << "AddressSpaceInternal | node " << node << " has not attribute: " << (uint32_t)attribute << std::endl;
        }
        else
        {
          if ( attrit->second.GetValueCallback )
          {
            if (Debug) std::cout << "AddressSpaceInternal | A callback is set for this value, calling callback" << std::endl;
            return attrit->second.GetValueCallback();
          }
          if (Debug) std::cout << "AddressSpaceInternal | No callback is set for this value returning stored value" << std::endl;
          return attrit->second.Value;
        }
      }
      DataValue value;
      value.Encoding = DATA_VALUE_STATUS_CODE;
      value.Status = StatusCode::BadNotReadable;
      return value;
    }

    uint32_t AddressSpaceInMemory::AddDataChangeCallback(const NodeId& node, AttributeId attribute, std::function<Server::DataChangeCallback> callback)
    {
      if (Debug) std::cout << "AddressSpaceInternal| Set data changes callback for node " << node
         << " and attribute " << (unsigned)attribute <<  std::endl;
      NodesMap::iterator it = Nodes.find(node);
      if ( it == Nodes.end() )
      {
        if (Debug) std::cout << "AddressSpaceInternal| Node '" << node << "' not found." << std::endl;
        throw std::runtime_error("AddressSpaceInternal | NodeId not found");
      }
      AttributesMap::iterator ait = it->second.Attributes.find(attribute);
      if ( ait == it->second.Attributes.end() )
      {
        if (Debug) std::cout << "address_space| Attribute " << (unsigned)attribute << " of node '" << node << "' not found." << std::endl;
        throw std::runtime_error("Attribute not found");
      }

      uint32_t handle = ++DataChangeCallbackHandle;
      DataChangeCallbackData data;
      data.Callback = callback;
      ait->second.DataChangeCallbacks[handle] = data;
      ClientIdToAttributeMap[handle] = NodeAttribute(node, attribute);
      return handle;
    }

    void AddressSpaceInMemory::DeleteDataChangeCallback(uint32_t serverhandle )
    {
      if (Debug) std::cout << "AddressSpaceInternal | Deleting callback with client id. " << serverhandle << std::endl;

      ClientIdToAttributeMapType::iterator it = ClientIdToAttributeMap.find(serverhandle);
      if ( it == ClientIdToAttributeMap.end() )
      {
        std::cout << "AddressSpaceInternal | Error, request to delete a callback using unknown handle: " << serverhandle << std::endl;
        return;
      }

      NodesMap::iterator nodeit = Nodes.find(it->second.Node);
      if ( nodeit != Nodes.end() )
      {
        AttributesMap::iterator ait = nodeit->second.Attributes.find(it->second.Attribute);
        if ( ait != nodeit->second.Attributes.end() )
        {
          size_t nb = ait->second.DataChangeCallbacks.erase(serverhandle);
          if (Debug) std::cout << "AddressSpaceInternal | deleted " << nb << " callbacks" << std::endl;
          ClientIdToAttributeMap.erase(serverhandle);
          return;
        }
      }
      throw std::runtime_error("AddressSpaceInternal | NodeId or attribute nor found");
    }

    StatusCode AddressSpaceInMemory::SetValueCallback(const NodeId& node, AttributeId attribute, std::function<DataValue(void)> callback)
    {
      NodesMap::iterator it = Nodes.find(node);
      if ( it != Nodes.end() )
      {
        AttributesMap::iterator ait = it->second.Attributes.find(attribute);
        if ( ait != it->second.Attributes.end() )
        {
          ait->second.GetValueCallback = callback;
          return StatusCode::Good;
        }
      }
      return StatusCode::BadAttributeIdInvalid;
    }

    StatusCode AddressSpaceInMemory::SetValue(const NodeId& node, AttributeId attribute, const DataValue& data)
    {
      NodesMap::iterator it = Nodes.find(node);
      if ( it != Nodes.end() )
      {
        AttributesMap::iterator ait = it->second.Attributes.find(attribute);
        if ( ait != it->second.Attributes.end() )
        {
          DataValue value(data);
          value.SetServerTimestamp(DateTime::Current());
          ait->second.Value = value;
          //call registered callback
          for (auto pair : ait->second.DataChangeCallbacks)
          {
            pair.second.Callback(it->first, ait->first, ait->second.Value);
          }
          return StatusCode::Good;
        }
      }
      return StatusCode::BadAttributeIdInvalid;
    }

    bool AddressSpaceInMemory::IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const
    {
      if (Debug) std::cout << "AddressSpaceInternal | Checking reference '" << reference.ReferenceTypeId << "' to the node '" << reference.TargetNodeId << "' (" << reference.BrowseName << ") which must fit ref: " << desc.ReferenceTypeId << " with include subtype: " << desc.IncludeSubtypes << std::endl;

      if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
      {
        if (Debug) std::cout << "AddressSpaceInternal | Reference in different direction." << std::endl;
        return false;
      }
      if (desc.ReferenceTypeId != ObjectId::Null && !IsSuitableReferenceType(reference, desc.ReferenceTypeId, desc.IncludeSubtypes))
      {
        if (Debug) std::cout << "AddressSpaceInternal | Reference has wrong type." << std::endl;
        return false;
      }
      if (desc.NodeClasses != NodeClass::Unspecified && (desc.NodeClasses & reference.TargetNodeClass) == NodeClass::Unspecified)
      {
        if (Debug) std::cout << "AddressSpaceInternal | Reference has wrong class." << std::endl;
        return false;
      }
      if (Debug) std::cout << "AddressSpaceInternal | Reference suitable." << std::endl;
      return true;
    }

    bool AddressSpaceInMemory::IsSuitableReferenceType(const ReferenceDescription& reference, const NodeId& typeId, bool includeSubtypes) const
    {
      if (!includeSubtypes)
      {
        return reference.ReferenceTypeId == typeId;
      }
      const std::vector<NodeId> suitableTypes = SelectNodesHierarchy(std::vector<NodeId>(1, typeId));
      const auto resultIt = std::find(suitableTypes.begin(), suitableTypes.end(), reference.ReferenceTypeId);\
      return resultIt != suitableTypes.end();
    }

    std::vector<NodeId> AddressSpaceInMemory::SelectNodesHierarchy(std::vector<NodeId> sourceNodes) const
    {
      std::vector<NodeId> subNodes;
      for ( NodeId nodeid: sourceNodes )
      {
          NodesMap::const_iterator node_it = Nodes.find(nodeid);
          if ( node_it != Nodes.end() )
          {
            for (auto& ref:  node_it->second.References )
            {
              subNodes.push_back(ref.TargetNodeId);
          }
        }
      }
      if (subNodes.empty())
      {
        return sourceNodes;
      }

      const std::vector<NodeId> allChilds = SelectNodesHierarchy(subNodes);
      sourceNodes.insert(sourceNodes.end(), allChilds.begin(), allChilds.end());
      return sourceNodes;
    }

    AddNodesResult AddressSpaceInMemory::AddNode( const AddNodesItem& item )
    {
      AddNodesResult result;
      if (Debug) std::cout << "AddressSpaceInternal | address_space| Adding new node id='" << item.RequestedNewNodeId << "' name=" << item.BrowseName.Name << std::endl;

      if (!Nodes.empty() && item.RequestedNewNodeId != ObjectId::Null && Nodes.find(item.RequestedNewNodeId) != Nodes.end())
      {
        std::cerr << "AddressSpaceInternal | Error: NodeId '"<< item.RequestedNewNodeId << "' allready exist: " << std::endl;
        result.Status = StatusCode::BadNodeIdExists;
        return result;
      }

      NodesMap::iterator parent_node_it = Nodes.end();
      if (item.ParentNodeId != NodeId())
      {
        parent_node_it = Nodes.find(item.ParentNodeId);
        if ( parent_node_it == Nodes.end() )
        {
          if (Debug) std::cout << "AddressSpaceInternal | Error: Parent node '"<< item.ParentNodeId << "'does not exist" << std::endl;
          result.Status = StatusCode::BadParentNodeIdInvalid;
          return result;
        }
      }

      const NodeId resultId = GetNewNodeId(item.RequestedNewNodeId);
      NodeStruct nodestruct;
      //Add Common attributes
      nodestruct.Attributes[AttributeId::NodeId].Value = resultId;
      nodestruct.Attributes[AttributeId::BrowseName].Value = item.BrowseName;
      nodestruct.Attributes[AttributeId::NodeClass].Value = static_cast<int32_t>(item.Class);

      // Add requested attributes
      for (const auto& attr: item.Attributes.Attributes)
      {
        AttributeValue attval;
        attval.Value = attr.second;

        nodestruct.Attributes.insert(std::make_pair(attr.first, attval));
      }

      Nodes.insert(std::make_pair(resultId, nodestruct));

      if (parent_node_it != Nodes.end())
      {
        // Link to parent
        ReferenceDescription desc;
        desc.ReferenceTypeId = item.ReferenceTypeId;
        desc.TargetNodeId = resultId;
        desc.TargetNodeClass = item.Class;
        desc.BrowseName = item.BrowseName;
        desc.DisplayName = LocalizedText(item.BrowseName.Name);
        desc.TargetNodeTypeDefinition = item.TypeDefinition;
        desc.IsForward = true; // should this be in constructor?

        parent_node_it->second.References.push_back(desc);
      }

      if (item.TypeDefinition != ObjectId::Null)
      {
        // Link to parent
        AddReferencesItem typeRef;
        typeRef.SourceNodeId = resultId;
        typeRef.IsForward = true;
        typeRef.ReferenceTypeId = ObjectId::HasTypeDefinition;
        typeRef.TargetNodeId = item.TypeDefinition;
        typeRef.TargetNodeClass = NodeClass::DataType;
        AddReference(typeRef);
      }

      result.Status = StatusCode::Good;
      result.AddedNodeId = resultId;
      if (Debug) std::cout << "AddressSpaceInternal | node added." << std::endl;
      return result;
    }

    StatusCode AddressSpaceInMemory::AddReference(const AddReferencesItem& item)
    {
      NodesMap::iterator node_it = Nodes.find(item.SourceNodeId);
      if ( node_it == Nodes.end() )
      {
        return StatusCode::BadSourceNodeIdInvalid;
      }
      NodesMap::iterator targetnode_it = Nodes.find(item.TargetNodeId);
      if ( targetnode_it == Nodes.end() )
      {
        return StatusCode::BadTargetNodeIdInvalid;
      }
      ReferenceDescription desc;
      desc.ReferenceTypeId = item.ReferenceTypeId;
      desc.IsForward = item.IsForward;
      desc.TargetNodeId = item.TargetNodeId;
      desc.TargetNodeClass = item.TargetNodeClass;
      DataValue dv = GetValue(item.TargetNodeId, AttributeId::BrowseName);
      if (dv.Status == StatusCode::Good)
      {
        desc.BrowseName = dv.Value.As<QualifiedName>();
      }
      else
      {
        desc.BrowseName = QualifiedName("NONAME", 0);
      }
      dv = GetValue(item.TargetNodeId, AttributeId::DisplayName);
      if (dv.Status == StatusCode::Good)
      {
        desc.DisplayName = dv.Value.As<LocalizedText>();
      }
      else
      {
        desc.DisplayName = LocalizedText(desc.BrowseName.Name);
      }
      node_it->second.References.push_back(desc);
      return StatusCode::Good;
    }

    NodeId AddressSpaceInMemory::GetNewNodeId(const NodeId& id)
    {
      if (id == ObjectId::Null)
      {
        return OpcUa::NumericNodeId(++MaxNodeIdNum);
      }

      if (id.GetNamespaceIndex() == 0)
      {
        const uint32_t number = id.GetIntegerIdentifier();
        if (MaxNodeIdNum < number)
        {
          MaxNodeIdNum = number;
        }
      }

      return id;
    }
  }

  namespace Server
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug)
    {
      return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory(debug));
    }
  }
}
