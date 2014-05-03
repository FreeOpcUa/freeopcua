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
#include "opc/ua/server/subscriptions_server.h"

#include <boost/thread/shared_mutex.hpp>
#include <map>
#include <set>
#include <ctime>
#include <list>



namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Remote;

  typedef std::multimap<NodeID, ReferenceDescription> ReferenciesMap;



  struct AttributeValue
  {
    NodeID Node;
    AttributeID Attribute;
    DataValue Value;
    std::list<uint32_t> AttSubscriptions;
    //std::vector<MonitoredItemData> AttSubscriptions;
    //std::map<IntegerID, std::vector<uint32_t>> AttSubscriptions; //A map SubscriptionID, MonitoredItemID

    //AttributeValue();
  };

  class ServicesRegistry : public Internal::AddressSpaceMultiplexor
  {
  public:

    virtual SubscriptionData CreateSubscription(const SubscriptionParameters& params)
    {
      SubscriptionData data;
      LastSubscriptionID += 1;
      data.ID = LastSubscriptionID;
      data.RevisedLifetimeCount = params.RequestedLifetimeCount;
      data.RevisedPublishingInterval = params.RequestedPublishingInterval;
      data.RevizedMaxKeepAliveCount = params.RequestedMaxKeepAliveCount;
      Subscriptions[data.ID] = data;
      return data;
    }

    virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& params)
    {
      MonitoredItemsData data;
      for (const MonitoredItemRequest& req: params.ItemsToCreate)
      {
        std::cout << "Creating monitored request for one item" << std::endl;
        CreateMonitoredItemsResult res;
        bool found = false;
        for (AttributeValue value : AttributeValues)
        {
          if (value.Node == req.ItemToMonitor.Node && value.Attribute == req.ItemToMonitor.Attribute)
          { //We found attribute //FIXME: what to do if item allready exist?
              res.Status = OpcUa::StatusCode::Good;
              LastMonitoredItemID += 1;
              res.MonitoredItemID = LastMonitoredItemID;
              res.RevisedSamplingInterval = Subscriptions[params.SubscriptionID].RevisedPublishingInterval;
              res.RevizedQueueSize = req.Parameters.QueueSize; // We should check that value, maybe set to a default...
              //res.FilterResult = //We can omit that one if we do not change anything in filter
              MonitoredItemData mdata;
              mdata.SubscriptionID = params.SubscriptionID;
              mdata.Parameters = res;
              mdata.Mode = req.Mode;
              MonitoredItems[res.MonitoredItemID] = mdata;
              value.AttSubscriptions.push_back(res.MonitoredItemID); 
              found = true;
              std::cout << "monitored item fore one item created " << std::endl;
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

    virtual void AddAttribute(const NodeID& node, AttributeID attribute, const Variant& value)
    {
      boost::unique_lock<boost::shared_mutex> lock(_dbmutex);
      //std::unique_lock<std::mutex> lock(DBMutex);

      AttributeValue data;
      data.Node = node;
      data.Attribute = attribute;
      data.Value.Encoding = DATA_VALUE;
      data.Value.Value = value;

      AttributeValues.push_back(data);
    }

    virtual void AddReference(const NodeID& sourceNode, const ReferenceDescription& reference)
    {
      boost::unique_lock<boost::shared_mutex> lock(_dbmutex);

      Referencies.insert({sourceNode, reference});
    }

    virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(_dbmutex);

      std::vector<BrowsePathResult> results;
      NodeID current;
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
          if (! found ) { break;}
        }
        BrowsePathResult res;
        if ( !found) {
          res.Status = OpcUa::StatusCode::BadNotReadable;
        }
        else
        {
          res.Status = OpcUa::StatusCode::Good;
          std::vector<BrowsePathTarget> targets;
          BrowsePathTarget target;
          target.Node = current;
          target.RemainingPathIndex = UINT32_MAX;
          targets.push_back(target);
          res.Targets = targets;
        }
        results.push_back(res);
        }
      return results;
    }

    virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
    {
      boost::shared_lock<boost::shared_mutex> lock(_dbmutex);
      //std::unique_lock<std::mutex> lock(DBMutex);

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
      boost::shared_lock<boost::shared_mutex> lock(_dbmutex);
      //std::unique_lock<std::mutex> lock(DBMutex);

      return std::vector<ReferenceDescription>();
    }

    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      boost::shared_lock<boost::shared_mutex> lock(_dbmutex);
      //std::unique_lock<std::mutex> lock(DBMutex);

      std::vector<DataValue> values;
      for (const AttributeValueID& attribute : params.AttributesToRead)
      {
        values.push_back(GetValue(attribute.Node, attribute.Attribute));
      }
      return values;
    }

    virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values)
    {
      boost::unique_lock<boost::shared_mutex> lock(_dbmutex);
      //std::unique_lock<std::mutex> lock(DBMutex);

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

    std::vector<MonitoredItemData> PopItemsToPublish(const std::vector<IntegerID>& subscriptions)
    {
      std::vector<MonitoredItemData> result;
      std::vector<uint32_t> published;
      for ( uint32_t miid: EventsToFire)
      {
        if (MonitoredItems.find( miid ) != MonitoredItems.end() )
        {
          for (const IntegerID& subID: subscriptions )
          {
            if  ( MonitoredItems[miid].SubscriptionID == subID )  
            {
              time_t now = std::time(0);
              if ( ( now - MonitoredItems[miid].LastTrigger ) >= MonitoredItems[miid].Parameters.RevisedSamplingInterval )
              {
                MonitoredItems[miid].LastTrigger = now;
                result.push_back(MonitoredItems[miid]);
                published.push_back(miid);
              }
            }
          }
        }
      }
      for (uint32_t miid: published)
      {
        EventsToFire.remove(miid);
      }
      return result;
    }


  private:

    DataValue GetValue(const NodeID& node, AttributeID attribute) const
    {
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


    void UpdateSubscriptions(AttributeValue val)
    {
      std::vector<uint32_t> toremove;
      for (uint32_t miid : val.AttSubscriptions)
      {
        if (MonitoredItems.find( miid ) != MonitoredItems.end() )
        {
          EventsToFire.push_back(miid);
        }
        else
        {
          toremove.push_back(miid);
        }
      }
      for (uint32_t miid: toremove)
      {
        val.AttSubscriptions.remove(miid);
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
      for (const NodeID& id : suitableTypes)
      {
        if (id == reference.ReferenceTypeID)
        {
          return true;
        }
      }
      return false;
//      return std::find(suitableTypes.begin(), suitableTypes.end(), reference.ReferenceTypeID) != suitableTypes.end();
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
    //mutable std::mutex DBMutex;
    mutable boost::shared_mutex _dbmutex;
    ReferenciesMap Referencies;
    std::vector<AttributeValue> AttributeValues;
    std::map <IntegerID, SubscriptionData> Subscriptions; // Map SubscptioinID, SubscriptionData
    std::map <uint32_t, MonitoredItemData> MonitoredItems; //Map MonitoredItemID, MonitoredItemData
    uint32_t LastSubscriptionID = 2;
    uint32_t LastMonitoredItemID = 2;
    //std::vector<MonitoredItemData> EventsToFire; // Map SubscptioinID, EventToFire
    std::list<uint32_t> EventsToFire; 
  };

}

namespace OpcUa
{
  Internal::AddressSpaceMultiplexor::UniquePtr Internal::CreateAddressSpaceMultiplexor()
  {
    return AddressSpaceMultiplexor::UniquePtr(new ServicesRegistry());
  }
}
