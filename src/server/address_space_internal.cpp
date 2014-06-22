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

#include <opc/ua/attributes.h>
#include <opc/ua/node_management.h>
#include <opc/ua/protocol/subscriptions.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/view.h>

#include <boost/thread/shared_mutex.hpp>
#include <ctime>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <deque>
#include <set>



namespace OpcUa
{
  namespace Internal
  {

    using namespace OpcUa::Remote;

    //Structure to store in memory description of current MonitoredItems
    struct DataMonitoredItems
    {
      IntegerID SubscriptionID;
      MonitoringMode Mode;
      time_t LastTrigger;
      CreateMonitoredItemsResult Parameters;
    };

    //Structure to store in memory description of current Subscriptions
    struct DataSubscription
    {
      SubscriptionData Data;
      uint32_t NotificationSequence = 1; //NotificationSequence start at 1! not 0
      uint32_t KeepAliveCount = std::numeric_limits<uint32_t>::max(); //High at startup in order to force the message, required by spec
      time_t lastNotificationTime = 0;
      uint32_t LastMonitoredItemID = 2;
      std::map <uint32_t, DataMonitoredItems> MonitoredItemsMap; //Map MonitoredItemID, DataMonitoredItems
      std::list<PublishResult> NotAcknowledgedResults; //result that have not be acknowledeged and may have to resent
      std::list<MonitoredItems> MonitoredItemsTriggered; 
      std::list<EventFieldList> EventTriggered; 

      std::vector<PublishResult> PopPublishResult()
      {
        std::cout << "PopPublishresult for subscription: " << Data.ID << " with " << MonitoredItemsTriggered.size() << " triggered items in queue" << std::endl;
        std::vector<PublishResult> resultlist;
        PublishResult result;
        result.SubscriptionID = Data.ID;
        result.Message.PublishTime = CurrentDateTime();

        DataChangeNotification notification;
        for ( const MonitoredItems& monitoreditem: MonitoredItemsTriggered)
        {
          notification.Notification.push_back(monitoreditem);
        }
        if (notification.Notification.size() > 0) 
        {
          NotificationData data(notification);
          result.Message.Data.push_back(data);
          result.Statuses.push_back(StatusCode::Good);
        }
        MonitoredItemsTriggered.clear();
        
        // FIXME: parse events and statuschange notification since they can be send in same result

        if ( result.Statuses.size() == 0 && ( KeepAliveCount < Data.RevizedMaxKeepAliveCount ) ) 
        {
          std::cout << "No event and not need to send keep-alive notification" << std::endl;
          ++KeepAliveCount;
          return  resultlist; //No event and we do not need to send keepalive notification yet so return empty list
        }
        KeepAliveCount = 0;
        result.Message.SequenceID = NotificationSequence;
        ++NotificationSequence;
        result.MoreNotifications = false;
        for (const PublishResult& res: NotAcknowledgedResults)
        {
          result.AvailableSequenceNumber.push_back(res.Message.SequenceID);
        }
        NotAcknowledgedResults.push_back(result);
        std::cout << "Sending Notification with " << result.Message.Data.size() << " notifications"  << std::endl;
        resultlist.push_back(result);
        return resultlist;
      };
    };

    typedef std::map <IntegerID, DataSubscription> SubscriptionsIDMap; // Map SubscptioinID, SubscriptionData
    
    //store subscription for one attribute
    struct AttSubscription
    {
      IntegerID SubscriptionId;
      uint32_t MonitoredItemId;
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

      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID> subscriptions)
      {
        std::vector<StatusCode> result;
        for (const IntegerID& subid: subscriptions)
        {
          std::cout << "Delete Subscription: " << subid << std::endl;
          size_t count = SubscriptionsMap.erase(subid);
          if ( count == 1)
          {
            result.push_back(StatusCode::Good);
          }
          else
          {
            result.push_back(StatusCode::BadAttributeIdInvalid);
          }
        }
        return result;
      }

      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& params)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        SubscriptionData data;
        data.ID = ++LastSubscriptionID;
        data.RevisedLifetimeCount = params.RequestedLifetimeCount;
        data.RevisedPublishingInterval = params.RequestedPublishingInterval;
        data.RevizedMaxKeepAliveCount = params.RequestedMaxKeepAliveCount;
        std::cout << "Creating Subscription with ID: " << data.ID << std::endl;

