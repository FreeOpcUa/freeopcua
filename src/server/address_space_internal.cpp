/// @author Alexander Rykovanov 2013-2014
/// @author Olivier Roulet-dubonnet 2014
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

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
      AddressSpaceInMemory(bool debug)
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
        rootNode.Attributes = attrs;
        AddNode(rootNode);
      }

     ~AddressSpaceInMemory()
      {
        if (Debug) std::cout << "address_space_in_memory| Stopping boost io service." << std::endl;
        io.stop();
        if (Debug) std::cout << "address_space_in_memory| Joining serivice thread." << std::endl;
        service_thread.join();
      }


      void DeleteAllSubscriptions()
      {
        if (Debug) std::cout << "Deleting all subscriptions." << std::endl;
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<IntegerID> ids(SubscriptionsMap.size());\
        std::transform(SubscriptionsMap.begin(), SubscriptionsMap.end(), ids.begin(), [](const SubscriptionsIDMap::value_type& i){return i.first;});
        DeleteSubscriptions(ids);
      }

  /*    
      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
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

      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
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

      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& params, std::function<void (PublishResult)> callback)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        SubscriptionData data;
        data.ID = ++LastSubscriptionID;
        data.RevisedLifetimeCount = params.RequestedLifetimeCount;
        data.RevisedPublishingInterval = params.RequestedPublishingInterval;
        data.RevizedMaxKeepAliveCount = params.RequestedMaxKeepAliveCount;
        std::cout << "Creating Subscription with ID: " << data.ID << std::endl;

        std::shared_ptr<InternalSubscription> sub(new InternalSubscription(data, io, callback));
        SubscriptionsMap[data.ID] = sub;
        return data;
      }

      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& params)
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

      virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters params)
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



      virtual std::vector<AddNodesResult> AddNodes(const std::vector<AddNodesItem>& items)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<AddNodesResult> results;
        for (const AddNodesItem& item: items)
        {
          results.push_back(AddNode(item));
        }
        return results;
      }

      virtual std::vector<StatusCode> AddReferences(const std::vector<AddReferencesItem>& items)
      {
        std::vector<StatusCode> results;
        for (const auto& item : items)
        {
          results.push_back(AddReference(item));
        }
        return results;
      }

      virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
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

      virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
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

      virtual std::vector<ReferenceDescription> BrowseNext() const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        return std::vector<ReferenceDescription>();
      }

      virtual std::vector<DataValue> Read(const ReadParameters& params) const
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<DataValue> values;
        for (const AttributeValueID& attribute : params.AttributesToRead)
        {
          values.push_back(GetValue(attribute.Node, attribute.Attribute));
        }
        return values;
      }

      virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values)
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

      std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIds)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<PublishResult> result;
        for (const IntegerID& subscription: subscriptionsIds)
        {
          if ( PublishRequestsQueue == 0)
          {
            break;
          }
          SubscriptionsIDMap::iterator sub_it =  SubscriptionsMap.find(subscription); 
          {
            if ( sub_it != SubscriptionsMap.end() )
            {
              for ( const PublishResult& res: sub_it->second->PopPublishResult() )
              {
                result.push_back(res);
              }
            }
          }
        }
        return result;
      }

      virtual void Publish(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);
        
        if ( PublishRequestsQueue > 10000 ) //FIXME: It this number defined by spec?
        {
          return; //FIXME: spec says we should return error to warn client
        }
        ++ PublishRequestsQueue; 

        for (SubscriptionAcknowledgement ack: acknowledgements)
        {
          SubscriptionsIDMap::iterator sub_it = SubscriptionsMap.find(ack.SubscriptionID);
          if ( sub_it != SubscriptionsMap.end())
          {
            sub_it->second->NewAcknowlegment(ack); 
          }
        }
      }

      void TriggerEvent(NodeID node, Event event)
      {
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
            std::cout << "attempt o trigger node which has not event_notifier attribute, should raise exception\n";
          }
          else
          {
            EnqueueEvent(ait->second, event);
          }
        }
      }

    private:


      void EnqueueEvent(AttributeValue& attval, const Event& event)
      {
        //go through all subscription of attribute and add new event
        for (auto attsub = attval.AttSubscriptions.begin(); attsub != attval.AttSubscriptions.end() ;)
        {
          SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(attsub->SubscriptionId);
          if ( itsub == SubscriptionsMap.end() )
          {
            attsub = attval.AttSubscriptions.erase(attsub);
          }
          bool res = itsub->second->EnqueueEvent(attsub->MonitoredItemId, event);
          if ( ! res )
          {
            attsub = attval.AttSubscriptions.erase(attsub);
          }
          ++attsub;
        }
      }


      std::tuple<bool, NodeID> FindElementInNode(const NodeID& nodeid, const RelativePathElement& element) const
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

      BrowsePathResult TranslateBrowsePath(const BrowsePath& browsepath) const
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


      CreateMonitoredItemsResult CreateMonitoredItem( SubscriptionsIDMap::iterator& subscription_it,  const MonitoredItemRequest& request)
      {
        std::cout << "Creating monitored request for one item" << std::endl;

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
      
        std::cout << "creating monitored item with sub: " << subscription_it->first << std::endl;
        CreateMonitoredItemsResult res = subscription_it->second->AddMonitoredItem(request);

        AttSubscription attsub;
        attsub.SubscriptionId = subscription_it->first;
        attsub.MonitoredItemId = res.MonitoredItemID;
        attsub.Parameters = request.Parameters;
        attrit->second.AttSubscriptions.push_back(attsub); 

        return res;
      }

      DataValue GetValue(const NodeID& node, AttributeID attribute) const
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

      StatusCode SetValue(const NodeID& node, AttributeID attribute, const Variant& data)
      {
        NodesMap::iterator it = Nodes.find(node);
        if ( it != Nodes.end() )
        {
          AttributesMap::iterator ait = it->second.Attributes.find(attribute);
          if ( ait != it->second.Attributes.end() )
          {
            ait->second.Value = data;
            UpdateSubscriptions(ait->second);
            return StatusCode::Good;
          }
        }
        return StatusCode::BadAttributeIdInvalid;
      }

      void UpdateSubscriptions(AttributeValue& val)
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

      bool IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const
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

      bool IsSuitableReferenceType(const ReferenceDescription& reference, const NodeID& typeID, bool includeSubtypes) const
      {
        if (!includeSubtypes)
        {
          return reference.ReferenceTypeID == typeID;
        }
        const std::vector<NodeID> suitableTypes = SelectNodesHierarchy(std::vector<NodeID>(1, typeID));
        const auto resultIt = std::find(suitableTypes.begin(), suitableTypes.end(), reference.ReferenceTypeID);\
        return resultIt != suitableTypes.end();
      }

      std::vector<NodeID> SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const
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

      AddNodesResult AddNode( const AddNodesItem& item )
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
          typeRef.TargetNodeClass = NodeClass::DataType; // FIXME: Take from node with ID item.TypeDefinition
          AddReference(typeRef);
        }

        result.Status = StatusCode::Good;
        result.AddedNodeID = item.RequestedNewNodeID;
        if (Debug) std::cout << "address_space| node added." << std::endl;
        return result;
      }

      StatusCode AddReference(const AddReferencesItem& item)
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

    private:
      bool Debug = false;
      mutable boost::shared_mutex DbMutex;
      NodesMap Nodes;
      SubscriptionsIDMap SubscriptionsMap; // Map SubscptioinID, SubscriptionData
      uint32_t LastSubscriptionID = 2;
      uint32_t PublishRequestsQueue = 0;
      boost::asio::io_service io;
      std::shared_ptr<boost::asio::io_service::work> work; //work object prevent worker thread to exist even whenre there are no subsciptions
      std::thread service_thread;

    };
  }

  namespace UaServer
  {
    AddressSpace::UniquePtr CreateAddressSpace(bool debug)
    {
      return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory(debug));
    }
  }
}
