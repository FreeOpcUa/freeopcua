/******************************************************************************
 *   Copyright (C) 2014-2014 Olivier Roulet-Dubonnet          *
 *   olivier.roulet@gmail.com          *
 *                      *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as      *
 *   published by the Free Software Foundation; version 3 of the License.   *
 *                      *
 *   This library is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      *
 *   GNU Lesser General Public License for more details.        *
 *                      *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the          *
 *   Free Software Foundation, Inc.,              *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.        *
 ******************************************************************************/


#include <iostream>

#include <opc/ua/node.h>

#include <opc/common/object_id.h>
#include <opc/ua/protocol/node_management.h>
#include <opc/ua/protocol/method.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/variable_access_level.h>

namespace OpcUa
{

  Node::Node(Services::SharedPtr srv)
    : Node(srv, NumericNodeId(0, 0))
  {
  }

  Node::Node(Services::SharedPtr srv, const NodeId& id)
    : Server(srv)
    , Id(id)
  {
  }

  Node::Node(const Node& other)
    : Server(other.Server)
    , Id(other.Id)
  {
  }

  NodeId Node::GetId() const
  {
    return Id;
  }

  Variant Node::GetAttribute(const AttributeId attr) const
  {
    ReadParameters params;
    ReadValueId attribute;
    attribute.NodeId = Id;
    attribute.AttributeId = attr;
    params.AttributesToRead.push_back(attribute);
    std::vector<DataValue> vec =  Server->Attributes()-> Read(params); 
    if ( vec.size() > 0 )
    {
      DataValue dv =  vec.front(); 
      return dv.Value;
    }
    else
    {
      return Variant(); //FIXME: What does it mean when not value is found?
    }
  }

  std::vector<Variant> Node::CallMethod(const NodeId methodId, const std::vector<Variant> inputArguments) const
  {
    std::vector<CallMethodRequest> methodsToCall;
    CallMethodRequest callMethod;
    callMethod.ObjectId = Id;
    callMethod.MethodId = methodId;
    callMethod.InputArguments = inputArguments;

    methodsToCall.push_back(callMethod);

    std::vector<CallMethodResult> results = Server->Method()->Call(methodsToCall);
    // TODO: add checking of StatusCode

    // TODO: add to call multiple methods --> Rename to CallMethods and check how to return them as list of Variants
    return results.front().OutputArguments;
  }

  void Node::SetAttribute(AttributeId attr, const DataValue &dval) const
  {
    WriteValue attribute;
    attribute.NodeId = Id;
    attribute.AttributeId = attr;
    attribute.Value = dval;
    std::vector<StatusCode> codes = Server->Attributes()->Write(std::vector<WriteValue>(1, attribute));
    CheckStatusCode(codes.front());
  }

  void Node::SetValue(const Variant& val) const
  {
    DataValue dval(val);
    SetAttribute(AttributeId::Value, dval);
  }

  void Node::SetValue(const DataValue &dval) const
  {
    SetAttribute(AttributeId::Value, dval);
  }


  std::vector<Node> Node::GetChildren(const ReferenceId& refid) const
  {
    BrowseDescription description;
    description.NodeToBrowse = Id;
    description.Direction = BrowseDirection::Forward;
    description.IncludeSubtypes = true;
    description.NodeClasses = NodeClass::Unspecified;
    description.ResultMask = BrowseResultMask::All;
    description.ReferenceTypeId =  refid;

    NodesQuery query;
    query.NodesToBrowse.push_back(description);
    query.MaxReferenciesPerNode = 100;
    std::vector<Node> nodes;
    std::vector<BrowseResult> results = Server->Views()->Browse(query);
    if ( results.empty() )
    {
      return nodes;
    }
    while(!results[0].Referencies.empty())
    {
      for (auto refIt : results[0].Referencies)
      {
        Node node(Server, refIt.TargetNodeId);
        nodes.push_back(node);
      }
      results = Server->Views()->BrowseNext();
      if ( results.empty() )
      {
        return nodes;
      }
    }
    return nodes;
  }

