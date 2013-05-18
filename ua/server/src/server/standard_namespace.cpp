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

  struct AttributeValue
  {
    NodeID Node;
    AttributeID Attribute;
    DataValue Value;
  };



  class StandardNamespaceInMemory : public OpcUa::StandardNamespace
  {
  public:
    StandardNamespaceInMemory()
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

    virtual std::vector<DataValue> Read(const ReadParameters& params) const
    {
      std::vector<DataValue> values;
      for (auto attribute : params.AttributesToRead)
      {
        values.push_back(GetValue(attribute.Node, attribute.Attribute));
      }
      return values;
    }

    virtual std::vector<StatusCode> Write(const std::vector<OpcUa::WriteValue>& values)
    {
      return std::vector<StatusCode>(values.size(), StatusCode::BadWriteNotSupported);
    }

  private:
    DataValue GetValue(const NodeID& node, AttributeID attribute) const
    {
      for (auto value : AttributeValues)
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
      return std::find(suitableTypes.cbegin(), suitableTypes.cend(), reference.TypeID) != suitableTypes.end();
    }

    std::vector<NodeID> SelectNodesHierarchy(std::vector<NodeID> sourceNodes) const
    {
      std::vector<NodeID> subNodes;
      for (ReferenciesMap::const_iterator refIt = Referencies.begin(); refIt != Referencies.end(); ++refIt)
      {
        if (std::find(sourceNodes.cbegin(), sourceNodes.cend(), refIt->first) != sourceNodes.end())
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

    void AddValue(NodeID node, AttributeID attribute, Variant value)
    {
      AttributeValue data;
      data.Node = node;
      data.Attribute = attribute;
      data.Value.Encoding = DATA_VALUE;
      AttributeValues.push_back(data);
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
         DataTypes();
         ReferenceTypes();
           Refs();
             HierarchicalReferences();
             HasChild();
             HasEventSource();
             Organizes();
    }

    void Root()
    {
      // Attributes
      AddValue(ObjectID::RootFolder, AttributeID::NODE_ID,      NodeID(ObjectID::RootFolder));
      AddValue(ObjectID::RootFolder, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::RootFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Root));
      AddValue(ObjectID::RootFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Root));
      AddValue(ObjectID::RootFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Root));
      AddValue(ObjectID::RootFolder, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::RootFolder, AttributeID::USER_WRITE_MASK, 0);

      // Referencies
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ObjectsFolder, Names::Objects,    NodeClass::Object,     ObjectID::FolderType);
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::TypesFolder,   Names::Types,      NodeClass::Object,     ObjectID::FolderType);
      AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ViewsFolder,   Names::Views,      NodeClass::Object,     ObjectID::FolderType);
    }

    void Types()
    {
      // Attributes
      AddValue(ObjectID::TypesFolder, AttributeID::NODE_ID,      NodeID(ObjectID::TypesFolder));
      AddValue(ObjectID::TypesFolder, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::TypesFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Types));
      AddValue(ObjectID::TypesFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Types));
      AddValue(ObjectID::TypesFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Types));
      AddValue(ObjectID::TypesFolder, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::TypesFolder, AttributeID::USER_WRITE_MASK, 0);

      // References
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::ReferenceTypes, Names::ReferenceTypes, NodeClass::Object, ObjectID::FolderType);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::DataTypes, Names::DataTypes, NodeClass::Object, ObjectID::FolderType);
    }

    void DataTypes()
    {
      // Attributes
      AddValue(ObjectID::DataTypes, AttributeID::NODE_ID,      NodeID(ObjectID::DataTypes));
      AddValue(ObjectID::DataTypes, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::DataTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataTypes));
      AddValue(ObjectID::DataTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataTypes));
      AddValue(ObjectID::DataTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataTypes));
      AddValue(ObjectID::DataTypes, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DataTypes, AttributeID::USER_WRITE_MASK, 0);

      // References
      AddReference(ObjectID::DataTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::DataTypes, forward, ReferenceID::Organizes, ObjectID::BaseDataType, Names::BaseDataType, NodeClass::DataType, ObjectID::Null);
    }


    void ReferenceTypes()
    {
      // Attributes
      AddValue(ObjectID::ReferenceTypes, AttributeID::NODE_ID,      NodeID(ObjectID::ReferenceTypes));
      AddValue(ObjectID::ReferenceTypes, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ReferenceTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ReferenceTypes));
      AddValue(ObjectID::ReferenceTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ReferenceTypes));
      AddValue(ObjectID::ReferenceTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ReferenceTypes));
      AddValue(ObjectID::ReferenceTypes, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ReferenceTypes, AttributeID::USER_WRITE_MASK, 0);

      // References
      AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::Organizes, ObjectID::References, Names::References, NodeClass::ReferenceType, ObjectID::Null);
    }

    void Refs()
    {
      // Attributes
      AddValue(ObjectID::References, AttributeID::NODE_ID,      NodeID(ObjectID::References));
      AddValue(ObjectID::References, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::References, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::References));
      AddValue(ObjectID::References, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::References));
      AddValue(ObjectID::References, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::References));
      AddValue(ObjectID::References, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::References, AttributeID::USER_WRITE_MASK, 0);
      AddValue(ObjectID::References, AttributeID::SYMMETRIC, true);

      // References
      AddReference(ObjectID::References, forward, ReferenceID::HasSubtype, ObjectID::HierarchicalReferences, Names::HierarchicalReferences, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::References, forward, ReferenceID::HasSubtype, ObjectID::NonHierarchicalReferences, Names::NonHierarchicalReferences, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HierarchicalReferences()
    {
      // Attributes
      AddValue(ObjectID::HierarchicalReferences, AttributeID::NODE_ID,      NodeID(ObjectID::HierarchicalReferences));
      AddValue(ObjectID::HierarchicalReferences, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HierarchicalReferences, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HierarchicalReferences));
      AddValue(ObjectID::HierarchicalReferences, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HierarchicalReferences));
      AddValue(ObjectID::HierarchicalReferences, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HierarchicalReferences));
      AddValue(ObjectID::HierarchicalReferences, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HierarchicalReferences, AttributeID::USER_WRITE_MASK, 0);
      AddValue(ObjectID::HierarchicalReferences, AttributeID::SYMMETRIC, false);

      // References
      AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasChild, Names::HasChild, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasEventSource, Names::HasEventSource, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::Organizes, Names::Organizes, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HasChild()
    {
      // Attributes
      AddValue(ObjectID::HasChild, AttributeID::NODE_ID,      NodeID(ObjectID::HasChild));
      AddValue(ObjectID::HasChild, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasChild, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasChild));
      AddValue(ObjectID::HasChild, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasChild));
      AddValue(ObjectID::HasChild, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasChild));
      AddValue(ObjectID::HasChild, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasChild, AttributeID::USER_WRITE_MASK, 0);
      AddValue(ObjectID::HasChild, AttributeID::SYMMETRIC, false);

      // References
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
    std::vector<AttributeValue> AttributeValues;
  };

}

std::unique_ptr<OpcUa::StandardNamespace> OpcUa::CreateStandardNamespace()
{
  return std::unique_ptr<OpcUa::StandardNamespace>(new StandardNamespaceInMemory());
}

