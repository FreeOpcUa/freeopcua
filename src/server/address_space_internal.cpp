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

AddressSpaceInMemory::AddressSpaceInMemory(const Common::Logger::SharedPtr & logger)
  : Logger(logger)
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

std::vector<AddNodesResult> AddressSpaceInMemory::AddNodes(const std::vector<AddNodesItem> & items)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<AddNodesResult> results;

  for (const AddNodesItem & item : items)
    {
      results.push_back(AddNode(item));
    }

  return results;
}

std::vector<StatusCode> AddressSpaceInMemory::AddReferences(const std::vector<AddReferencesItem> & items)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<StatusCode> results;

  for (const auto & item : items)
    {
      results.push_back(AddReference(item));
    }

  return results;
}

std::vector<BrowsePathResult> AddressSpaceInMemory::TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters & params) const
{
  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<BrowsePathResult> results;

  for (BrowsePath browsepath : params.BrowsePaths)
    {
      BrowsePathResult result = TranslateBrowsePath(browsepath);
      results.push_back(result);
    }

  return results;
}

std::vector<BrowseResult> AddressSpaceInMemory::Browse(const OpcUa::NodesQuery & query) const
{
  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  LOG_TRACE(Logger, "address_space_internal| browse");

  std::vector<BrowseResult> results;

  for (BrowseDescription browseDescription : query.NodesToBrowse)
    {
      BrowseResult result;

      if (Logger && Logger->should_log(spdlog::level::trace))
        {
          Logger->trace("address_space_internal| browsing");
          Logger->trace("  NodeId: '{}'", browseDescription.NodeToBrowse);
          Logger->trace("  ReferenceId: '{}'", browseDescription.ReferenceTypeId);
          Logger->trace("  Direction: {}", browseDescription.Direction);
          Logger->trace("  NodeClasses: {:#x}", (unsigned)browseDescription.NodeClasses);
          Logger->trace("  ResultMask:  {:#x}", (unsigned)browseDescription.ResultMask);
        }

      NodesMap::const_iterator node_it = Nodes.find(browseDescription.NodeToBrowse);

      if (node_it == Nodes.end())
        {
          LOG_WARN(Logger, "address_space_internal| Node '{}' not found in the address space", OpcUa::ToString(browseDescription.NodeToBrowse));

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

std::vector<NodeId> AddressSpaceInMemory::RegisterNodes(const std::vector<NodeId> & params) const
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  return params;
}

void AddressSpaceInMemory::UnregisterNodes(const std::vector<NodeId> & params) const
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  return;
}

std::vector<DataValue> AddressSpaceInMemory::Read(const ReadParameters & params) const
{
  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<DataValue> values;

  for (const ReadValueId & attribute : params.AttributesToRead)
    {
      values.push_back(GetValue(attribute.NodeId, attribute.AttributeId));
    }

  return values;
}

std::vector<StatusCode> AddressSpaceInMemory::Write(const std::vector<OpcUa::WriteValue> & values)
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

std::tuple<bool, NodeId> AddressSpaceInMemory::FindElementInNode(const NodeId & nodeid, const RelativePathElement & element) const
{
  NodesMap::const_iterator nodeit = Nodes.find(nodeid);

  if (nodeit != Nodes.end())
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

BrowsePathResult AddressSpaceInMemory::TranslateBrowsePath(const BrowsePath & browsepath) const
{
  NodeId current = browsepath.StartingNode;
  BrowsePathResult result;

  for (RelativePathElement element : browsepath.Path.Elements)
    {
      auto res = FindElementInNode(current, element);

      if (std::get<0>(res) == false)
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

DataValue AddressSpaceInMemory::GetValue(const NodeId & node, AttributeId attribute) const
{
  NodesMap::const_iterator nodeit = Nodes.find(node);

  if (nodeit == Nodes.end())
    {
//      LOG_DEBUG(Logger, "address_space_internal| node not found: {}", node);
    }

  else
    {
      AttributesMap::const_iterator attrit = nodeit->second.Attributes.find(attribute);

      if (attrit == nodeit->second.Attributes.end())
        {
//          LOG_DEBUG(Logger, "address_space_internal| node: {} has no attribute: {}", node, ToString(attribute));
        }

      else
        {
          if (attrit->second.GetValueCallback)
            {
              LOG_DEBUG(Logger, "address_space_internal| invoke registered callback");

              return attrit->second.GetValueCallback();
            }

//          LOG_TRACE(Logger, "address_space_internal| no callback registered, returning stored value");

          return attrit->second.Value;
        }
    }

  DataValue value;
  value.Encoding = DATA_VALUE_STATUS_CODE;
  value.Status = StatusCode::BadNotReadable;
  return value;
}

uint32_t AddressSpaceInMemory::AddDataChangeCallback(const NodeId & node, AttributeId attribute, std::function<Server::DataChangeCallback> callback)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  LOG_DEBUG(Logger, "address_space_internal| set data changes callback for node {} and attribute {}", node, (unsigned)attribute);

  NodesMap::iterator it = Nodes.find(node);

  if (it == Nodes.end())
    {
      LOG_ERROR(Logger, "address_space_internal| Node: '{}' not found", node);
      throw std::runtime_error("address_space_internal| NodeId not found");
    }

  AttributesMap::iterator ait = it->second.Attributes.find(attribute);

  if (ait == it->second.Attributes.end())
    {
      LOG_ERROR(Logger, "address_space_internal| Attribute: {} of node: ‘{}‘ not found", (unsigned)attribute, node);
      throw std::runtime_error("Attribute not found");
    }

  uint32_t handle = ++DataChangeCallbackHandle;
  DataChangeCallbackData data;
  data.Callback = callback;
  ait->second.DataChangeCallbacks[handle] = data;
  ClientIdToAttributeMap[handle] = NodeAttribute(node, attribute);
  return handle;
}

void AddressSpaceInMemory::DeleteDataChangeCallback(uint32_t serverhandle)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  LOG_DEBUG(Logger, "address_space_internal| deleting callback with client id: {}", serverhandle);

  ClientIdToAttributeMapType::iterator it = ClientIdToAttributeMap.find(serverhandle);

  if (it == ClientIdToAttributeMap.end())
    {
      LOG_WARN(Logger, "address_space_internal| request to delete a callback using unknown handle: {1}", serverhandle);
      return;
    }

  NodesMap::iterator nodeit = Nodes.find(it->second.Node);

  if (nodeit != Nodes.end())
    {
      AttributesMap::iterator ait = nodeit->second.Attributes.find(it->second.Attribute);

      if (ait != nodeit->second.Attributes.end())
        {
          size_t nb = ait->second.DataChangeCallbacks.erase(serverhandle);

          LOG_DEBUG(Logger, "address_space_internal| deleted {} callbacks", nb);

          ClientIdToAttributeMap.erase(serverhandle);
          return;
        }
    }

  throw std::runtime_error("address_space_internal| NodeId or attribute nor found");
}

StatusCode AddressSpaceInMemory::SetValueCallback(const NodeId & node, AttributeId attribute, std::function<DataValue(void)> callback)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  NodesMap::iterator it = Nodes.find(node);

  if (it != Nodes.end())
    {
      AttributesMap::iterator ait = it->second.Attributes.find(attribute);

      if (ait != it->second.Attributes.end())
        {
          ait->second.GetValueCallback = callback;
          return StatusCode::Good;
        }
    }

  return StatusCode::BadAttributeIdInvalid;
}

void AddressSpaceInMemory::SetMethod(const NodeId & node, std::function<std::vector<OpcUa::Variant> (NodeId context, std::vector<OpcUa::Variant> arguments)> callback)
{
  boost::unique_lock<boost::shared_mutex> lock(DbMutex);

  NodesMap::iterator it = Nodes.find(node);

  if (it != Nodes.end())
    {
      it->second.Method = callback;
    }

  else
    {
      throw std::runtime_error("address_space_internal| while setting node callback: node does not exist.");
    }
}

std::vector<OpcUa::CallMethodResult> AddressSpaceInMemory::Call(const std::vector<OpcUa::CallMethodRequest> & methodsToCall)
{
  boost::shared_lock<boost::shared_mutex> lock(DbMutex);

  std::vector<OpcUa::CallMethodResult>  results;

  for (auto method : methodsToCall)
    {
      results.push_back(CallMethod(method));
    }

  return results;
}

CallMethodResult AddressSpaceInMemory::CallMethod(CallMethodRequest request)
{
  CallMethodResult result;
  NodesMap::iterator node_it = Nodes.find(request.ObjectId);

  if (node_it == Nodes.end())
    {
      result.Status = StatusCode::BadNodeIdUnknown;
      return result;
    }

  NodesMap::iterator method_it = Nodes.find(request.MethodId);

  if (method_it == Nodes.end())
    {
      result.Status = StatusCode::BadNodeIdUnknown;
      return result;
    }

  if (! method_it->second.Method)
    {
      result.Status = StatusCode::BadNothingToDo;
      return result;
    }

  //FIXME: find a way to return more information about failure to client
  try
    {
      result.OutputArguments = method_it->second.Method(node_it->first, request.InputArguments);
    }

  catch (std::exception & ex)
    {
      LOG_ERROR(Logger, "address_space_internal| exception while calling method: {}: {}", request.MethodId, ex.what());
      result.Status = StatusCode::BadUnexpectedError;
      return result;
    }

  for (auto var : request.InputArguments)
    {
      result.InputArgumentResults.push_back(StatusCode::Good);
    }

  result.Status = StatusCode::Good;
  return result;
}

StatusCode AddressSpaceInMemory::SetValue(const NodeId & node, AttributeId attribute, const DataValue & data)
{
  NodesMap::iterator it = Nodes.find(node);

  if (it != Nodes.end())
    {
      AttributesMap::iterator ait = it->second.Attributes.find(attribute);

      if (ait != it->second.Attributes.end())
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

bool AddressSpaceInMemory::IsSuitableReference(const BrowseDescription & desc, const ReferenceDescription & reference) const
{
//  LOG_TRACE(Logger, "address_space_internal| checking reference: '{}' to node: '{}' ({}) which must fit ref: '{}' with IncludeSubtypes: '{}'", reference.ReferenceTypeId, reference.TargetNodeId, reference.BrowseName, desc.ReferenceTypeId, desc.IncludeSubtypes);

  if ((desc.Direction == BrowseDirection::Forward && !reference.IsForward) || (desc.Direction == BrowseDirection::Inverse && reference.IsForward))
    {
//      LOG_TRACE(Logger, "address_space_internal| reference in wrong direction");
      return false;
    }

  if (desc.ReferenceTypeId != ObjectId::Null && !IsSuitableReferenceType(reference, desc.ReferenceTypeId, desc.IncludeSubtypes))
    {
//      LOG_TRACE(Logger, "address_space_internal| reference has wrong type");
      return false;
    }

  if (desc.NodeClasses != NodeClass::Unspecified && (desc.NodeClasses & reference.TargetNodeClass) == NodeClass::Unspecified)
    {
//      LOG_TRACE(Logger, "address_space_internal| reference has wrong class");
      return false;
    }

//  LOG_TRACE(Logger, "address_space_internal| reference suitable");
  return true;
}

bool AddressSpaceInMemory::IsSuitableReferenceType(const ReferenceDescription & reference, const NodeId & typeId, bool includeSubtypes) const
{
  if (!includeSubtypes)
    {
      return reference.ReferenceTypeId == typeId;
    }

  const std::vector<NodeId> suitableTypes = SelectNodesHierarchy(std::vector<NodeId>(1, typeId));
  const auto resultIt = std::find(suitableTypes.begin(), suitableTypes.end(), reference.ReferenceTypeId);
  \
  return resultIt != suitableTypes.end();
}

std::vector<NodeId> AddressSpaceInMemory::SelectNodesHierarchy(std::vector<NodeId> sourceNodes) const
{
  std::vector<NodeId> subNodes;

  for (NodeId nodeid : sourceNodes)
    {
      NodesMap::const_iterator node_it = Nodes.find(nodeid);

      if (node_it != Nodes.end())
        {
          for (auto & ref : node_it->second.References)
            {
              if (ref.IsForward)
                {
                  subNodes.push_back(ref.TargetNodeId);
                }
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

AddNodesResult AddressSpaceInMemory::AddNode(const AddNodesItem & item)
{
  AddNodesResult result;

  LOG_TRACE(Logger, "address_space_internal| adding new node id: '{}' name: '{}'", item.RequestedNewNodeId, item.BrowseName.Name);

  const NodeId resultId = GetNewNodeId(item.RequestedNewNodeId);

  if (!Nodes.empty() && resultId != ObjectId::Null && Nodes.find(resultId) != Nodes.end())
    {
      LOG_ERROR(Logger, "address_space_internal| NodeId: '{}' already exists", resultId);
      result.Status = StatusCode::BadNodeIdExists;
      return result;
    }

  NodesMap::iterator parent_node_it = Nodes.end();

  if (item.ParentNodeId != NodeId())
    {
      parent_node_it = Nodes.find(item.ParentNodeId);

      if (parent_node_it == Nodes.end())
        {
          LOG_ERROR(Logger, "address_space_internal| parent node '{}' does not exists", item.ParentNodeId);
          result.Status = StatusCode::BadParentNodeIdInvalid;
          return result;
        }
    }

  NodeStruct nodestruct;
  //Add Common attributes
  nodestruct.Attributes[AttributeId::NodeId].Value = resultId;
  nodestruct.Attributes[AttributeId::BrowseName].Value = item.BrowseName;
  nodestruct.Attributes[AttributeId::NodeClass].Value = static_cast<int32_t>(item.Class);

  // Add requested attributes
  for (const auto & attr : item.Attributes.Attributes)
    {
      AttributeValue attval;
      attval.Value = attr.second;

      nodestruct.Attributes.insert(std::make_pair(attr.first, attval));
    }

  Nodes.insert(std::make_pair(resultId, nodestruct));

  if (parent_node_it != Nodes.end())
    {
      // Link from parent to child
      NodeStruct& parent = parent_node_it->second;

      ReferenceDescription desc;
      desc.ReferenceTypeId = item.ReferenceTypeId;
      desc.TargetNodeId = resultId;
      desc.TargetNodeClass = item.Class;
      desc.BrowseName = item.BrowseName;
      desc.DisplayName = LocalizedText(item.BrowseName.Name);
      desc.TargetNodeTypeDefinition = item.TypeDefinition;
      desc.IsForward = true; // should this be in constructor?

      parent.References.push_back(desc);

      // Link to parent
      AddReferencesItem typeRef;
      typeRef.ReferenceTypeId = item.ReferenceTypeId;
      typeRef.SourceNodeId = resultId;
      typeRef.TargetNodeId = item.ParentNodeId;
      typeRef.TargetNodeClass = static_cast<NodeClass>(parent.Attributes[AttributeId::NodeClass].Value.Value.As<int32_t>());
      typeRef.IsForward = false;
      AddReference(typeRef);
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

  LOG_TRACE(Logger, "address_space_internal| node added");

  return result;
}

StatusCode AddressSpaceInMemory::AddReference(const AddReferencesItem & item)
{
  NodesMap::iterator node_it = Nodes.find(item.SourceNodeId);

  if (node_it == Nodes.end())
    {
      return StatusCode::BadSourceNodeIdInvalid;
    }

  NodesMap::iterator targetnode_it = Nodes.find(item.TargetNodeId);

  if (targetnode_it == Nodes.end())
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

NodeId AddressSpaceInMemory::GetNewNodeId(const NodeId & id)
{
  uint32_t idx;

  if (id == ObjectId::Null || id.IsNull())
    {
      idx = DefaultIdx;
    }

  else
    {
      if (id.HasNullIdentifier())
        {
          idx = id.GetNamespaceIndex();
        }

      else
        {
          return id;
        }
    }

  // skip over already assigned node id's
  // this should be a very seldom operation - it only happens when
  // we actively assign static node id's to some nodes and after that
  // create nodes using automatic id allocation and even then it
  // only happens once. So we don't care about optimiziation here.
  for (;;)
    {
      NodeId result = OpcUa::NumericNodeId(++MaxNodeIdNum, idx);

      if (Nodes.find(result) == Nodes.end())
        {
          return result;
        }

      // completly unlikly - we would have to allocate 4gig nodes to
      // fullfill this condition
      if (MaxNodeIdNum == std::numeric_limits<uint32_t>::max())
        {
          throw std::runtime_error("address_space_internal| unable to assign new NodeId: range exceeded");
        }
    }
}
}

namespace Server
{
AddressSpace::UniquePtr CreateAddressSpace(const Common::Logger::SharedPtr & logger)
{
  return AddressSpace::UniquePtr(new Internal::AddressSpaceInMemory(logger));
}
}
}