        DataSubscription db;
        db.Data = data;
        SubscriptionsMap[data.ID] = db;
        return data;
      }

      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& params)
      {
        MonitoredItemsData data;

        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(params.SubscriptionID);
        if ( itsub == SubscriptionsMap.end()) //SubscriptionID does not exist, return errors for all items
        {
          for (int j=0; j<(int)params.ItemsToCreate.size(); j++)
          {
            CreateMonitoredItemsResult res;
            res.Status = StatusCode::BadAttributeIdInvalid;
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
      
      // Deprecated method, remove when not called anymore
      virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        NodesMap::iterator node_it = Nodes.find(node);
        if ( node_it == Nodes.end() )
        {
          if ( attribute == AttributeID::NODE_ID ) // create node
          {
            NodeStruct ns;
            Nodes[node] = ns;
            node_it =  Nodes.find(node);//should always be good since we just created it
          }
          else
          {
            std::cerr << "Error trying to sett attribute to non existing node: " << node << std::endl; 
            return;
          }
        }

        AttributeValue attrval;
        attrval.Value.Encoding = DATA_VALUE;
        attrval.Value = value;
        node_it->second.Attributes[attribute] = attrval;
      }

      // Deprecated method, remove when not called anymore
      virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        NodesMap::iterator node_it = Nodes.find(sourceNode);
        if ( node_it != Nodes.end() )
        {
          node_it->second.References.push_back(reference);
        }
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

        std::vector<ReferenceDescription> result;
        for ( BrowseDescription browseDescription: query.NodesToBrowse)
        {
          NodesMap::const_iterator node_it = Nodes.find(browseDescription.NodeToBrowse);
          if ( node_it != Nodes.end() )
          {
            for (auto reference : node_it->second.References)
            {
              if (IsSuitableReference(browseDescription, reference))
              {
                result.push_back(reference);
              }
            }
          }
          break; //FIXME: It looks like we only support browsing one node at a time!!!
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
          std::map <IntegerID, DataSubscription>::iterator sub_it =  SubscriptionsMap.find(subscription); 
          {
            if ( sub_it != SubscriptionsMap.end() )
            {
              for ( const PublishResult& res: sub_it->second.PopPublishResult() )
              {
                result.push_back(res);
              }
            }
          }
        }
        return result;
      }

      virtual void CreatePublishRequest(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        for (SubscriptionAcknowledgement ack: acknowledgements)
        {
          SubscriptionsIDMap::iterator sub_it = SubscriptionsMap.find(ack.SubscriptionID);
          if ( sub_it != SubscriptionsMap.end())
          {
            sub_it->second.NotAcknowledgedResults.remove_if([&](PublishResult res){ return ack.SequenceNumber == res.Message.SequenceID; });
          }
        }
      }


    private:


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
            result.Status = OpcUa::StatusCode::BadNotReadable;
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
        CreateMonitoredItemsResult res;

        NodesMap::iterator node_it = Nodes.find(request.ItemToMonitor.Node);
        if ( node_it == Nodes.end() )
        {
          res.Status = OpcUa::StatusCode::BadAttributeIdInvalid;
          std::cout << "NodeID does not exist: " << request.ItemToMonitor.Node << std::endl;
          return res;
        }

        AttributesMap::iterator attrit = node_it->second.Attributes.find(request.ItemToMonitor.Attribute);
        if ( attrit == node_it->second.Attributes.end() )
        {
          res.Status = OpcUa::StatusCode::BadAttributeIdInvalid;
          std::cout << "attribute not found" << std::endl;
          return res;
        }

        res.Status = OpcUa::StatusCode::Good;
        subscription_it->second.LastMonitoredItemID += 1;
        res.MonitoredItemID = subscription_it->second.LastMonitoredItemID ;
        res.RevisedSamplingInterval = subscription_it->second.Data.RevisedPublishingInterval;
        res.RevizedQueueSize = request.Parameters.QueueSize; // We should check that value, maybe set to a default...
        //res.FilterResult = //We can omit that one if we do not change anything in filter
        DataMonitoredItems mdata;
        mdata.SubscriptionID = subscription_it->first;
        mdata.Parameters = res;
        mdata.Mode = request.Mode;
        subscription_it->second.MonitoredItemsMap[res.MonitoredItemID] = mdata;
        AttSubscription attsub;
        std::cout << "creating monitored item with sub: " << subscription_it->first << std::endl;
        attsub.SubscriptionId = subscription_it->first;
        attsub.MonitoredItemId = res.MonitoredItemID;
        attsub.Parameters = request.Parameters;
        attrit->second.AttSubscriptions.push_back(attsub); 
        //data.Diagnostics =  Not necessary
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
        for (auto attsub = val.AttSubscriptions.begin(); attsub !=val.AttSubscriptions.end() ;)
        {
          SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(attsub->SubscriptionId);
          if ( itsub == SubscriptionsMap.end() || (itsub->second.MonitoredItemsMap.find( attsub->MonitoredItemId ) == itsub->second.MonitoredItemsMap.end() ) )
          {
            attsub = val.AttSubscriptions.erase(attsub);
            continue;
          }

          MonitoredItems event;
          event.ClientHandle = attsub->Parameters.ClientHandle;
          event.Value = val.Value;
          std::cout << "Adding triggered item for sub: " << attsub->SubscriptionId << " and clienthandle: " << attsub->Parameters.ClientHandle << std::endl;
          itsub->second.MonitoredItemsTriggered.push_back(event);

          ++attsub;
        }
      }

      bool IsSuitableReference(const BrowseDescription& desc, const ReferenceDescription& reference) const
      {
        if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
        {
          return false;
        }
        if (desc.ReferenceTypeID != ObjectID::Null && !IsSuitableReferenceType(reference, desc.ReferenceTypeID, desc.IncludeSubtypes))
        {
          return false;
        }
        if (desc.NodeClasses && (desc.NodeClasses & static_cast<int32_t>(reference.TargetNodeClass)) == 0)
        {
          return false;
        }
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

        NodesMap::iterator node_it = Nodes.find(item.RequestedNewNodeID);
        if ( node_it != Nodes.end() )
        {
          std::cout << "Error: NodeID allready exist" << std::endl;
          result.Status = StatusCode::BadAttributeIdInvalid;
          return result;
        }

        NodesMap::iterator parent_node_it = Nodes.find(item.ParentNodeId);
        if ( parent_node_it == Nodes.end() )
        {
          std::cout << "Error: Parent node does not exist" << std::endl;
          result.Status = StatusCode::BadAttributeIdInvalid; //FiXME return correct error type
          return result;
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

        result.Status = StatusCode::Good;
        result.AddedNodeID = item.RequestedNewNodeID;
        return result;
      }

      StatusCode AddReference(const AddReferencesItem& item)
      {
        NodesMap::iterator node_it = Nodes.find(item.SourceNodeID);
        if ( node_it == Nodes.end() )
        {
          return StatusCode::BadAttributeIdInvalid;
        }
        ReferenceDescription desc;
        desc.ReferenceTypeID = item.ReferenceTypeId;
        desc.IsForward = item.IsForward;
        desc.TargetNodeID = item.TargetNodeID;
        desc.TargetNodeClass = item.TargetNodeClass;
        desc.BrowseName = QualifiedName(GetObjectIdName(item.TargetNodeID));
        desc.DisplayName = LocalizedText(GetObjectIdName(item.TargetNodeID));
        node_it->second.References.push_back(desc);
        return StatusCode::Good;
      }

    private:
      mutable boost::shared_mutex DbMutex;
      NodesMap Nodes;
      SubscriptionsIDMap SubscriptionsMap; // Map SubscptioinID, SubscriptionData
      uint32_t LastSubscriptionID = 2;
    };
  }

  namespace UaServer
  {
    AddressSpace::UniquePtr CreateAddressSpace()
    {
      return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory());
    }
  }
}