  std::vector<Node> Node::GetChildren() const
  {
    return GetChildren(ReferenceId::HierarchicalReferences);
  }

  QualifiedName Node::GetBrowseName() const
  {
    Variant var = GetAttribute(AttributeId::BrowseName);
    if (var.Type() != VariantType::QUALIFIED_NAME)
    {
      throw std::runtime_error("Could not retrieve browse name.");
    }
    return var.As<QualifiedName>();
  }

  std::vector<AddNodesResult> Node::AddNodes(std::vector<AddNodesItem> items) const
  {
    return Server->NodeManagement()->AddNodes(items);
  }

  std::vector<StatusCode> Node::AddReferences(std::vector<AddReferencesItem> items) const
  {
    return Server->NodeManagement()->AddReferences(items);
  }

  Node Node::GetChild(const std::string& browsename) const
  {
    return GetChild(std::vector<std::string>({browsename}));
  }

  Node Node::GetChild(const std::vector<std::string>& path) const
  {
    std::vector<QualifiedName> vec;
    uint16_t ns = Id.GetNamespaceIndex();
    for (std::string str: path)
    {
      QualifiedName qname = ToQualifiedName(str, ns);
      ns = qname.NamespaceIndex;
      vec.push_back(qname);
    }
    return GetChild(vec);
  }


  Node Node::GetChild(const std::vector<QualifiedName>& path) const
  {
    std::vector<RelativePathElement> rpath;
    for (QualifiedName qname: path)
    {
      RelativePathElement el;
      el.TargetName = qname;
      rpath.push_back(el);
    }
    BrowsePath bpath;
    bpath.Path.Elements = rpath;
    bpath.StartingNode = Id;
    std::vector<BrowsePath> bpaths;
    bpaths.push_back(bpath);
    TranslateBrowsePathsParameters params;
    params.BrowsePaths = bpaths;

    std::vector<BrowsePathResult> result = Server->Views()->TranslateBrowsePathsToNodeIds(params);
    CheckStatusCode(result.front().Status);

    NodeId node =result.front().Targets.front().Node ;
    return Node(Server, node);
  }

  std::string Node::ToString() const
  {
    std::ostringstream os;
    os << "Node(" << Id << ")";
    return os.str();
  }

  Node Node::AddFolder(const std::string& nodeid, const std::string& browsename) const
   {
     NodeId node = ToNodeId(nodeid, this->Id.GetNamespaceIndex());
     QualifiedName qn = ToQualifiedName(browsename, GetBrowseName().NamespaceIndex);
     return AddFolder(node, qn);
   }

  Node Node::AddFolder(uint32_t namespaceIdx, const std::string& name) const
  {
    NodeId nodeid = NumericNodeId(Common::GenerateNewId(), namespaceIdx);
    QualifiedName qn = ToQualifiedName(name, namespaceIdx);
    return AddFolder(nodeid, qn);
  }

