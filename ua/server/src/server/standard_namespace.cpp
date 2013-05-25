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
#include <opc/ua/variable_access_level.h>
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
                   IdTypeEnumStrings();
                 MessageSecurityModeType();
                   MessageSecurityModeTypeEnumStrings();
                 NodeClassType();
                 RedundancySupportType();
                   RedundancySupportTypeEnumStrings();
                 SecurityTokenRequestType();
                   SecurityTokenRequestTypeEnumStrings();
                 ServerStateType();
                   ServerStateEnumStrings();
                 ExpandedNodeID();
                 Guid();
                 LocalizedTextType();
                 NodeIDType();
                 NumberType();
                   DoubleType();
                     DurationType();
                   FloatType();
                   IntegerType();
                     Int16Type();
                     Int32Type();
                     Int64Type();
                     SByteType();
                   UIntegerType();
                     ByteType();
                     UInt16Type();
                     UInt32Type();
                     UInt64Type();
                   QualifiedNameType();
                   StatusCodeType();
                   StringType();
                     LocaleIDType();
                     NumericRangeType();
                   StructureType();
                     AddNodesItem();
                     AddReferencesItem();
                     ApplicationDescriptionType();
                     Argument();
                     BuildInfo();
                     DeleteNodesItem();
                     DeleteReferencesItem();
                     EUInformation();
                     ModelChangeStructureDataType();
                     Range();
                     SamplingIntervalDiagnosticsDataType();
                     SemanticChangeStructureDataType();
                     ServerDiagnosticsSummaryDataType();
                     ServerStatusDataType();
                     ServiceCounterDataType();
                     SessionDiagnosticsDataType();
                     SessionSecurityDiagnosticsDataType();
                     SignedSoftwareCertificate();
                     StatusResult();
                     SubscriptionDiagnosticsDataType();
                     UserIdentifyTokenType();
                       AnonymousIdentifyToken();
                       UserNameIdentifyToken();
                       X509IdentifyToken();
                   XmlElement();
         EventTypes();
           BaseEventType();
             BaseModelChangeEventType();
             SemanticChangeEventType();
             SystemEventType();
             EventID();
             EventType();
             LocalTime();
             Message();
             ReceiveTime();
             Severity();
             SourceName();
             SourceNode();
             Time();
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

    void EnumStrings(ObjectID nodeID, const std::vector<LocalizedText>& values)
    {
      // Base Attributes
      AddValue(nodeID, AttributeID::NODE_ID,      NodeID(nodeID));
      AddValue(nodeID, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(nodeID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EnumStrings));
      AddValue(nodeID, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EnumStrings));
      AddValue(nodeID, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EnumStrings));
      AddValue(nodeID, AttributeID::WRITE_MASK,   0);
      AddValue(nodeID, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(nodeID, AttributeID::VALUE, values);
      AddValue(nodeID, AttributeID::DATA_TYPE, NodeID(ObjectID::LocalizedText));
      AddValue(nodeID, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(1,0));
      AddValue(nodeID, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(nodeID, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(nodeID, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(100));
      AddValue(nodeID, AttributeID::HISTORIZING, false);
      AddValue(nodeID, AttributeID::VALUE_RANK, int32_t(1));
      // References
      AddReference(nodeID, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
    }

    void IdTypeEnumStrings()
    {
      std::vector<OpcUa::LocalizedText> values;
      values.push_back(LocalizedText("Numeric"));
      values.push_back(LocalizedText("String"));
      values.push_back(LocalizedText("Guid"));
      values.push_back(LocalizedText("Opaque"));
      EnumStrings(ObjectID::IdTypeEnumStrings, values);
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
      // References
      AddReference(ObjectID::MessageSecurityMode, forward, ReferenceID::HasProperty, ObjectID::MessageSecurityModeEnumStrings, Names::EnumStrings, NodeClass::Variable, ObjectID::PropertyType);
    }

    void MessageSecurityModeTypeEnumStrings()
    {
      std::vector<OpcUa::LocalizedText> values;
      values.push_back(LocalizedText("Invalid"));
      values.push_back(LocalizedText("None"));
      values.push_back(LocalizedText("Sign"));
      values.push_back(LocalizedText("SignAndEncrypt"));
      EnumStrings(ObjectID::MessageSecurityModeEnumStrings, values);
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
      // References
      AddReference(ObjectID::RedundancySupport, forward, ReferenceID::HasProperty, ObjectID::RedundancySupportEnumStrings, Names::EnumStrings, NodeClass::Variable, ObjectID::PropertyType);
    }

    void RedundancySupportTypeEnumStrings()
    {
      std::vector<OpcUa::LocalizedText> values;
      values.push_back(LocalizedText("None"));
      values.push_back(LocalizedText("Cold"));
      values.push_back(LocalizedText("Warm"));
      values.push_back(LocalizedText("Hot"));
      values.push_back(LocalizedText("Transparent"));
      EnumStrings(ObjectID::RedundancySupportEnumStrings, values);
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
      // References
      AddReference(ObjectID::SecurityTokenRequestType, forward, ReferenceID::HasProperty, ObjectID::SecurityTokenRequestTypeEnumStrings, Names::EnumStrings, NodeClass::Variable, ObjectID::PropertyType);
    }

    void SecurityTokenRequestTypeEnumStrings()
    {
      std::vector<OpcUa::LocalizedText> values;
      values.push_back(LocalizedText("Issue"));
      values.push_back(LocalizedText("Renew"));
      EnumStrings(ObjectID::SecurityTokenRequestTypeEnumStrings, values);
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
      // Type Attributes
      AddValue(ObjectID::ServerState, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::ServerState, forward, ReferenceID::HasProperty, ObjectID::ServerStateEnumStrings, Names::EnumStrings, NodeClass::Variable, ObjectID::PropertyType);
    }

    void ServerStateEnumStrings()
    {
      std::vector<OpcUa::LocalizedText> values;
      values.push_back(LocalizedText("Running"));
      values.push_back(LocalizedText("Failed"));
      values.push_back(LocalizedText("NoConfiguration"));
      values.push_back(LocalizedText("Suspended"));
      values.push_back(LocalizedText("Shutdown"));
      values.push_back(LocalizedText("Test"));
      values.push_back(LocalizedText("CommunicationFault"));
      values.push_back(LocalizedText("Unknown"));
      EnumStrings(ObjectID::ServerStateEnumStrings, values);
    }

    void ExpandedNodeID()
    {
      // Base Attributes
      AddValue(ObjectID::ExpandedNodeID, AttributeID::NODE_ID,      NodeID(ObjectID::ExpandedNodeID));
      AddValue(ObjectID::ExpandedNodeID, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ExpandedNodeID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ExpandedNodeID));
      AddValue(ObjectID::ExpandedNodeID, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ExpandedNodeID));
      AddValue(ObjectID::ExpandedNodeID, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ExpandedNodeID));
      AddValue(ObjectID::ExpandedNodeID, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ExpandedNodeID, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ExpandedNodeID, AttributeID::IS_ABSTRACT, false);
    }

    void Guid()
    {
      // Base Attributes
      AddValue(ObjectID::Guid, AttributeID::NODE_ID,      NodeID(ObjectID::Guid));
      AddValue(ObjectID::Guid, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Guid, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Guid));
      AddValue(ObjectID::Guid, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Guid));
      AddValue(ObjectID::Guid, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Guid));
      AddValue(ObjectID::Guid, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Guid, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Guid, AttributeID::IS_ABSTRACT, false);
    }

    void LocalizedTextType()
    {
      // Base Attributes
      AddValue(ObjectID::LocalizedText, AttributeID::NODE_ID,      NodeID(ObjectID::LocalizedText));
      AddValue(ObjectID::LocalizedText, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::LocalizedText, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocalizedText));
      AddValue(ObjectID::LocalizedText, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::LocalizedText));
      AddValue(ObjectID::LocalizedText, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::LocalizedText));
      AddValue(ObjectID::LocalizedText, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::LocalizedText, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::LocalizedText, AttributeID::IS_ABSTRACT, false);
    }

    void NodeIDType()
    {
      // Base Attributes
      AddValue(ObjectID::NodeID, AttributeID::NODE_ID,      NodeID(ObjectID::NodeID));
      AddValue(ObjectID::NodeID, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::NodeID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NodeID));
      AddValue(ObjectID::NodeID, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::NodeID));
      AddValue(ObjectID::NodeID, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::NodeID));
      AddValue(ObjectID::NodeID, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::NodeID, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::NodeID, AttributeID::IS_ABSTRACT, false);
    }

    void NumberType()
    {
      // Base Attributes
      AddValue(ObjectID::Number, AttributeID::NODE_ID,      NodeID(ObjectID::Number));
      AddValue(ObjectID::Number, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Number, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Number));
      AddValue(ObjectID::Number, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Number));
      AddValue(ObjectID::Number, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Number));
      AddValue(ObjectID::Number, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Number, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Number, AttributeID::IS_ABSTRACT, true);
      // References
      AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::Double,   Names::Double,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::Float,    Names::Float,    NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::Integer,  Names::Integer,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::UInteger, Names::UInteger, NodeClass::DataType, ObjectID::Null);
    }

    void DoubleType()
    {
      // Base Attributes
      AddValue(ObjectID::Double, AttributeID::NODE_ID,      NodeID(ObjectID::Number));
      AddValue(ObjectID::Double, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Double, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Number));
      AddValue(ObjectID::Double, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Number));
      AddValue(ObjectID::Double, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Number));
      AddValue(ObjectID::Double, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Double, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Double, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::Double, forward, ReferenceID::HasSubtype, ObjectID::Duration,   Names::Duration,   NodeClass::DataType, ObjectID::Null);
    }

    void DurationType()
    {
      // Base Attributes
      AddValue(ObjectID::Duration, AttributeID::NODE_ID,      NodeID(ObjectID::Duration));
      AddValue(ObjectID::Duration, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Duration, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Duration));
      AddValue(ObjectID::Duration, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Duration));
      AddValue(ObjectID::Duration, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Duration));
      AddValue(ObjectID::Duration, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Duration, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Duration, AttributeID::IS_ABSTRACT, false);
    }

    void FloatType()
    {
      // Base Attributes
      AddValue(ObjectID::Float, AttributeID::NODE_ID,      NodeID(ObjectID::Float));
      AddValue(ObjectID::Float, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Float, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Float));
      AddValue(ObjectID::Float, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Float));
      AddValue(ObjectID::Float, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Float));
      AddValue(ObjectID::Float, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Float, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Float, AttributeID::IS_ABSTRACT, false);
    }

    void IntegerType()
    {
      // Base Attributes
      AddValue(ObjectID::Integer, AttributeID::NODE_ID,      NodeID(ObjectID::Integer));
      AddValue(ObjectID::Integer, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Integer, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Integer));
      AddValue(ObjectID::Integer, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Integer));
      AddValue(ObjectID::Integer, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Integer));
      AddValue(ObjectID::Integer, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Integer, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Integer, AttributeID::IS_ABSTRACT, true);
      // References
      AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::Int16,   Names::Int16,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::Int32,   Names::Int32,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::Int64,   Names::Int64,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::SByte,   Names::SByte,   NodeClass::DataType, ObjectID::Null);
    }

    void Int16Type()
    {
      // Base Attributes
      AddValue(ObjectID::Int16, AttributeID::NODE_ID,      NodeID(ObjectID::Int16));
      AddValue(ObjectID::Int16, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Int16, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Int16));
      AddValue(ObjectID::Int16, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Int16));
      AddValue(ObjectID::Int16, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Int16));
      AddValue(ObjectID::Int16, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Int16, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Int16, AttributeID::IS_ABSTRACT, false);
    }

    void Int32Type()
    {
      // Base Attributes
      AddValue(ObjectID::Int32, AttributeID::NODE_ID,      NodeID(ObjectID::Int32));
      AddValue(ObjectID::Int32, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Int32, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Int32));
      AddValue(ObjectID::Int32, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Int32));
      AddValue(ObjectID::Int32, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Int32));
      AddValue(ObjectID::Int32, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Int32, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Int32, AttributeID::IS_ABSTRACT, false);
    }

    void Int64Type()
    {
      // Base Attributes
      AddValue(ObjectID::Int64, AttributeID::NODE_ID,      NodeID(ObjectID::Int64));
      AddValue(ObjectID::Int64, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Int64, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Int64));
      AddValue(ObjectID::Int64, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Int64));
      AddValue(ObjectID::Int64, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Int64));
      AddValue(ObjectID::Int64, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Int64, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Int64, AttributeID::IS_ABSTRACT, false);
    }

    void SByteType()
    {
      // Base Attributes
      AddValue(ObjectID::SByte, AttributeID::NODE_ID,      NodeID(ObjectID::SByte));
      AddValue(ObjectID::SByte, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SByte, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SByte));
      AddValue(ObjectID::SByte, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SByte));
      AddValue(ObjectID::SByte, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SByte));
      AddValue(ObjectID::SByte, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SByte, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SByte, AttributeID::IS_ABSTRACT, false);
    }

    void UIntegerType()
    {
      // Base Attributes
      AddValue(ObjectID::UInteger, AttributeID::NODE_ID,      NodeID(ObjectID::UInteger));
      AddValue(ObjectID::UInteger, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::UInteger, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInteger));
      AddValue(ObjectID::UInteger, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInteger));
      AddValue(ObjectID::UInteger, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInteger));
      AddValue(ObjectID::UInteger, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UInteger, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::UInteger, AttributeID::IS_ABSTRACT, true);
      // References
      AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::Byte,     Names::SByte,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::UInt16,   Names::Int16,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::UInt32,   Names::Int32,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::UInt64,   Names::Int64,   NodeClass::DataType, ObjectID::Null);
    }

    void UInt16Type()
    {
      // Base Attributes
      AddValue(ObjectID::UInt16, AttributeID::NODE_ID,      NodeID(ObjectID::UInt16));
      AddValue(ObjectID::UInt16, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::UInt16, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInt16));
      AddValue(ObjectID::UInt16, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInt16));
      AddValue(ObjectID::UInt16, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInt16));
      AddValue(ObjectID::UInt16, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UInt16, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::UInt16, AttributeID::IS_ABSTRACT, false);
    }

    void UInt32Type()
    {
      // Base Attributes
      AddValue(ObjectID::UInt32, AttributeID::NODE_ID,      NodeID(ObjectID::UInt32));
      AddValue(ObjectID::UInt32, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::UInt32, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInt32));
      AddValue(ObjectID::UInt32, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInt32));
      AddValue(ObjectID::UInt32, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInt32));
      AddValue(ObjectID::UInt32, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UInt32, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::UInt32, AttributeID::IS_ABSTRACT, false);
    }

    void UInt64Type()
    {
      // Base Attributes
      AddValue(ObjectID::UInt64, AttributeID::NODE_ID,      NodeID(ObjectID::UInt64));
      AddValue(ObjectID::UInt64, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::UInt64, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInt64));
      AddValue(ObjectID::UInt64, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInt64));
      AddValue(ObjectID::UInt64, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInt64));
      AddValue(ObjectID::UInt64, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UInt64, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::UInt64, AttributeID::IS_ABSTRACT, false);
    }

    void ByteType()
    {
      // Base Attributes
      AddValue(ObjectID::Byte, AttributeID::NODE_ID,      NodeID(ObjectID::Byte));
      AddValue(ObjectID::Byte, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Byte, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Byte));
      AddValue(ObjectID::Byte, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Byte));
      AddValue(ObjectID::Byte, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Byte));
      AddValue(ObjectID::Byte, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Byte, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Byte, AttributeID::IS_ABSTRACT, false);
    }

    void QualifiedNameType()
    {
      // Base Attributes
      AddValue(ObjectID::QualifiedName, AttributeID::NODE_ID,      NodeID(ObjectID::Byte));
      AddValue(ObjectID::QualifiedName, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::QualifiedName, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::QualifiedName));
      AddValue(ObjectID::QualifiedName, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::QualifiedName));
      AddValue(ObjectID::QualifiedName, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::QualifiedName));
      AddValue(ObjectID::QualifiedName, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::QualifiedName, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::QualifiedName, AttributeID::IS_ABSTRACT, false);
    }

    void StatusCodeType()
    {
      // Base Attributes
      AddValue(ObjectID::StatusCode, AttributeID::NODE_ID,      NodeID(ObjectID::StatusCode));
      AddValue(ObjectID::StatusCode, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StatusCode, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::StatusCode));
      AddValue(ObjectID::StatusCode, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::StatusCode));
      AddValue(ObjectID::StatusCode, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::StatusCode));
      AddValue(ObjectID::StatusCode, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StatusCode, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StatusCode, AttributeID::IS_ABSTRACT, false);
    }

    void StringType()
    {
      // Base Attributes
      AddValue(ObjectID::String, AttributeID::NODE_ID,      NodeID(ObjectID::String));
      AddValue(ObjectID::String, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::String, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::String));
      AddValue(ObjectID::String, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::String));
      AddValue(ObjectID::String, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::String));
      AddValue(ObjectID::String, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::String, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::String, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::String, forward, ReferenceID::HasSubtype, ObjectID::LocaleID,   Names::LocaleID,   NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::String, forward, ReferenceID::HasSubtype, ObjectID::NumericRange,   Names::NumericRange,   NodeClass::DataType, ObjectID::Null);
    }

    void LocaleIDType()
    {
      // Base Attributes
      AddValue(ObjectID::LocaleID, AttributeID::NODE_ID,      NodeID(ObjectID::LocaleID));
      AddValue(ObjectID::LocaleID, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::LocaleID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocaleID));
      AddValue(ObjectID::LocaleID, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::LocaleID));
      AddValue(ObjectID::LocaleID, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::LocaleID));
      AddValue(ObjectID::LocaleID, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::LocaleID, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::LocaleID, AttributeID::IS_ABSTRACT, false);
    }

    void NumericRangeType()
    {
      // Base Attributes
      AddValue(ObjectID::NumericRange, AttributeID::NODE_ID,      NodeID(ObjectID::NumericRange));
      AddValue(ObjectID::NumericRange, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::NumericRange, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NumericRange));
      AddValue(ObjectID::NumericRange, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::NumericRange));
      AddValue(ObjectID::NumericRange, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::NumericRange));
      AddValue(ObjectID::NumericRange, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::NumericRange, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::NumericRange, AttributeID::IS_ABSTRACT, false);
    }

    void StructureType()
    {
      // Base Attributes
      AddValue(ObjectID::Structure, AttributeID::NODE_ID,      NodeID(ObjectID::Structure));
      AddValue(ObjectID::Structure, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Structure, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Structure));
      AddValue(ObjectID::Structure, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Structure));
      AddValue(ObjectID::Structure, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Structure));
      AddValue(ObjectID::Structure, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Structure, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Structure, AttributeID::IS_ABSTRACT, true);
      // References
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::AddNodesItem,      Names::AddNodesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::AddReferencesItem, Names::AddReferencesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::ApplicationDescription, Names::ApplicationDescription, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::Argument,          Names::Argument,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::BuildInfo,         Names::BuildInfo, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::DeleteNodesItem,   Names::DeleteNodesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::DeleteReferencesItem, Names::DeleteReferencesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::EUInformation,     Names::EUInformation, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::ModelChangeStructureDataType, Names::ModelChangeStructureDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::Range,             Names::Range, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsDataType, Names::SamplingIntervalDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::SemanticChangeStructureDataType, Names::SemanticChangeStructureDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsSummaryDataType, Names::ServerDiagnosticsSummaryDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::ServerStatusDataType, Names::ServerStatusDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::ServiceCounterDataType, Names::ServiceCounterDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsDataType, Names::SessionDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsDataType, Names::SessionSecurityDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::SignedSoftwareCertificate, Names::SignedSoftwareCertificate, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StatusResult, Names::StatusResult, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsDataType, Names::SubscriptionDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::UserIdentifyToken, Names::UserIdentifyToken, NodeClass::DataType, ObjectID::Null);
    }

    void AddNodesItem()
    {
      // Base Attributes
      AddValue(ObjectID::AddNodesItem, AttributeID::NODE_ID,      NodeID(ObjectID::AddNodesItem));
      AddValue(ObjectID::AddNodesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::AddNodesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AddNodesItem));
      AddValue(ObjectID::AddNodesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AddNodesItem));
      AddValue(ObjectID::AddNodesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AddNodesItem));
      AddValue(ObjectID::AddNodesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::AddNodesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::AddNodesItem, AttributeID::IS_ABSTRACT, false);
    }

    void AddReferencesItem()
    {
      // Base Attributes
      AddValue(ObjectID::AddReferencesItem, AttributeID::NODE_ID,      NodeID(ObjectID::AddReferencesItem));
      AddValue(ObjectID::AddReferencesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::AddReferencesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AddReferencesItem));
      AddValue(ObjectID::AddReferencesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AddReferencesItem));
      AddValue(ObjectID::AddReferencesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AddReferencesItem));
      AddValue(ObjectID::AddReferencesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::AddReferencesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::AddReferencesItem, AttributeID::IS_ABSTRACT, false);
    }

    void ApplicationDescriptionType()
    {
      // Base Attributes
      AddValue(ObjectID::ApplicationDescription, AttributeID::NODE_ID,      NodeID(ObjectID::ApplicationDescription));
      AddValue(ObjectID::ApplicationDescription, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ApplicationDescription, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ApplicationDescription));
      AddValue(ObjectID::ApplicationDescription, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ApplicationDescription));
      AddValue(ObjectID::ApplicationDescription, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ApplicationDescription));
      AddValue(ObjectID::ApplicationDescription, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ApplicationDescription, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ApplicationDescription, AttributeID::IS_ABSTRACT, false);
    }

    void Argument()
    {
      // Base Attributes
      AddValue(ObjectID::Argument, AttributeID::NODE_ID,      NodeID(ObjectID::Argument));
      AddValue(ObjectID::Argument, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Argument, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Argument));
      AddValue(ObjectID::Argument, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Argument));
      AddValue(ObjectID::Argument, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Argument));
      AddValue(ObjectID::Argument, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Argument, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Argument, AttributeID::IS_ABSTRACT, false);
    }

    void BuildInfo()
    {
      // Base Attributes
      AddValue(ObjectID::BuildInfo, AttributeID::NODE_ID,      NodeID(ObjectID::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::BuildInfo, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BuildInfo, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::BuildInfo, AttributeID::IS_ABSTRACT, false);
    }

    void DeleteNodesItem()
    {
      // Base Attributes
      AddValue(ObjectID::DeleteNodesItem, AttributeID::NODE_ID,      NodeID(ObjectID::DeleteNodesItem));
      AddValue(ObjectID::DeleteNodesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::DeleteNodesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EUInformation));
      AddValue(ObjectID::DeleteNodesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::DeleteNodesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::DeleteNodesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DeleteNodesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::DeleteNodesItem, AttributeID::IS_ABSTRACT, false);
    }

    void DeleteReferencesItem()
    {
      // Base Attributes
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::NODE_ID,      NodeID(ObjectID::DeleteReferencesItem));
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DeleteReferencesItem));
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::DeleteReferencesItem));
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::DeleteReferencesItem));
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::DeleteReferencesItem, AttributeID::IS_ABSTRACT, false);
    }

    void EUInformation()
    {
      // Base Attributes
      AddValue(ObjectID::EUInformation, AttributeID::NODE_ID,      NodeID(ObjectID::EUInformation));
      AddValue(ObjectID::EUInformation, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::EUInformation, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EUInformation));
      AddValue(ObjectID::EUInformation, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::EUInformation, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::EUInformation, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::EUInformation, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::EUInformation, AttributeID::IS_ABSTRACT, false);
    }

    void ModelChangeStructureDataType()
    {
      // Base Attributes
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::NODE_ID,      NodeID(ObjectID::ModelChangeStructureDataType));
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModelChangeStructureDataType));
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ModelChangeStructureDataType));
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ModelChangeStructureDataType));
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ModelChangeStructureDataType, AttributeID::IS_ABSTRACT, false);
    }

    void Range()
    {
      // Base Attributes
      AddValue(ObjectID::Range, AttributeID::NODE_ID,      NodeID(ObjectID::Range));
      AddValue(ObjectID::Range, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::Range, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Range));
      AddValue(ObjectID::Range, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Range));
      AddValue(ObjectID::Range, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Range));
      AddValue(ObjectID::Range, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Range, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Range, AttributeID::IS_ABSTRACT, false);
    }

    void SamplingIntervalDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SamplingIntervalDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void SemanticChangeStructureDataType()
    {
      // Base Attributes
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::NODE_ID,      NodeID(ObjectID::SemanticChangeStructureDataType));
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SemanticChangeStructureDataType));
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SemanticChangeStructureDataType));
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SemanticChangeStructureDataType));
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SemanticChangeStructureDataType, AttributeID::IS_ABSTRACT, false);
    }

    void ServerDiagnosticsSummaryDataType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnosticsSummaryDataType));
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServerDiagnosticsSummaryDataType));
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServerDiagnosticsSummaryDataType));
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServerDiagnosticsSummaryDataType));
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerDiagnosticsSummaryDataType, AttributeID::IS_ABSTRACT, false);
    }

    void ServerStatusDataType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerStatusDataType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerStatusDataType));
      AddValue(ObjectID::ServerStatusDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ServerStatusDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServerStatusDataType));
      AddValue(ObjectID::ServerStatusDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServerStatusDataType));
      AddValue(ObjectID::ServerStatusDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServerStatusDataType));
      AddValue(ObjectID::ServerStatusDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerStatusDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerStatusDataType, AttributeID::IS_ABSTRACT, false);
    }

    void ServiceCounterDataType()
    {
      // Base Attributes
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::NODE_ID,      NodeID(ObjectID::ServiceCounterDataType));
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServiceCounterDataType));
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServiceCounterDataType));
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServiceCounterDataType));
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServiceCounterDataType, AttributeID::IS_ABSTRACT, false);
    }

    void SessionDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::SessionDiagnosticsDataType));
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SessionDiagnosticsDataType));
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SessionDiagnosticsDataType));
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SessionDiagnosticsDataType));
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SessionDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void SessionSecurityDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SessionSecurityDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void SignedSoftwareCertificate()
    {
      // Base Attributes
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::NODE_ID,      NodeID(ObjectID::SignedSoftwareCertificate));
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SignedSoftwareCertificate));
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SignedSoftwareCertificate));
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SignedSoftwareCertificate));
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SignedSoftwareCertificate, AttributeID::IS_ABSTRACT, false);
    }

    void StatusResult()
    {
      // Base Attributes
      AddValue(ObjectID::StatusResult, AttributeID::NODE_ID,      NodeID(ObjectID::StatusResult));
      AddValue(ObjectID::StatusResult, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StatusResult, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::StatusResult));
      AddValue(ObjectID::StatusResult, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::StatusResult));
      AddValue(ObjectID::StatusResult, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::StatusResult));
      AddValue(ObjectID::StatusResult, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StatusResult, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StatusResult, AttributeID::IS_ABSTRACT, false);
    }

    void SubscriptionDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SubscriptionDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void UserIdentifyTokenType()
    {
      // Base Attributes
      AddValue(ObjectID::UserIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::UserIdentifyToken));
      AddValue(ObjectID::UserIdentifyToken, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::UserIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::UserIdentifyToken));
      AddValue(ObjectID::UserIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UserIdentifyToken));
      AddValue(ObjectID::UserIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UserIdentifyToken));
      AddValue(ObjectID::UserIdentifyToken, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UserIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::UserIdentifyToken, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::UserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::AnonymousIdentifyToken, Names::AnonymousIdentifyToken, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::UserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::UserNameIdentifyToken,  Names::UserNameIdentifyToken,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::UserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::X509IdentifyToken,      Names::X509IdentifyToken,      NodeClass::DataType, ObjectID::Null);
    }

    void AnonymousIdentifyToken()
    {
      // Base Attributes
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::AnonymousIdentifyToken));
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::AnonymousIdentifyToken));
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AnonymousIdentifyToken));
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AnonymousIdentifyToken));
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::AnonymousIdentifyToken, AttributeID::IS_ABSTRACT, false);
    }

    void UserNameIdentifyToken()
    {
      // Base Attributes
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::UserNameIdentifyToken));
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::UserNameIdentifyToken));
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UserNameIdentifyToken));
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UserNameIdentifyToken));
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::UserNameIdentifyToken, AttributeID::IS_ABSTRACT, false);
    }

    void X509IdentifyToken()
    {
      // Base Attributes
      AddValue(ObjectID::X509IdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::X509IdentifyToken));
      AddValue(ObjectID::X509IdentifyToken, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::X509IdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::X509IdentifyToken));
      AddValue(ObjectID::X509IdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::X509IdentifyToken));
      AddValue(ObjectID::X509IdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::X509IdentifyToken));
      AddValue(ObjectID::X509IdentifyToken, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::X509IdentifyToken, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::X509IdentifyToken, AttributeID::IS_ABSTRACT, false);
    }

    void XmlElement()
    {
      // Base Attributes
      AddValue(ObjectID::XmlElement, AttributeID::NODE_ID,      NodeID(ObjectID::XmlElement));
      AddValue(ObjectID::XmlElement, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::XmlElement, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::XmlElement));
      AddValue(ObjectID::XmlElement, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::XmlElement));
      AddValue(ObjectID::XmlElement, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::XmlElement));
      AddValue(ObjectID::XmlElement, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::XmlElement, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::XmlElement, AttributeID::IS_ABSTRACT, false);
    }

    void EventTypes()
    {
      // Base Attributes
      AddValue(ObjectID::EventTypes, AttributeID::NODE_ID,      NodeID(ObjectID::EventTypes));
      AddValue(ObjectID::EventTypes, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::EventTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EventTypes));
      AddValue(ObjectID::EventTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EventTypes));
      AddValue(ObjectID::EventTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EventTypes));
      AddValue(ObjectID::EventTypes, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::EventTypes, AttributeID::USER_WRITE_MASK, 0);

      // References
      AddReference(ObjectID::EventTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::EventTypes, forward, ReferenceID::Organizes, ObjectID::BaseEventType, Names::BaseEventType, NodeClass::ObjectType, ObjectID::Null);
    }

    void BaseEventType()
    {
      // Base Attributes
      AddValue(ObjectID::BaseEventType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseEventType));
      AddValue(ObjectID::BaseEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::BaseEventType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseEventType));
      AddValue(ObjectID::BaseEventType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseEventType));
      AddValue(ObjectID::BaseEventType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseEventType));
      AddValue(ObjectID::BaseEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BaseEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::BaseEventType, AttributeID::IS_ABSTRACT, true);

      // References
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasSubtype, ObjectID::BaseModelChangeEventType, Names::BaseModelChangeEventType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasSubtype, ObjectID::SemanticChangeEventType, Names::SemanticChangeEventType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasSubtype, ObjectID::SystemEventType, Names::SystemEventType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::EventID, Names::EventID, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::EventType, Names::EventType, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::LocalTime, Names::LocalTime, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::Message, Names::Message, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::ReceiveTime, Names::ReceiveTime, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::Severity, Names::Severity, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::SourceName, Names::SourceName, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::SourceNode, Names::SourceNode, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::BaseEventType, forward, ReferenceID::HasProperty, ObjectID::Time, Names::Time, NodeClass::Variable, ObjectID::PropertyType);
    }

    void BaseModelChangeEventType()
    {
      // Base Attributes
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseModelChangeEventType));
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::BaseModelChangeEventType));
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::BaseModelChangeEventType));
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::BaseModelChangeEventType));
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::IS_ABSTRACT, false);
    }

    void SemanticChangeEventType()
    {
      // Base Attributes
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::NODE_ID,      NodeID(ObjectID::SemanticChangeEventType));
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SemanticChangeEventType));
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SemanticChangeEventType));
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SemanticChangeEventType));
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::IS_ABSTRACT, false);
    }

    void SystemEventType()
    {
      // Base Attributes
      AddValue(ObjectID::SystemEventType, AttributeID::NODE_ID,      NodeID(ObjectID::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::SystemEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SystemEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SystemEventType, AttributeID::IS_ABSTRACT, false);
    }

    void EventID()
    {
      // Base Attributes
      AddValue(ObjectID::EventID, AttributeID::NODE_ID,      NodeID(ObjectID::EventID));
      AddValue(ObjectID::EventID, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::EventID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EventID));
      AddValue(ObjectID::EventID, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EventID));
      AddValue(ObjectID::EventID, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EventID));
      AddValue(ObjectID::EventID, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::EventID, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes

      AddValue(ObjectID::EventID, AttributeID::VALUE, ByteString());
      AddValue(ObjectID::EventID, AttributeID::DATA_TYPE, NodeID(ObjectID::ByteString));
      AddValue(ObjectID::EventID, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::EventID, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::EventID, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::EventID, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::EventID, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::EventID, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::EventID, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::EventID, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void EventType()
    {
      // Base Attributes
      AddValue(ObjectID::EventType, AttributeID::NODE_ID,      NodeID(ObjectID::EventType));
      AddValue(ObjectID::EventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::EventType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EventType));
      AddValue(ObjectID::EventType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EventType));
      AddValue(ObjectID::EventType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EventType));
      AddValue(ObjectID::EventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::EventType, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes

      AddValue(ObjectID::EventType, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::EventType, AttributeID::DATA_TYPE, NodeID(ObjectID::NodeID));
      AddValue(ObjectID::EventType, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::EventType, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::EventType, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::EventType, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::EventType, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::EventType, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::EventType, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::EventType, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }
    void LocalTime()
    {
      // Base Attributes
      AddValue(ObjectID::LocalTime, AttributeID::NODE_ID,      NodeID(ObjectID::LocalTime));
      AddValue(ObjectID::LocalTime, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::LocalTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocalTime));
      AddValue(ObjectID::LocalTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::LocalTime));
      AddValue(ObjectID::LocalTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::LocalTime));
      AddValue(ObjectID::LocalTime, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::LocalTime, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes

      AddValue(ObjectID::LocalTime, AttributeID::VALUE, DateTime());
      AddValue(ObjectID::LocalTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::LocalTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::LocalTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::LocalTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::LocalTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::LocalTime, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::LocalTime, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::LocalTime, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::LocalTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void Message()
    {
      // Base Attributes
      AddValue(ObjectID::Message, AttributeID::NODE_ID,      NodeID(ObjectID::Message));
      AddValue(ObjectID::Message, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::Message, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Message));
      AddValue(ObjectID::Message, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Message));
      AddValue(ObjectID::Message, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Message));
      AddValue(ObjectID::Message, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Message, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::Message, AttributeID::VALUE, LocalizedText());
      AddValue(ObjectID::Message, AttributeID::DATA_TYPE, NodeID(ObjectID::LocalizedText));
      AddValue(ObjectID::Message, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::Message, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Message, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Message, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::Message, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::Message, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::Message, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Message, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void ReceiveTime()
    {
      // Base Attributes
      AddValue(ObjectID::ReceiveTime, AttributeID::NODE_ID,      NodeID(ObjectID::ReceiveTime));
      AddValue(ObjectID::ReceiveTime, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ReceiveTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ReceiveTime));
      AddValue(ObjectID::ReceiveTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ReceiveTime));
      AddValue(ObjectID::ReceiveTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ReceiveTime));
      AddValue(ObjectID::ReceiveTime, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ReceiveTime, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ReceiveTime, AttributeID::VALUE, DateTime());
      AddValue(ObjectID::ReceiveTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::ReceiveTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ReceiveTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ReceiveTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ReceiveTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ReceiveTime, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ReceiveTime, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ReceiveTime, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ReceiveTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void Severity()
    {
      // Base Attributes
      AddValue(ObjectID::Severity, AttributeID::NODE_ID,      NodeID(ObjectID::Severity));
      AddValue(ObjectID::Severity, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::Severity, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Severity));
      AddValue(ObjectID::Severity, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Severity));
      AddValue(ObjectID::Severity, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Severity));
      AddValue(ObjectID::Severity, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Severity, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::Severity, AttributeID::VALUE, uint16_t());
      AddValue(ObjectID::Severity, AttributeID::DATA_TYPE, NodeID(ObjectID::UInt16));
      AddValue(ObjectID::Severity, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::Severity, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Severity, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Severity, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::Severity, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::Severity, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::Severity, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Severity, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void SourceName()
    {
      // Base Attributes
      AddValue(ObjectID::SourceName, AttributeID::NODE_ID,      NodeID(ObjectID::SourceName));
      AddValue(ObjectID::SourceName, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SourceName, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SourceName));
      AddValue(ObjectID::SourceName, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SourceName));
      AddValue(ObjectID::SourceName, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SourceName));
      AddValue(ObjectID::SourceName, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SourceName, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SourceName, AttributeID::VALUE, std::string());
      AddValue(ObjectID::SourceName, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::SourceName, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::SourceName, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SourceName, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SourceName, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SourceName, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SourceName, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::SourceName, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::SourceName, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void SourceNode()
    {
      // Base Attributes
      AddValue(ObjectID::SourceNode, AttributeID::NODE_ID,      NodeID(ObjectID::SourceNode));
      AddValue(ObjectID::SourceNode, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SourceNode, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SourceNode));
      AddValue(ObjectID::SourceNode, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SourceNode));
      AddValue(ObjectID::SourceNode, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SourceNode));
      AddValue(ObjectID::SourceNode, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SourceNode, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SourceNode, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::SourceNode, AttributeID::DATA_TYPE, NodeID(ObjectID::NodeID));
      AddValue(ObjectID::SourceNode, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::SourceNode, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SourceNode, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SourceNode, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SourceNode, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SourceNode, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::SourceNode, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::SourceNode, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void Time()
    {
      // Base Attributes
      AddValue(ObjectID::Time, AttributeID::NODE_ID,      NodeID(ObjectID::Time));
      AddValue(ObjectID::Time, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::Time, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Time));
      AddValue(ObjectID::Time, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Time));
      AddValue(ObjectID::Time, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Time));
      AddValue(ObjectID::Time, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Time, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::Time, AttributeID::VALUE, DateTime());
      AddValue(ObjectID::Time, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::Time, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::Time, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Time, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Time, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::Time, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::Time, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::Time, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Time, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
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
