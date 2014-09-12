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

    using namespace OpcUa::Remote;

    AddressSpaceInMemory::AddressSpaceInMemory(bool debug)
        : Debug(debug), work(new boost::asio::io_service::work(io))
      {
        //Initialize the worker thread for subscriptions
        service_thread = std::thread([&](){ io.run(); });

        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Root);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Root);

        AddNodesItem rootNode;
        rootNode.BrowseName = QualifiedName(0, OpcUa::Names::Root);
        rootNode.Class = NodeClass::Object;
        rootNode.RequestedNewNodeID = ObjectID::RootFolder;
        rootNode.TypeDefinition = ObjectID::FolderType;
        rootNode.Attributes = attrs;
        AddNode(rootNode);
      }

    AddressSpaceInMemory::~AddressSpaceInMemory()
      {
        if (Debug) std::cout << "address_space_in_memory| Stopping boost io service." << std::endl;
        io.stop();
        if (Debug) std::cout << "address_space_in_memory| Joining service thread." << std::endl;
        service_thread.join();
      }

      boost::asio::io_service& AddressSpaceInMemory::GetIOService() 
      {
        return io;
      }

      void AddressSpaceInMemory::DeleteAllSubscriptions()
      {
        if (Debug) std::cout << "Deleting all subscriptions." << std::endl;
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<IntegerID> ids(SubscriptionsMap.size());\
        std::transform(SubscriptionsMap.begin(), SubscriptionsMap.end(), ids.begin(), [](const SubscriptionsIDMap::value_type& i){return i.first;});
        DeleteSubscriptions(ids);
      }

  /*    
      std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        //We need to stop the subscriptions and then delete them in io_service to make sure their are stopped before they are deleted
        for (auto id: subscriptions)
        {
          std::cout << "Stopping Subscription (part 1): " << id << std::endl;
          SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(id);
          if ( itsub != SubscriptionsMap.end()) 
          {
            itsub->second->Stop();
          }
        }

        io.dispatch([=](){ this->_DeleteSubscriptions(subscriptions); });
        //Now return good for everything, we do not care :-)
        std::vector<StatusCode> codes;
        for ( auto _ : subscriptions)
        {
          codes.push_back(StatusCode::Good);
        }
        return codes;
      }
      
 */     

      std::vector<StatusCode> AddressSpaceInMemory::DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<StatusCode> result;
        for (const IntegerID& subid: subscriptions)
        {
          std::cout << "Deleting Subscription: " << subid << std::endl;
          size_t count = SubscriptionsMap.erase(subid);
          if ( count > 0)
          {
            result.push_back(StatusCode::Good);
          }
          else
          {
            std::cout << "Error, got request to delete non existing Subscription: " << subid << std::endl;
            result.push_back(StatusCode::BadSubscriptionIdInvalid);
          }
        }
        return result;
      }

      SubscriptionData AddressSpaceInMemory::CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        SubscriptionData data;
        data.ID = ++LastSubscriptionID;
        data.RevisedLifetimeCount = request.Parameters.RequestedLifetimeCount;
        data.RevisedPublishingInterval = request.Parameters.RequestedPublishingInterval;
        data.RevizedMaxKeepAliveCount = request.Parameters.RequestedMaxKeepAliveCount;
        std::cout << "Creating Subscription with ID: " << data.ID << std::endl;

        std::shared_ptr<InternalSubscription> sub(new InternalSubscription(data, request.Header.SessionAuthenticationToken, *this, callback));
        SubscriptionsMap[data.ID] = sub;
        return data;
      }

      MonitoredItemsData AddressSpaceInMemory::CreateMonitoredItems(const MonitoredItemsParameters& params)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        MonitoredItemsData data;

        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(params.SubscriptionID);
        if ( itsub == SubscriptionsMap.end()) //SubscriptionID does not exist, return errors for all items
        {
          for (int j=0; j<(int)params.ItemsToCreate.size(); j++)
          {
            CreateMonitoredItemsResult res;
            res.Status = StatusCode::BadSubscriptionIdInvalid;
            data.Results.push_back(res);
          }
          return data;
        }

        for (const MonitoredItemRequest& req: params.ItemsToCreate)
        {
          CreateMonitoredItemsResult result = CreateMonitoredItem(itsub, req);
          data.Results.push_back(result);
        }
        return data;
     
      }

      std::vector<StatusCode> AddressSpaceInMemory::DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<StatusCode> results;

        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(params.SubscriptionId);
        if ( itsub == SubscriptionsMap.end()) //SubscriptionID does not exist, return errors for all items
        {
          for (int j=0; j<(int)params.MonitoredItemsIds.size(); j++)
          {
            results.push_back(StatusCode::BadSubscriptionIdInvalid);
          }
          return results;
        }

        results = itsub->second->DeleteMonitoredItemsIds(params.MonitoredItemsIds);
        return results;
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

      std::vector<ReferenceDescription> AddressSpaceInMemory::Browse(const OpcUa::NodesQuery& query) const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        if (Debug) std::cout << "Browsing." << std::endl;
        std::vector<ReferenceDescription> result;
        for ( BrowseDescription browseDescription: query.NodesToBrowse)
        {
          if(Debug)
          {
            std::cout << "Browsing ";
            std::cout << " NodeID: '" << browseDescription.NodeToBrowse << "'";
            std::cout << ", ReferenceID: '" << browseDescription.ReferenceTypeID << "'";
            std::cout << ", Direction: " << browseDescription.Direction;
            std::cout << ", NodeClasses: 0x" << std::hex << (unsigned)browseDescription.NodeClasses;
            std::cout << ", ResultMask: '0x" << std::hex << (unsigned)browseDescription.ResultMask << std::endl;
          }

          NodesMap::const_iterator node_it = Nodes.find(browseDescription.NodeToBrowse);
          if ( node_it == Nodes.end() )
          {
            if (Debug) std::cout << "Node not found in the address space." << std::endl;
            continue;
          }

          if(Debug)
          {
            std::cout << "Node found in the address space." << std::endl;
            std::cout << "Finding reference." << std::endl;
          }

          std::copy_if(node_it->second.References.begin(), node_it->second.References.end(), std::back_inserter(result),
              std::bind(&AddressSpaceInMemory::IsSuitableReference, this, std::cref(browseDescription), std::placeholders::_1)
          );
        }
        return result;
      }

      std::vector<ReferenceDescription> AddressSpaceInMemory::BrowseNext() const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        return std::vector<ReferenceDescription>();
      }

      std::vector<DataValue> AddressSpaceInMemory::Read(const ReadParameters& params) const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<DataValue> values;
        for (const AttributeValueID& attribute : params.AttributesToRead)
        {
          values.push_back(GetValue(attribute.Node, attribute.Attribute));
        }
        return values;
      }

      std::vector<StatusCode> AddressSpaceInMemory::Write(const std::vector<OpcUa::WriteValue>& values)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<StatusCode> statuses;
        for (WriteValue value : values)
        {
          if (value.Data.Encoding & DATA_VALUE)
          {
            statuses.push_back(SetValue(value.Node, value.Attribute, value.Data.Value));
            continue;
          }
          statuses.push_back(StatusCode::BadNotWritable);
        }
        return statuses;
      }

      void AddressSpaceInMemory::Publish(const PublishRequest& request)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);
        if ( PublishRequestQueues[request.Header.SessionAuthenticationToken] < 100 )
        {
          PublishRequestQueues[request.Header.SessionAuthenticationToken] += 1;
        }
        //FIXME: else spec says we should return error to warn client

        for (SubscriptionAcknowledgement ack:  request.Parameters.Acknowledgements)
        {
          SubscriptionsIDMap::iterator sub_it = SubscriptionsMap.find(ack.SubscriptionID);
          if ( sub_it != SubscriptionsMap.end())
          {
            sub_it->second->NewAcknowlegment(ack); 
          }
        }
      }

      bool AddressSpaceInMemory::PopPublishRequest(NodeID node)
      {
        std::map<NodeID, uint32_t>::iterator queue_it = PublishRequestQueues.find(node); 
        if ( queue_it == PublishRequestQueues.end() )
        {
          std::cout << "Error request for publish queue for unknown sessino" << node << std::endl;
          return false;
        }
        else
        {
          if ( queue_it->second == 0 )
          {
            return false;
          }
          else
          {
            --queue_it->second;
            return true;
          }
        }
      }

      void AddressSpaceInMemory::TriggerEvent(NodeID node, Event event)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        //find node
        NodesMap::iterator it = Nodes.find(node);
        if ( it == Nodes.end() )
        {
          std::cout << "NodeID does not exist, raise exception\n";
        }
        else
        {
          //find event_notifier attribute of node
          AttributesMap::iterator ait = it->second.Attributes.find(AttributeID::EVENT_NOTIFIER);
          if ( ait == it->second.Attributes.end() )
          {
            std::cout << "Attempt to trigger event for node " << it->first << " which has no event_notifier attribute. This should raise exception" << std::endl;;
          }
          else
          {
            EnqueueEvent(ait->second, event);
          }
        }
      }


      void AddressSpaceInMemory::EnqueueEvent(AttributeValue& attval, const Event& event)
      {
        //go through all subscription of attribute and add new event
        for (auto attsub = attval.AttSubscriptions.begin(); attsub != attval.AttSubscriptions.end() ;)
        {
          SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(attsub->SubscriptionId);
          if ( itsub == SubscriptionsMap.end() )
          {
            attsub = attval.AttSubscriptions.erase(attsub);
          }
          else
          {
            bool res = itsub->second->EnqueueEvent(attsub->MonitoredItemId, event);
            if ( ! res )
            {
              attsub = attval.AttSubscriptions.erase(attsub);
            }
          }
          ++attsub;
        }
      }


      std::tuple<bool, NodeID> AddressSpaceInMemory::FindElementInNode(const NodeID& nodeid, const RelativePathElement& element) const
      {
        NodesMap::const_iterator nodeit = Nodes.find(nodeid);
        if ( nodeit != Nodes.end() )
        {
          for (auto reference : nodeit->second.References)
          {
            //if (reference.first == current) { std::cout <<   reference.second.BrowseName.NamespaceIndex << reference.second.BrowseName.Name << " to " << element.TargetName.NamespaceIndex << element.TargetName.Name <<std::endl; }
            if (reference.BrowseName == element.TargetName)
            {
              return std::make_tuple(true, reference.TargetNodeID);
            }
          }
        }
        return std::make_tuple(false, NodeID());
      }

      BrowsePathResult AddressSpaceInMemory::TranslateBrowsePath(const BrowsePath& browsepath) const
      {
        NodeID current = browsepath.StartingNode;
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


      CreateMonitoredItemsResult AddressSpaceInMemory::CreateMonitoredItem( SubscriptionsIDMap::iterator& subscription_it,  const MonitoredItemRequest& request)
      {
        NodesMap::iterator node_it = Nodes.find(request.ItemToMonitor.Node);
        if ( node_it == Nodes.end() )
        {
          CreateMonitoredItemsResult res;
          res.Status = OpcUa::StatusCode::BadNodeIdUnknown;
          std::cout << "NodeID does not exist: " << request.ItemToMonitor.Node << std::endl;
          return res;
        }

        AttributesMap::iterator attrit = node_it->second.Attributes.find(request.ItemToMonitor.Attribute);
        if ( attrit == node_it->second.Attributes.end() )
        {
          CreateMonitoredItemsResult res;
          res.Status = OpcUa::StatusCode::BadAttributeIdInvalid;
          std::cout << "attribute not found" << std::endl;
          return res;
        }
      
        std::cout << "creating monitored item using subscription: " << subscription_it->first << std::endl;
        CreateMonitoredItemsResult res = subscription_it->second->AddMonitoredItem(request);

        AttSubscription attsub;
        attsub.SubscriptionId = subscription_it->first;
        attsub.MonitoredItemId = res.MonitoredItemID;
        attsub.Parameters = request.Parameters;
        attrit->second.AttSubscriptions.push_back(attsub); 

        return res;
      }

      DataValue AddressSpaceInMemory::GetValue(const NodeID& node, AttributeID attribute) const
      {
        NodesMap::const_iterator nodeit = Nodes.find(node);
        if ( nodeit != Nodes.end() )
        {
          AttributesMap::const_iterator attrit = nodeit->second.Attributes.find(attribute);
          if ( attrit != nodeit->second.Attributes.end() )
          {
            return attrit->second.Value;
          }
        }
        DataValue value;
        value.Encoding = DATA_VALUE_STATUS_CODE;
        value.Status = StatusCode::BadNotReadable;
        return value;
      }

      StatusCode AddressSpaceInMemory::SetValue(const NodeID& node, AttributeID attribute, const Variant& data)
      {
        NodesMap::iterator it = Nodes.find(node);
        if ( it != Nodes.end() )
        {
          AttributesMap::iterator ait = it->second.Attributes.find(attribute);
          if ( ait != it->second.Attributes.end() )
          {
            DataValue value(data);
            value.ServerTimestamp = CurrentDateTime();
            value.SourceTimestamp = CurrentDateTime(); //FIXME: should allow client to set this one
            ait->second.Value = value;
            UpdateSubscriptions(ait->second);
            return StatusCode::Good;
          }
        }
        return StatusCode::BadAttributeIdInvalid;
      }

      void AddressSpaceInMemory::UpdateSubscriptions(AttributeValue& val)
      {
        for (auto attsub = val.AttSubscriptions.begin(); attsub !=val.AttSubscriptions.end();)
        {
          SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(attsub->SubscriptionId);
          if ( itsub == SubscriptionsMap.end() )
          {
            attsub = val.AttSubscriptions.erase(attsub);
            continue;
          }
          bool res = itsub->second->EnqueueDataChange(attsub->MonitoredItemId, val.Value);
          if ( ! res ) 
          {
            attsub = val.AttSubscriptions.erase(attsub);
            continue;
          }
  
          ++attsub;
        }
      }

      bool AddressSpaceInMemory::IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const
      {
        if (Debug)
          std::cout << "Checking reference '" << reference.ReferenceTypeID << "' to the node '" << reference.TargetNodeID << "' (" << reference.BrowseName << "_." << std::endl;

        if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
        {
          if (Debug) std::cout << "Reference in different direction." << std::endl;
          return false;
        }
        if (desc.ReferenceTypeID != ObjectID::Null && !IsSuitableReferenceType(reference, desc.ReferenceTypeID, desc.IncludeSubtypes))
        {
          if (Debug) std::cout << "Reference has wrong type." << std::endl;
          return false;
        }
        if (desc.NodeClasses && (desc.NodeClasses & static_cast<int32_t>(reference.TargetNodeClass)) == 0)
        {
          if (Debug) std::cout << "Reference has wrong class." << std::endl;
          return false;
        }
        if (Debug) std::cout << "Reference suitable." << std::endl;
        return true;
      }

      bool AddressSpaceInMemory::IsSuitableReferenceType(const ReferenceDescription& reference, const NodeID& typeID, bool includeSubtypes) const
      {
        if (!includeSubtypes)
        {
          return reference.ReferenceTypeID == typeID;
        }
        const std::vector<NodeID> suitableTypes = SelectNodesHierarchy(std::vector<NodeID>(1, typeID));
        const auto resultIt = std::find(suitableTypes.begin(), suitableTypes.end(), reference.ReferenceTypeID);\
        return resultIt != suitableTypes.end();
      }

      std::vector<NodeID> AddressSpaceInMemory::SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const
      {
        std::vector<NodeID> subNodes;
        for ( NodeID nodeid: sourceNodes )
        {
            NodesMap::const_iterator node_it = Nodes.find(nodeid);
            if ( node_it != Nodes.end() )
            {
              for (auto& ref:  node_it->second.References )
              {
                subNodes.push_back(ref.TargetNodeID);
            }
          }
        }
        if (subNodes.empty())
        {
          return sourceNodes;
        }

        const std::vector<NodeID> allChilds = SelectNodesHierarchy(subNodes);
        sourceNodes.insert(sourceNodes.end(), allChilds.begin(), allChilds.end());
        return sourceNodes;
      }

      AddNodesResult AddressSpaceInMemory::AddNode( const AddNodesItem& item )
      {
        AddNodesResult result;
        if (Debug) std::cout << "address_space| Adding new node id='" << item.RequestedNewNodeID << "' name=" << item.BrowseName.Name << std::endl;

        if (!Nodes.empty() && Nodes.find(item.RequestedNewNodeID) != Nodes.end())
        {
          std::cout << "Error: NodeID '"<< item.RequestedNewNodeID << "' allready exist: " << std::endl;
          result.Status = StatusCode::BadNodeIdExists;
          return result;
        }

        NodesMap::iterator parent_node_it = Nodes.end();
        if (item.ParentNodeId != NodeID())
        {
          parent_node_it = Nodes.find(item.ParentNodeId);
          if ( parent_node_it == Nodes.end() )
          {
            std::cout << "Error: Parent node '"<< item.ParentNodeId << "'does not exist" << std::endl;
            result.Status = StatusCode::BadParentNodeIdInvalid;
            return result;
          }
        }

        NodeStruct nodestruct;
        //Add Common attributes
        AttributeValue attv;
        attv.Value = item.RequestedNewNodeID;
        nodestruct.Attributes[AttributeID::NODE_ID] = attv;
        attv.Value = item.BrowseName;
        nodestruct.Attributes[AttributeID::BROWSE_NAME] = attv;
        attv.Value = static_cast<int32_t>(item.Class);
        nodestruct.Attributes[AttributeID::NODE_CLASS] = attv;

        // Add requested attributes
        for (const auto& attr: item.Attributes.Attributes)
        {
          AttributeValue attval;
          attval.Value = attr.second;

          nodestruct.Attributes.insert(std::make_pair(attr.first, attval));
        }
        Nodes.insert(std::make_pair(item.RequestedNewNodeID, nodestruct));


        if (parent_node_it != Nodes.end())
        {
          // Link to parent
          ReferenceDescription desc;
          desc.ReferenceTypeID = item.ReferenceTypeId;
          desc.TargetNodeID = item.RequestedNewNodeID;
          desc.TargetNodeClass = item.Class;
          desc.BrowseName = item.BrowseName;
          desc.DisplayName = LocalizedText(item.BrowseName.Name);
          desc.TargetNodeTypeDefinition = item.TypeDefinition;
          desc.IsForward = true; // should this be in constructor?

          parent_node_it->second.References.push_back(desc);
        }

        if (item.TypeDefinition != ObjectID::Null)
        {
          // Link to parent
          AddReferencesItem typeRef;
          typeRef.SourceNodeID = item.RequestedNewNodeID;
          typeRef.IsForward = true;
          typeRef.ReferenceTypeId = ObjectID::HasTypeDefinition;
          typeRef.TargetNodeID = item.TypeDefinition;
          typeRef.TargetNodeClass = NodeClass::DataType;
          AddReference(typeRef);
        }

        result.Status = StatusCode::Good;
        result.AddedNodeID = item.RequestedNewNodeID;
        if (Debug) std::cout << "address_space| node added." << std::endl;
        return result;
      }

      StatusCode AddressSpaceInMemory::AddReference(const AddReferencesItem& item)
      {
        NodesMap::iterator node_it = Nodes.find(item.SourceNodeID);
        if ( node_it == Nodes.end() )
        {
          return StatusCode::BadSourceNodeIdInvalid;
        }
        ReferenceDescription desc;
        desc.ReferenceTypeID = item.ReferenceTypeId;
        desc.IsForward = item.IsForward;
        desc.TargetNodeID = item.TargetNodeID;
        desc.TargetNodeClass = item.TargetNodeClass;
        // FIXME: these fields have to be filled from address space dynamically.
        // FIXME: note! Target node ID can be absent according standard.
        desc.BrowseName = QualifiedName(GetObjectIdName(item.TargetNodeID));
        desc.DisplayName = LocalizedText(GetObjectIdName(item.TargetNodeID));
        node_it->second.References.push_back(desc);
        return StatusCode::Good;
      }

  }

  namespace UaServer
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug)
    {
      return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory(debug));
    }
  }


}
