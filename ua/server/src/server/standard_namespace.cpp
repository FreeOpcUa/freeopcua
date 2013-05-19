/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

// TODO Add EventNotifier Attribute to all requred nodes.

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
        return reference.ReferenceTypeID == typeID;
      }
      const std::vector<NodeID> suitableTypes = SelectNodesHierarchy(std::vector<NodeID>(1, typeID));
      return std::find(suitableTypes.cbegin(), suitableTypes.cend(), reference.ReferenceTypeID) != suitableTypes.end();
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
      desc.ReferenceTypeID = referenceType;
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
       Objects();
       Types();
         DataTypes();
           BaseDataType();
             BooleanType();
             ByteStringType();
               ImageType();
                 ImageBmpType();
                 ImageGifType();
                 ImageJpgType();
                 ImagePngType();
               DataValueType();
               DateTimeType();
                 UtcTimeType();
               DiagnosticInfoType();
               EnumerationType();
                 IdType();
                 MessageSecurityModeType();
                 NodeClassType();
                 RedundancySupportType();
                 SecurityTokenRequestType();
                 ServerStateType();
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

    void Objects()
    {
      // Attributes
      AddValue(ObjectID::ObjectsFolder, AttributeID::NODE_ID,      NodeID(ObjectID::ObjectsFolder));
      AddValue(ObjectID::ObjectsFolder, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ObjectsFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Objects));
      AddValue(ObjectID::ObjectsFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Objects));
      AddValue(ObjectID::ObjectsFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Objects));
      AddValue(ObjectID::ObjectsFolder, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ObjectsFolder, AttributeID::USER_WRITE_MASK, 0);
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
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::DataTypes, Names::DataTypes, NodeClass::Object, ObjectID::FolderType);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::EventTypes, Names::EventTypes, NodeClass::Object, ObjectID::FolderType);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::ObjectTypes, Names::ObjectTypes, NodeClass::Object, ObjectID::FolderType);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::ReferenceTypes, Names::ReferenceTypes, NodeClass::Object, ObjectID::FolderType);
      AddReference(ObjectID::TypesFolder, forward, ReferenceID::Organizes, ObjectID::VariableTypes, Names::VariableTypes, NodeClass::Object, ObjectID::FolderType);
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

    void BaseDataType()
    {
      // Base Attributes
      AddValue(ObjectID::BaseDataType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseDataType));
      AddValue(ObjectID::BaseDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::BaseDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseDataType));
      AddValue(ObjectID::BaseDataType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseDataType));
      AddValue(ObjectID::BaseDataType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseDataType));
      AddValue(ObjectID::BaseDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BaseDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::BaseDataType, AttributeID::IS_ABSTRACT, true);

      // References
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Boolean,        Names::Boolean,        NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::ByteString,     Names::ByteString,     NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::DataValue,      Names::DataValue,      NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::DateTime,       Names::DateTime,       NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::DiagnosticInfo, Names::DiagnosticInfo, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Enumeration,    Names::Enumeration,    NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::ExpandedNodeID, Names::ExpandedNodeID, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Guid,           Names::Guid,           NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::LocalizedText,  Names::LocalizedText,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::NodeID,         Names::NodeID,         NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Number,         Names::Number,         NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::QualifiedName,  Names::QualifiedName,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::StatusCode,     Names::StatusCode,     NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::String,         Names::String,         NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Structure,      Names::Structure,      NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::XmlElement,     Names::XmlElement,     NodeClass::DataType, ObjectID::Null);
    }

    void BooleanType()
    {
      // Base Attributes
      AddValue(ObjectID::Boolean, AttributeID::NODE_ID,      NodeID(ObjectID::Boolean));
      AddValue(ObjectID::Boolean, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Boolean, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Boolean));
      AddValue(ObjectID::Boolean, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Boolean));
      AddValue(ObjectID::Boolean, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Boolean));
      AddValue(ObjectID::Boolean, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Boolean, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::Boolean, AttributeID::IS_ABSTRACT, false);
    }

    void ByteStringType()
    {
      // Base Attributes
      AddValue(ObjectID::ByteString, AttributeID::NODE_ID,      NodeID(ObjectID::Boolean));
      AddValue(ObjectID::ByteString, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ByteString, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ByteString));
      AddValue(ObjectID::ByteString, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ByteString));
      AddValue(ObjectID::ByteString, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ByteString));
      AddValue(ObjectID::ByteString, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ByteString, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::ByteString, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::ByteString, forward, ReferenceID::HasSubtype, ObjectID::Image, Names::Image, NodeClass::DataType, ObjectID::Null);
    }

    void ImageType()
    {
      // Base Attributes
      AddValue(ObjectID::Image, AttributeID::NODE_ID,      NodeID(ObjectID::Image));
      AddValue(ObjectID::Image, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Image, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Image));
      AddValue(ObjectID::Image, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Image));
      AddValue(ObjectID::Image, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Image));
      AddValue(ObjectID::Image, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Image, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::Image, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImageBmp, Names::ImageBmp, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImageGif, Names::ImageGif, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImageJpg, Names::ImageJpg, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImagePng, Names::ImagePng, NodeClass::DataType, ObjectID::Null);
    }

    void ImageBmpType()
    {
      // Base Attributes
      AddValue(ObjectID::ImageBmp, AttributeID::NODE_ID,      NodeID(ObjectID::ImageBmp));
      AddValue(ObjectID::ImageBmp, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ImageBmp, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImageBmp));
      AddValue(ObjectID::ImageBmp, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImageBmp));
      AddValue(ObjectID::ImageBmp, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImageBmp));
      AddValue(ObjectID::ImageBmp, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ImageBmp, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::ImageBmp, AttributeID::IS_ABSTRACT, false);
    }

    void ImageGifType()
    {
      // Base Attributes
      AddValue(ObjectID::ImageGif, AttributeID::NODE_ID,      NodeID(ObjectID::ImageGif));
      AddValue(ObjectID::ImageGif, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ImageGif, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImageGif));
      AddValue(ObjectID::ImageGif, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImageGif));
      AddValue(ObjectID::ImageGif, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImageGif));
      AddValue(ObjectID::ImageGif, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ImageGif, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::ImageGif, AttributeID::IS_ABSTRACT, false);
    }

    void ImageJpgType()
    {
      // Base Attributes
      AddValue(ObjectID::ImageJpg, AttributeID::NODE_ID,      NodeID(ObjectID::ImageJpg));
      AddValue(ObjectID::ImageJpg, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ImageJpg, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImageJpg));
      AddValue(ObjectID::ImageJpg, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImageJpg));
      AddValue(ObjectID::ImageJpg, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImageJpg));
      AddValue(ObjectID::ImageJpg, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ImageJpg, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::ImageJpg, AttributeID::IS_ABSTRACT, false);
    }

    void ImagePngType()
    {
      // Base Attributes
      AddValue(ObjectID::ImagePng, AttributeID::NODE_ID,      NodeID(ObjectID::ImagePng));
      AddValue(ObjectID::ImagePng, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ImagePng, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImagePng));
      AddValue(ObjectID::ImagePng, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImagePng));
      AddValue(ObjectID::ImagePng, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImagePng));
      AddValue(ObjectID::ImagePng, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ImagePng, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::ImagePng, AttributeID::IS_ABSTRACT, false);
    }

    void DataValueType()
    {
      // Base Attributes
      AddValue(ObjectID::DataValue, AttributeID::NODE_ID,      NodeID(ObjectID::DataValue));
      AddValue(ObjectID::DataValue, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::DataValue, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataValue));
      AddValue(ObjectID::DataValue, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataValue));
      AddValue(ObjectID::DataValue, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataValue));
      AddValue(ObjectID::DataValue, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DataValue, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::DataValue, AttributeID::IS_ABSTRACT, false);
    }

    void DateTimeType()
    {
      // Attributes
      AddValue(ObjectID::DateTime, AttributeID::NODE_ID,      NodeID(ObjectID::DateTime));
      AddValue(ObjectID::DateTime, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::DateTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DateTime));
      AddValue(ObjectID::DateTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DateTime));
      AddValue(ObjectID::DateTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DateTime));
      AddValue(ObjectID::DateTime, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DateTime, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::DateTime, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::DateTime, forward, ReferenceID::HasSubtype, ObjectID::UtcTime, Names::UtcTime, NodeClass::ObjectType, ObjectID::Null);
    }

    void UtcTimeType()
    {
      // Attributes
      AddValue(ObjectID::UtcTime, AttributeID::NODE_ID,      NodeID(ObjectID::UtcTime));
      AddValue(ObjectID::UtcTime, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::UtcTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UtcTime));
      AddValue(ObjectID::UtcTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::UtcTime));
      AddValue(ObjectID::UtcTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::UtcTime));
      AddValue(ObjectID::UtcTime, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UtcTime, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::UtcTime, AttributeID::IS_ABSTRACT, false);
    }

    void DiagnosticInfoType()
    {
      // Attributes
      AddValue(ObjectID::DiagnosticInfo, AttributeID::NODE_ID,      NodeID(ObjectID::DiagnosticInfo));
      AddValue(ObjectID::DiagnosticInfo, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::DiagnosticInfo, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DiagnosticInfo));
      AddValue(ObjectID::DiagnosticInfo, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DiagnosticInfo));
      AddValue(ObjectID::DiagnosticInfo, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DiagnosticInfo));
      AddValue(ObjectID::DiagnosticInfo, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DiagnosticInfo, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::DiagnosticInfo, AttributeID::IS_ABSTRACT, false);
    }

    void EnumerationType()
    {
      // Base Attributes
      AddValue(ObjectID::Enumeration, AttributeID::NODE_ID,      NodeID(ObjectID::Enumeration));
      AddValue(ObjectID::Enumeration, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Enumeration, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Enumeration));
      AddValue(ObjectID::Enumeration, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Enumeration));
      AddValue(ObjectID::Enumeration, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Enumeration));
      AddValue(ObjectID::Enumeration, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Enumeration, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::Enumeration, AttributeID::IS_ABSTRACT, true);

      // References
      AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::IdType, Names::IdType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::MessageSecurityMode, Names::MessageSecurityMode, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::NodeClass, Names::NodeClass, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::RedundancySupport, Names::RedundancySupport, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::SecurityTokenRequestType, Names::SecurityTokenRequestType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::ServerState, Names::ServerState, NodeClass::DataType, ObjectID::Null);
    }

    void IdType()
    {
      // Base Attributes
      AddValue(ObjectID::IdType, AttributeID::NODE_ID,      NodeID(ObjectID::IdType));
      AddValue(ObjectID::IdType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::IdType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::IdType));
      AddValue(ObjectID::IdType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::IdType));
      AddValue(ObjectID::IdType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::IdType));
      AddValue(ObjectID::IdType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::IdType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::IdType, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::IdType, forward, ReferenceID::HasProperty, ObjectID::IdTypeEnumStrings, Names::EnumStrings, NodeClass::Variable, ObjectID::PropertyType);
    }

    void MessageSecurityModeType()
    {
      // Base Attributes
      AddValue(ObjectID::MessageSecurityMode, AttributeID::NODE_ID,      NodeID(ObjectID::MessageSecurityMode));
      AddValue(ObjectID::MessageSecurityMode, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::MessageSecurityMode, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MessageSecurityMode));
      AddValue(ObjectID::MessageSecurityMode, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MessageSecurityMode));
      AddValue(ObjectID::MessageSecurityMode, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MessageSecurityMode));
      AddValue(ObjectID::MessageSecurityMode, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::MessageSecurityMode, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::MessageSecurityMode, AttributeID::IS_ABSTRACT, false);
    }

    void NodeClassType()
    {
      // Base Attributes
      AddValue(ObjectID::NodeClass, AttributeID::NODE_ID,      NodeID(ObjectID::NodeClass));
      AddValue(ObjectID::NodeClass, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::NodeClass, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NodeClass));
      AddValue(ObjectID::NodeClass, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::NodeClass));
      AddValue(ObjectID::NodeClass, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::NodeClass));
      AddValue(ObjectID::NodeClass, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::NodeClass, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::NodeClass, AttributeID::IS_ABSTRACT, false);
    }

    void RedundancySupportType()
    {
      // Base Attributes
      AddValue(ObjectID::RedundancySupport, AttributeID::NODE_ID,      NodeID(ObjectID::RedundancySupport));
      AddValue(ObjectID::RedundancySupport, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::RedundancySupport, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::RedundancySupport));
      AddValue(ObjectID::RedundancySupport, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::RedundancySupport));
      AddValue(ObjectID::RedundancySupport, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::RedundancySupport));
      AddValue(ObjectID::RedundancySupport, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::RedundancySupport, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::RedundancySupport, AttributeID::IS_ABSTRACT, false);
    }

    void SecurityTokenRequestType()
    {
      // Base Attributes
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::NODE_ID,      NodeID(ObjectID::SecurityTokenRequestType));
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SecurityTokenRequestType));
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SecurityTokenRequestType));
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SecurityTokenRequestType));
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::SecurityTokenRequestType, AttributeID::IS_ABSTRACT, false);
    }

    void ServerStateType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerState, AttributeID::NODE_ID,      NodeID(ObjectID::ServerState));
      AddValue(ObjectID::ServerState, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ServerState, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerState));
      AddValue(ObjectID::ServerState, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerState));
      AddValue(ObjectID::ServerState, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerState));
      AddValue(ObjectID::ServerState, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerState, AttributeID::USER_WRITE_MASK, 0);
      // Type Attribute
      AddValue(ObjectID::ServerState, AttributeID::IS_ABSTRACT, false);
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
