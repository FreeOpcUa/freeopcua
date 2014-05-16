/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "address_space_internal.h"

#include <boost/thread/shared_mutex.hpp>
#include <limits>
#include <map>
#include <set>
#include <ctime>
#include <list>
#include <queue>
#include <deque>



namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Remote;

  typedef std::multimap<NodeID, ReferenceDescription> ReferenciesMap;

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

  struct AttSubscription
  {
    IntegerID SubscriptionId;
    uint32_t MonitoredItemId;
    MonitoringParameters Parameters;
  };

  struct AttributeValue
  {
    NodeID Node;
    AttributeID Attribute;
    DataValue Value;
    //List to keep track of monitoredItems monitoring this AttributeValue
    std::list<AttSubscription> AttSubscriptions; // a pair is subscirotionID, monitoredItemID
  };

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
      else
      {
        for (const MonitoredItemRequest& req: params.ItemsToCreate)
        {
          std::cout << "Creating monitored request for one item" << std::endl;
          CreateMonitoredItemsResult res;
          bool found = false;

          for ( AttributeValue& value : AttributeValues)
          {
            if (value.Node == req.ItemToMonitor.Node && value.Attribute == req.ItemToMonitor.Attribute)
            { //We found attribute //FIXME: what to do if item allready exist?
                res.Status = OpcUa::StatusCode::Good;
                itsub->second.LastMonitoredItemID += 1;
                res.MonitoredItemID = itsub->second.LastMonitoredItemID ;
                res.RevisedSamplingInterval = itsub->second.Data.RevisedPublishingInterval;
                res.RevizedQueueSize = req.Parameters.QueueSize; // We should check that value, maybe set to a default...
                //res.FilterResult = //We can omit that one if we do not change anything in filter
                DataMonitoredItems mdata;
                mdata.SubscriptionID = params.SubscriptionID;
                mdata.Parameters = res;
                mdata.Mode = req.Mode;
                itsub->second.MonitoredItemsMap[res.MonitoredItemID] = mdata;
                AttSubscription attsub;
                std::cout << "creating monitored item with sub: " << params.SubscriptionID  << std::endl;
                attsub.SubscriptionId = params.SubscriptionID;
                attsub.MonitoredItemId = res.MonitoredItemID;
                attsub.Parameters = req.Parameters;
                value.AttSubscriptions.push_back(attsub); 
                found = true;
                break;
            }
          }
          if ( ! found )
          {
            res.Status = OpcUa::StatusCode::BadAttributeIdInvalid;
            std::cout << "item not found" << std::endl;
          }
          //data.Diagnostics =  Not necessary
          data.Results.push_back(res);
        }
      return data;
      }
    }

    virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      AttributeValue data;
      data.Node = node;
      data.Attribute = attribute;
      data.Value.Encoding = DATA_VALUE;
      data.Value.Value = value;

      AttributeValues.push_back(data);
    }

    virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      Referencies.insert({sourceNode, reference});
    }

    virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<BrowsePathResult> results;
      NodeID current;
      // TODO: Rewrite after adding unit tests - too much loops and ifs.
      for (BrowsePath browsepath : params.BrowsePaths )
      {
        current = browsepath.StartingNode;
        bool found = false;
        for (RelativePathElement element : browsepath.Path.Elements)
        {
          found = false;
          for (auto reference : Referencies)
          {
            //if (reference.first == current) { std::cout <<   reference.second.BrowseName.NamespaceIndex << reference.second.BrowseName.Name << " to " << element.TargetName.NamespaceIndex << element.TargetName.Name <<std::endl; }
            if (reference.first == current && reference.second.BrowseName == element.TargetName)
            {
              current = reference.second.TargetNodeID;
              found = true;
              break;
            }
          }
          if (!found )
          {
            break;
          }
        }
        BrowsePathResult res;
        if (!found)
        {
          res.Status = OpcUa::StatusCode::BadNotReadable;
        }
        else
        {
          res.Status = OpcUa::StatusCode::Good;
          std::vector<BrowsePathTarget> targets;
          BrowsePathTarget target;
          target.Node = current;
          target.RemainingPathIndex = std::numeric_limits<uint32_t>::max();
          targets.push_back(target);
          res.Targets = targets;
        }
        results.push_back(res);
        }
      return results;
    }

    virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
    {
      boost::shared_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<ReferenceDescription> result;
      for (auto reference : Referencies)
      {
        for (auto browseDescription : query.NodesToBrowse)
        {
          if (IsSuitableReference(browseDescription, reference))
          {
            result.push_back(reference.second);
          }
        }
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
        std::map <IntegerID, DataSubscription>::iterator it =  SubscriptionsMap.find(subscription); 
        {
          if ( it != SubscriptionsMap.end() )
          {
            for ( const PublishResult& res: SubscriptionsMap[subscription].PopPublishResult() )
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
        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(ack.SubscriptionID);
        if ( itsub != SubscriptionsMap.end())
        {
          itsub->second.NotAcknowledgedResults.remove_if([&](PublishResult res){ return ack.SequenceNumber == res.Message.SequenceID; });
        }
      }
    }


  private:

    DataValue GetValue(const NodeID& node, AttributeID attribute) const
    {
      // TODO: Copy-past at if.
      for (const AttributeValue& value : AttributeValues)
      {
        if (value.Node == node && value.Attribute == attribute)
        {
          return value.Value;
        }
      }
      DataValue value;
      value.Encoding = DATA_VALUE_STATUS_CODE;
      value.Status = StatusCode::BadNotReadable;
      return value;
    }

    StatusCode SetValue(const NodeID& node, AttributeID attribute, const Variant& data)
    {
      for (AttributeValue& value : AttributeValues)
      {
        if (value.Node == node && value.Attribute == attribute)
        {
          value.Value = data;
          UpdateSubscriptions(value);
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
        if ( itsub == SubscriptionsMap.end() || (itsub->second.MonitoredItemsMap.find( attsub->MonitoredItemId ) == SubscriptionsMap[attsub->SubscriptionId].MonitoredItemsMap.end() ) )
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

    bool IsSuitableReference(const BrowseDescription& desc, const ReferenciesMap::value_type& refPair) const
    {
      const NodeID sourceNode = refPair.first;
      if (desc.NodeToBrowse != sourceNode)
      {
        return false;
      }
      const ReferenceDescription reference = refPair.second;
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
      for (ReferenciesMap::const_iterator refIt = Referencies.begin(); refIt != Referencies.end(); ++refIt)
      {

        for (const NodeID& id : sourceNodes)
        {
          if (id == refIt->first)
          {
            subNodes.push_back(refIt->second.TargetNodeID);
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

  private:
    mutable boost::shared_mutex DbMutex;
    ReferenciesMap Referencies;
    std::vector<AttributeValue> AttributeValues;
    SubscriptionsIDMap SubscriptionsMap; // Map SubscptioinID, SubscriptionData
    uint32_t LastSubscriptionID = 2;
  };

}

namespace OpcUa
{
  UaServer::AddressSpace::UniquePtr Internal::CreateAddressSpaceInMemory()
  {
    return UaServer::AddressSpace::UniquePtr(new AddressSpaceInMemory());
  }
}