  Node Node::AddFolder(const NodeId& nodeid, const QualifiedName& browsename) const
  {

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeId = nodeid;
    item.Class = NodeClass::Object;
    item.ReferenceTypeId = ReferenceId::Organizes; 
    item.TypeDefinition = ObjectId::FolderType; 
    ObjectAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.EventNotifier = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));
    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeId);
  }

  Node Node::AddObject(const std::string& nodeid, const std::string& browsename) const
   {
     NodeId node = ToNodeId(nodeid, this->Id.GetNamespaceIndex());
     QualifiedName qn = ToQualifiedName(browsename, GetBrowseName().NamespaceIndex);
     return AddObject(node, qn);
   }

  Node Node::AddObject(uint32_t ns, const std::string& name) const
  {
    //FIXME: should default namespace be the onde from the parent of the browsename?
    NodeId nodeid = NumericNodeId(Common::GenerateNewId(), ns);
    QualifiedName qn = ToQualifiedName(name, ns);
    return AddObject(nodeid, qn);
  }

  Node Node::AddObject(const NodeId& nodeid, const QualifiedName& browsename) const
  {
    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeId = nodeid;
    item.Class = NodeClass::Object;
    item.ReferenceTypeId = ReferenceId::HasComponent; 
    item.TypeDefinition = ObjectId::BaseObjectType; 
    ObjectAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.EventNotifier = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeId);
  }

  Node Node::AddVariable(uint32_t ns, const std::string& name, const Variant& val) const
  {
    NodeId nodeid = NumericNodeId(Common::GenerateNewId(), ns);
    QualifiedName qn = ToQualifiedName(name, ns);
    return AddVariable(nodeid, qn, val);
  }

  Node Node::AddVariable(const std::string& nodeid, const std::string& browsename, const Variant& val) const
  {
    NodeId node = ToNodeId(nodeid, this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(browsename, GetBrowseName().NamespaceIndex);
    return AddVariable(node, qn, val);
  }

  Node Node::AddVariable(const NodeId& nodeid, const QualifiedName& browsename, const Variant& val) const
  {
    ObjectId datatype = VariantTypeToDataType(val.Type());

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeId = nodeid;
    item.Class = NodeClass::Variable;
    item.ReferenceTypeId = ReferenceId::HasComponent; 
    item.TypeDefinition = ObjectId::BaseDataVariableType; 
    VariableAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.Value = val;
    attr.Type = datatype;
    attr.Rank  = 0;
    attr.Dimensions = val.Dimensions;
    attr.AccessLevel = VariableAccessLevel::CurrentRead;
    attr.UserAccessLevel = VariableAccessLevel::CurrentRead;
    attr.MinimumSamplingInterval = 1;
    attr.Historizing = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeId);
  }


  Node Node::AddProperty(uint32_t ns, const std::string& name, const Variant& val) const
  {
    NodeId nodeid = NumericNodeId(Common::GenerateNewId(), ns);
    const QualifiedName& qname = ToQualifiedName(name, ns);
    return AddProperty(nodeid, qname, val);
  }

  Node Node::AddProperty(const std::string& nodeid, const std::string& browsename, const Variant& val) const
  {
    NodeId node = ToNodeId(nodeid, this->Id.GetNamespaceIndex());
    QualifiedName qn = ToQualifiedName(browsename, GetBrowseName().NamespaceIndex);
    return AddProperty(node, qn, val);
  }

  Node Node::AddProperty(const NodeId& nodeid, const QualifiedName& browsename, const Variant& val) const
  {

    ObjectId datatype = VariantTypeToDataType(val.Type());

    AddNodesItem item;
    item.BrowseName = browsename;
    item.ParentNodeId = this->Id;
    item.RequestedNewNodeId = nodeid;
    item.Class = NodeClass::Variable;
    item.ReferenceTypeId = ReferenceId::HasProperty; 
    item.TypeDefinition = ObjectId::PropertyType; 
    VariableAttributes attr;
    attr.DisplayName = LocalizedText(browsename.Name);
    attr.Description = LocalizedText(browsename.Name);
    attr.WriteMask = 0;
    attr.UserWriteMask = 0;
    attr.Value = val;
    attr.Type = datatype;
    attr.Rank  = 0;
    attr.Dimensions = val.Dimensions;
    attr.AccessLevel = VariableAccessLevel::CurrentRead;
    attr.UserAccessLevel = VariableAccessLevel::CurrentRead;
    attr.MinimumSamplingInterval = 0;
    attr.Historizing = 0;
    item.Attributes = attr;

    std::vector<AddNodesResult> addnodesresults = Server->NodeManagement()->AddNodes(std::vector<AddNodesItem>({item}));

    AddNodesResult res = addnodesresults.front(); //This should always work
    CheckStatusCode(res.Status);

    return Node(Server, res.AddedNodeId);

  }

  Variant Node::GetValue() const
  {
    return GetAttribute(AttributeId::Value);
  }

  Variant Node::GetDataType() const
  {
    return GetAttribute(AttributeId::DataType);
  }

} // namespace OpcUa


std::ostream& OpcUa::operator<<(std::ostream& os, const Node& node)
{
  os << node.ToString();
  return os;
}

