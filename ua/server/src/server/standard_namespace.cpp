/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#include "standard_namespace.h"

#include <opc/ua/node_classes.h>
#include <opc/ua/strings.h>

#include <algorithm>
#include <map>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Remote;

  const bool forward = true;
  const bool reverse = true;


  typedef std::multimap<ObjectID, ReferenceDescription> ReferenciesMap;


  class StandardNamespace : public ViewServices
  {
  public:
    StandardNamespace()
    {
      Fill();
    }

    virtual std::vector<ReferenceDescription> Browse(const BrowseParameters& params)
    {
      std::vector<ReferenceDescription> result;
      for (auto reference : Referencies)
      {
        if (IsSuitableReference(params.Description, reference))
        {
          result.push_back(reference.second);
        }
      }
      return result;
    }

    virtual std::vector<ReferenceDescription> BrowseNext()
    {
      return std::vector<ReferenceDescription>();
    }

  private:
    bool IsSuitableReference(const BrowseDescription& desc, const ReferenciesMap::value_type& refPair) const
    {
      const ObjectID sourceNode = refPair.first;
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
      if (desc.NodeClasses && (desc.NodeClasses & static_cast<uint32_t>(reference.TargetNodeClass)) == 0)
      {
        return false;
      }
      return true;
    }

    bool IsSuitableReferenceType(const ReferenceDescription& reference, const NodeID& typeID, bool includeSubtypes) const
    {
      if (!includeSubtypes)
      {
        return reference.TypeID == typeID;
      }
      const std::vector<NodeID> suitableTypes = SelectNodesHierarchy(std::vector<NodeID>(1, typeID));
      return std::find(suitableTypes.begin(), suitableTypes.end(), reference.TypeID) != suitableTypes.end();
    }

    std::vector<NodeID> SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const
    {
      std::vector<NodeID> subNodes;
      for (ReferenciesMap::const_iterator refIt = Referencies.begin(); refIt != Referencies.end(); ++refIt)
      {
        if (std::find(sourceNodes.begin(), sourceNodes.end(), refIt->first) != sourceNodes.end())
        {
          subNodes.push_back(refIt->second.TargetNodeID);
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


    void AddReference(
      ObjectID sourceNode,
      bool isForward,
      ReferenceID referenceType,
      ObjectID targetNode,
      const char* name,
      NodeClass targetNodeClass,
      ObjectID targetNodeTypeDefinition)
    {
      ReferenceDescription desc;
      desc.TypeID = referenceType;
      desc.IsForward = isForward;
      desc.TargetNodeID = NodeID(targetNode);
      desc.BrowseName.Name = name;
      desc.DisplayName.Text = name;
      desc.TargetNodeClass = targetNodeClass;
      desc.TargetNodeTypeDefinition = NodeID(targetNodeTypeDefinition);

      Referencies.insert({sourceNode, desc});
    }

    void Fill()
    {

     Root();
       Types();
         ReferenceTypes();
           Refs();
             HierarchicalReferences();
             HasChild();
             HasEventSource();
             Organizes();
    }

    void Root()
    {
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ObjectsFolder, Names::Objects,    NodeClass::Object,     ObjectID::FolderType);
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::TypesFolder,   Names::Types,      NodeClass::Object,     ObjectID::FolderType);
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ViewsFolder,   Names::Views,      NodeClass::Object,     ObjectID::FolderType);
    }

    void Types()
    {
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::ReferenceTypes, Names::ReferenceTypes, NodeClass::Object, ObjectID::FolderType);
    }

    void ReferenceTypes()
    {
      AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::Organizes, ObjectID::References, Names::References, NodeClass::ReferenceType, ObjectID::Null);
    }

    void Refs()
    {
      AddReference(ObjectID::References, forward, ReferenceID::HasSubtype, ObjectID::HierarchicalReferences, Names::HierarchicalReferences, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::References, forward, ReferenceID::HasSubtype, ObjectID::NonHierarchicalReferences, Names::NonHierarchicalReferences, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HierarchicalReferences()
    {
      AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasChild, Names::HasChild, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasEventSource, Names::HasEventSource, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::Organizes, Names::Organizes, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HasChild()
    {
      AddReference(ObjectID::HasChild, forward, ReferenceID::HasSubtype, ObjectID::Aggregates, Names::Aggregates, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::HasChild, forward, ReferenceID::HasSubtype, ObjectID::HasSubtype, Names::HasSubtype, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HasEventSource()
    {
    }

    void Organizes()
    {
    }

  private:
    ReferenciesMap Referencies;
  };

}

std::unique_ptr<OpcUa::Remote::ViewServices> OpcUa::CreateStandardNamespace()
{
  return std::unique_ptr<Remote::ViewServices>(new StandardNamespace());
}

