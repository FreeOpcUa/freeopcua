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
  };

  class AddressSpaceInMemory : public UaServer::AddressSpace
  {
  public:
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
          return StatusCode::Good;
        }
      }
      return StatusCode::BadAttributeIdInvalid;
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
  };

}

namespace OpcUa
{
  UaServer::AddressSpace::UniquePtr Internal::CreateAddressSpaceInMemory()
  {
    return UaServer::AddressSpace::UniquePtr(new AddressSpaceInMemory());
  }
}
