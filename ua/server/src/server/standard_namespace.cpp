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
      std::vector<StatusCode> statuses;
      for (auto value : values)
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
                     StructureAddNodesItem();
                     StructureAddReferencesItem();
                     StructureApplicationDescriptionType();
                     StructureArgument();
                     StructureBuildInfo();
                     StructureDeleteNodesItem();
                     StructureDeleteReferencesItem();
                     StructureEUInformation();
                     StructureModelChangeStructureDataType();
                     StructureRange();
                     StructureSamplingIntervalDiagnosticsDataType();
                     StructureSemanticChangeStructureDataType();
                     StructureServerDiagnosticsSummaryDataType();
                     StructureServerStatusDataType();
                     StructureServiceCounterDataType();
                     StructureSessionDiagnosticsDataType();
                     StructureSessionSecurityDiagnosticsDataType();
                     StructureSignedSoftwareCertificate();
                     StructureStatusResult();
                     StructureSubscriptionDiagnosticsDataType();
                     StructureUserIdentifyTokenType();
                       AnonymousIdentifyToken();
                       UserNameIdentifyToken();
                       X509IdentifyToken();
                   XmlElement();
         EventTypes();
           BaseEventType();
             BaseModelChangeEventType();
             SemanticChangeEventType();
             SystemEventType();
               DeviceFailureEventType();
               RefreshEndEventType();
               RefreshRequiredEventType();
               RefreshStartEventType();
             EventID();
             EventType();
             LocalTime();
             Message();
             ReceiveTime();
             Severity();
             SourceName();
             SourceNode();
             Time();
         ObjectTypes();
           BaseObjectType();
             DataTypeEncodingType();
             DataTypeSystemType();
             FolderType();
             HistoricalEventConfigurationType();
             ModellingRuleType();
               NamingRule();
             ServerCapabilitiesType();
               AggregateFunctions();
               ModelingRules();
               LocaleIDArray();
               MaxBrowseContinuationPoints();
               MaxHistoryContinuationPoints();
               MinSupportedSampleRate();
               ServerProfileArray();
               SoftwareCertificates();
             ServerDiagnosticsType();
               EnableFlag();
               SamplingIntervalDiagnosticsArray();
               ServerDiagnosticsSummary();
               SessionsDiagnosticsSummary();
               SubscriptionDiagnosticsArray();
             ServerRedundancyType();
               RedundancySupportTypeRedundancySupport();
             ServerType();
               Auditing();
               NamespaceArray();
               ServerArray();
               ServerCapabilities();
               ServerDiagnostics();
               ServerRedundancy();
               ServerStatus();
                 BuildInfo();
                   BuildDate();
                   BuildNumber();
                   ManufacturerName();
                   ProductName();
                   ProductURI();
                   SoftwareVersion();
                 CurrentTime();
                 SecondsTillShutdown();
                 ShutdownReason();
                 StartTime();
                 State();
               ServiceLevel();
               VendorServerInfo();
             SessionDiagnosticsObjectType();
             SessionDiagnosticsSummaryType();
             StateType();
               StateNumber();
             TransitionType();
             VendorServerInfoType();
         ReferenceTypes();
           Refs();
             HierarchicalReferences();
               HasChild();
                 Aggregates();
                   HasComponent();
                     HasOrderedComponent();
                   HasHistoricalConfiguration();
                   HasProperty();
                 HasSubtype();
               HasEventSource();
                 HasNotifier();
               Organizes();
             NonHierarchicalReferences();
               FromState();
               GeneratesEvent();
               HasCause();
               HasDescription();
               HasEffect();
               HasEncoding();
               HasModelParent();
               HasModellingRule();
               HasTypeDefinition();
               ToState();
         VariableTypes();
           BaseVariableType();
             BaseDataVariableType();
               DataTypeDescriptionType();
               DataTypeDictionaryType();
               SamplingIntervalDiagnosticsArrayType();
               SamplingIntervalDiagnosticsType();
               ServerDiagnosticsSummaryType();
               ServerStatusType();
               ServerVendorCapabilityType();
               SessionsDiagnosticsArrayType();
               ServerDiagnosticsVariableType();
               SessionSecurityDiagnosticsArrayType();
               SessionSecurityDiagnosticsType();
               SubscriptionDiagnosticsArrayType();
               SubscriptionDiagnosticsType();
             PropertyType();
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
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureAddNodesItem,      Names::AddNodesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureAddReferencesItem, Names::AddReferencesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureApplicationDescription, Names::ApplicationDescription, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureArgument,          Names::Argument,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureBuildInfo,         Names::BuildInfo, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureDeleteNodesItem,   Names::DeleteNodesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureDeleteReferencesItem, Names::DeleteReferencesItem, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureEUInformation,     Names::EUInformation, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureModelChangeStructureDataType, Names::ModelChangeStructureDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureRange,             Names::Range, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureSamplingIntervalDiagnosticsDataType, Names::SamplingIntervalDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureSemanticChangeStructureDataType, Names::SemanticChangeStructureDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureServerDiagnosticsSummaryType, Names::ServerDiagnosticsSummaryType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureServerStatusDataType, Names::ServerStatusDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureServiceCounterDataType, Names::ServiceCounterDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureSessionDiagnosticsDataType, Names::SessionDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureSessionSecurityDiagnosticsDataType, Names::SessionSecurityDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureSignedSoftwareCertificate, Names::SignedSoftwareCertificate, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureStatusResult, Names::StatusResult, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureSubscriptionDiagnosticsDataType, Names::SubscriptionDiagnosticsDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Structure, forward, ReferenceID::HasSubtype, ObjectID::StructureUserIdentifyToken, Names::UserIdentifyToken, NodeClass::DataType, ObjectID::Null);
    }

    void StructureAddNodesItem()
    {
      // Base Attributes
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureAddNodesItem));
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AddNodesItem));
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AddNodesItem));
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AddNodesItem));
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureAddNodesItem, AttributeID::IS_ABSTRACT, false);
    }

    void StructureAddReferencesItem()
    {
      // Base Attributes
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureAddReferencesItem));
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AddReferencesItem));
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AddReferencesItem));
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AddReferencesItem));
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureAddReferencesItem, AttributeID::IS_ABSTRACT, false);
    }

    void StructureApplicationDescriptionType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::NODE_ID,      NodeID(ObjectID::StructureApplicationDescription));
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ApplicationDescription));
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ApplicationDescription));
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ApplicationDescription));
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureApplicationDescription, AttributeID::IS_ABSTRACT, false);
    }

    void StructureArgument()
    {
      // Base Attributes
      AddValue(ObjectID::StructureArgument, AttributeID::NODE_ID,      NodeID(ObjectID::StructureArgument));
      AddValue(ObjectID::StructureArgument, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureArgument, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Argument));
      AddValue(ObjectID::StructureArgument, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Argument));
      AddValue(ObjectID::StructureArgument, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Argument));
      AddValue(ObjectID::StructureArgument, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureArgument, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureArgument, AttributeID::IS_ABSTRACT, false);
    }

    void StructureBuildInfo()
    {
      // Base Attributes
      AddValue(ObjectID::StructureBuildInfo, AttributeID::NODE_ID,      NodeID(ObjectID::StructureBuildInfo));
      AddValue(ObjectID::StructureBuildInfo, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureBuildInfo, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BuildInfo));
      AddValue(ObjectID::StructureBuildInfo, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::BuildInfo));
      AddValue(ObjectID::StructureBuildInfo, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::BuildInfo));
      AddValue(ObjectID::StructureBuildInfo, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureBuildInfo, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureBuildInfo, AttributeID::IS_ABSTRACT, false);
    }

    void StructureDeleteNodesItem()
    {
      // Base Attributes
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureDeleteNodesItem));
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EUInformation));
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureDeleteNodesItem, AttributeID::IS_ABSTRACT, false);
    }

    void StructureDeleteReferencesItem()
    {
      // Base Attributes
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureDeleteReferencesItem));
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DeleteReferencesItem));
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::DeleteReferencesItem));
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::DeleteReferencesItem));
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureDeleteReferencesItem, AttributeID::IS_ABSTRACT, false);
    }

    void StructureEUInformation()
    {
      // Base Attributes
      AddValue(ObjectID::StructureEUInformation, AttributeID::NODE_ID,      NodeID(ObjectID::StructureEUInformation));
      AddValue(ObjectID::StructureEUInformation, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureEUInformation, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EUInformation));
      AddValue(ObjectID::StructureEUInformation, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::StructureEUInformation, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::EUInformation));
      AddValue(ObjectID::StructureEUInformation, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureEUInformation, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureEUInformation, AttributeID::IS_ABSTRACT, false);
    }

    void StructureModelChangeStructureDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureModelChangeStructureDataType));
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModelChangeStructureDataType));
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ModelChangeStructureDataType));
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ModelChangeStructureDataType));
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureRange()
    {
      // Base Attributes
      AddValue(ObjectID::StructureRange, AttributeID::NODE_ID,      NodeID(ObjectID::StructureRange));
      AddValue(ObjectID::StructureRange, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureRange, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Range));
      AddValue(ObjectID::StructureRange, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Range));
      AddValue(ObjectID::StructureRange, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Range));
      AddValue(ObjectID::StructureRange, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureRange, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureRange, AttributeID::IS_ABSTRACT, false);
    }

    void StructureSamplingIntervalDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsDataType));
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureSemanticChangeStructureDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSemanticChangeStructureDataType));
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SemanticChangeStructureDataType));
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SemanticChangeStructureDataType));
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SemanticChangeStructureDataType));
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureServerDiagnosticsSummaryDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureServerDiagnosticsSummaryType));
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServerDiagnosticsSummaryType));
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServerDiagnosticsSummaryType));
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServerDiagnosticsSummaryType));
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureServerStatusDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureServerStatusDataType));
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServerStatusDataType));
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServerStatusDataType));
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServerStatusDataType));
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureServerStatusDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureServiceCounterDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureServiceCounterDataType));
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServiceCounterDataType));
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServiceCounterDataType));
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServiceCounterDataType));
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureServiceCounterDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureSessionDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSessionDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SessionDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SessionDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SessionDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureSessionSecurityDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SessionSecurityDiagnosticsDataType));
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureSignedSoftwareCertificate()
    {
      // Base Attributes
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSignedSoftwareCertificate));
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SignedSoftwareCertificate));
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SignedSoftwareCertificate));
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SignedSoftwareCertificate));
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::IS_ABSTRACT, false);
    }

    void StructureStatusResult()
    {
      // Base Attributes
      AddValue(ObjectID::StructureStatusResult, AttributeID::NODE_ID,      NodeID(ObjectID::StructureStatusResult));
      AddValue(ObjectID::StructureStatusResult, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureStatusResult, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::StatusResult));
      AddValue(ObjectID::StructureStatusResult, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::StatusResult));
      AddValue(ObjectID::StructureStatusResult, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::StatusResult));
      AddValue(ObjectID::StructureStatusResult, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureStatusResult, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureStatusResult, AttributeID::IS_ABSTRACT, false);
    }

    void StructureSubscriptionDiagnosticsDataType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSubscriptionDiagnosticsDataType));
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SubscriptionDiagnosticsDataType));
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
    }

    void StructureUserIdentifyTokenType()
    {
      // Base Attributes
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::StructureUserIdentifyToken));
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::DataType));
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::UserIdentifyToken));
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UserIdentifyToken));
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UserIdentifyToken));
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StructureUserIdentifyToken, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::StructureUserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::AnonymousIdentifyToken, Names::AnonymousIdentifyToken, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::StructureUserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::UserNameIdentifyToken,  Names::UserNameIdentifyToken,  NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::StructureUserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::X509IdentifyToken,      Names::X509IdentifyToken,      NodeClass::DataType, ObjectID::Null);
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
      AddValue(ObjectID::BaseEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
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
      AddValue(ObjectID::BaseModelChangeEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
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
      AddValue(ObjectID::SemanticChangeEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
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
      AddValue(ObjectID::SystemEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::SystemEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SystemEventType));
      AddValue(ObjectID::SystemEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SystemEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SystemEventType, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::DeviceFailureEventType,   Names::DeviceFailureEventType,   NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::RefreshEndEventType,      Names::RefreshEndEventType,      NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::RefreshRequiredEventType, Names::RefreshRequiredEventType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::RefreshStartEventType,    Names::RefreshStartEventType,    NodeClass::ObjectType, ObjectID::Null);
    }

    void DeviceFailureEventType()
    {
      // Base Attributes
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::NODE_ID,      NodeID(ObjectID::DeviceFailureEventType));
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::DeviceFailureEventType));
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::DeviceFailureEventType));
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::DeviceFailureEventType));
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::DeviceFailureEventType, AttributeID::IS_ABSTRACT, false);
    }

    void RefreshEndEventType()
    {
      // Base Attributes
      AddValue(ObjectID::RefreshEndEventType, AttributeID::NODE_ID,      NodeID(ObjectID::RefreshEndEventType));
      AddValue(ObjectID::RefreshEndEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::RefreshEndEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::RefreshEndEventType));
      AddValue(ObjectID::RefreshEndEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::RefreshEndEventType));
      AddValue(ObjectID::RefreshEndEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::RefreshEndEventType));
      AddValue(ObjectID::RefreshEndEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::RefreshEndEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::RefreshEndEventType, AttributeID::IS_ABSTRACT, false);
    }

    void RefreshRequiredEventType()
    {
      // Base Attributes
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::NODE_ID,      NodeID(ObjectID::RefreshRequiredEventType));
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::RefreshRequiredEventType));
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::RefreshRequiredEventType));
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::RefreshRequiredEventType));
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::RefreshRequiredEventType, AttributeID::IS_ABSTRACT, false);
    }

    void RefreshStartEventType()
    {
      // Base Attributes
      AddValue(ObjectID::RefreshStartEventType, AttributeID::NODE_ID,      NodeID(ObjectID::RefreshStartEventType));
      AddValue(ObjectID::RefreshStartEventType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::RefreshStartEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::RefreshStartEventType));
      AddValue(ObjectID::RefreshStartEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::RefreshStartEventType));
      AddValue(ObjectID::RefreshStartEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::RefreshStartEventType));
      AddValue(ObjectID::RefreshStartEventType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::RefreshStartEventType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::RefreshStartEventType, AttributeID::IS_ABSTRACT, false);
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

    void ObjectTypes()
    {
      // Attributes
      AddValue(ObjectID::ObjectTypes, AttributeID::NODE_ID,      NodeID(ObjectID::ObjectTypes));
      AddValue(ObjectID::ObjectTypes, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ObjectTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ObjectTypes));
      AddValue(ObjectID::ObjectTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ObjectTypes));
      AddValue(ObjectID::ObjectTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ObjectTypes));
      AddValue(ObjectID::ObjectTypes, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ObjectTypes, AttributeID::USER_WRITE_MASK, 0);

      // References
      AddReference(ObjectID::ObjectTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::ObjectTypes, forward, ReferenceID::Organizes, ObjectID::BaseObjectType, Names::BaseObjectType, NodeClass::ObjectType, ObjectID::Null);
    }

    void BaseObjectType()
    {
      // Base Attributes
      AddValue(ObjectID::BaseObjectType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseObjectType));
      AddValue(ObjectID::BaseObjectType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::BaseObjectType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseObjectType));
      AddValue(ObjectID::BaseObjectType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseObjectType));
      AddValue(ObjectID::BaseObjectType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseObjectType));
      AddValue(ObjectID::BaseObjectType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BaseObjectType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::BaseObjectType, AttributeID::IS_ABSTRACT, true);

      // References
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::BaseEventType, Names::BaseEventType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::DataTypeEncodingType, Names::DataTypeEncodingType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::DataTypeSystemType, Names::DataTypeSystemType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::FolderType, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::HistoricalEventConfigurationType, Names::HistoricalEventConfigurationType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::ModellingRuleType, Names::ModellingRuleType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::ServerCapabilitiesType, Names::ServerCapabilitiesType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsType, Names::ServerDiagnosticsType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::ServerRedundancyType, Names::ServerRedundancyType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::ServerType, Names::ServerType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsObjectType, Names::SessionDiagnosticsObjectType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsSummaryType, Names::SessionDiagnosticsSummaryType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::StateType, Names::StateType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::TransitionType, Names::TransitionType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::VendorServerInfoType, Names::VendorServerInfoType, NodeClass::ObjectType, ObjectID::Null);
    }

    void DataTypeEncodingType()
    {
      // Base Attributes
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::NODE_ID,      NodeID(ObjectID::DataTypeEncodingType));
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataTypeEncodingType));
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataTypeEncodingType));
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataTypeEncodingType));
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::DataTypeEncodingType, AttributeID::IS_ABSTRACT, false);
    }

    void DataTypeSystemType()
    {
      // Base Attributes
      AddValue(ObjectID::DataTypeSystemType, AttributeID::NODE_ID,      NodeID(ObjectID::DataTypeSystemType));
      AddValue(ObjectID::DataTypeSystemType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::DataTypeSystemType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataTypeSystemType));
      AddValue(ObjectID::DataTypeSystemType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataTypeSystemType));
      AddValue(ObjectID::DataTypeSystemType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataTypeSystemType));
      AddValue(ObjectID::DataTypeSystemType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::DataTypeSystemType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::DataTypeSystemType, AttributeID::IS_ABSTRACT, false);
    }

    void FolderType()
    {
      // Base Attributes
      AddValue(ObjectID::FolderType, AttributeID::NODE_ID,      NodeID(ObjectID::FolderType));
      AddValue(ObjectID::FolderType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::FolderType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::FolderType));
      AddValue(ObjectID::FolderType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::FolderType));
      AddValue(ObjectID::FolderType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::FolderType));
      AddValue(ObjectID::FolderType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::FolderType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::FolderType, AttributeID::IS_ABSTRACT, false);
    }

    void HistoricalEventConfigurationType()
    {
      // Base Attributes
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::NODE_ID,      NodeID(ObjectID::HistoricalEventConfigurationType));
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HistoricalEventConfigurationType));
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HistoricalEventConfigurationType));
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HistoricalEventConfigurationType));
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HistoricalEventConfigurationType, AttributeID::IS_ABSTRACT, false);
    }

    void ModellingRuleType()
    {
      // Base Attributes
      AddValue(ObjectID::ModellingRuleType, AttributeID::NODE_ID,      NodeID(ObjectID::ModellingRuleType));
      AddValue(ObjectID::ModellingRuleType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::ModellingRuleType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModellingRuleType));
      AddValue(ObjectID::ModellingRuleType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ModellingRuleType));
      AddValue(ObjectID::ModellingRuleType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ModellingRuleType));
      AddValue(ObjectID::ModellingRuleType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ModellingRuleType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ModellingRuleType, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::ModellingRuleType, forward, ReferenceID::HasProperty, ObjectID::NamingRule, Names::NamingRule, NodeClass::Variable, ObjectID::Null);
    }

    void NamingRule()
    {
      // Base Attributes
      AddValue(ObjectID::NamingRule, AttributeID::NODE_ID,      NodeID(ObjectID::Time));
      AddValue(ObjectID::NamingRule, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::NamingRule, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Time));
      AddValue(ObjectID::NamingRule, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Time));
      AddValue(ObjectID::NamingRule, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Time));
      AddValue(ObjectID::NamingRule, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::NamingRule, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::NamingRule, AttributeID::VALUE, DateTime());
      AddValue(ObjectID::NamingRule, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::NamingRule, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::NamingRule, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::NamingRule, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::NamingRule, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::NamingRule, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::NamingRule, AttributeID::VALUE_RANK, int32_t(-1));
      // References
      AddReference(ObjectID::NamingRule, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
    }

    void ServerCapabilitiesType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::NODE_ID,      NodeID(ObjectID::ModellingRuleType));
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerCapabilitiesType));
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerCapabilitiesType));
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerCapabilitiesType));
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerCapabilitiesType, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasComponent, ObjectID::AggregateFunctions, Names::AggregateFunctions, NodeClass::Object, ObjectID::Null);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasComponent, ObjectID::ModellingRules, Names::ModellingRules, NodeClass::Object, ObjectID::Null);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasProperty,  ObjectID::LocaleIDArray, Names::LocaleIDArray, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasProperty,  ObjectID::MaxBrowseContinuationPoints, Names::MaxBrowseContinuationPoints, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasProperty,  ObjectID::MaxHistoryContinuationPoints, Names::MaxHistoryContinuationPoints, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasProperty,  ObjectID::MinSupportedSampleRate, Names::MinSupportedSampleRate, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasProperty,  ObjectID::ServerProfileArray, Names::ServerProfileArray, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerCapabilitiesType, forward, ReferenceID::HasProperty,  ObjectID::SoftwareCertificates, Names::SoftwareCertificates, NodeClass::Variable, ObjectID::PropertyType);
    }

    void AggregateFunctions()
    {
      // Attributes
      AddValue(ObjectID::AggregateFunctions, AttributeID::NODE_ID,      NodeID(ObjectID::AggregateFunctions));
      AddValue(ObjectID::AggregateFunctions, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::AggregateFunctions, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AggregateFunctions));
      AddValue(ObjectID::AggregateFunctions, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::AggregateFunctions));
      AddValue(ObjectID::AggregateFunctions, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::AggregateFunctions));
      AddValue(ObjectID::AggregateFunctions, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::AggregateFunctions, AttributeID::USER_WRITE_MASK, 0);
      //References
      AddReference(ObjectID::AggregateFunctions,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::AggregateFunctions, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void ModelingRules()
    {
      // Attributes
      AddValue(ObjectID::ModellingRules, AttributeID::NODE_ID,      NodeID(ObjectID::ModellingRules));
      AddValue(ObjectID::ModellingRules, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ModellingRules, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModellingRules));
      AddValue(ObjectID::ModellingRules, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ModellingRules));
      AddValue(ObjectID::ModellingRules, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ModellingRules));
      AddValue(ObjectID::ModellingRules, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ModellingRules, AttributeID::USER_WRITE_MASK, 0);
      //References
      AddReference(ObjectID::ModellingRules,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      AddReference(ObjectID::ModellingRules, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void LocaleIDArray()
    {
      // Base Attributes
      AddValue(ObjectID::LocaleIDArray, AttributeID::NODE_ID,      NodeID(ObjectID::LocaleIDArray));
      AddValue(ObjectID::LocaleIDArray, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::LocaleIDArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocaleIDArray));
      AddValue(ObjectID::LocaleIDArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::LocaleIDArray));
      AddValue(ObjectID::LocaleIDArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::LocaleIDArray));
      AddValue(ObjectID::LocaleIDArray, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::LocaleIDArray, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::LocaleIDArray, AttributeID::VALUE, std::string());
      AddValue(ObjectID::LocaleIDArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::LocaleIDArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>{0});
      AddValue(ObjectID::LocaleIDArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::LocaleIDArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::LocaleIDArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::LocaleIDArray, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::LocaleIDArray, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::LocaleIDArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::LocaleIDArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void MaxBrowseContinuationPoints()
    {
      // Base Attributes
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::NODE_ID,      NodeID(ObjectID::MaxBrowseContinuationPoints));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MaxBrowseContinuationPoints));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MaxBrowseContinuationPoints));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MaxBrowseContinuationPoints));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::VALUE, double());
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::DATA_TYPE, NodeID(ObjectID::Double));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::MaxBrowseContinuationPoints, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::MaxBrowseContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void MaxHistoryContinuationPoints()
    {
      // Base Attributes
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::NODE_ID,      NodeID(ObjectID::MaxHistoryContinuationPoints));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MaxHistoryContinuationPoints));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MaxHistoryContinuationPoints));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MaxHistoryContinuationPoints));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::VALUE, double());
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::DATA_TYPE, NodeID(ObjectID::Double));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::MaxHistoryContinuationPoints, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::MaxHistoryContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void MinSupportedSampleRate()
    {
      // Base Attributes
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::NODE_ID,      NodeID(ObjectID::MinSupportedSampleRate));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MinSupportedSampleRate));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MinSupportedSampleRate));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MinSupportedSampleRate));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::VALUE, double());
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::DATA_TYPE, NodeID(ObjectID::Double));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::MinSupportedSampleRate, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::MinSupportedSampleRate, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::MinSupportedSampleRate, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void ServerProfileArray()
    {
      // Base Attributes
      AddValue(ObjectID::ServerProfileArray, AttributeID::NODE_ID,      NodeID(ObjectID::ServerProfileArray));
      AddValue(ObjectID::ServerProfileArray, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ServerProfileArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerProfileArray));
      AddValue(ObjectID::ServerProfileArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerProfileArray));
      AddValue(ObjectID::ServerProfileArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerProfileArray));
      AddValue(ObjectID::ServerProfileArray, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerProfileArray, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ServerProfileArray, AttributeID::VALUE, std::string());
      AddValue(ObjectID::ServerProfileArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::ServerProfileArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>{0});
      AddValue(ObjectID::ServerProfileArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerProfileArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerProfileArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ServerProfileArray, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ServerProfileArray, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::ServerProfileArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServerProfileArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void SoftwareCertificates()
    {
      // Base Attributes
      AddValue(ObjectID::SoftwareCertificates, AttributeID::NODE_ID,      NodeID(ObjectID::SoftwareCertificates));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SoftwareCertificates));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SoftwareCertificates));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SoftwareCertificates));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SoftwareCertificates, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SoftwareCertificates, AttributeID::VALUE, Variant());
      AddValue(ObjectID::SoftwareCertificates, AttributeID::DATA_TYPE, NodeID(ObjectID::SoftwareCertificate));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>{0});
      AddValue(ObjectID::SoftwareCertificates, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SoftwareCertificates, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SoftwareCertificates, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::SoftwareCertificates, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::SoftwareCertificates, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
    }

    void ServerDiagnosticsType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnosticsType));
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerDiagnosticsType));
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerDiagnosticsType));
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerDiagnosticsType));
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerDiagnosticsType, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::ServerDiagnosticsType, forward, ReferenceID::HasProperty,  ObjectID::EnableFlag, Names::EnableFlag, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerDiagnosticsType, forward, ReferenceID::HasProperty,  ObjectID::SamplingIntervalDiagnosticsArray, Names::SamplingIntervalDiagnosticsArray, NodeClass::Variable, ObjectID::SamplingIntervalDiagnosticsArrayType);
      AddReference(ObjectID::ServerDiagnosticsType, forward, ReferenceID::HasProperty,  ObjectID::ServerDiagnosticsSummary, Names::ServerDiagnosticsSummary, NodeClass::Variable, ObjectID::StructureServerDiagnosticsSummaryType);
      AddReference(ObjectID::ServerDiagnosticsType, forward, ReferenceID::HasComponent, ObjectID::SessionsDiagnosticsSummary, Names::SessionDiagnosticsSummary, NodeClass::Variable, ObjectID::SessionDiagnosticsSummaryType);
      AddReference(ObjectID::ServerDiagnosticsType, forward, ReferenceID::HasProperty,  ObjectID::SubscriptionDiagnosticsArray, Names::SubscriptionDiagnosticsArray, NodeClass::Variable, ObjectID::SubscriptionDiagnosticsArrayType);
    }

    void EnableFlag()
    {
      // Base Attributes
      AddValue(ObjectID::EnableFlag, AttributeID::NODE_ID,      NodeID(ObjectID::EnableFlag));
      AddValue(ObjectID::EnableFlag, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::EnableFlag, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EnableFlag));
      AddValue(ObjectID::EnableFlag, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EnableFlag));
      AddValue(ObjectID::EnableFlag, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EnableFlag));
      AddValue(ObjectID::EnableFlag, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::EnableFlag, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::EnableFlag, AttributeID::VALUE, false);
      AddValue(ObjectID::EnableFlag, AttributeID::DATA_TYPE, NodeID(ObjectID::Boolean));
      AddValue(ObjectID::EnableFlag, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::EnableFlag, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::EnableFlag, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::EnableFlag, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::EnableFlag, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::EnableFlag, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::EnableFlag, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::EnableFlag, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void SamplingIntervalDiagnosticsArray()
    {
      // Base Attributes
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::NODE_ID,      NodeID(ObjectID::SamplingIntervalDiagnosticsArray));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsArray));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>{0});
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::SamplingIntervalDiagnosticsArray, forward, ReferenceID::HasTypeDefinition, ObjectID::SamplingIntervalDiagnosticsArrayType, Names::SamplingIntervalDiagnosticsArrayType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::SamplingIntervalDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void ServerDiagnosticsSummary()
    {
      // Base Attributes
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnosticsSummary));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerDiagnosticsSummary));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerDiagnosticsSummary));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerDiagnosticsSummary));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ServerDiagnosticsSummary, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ServerDiagnosticsSummary, forward, ReferenceID::HasTypeDefinition, ObjectID::StructureServerDiagnosticsSummaryType, Names::ServerDiagnosticsSummaryType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServerDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void SessionsDiagnosticsSummary()
    {
      // Base Attributes
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::NODE_ID,      NodeID(ObjectID::SessionsDiagnosticsSummary));
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SessionDiagnosticsSummary));
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SessionDiagnosticsSummary));
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SessionDiagnosticsSummary));
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::USER_WRITE_MASK, 0);

      // References
      AddReference(ObjectID::SessionsDiagnosticsSummary, forward, ReferenceID::HasTypeDefinition, ObjectID::SessionDiagnosticsSummaryType, Names::SessionDiagnosticsSummaryType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::SessionsDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void SubscriptionDiagnosticsArray()
    {
      // Base Attributes
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::NODE_ID,      NodeID(ObjectID::SubscriptionDiagnosticsArray));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SubscriptionDiagnosticsArray));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SubscriptionDiagnosticsArray));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SubscriptionDiagnosticsArray));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::DATA_TYPE, NodeID(ObjectID::StructureSubscriptionDiagnosticsDataType));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(0));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::SubscriptionDiagnosticsArray, forward, ReferenceID::HasTypeDefinition, ObjectID::SubscriptionDiagnosticsArrayType, Names::SubscriptionDiagnosticsArrayType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::SubscriptionDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void ServerRedundancyType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerRedundancyType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerRedundancyType));
      AddValue(ObjectID::ServerRedundancyType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::ServerRedundancyType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerRedundancyType));
      AddValue(ObjectID::ServerRedundancyType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerRedundancyType));
      AddValue(ObjectID::ServerRedundancyType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerRedundancyType));
      AddValue(ObjectID::ServerRedundancyType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerRedundancyType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerRedundancyType, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::ServerRedundancyType, forward, ReferenceID::HasProperty, ObjectID::RedundancySupport, Names::RedundancySupport, NodeClass::Variable, ObjectID::PropertyType);
    }

    void RedundancySupportTypeRedundancySupport()
    {
      // Base Attributes
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::NODE_ID,      NodeID(ObjectID::RedundancySupportTypeRedundancySupport));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::RedundancySupport));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::RedundancySupport));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::RedundancySupport));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::VALUE, int32_t(0));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::DATA_TYPE, NodeID(ObjectID::Int32));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::RedundancySupportTypeRedundancySupport, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
    }

    void ServerType()
    {
      // Base Attributes
      AddValue(ObjectID::ServerType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerType));
      AddValue(ObjectID::ServerType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::ServerType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerType));
      AddValue(ObjectID::ServerType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerType));
      AddValue(ObjectID::ServerType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerType));
      AddValue(ObjectID::ServerType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerType, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::Auditing, Names::Auditing, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::NamespaceArray, Names::NamespaceArray, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::ServerArray, Names::ServerArray, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasComponent, ObjectID::ServerCapabilities, Names::ServerCapabilities, NodeClass::Object, ObjectID::ServerCapabilitiesType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasComponent, ObjectID::ServerDiagnostics, Names::ServerDiagnostics, NodeClass::Object, ObjectID::ServerDiagnosticsType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasComponent, ObjectID::ServerRedundancy, Names::ServerRedundancy, NodeClass::Object, ObjectID::ServerRedundancyType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::ServerStatus, Names::ServerStatus, NodeClass::Variable, ObjectID::ServerStatusType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::ServiceLevel, Names::ServiceLevel, NodeClass::Variable, ObjectID::PropertyType);
      AddReference(ObjectID::ServerType, forward, ReferenceID::HasComponent, ObjectID::VendorServerInfo, Names::VendorServerInfo, NodeClass::Variable, ObjectID::VendorServerInfoType);
    }

    void Auditing()
    {
      // Base Attributes
      AddValue(ObjectID::Auditing, AttributeID::NODE_ID,      NodeID(ObjectID::Auditing));
      AddValue(ObjectID::Auditing, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::Auditing, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Auditing));
      AddValue(ObjectID::Auditing, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Auditing));
      AddValue(ObjectID::Auditing, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Auditing));
      AddValue(ObjectID::Auditing, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Auditing, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::Auditing, AttributeID::VALUE, false);
      AddValue(ObjectID::Auditing, AttributeID::DATA_TYPE, NodeID(ObjectID::Boolean));
      AddValue(ObjectID::Auditing, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(0));
      AddValue(ObjectID::Auditing, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Auditing, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::Auditing, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::Auditing, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::Auditing, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::Auditing, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::Auditing, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void NamespaceArray()
    {
      // Base Attributes
      AddValue(ObjectID::NamespaceArray, AttributeID::NODE_ID,      NodeID(ObjectID::NamespaceArray));
      AddValue(ObjectID::NamespaceArray, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::NamespaceArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NamespaceArray));
      AddValue(ObjectID::NamespaceArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::NamespaceArray));
      AddValue(ObjectID::NamespaceArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::NamespaceArray));
      AddValue(ObjectID::NamespaceArray, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::NamespaceArray, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      const std::vector<std::string> namespaces {"http://opcfoundation.org/UA/"};
      AddValue(ObjectID::NamespaceArray, AttributeID::VALUE, namespaces);
      AddValue(ObjectID::NamespaceArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::NamespaceArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(1));
      AddValue(ObjectID::NamespaceArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::NamespaceArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::NamespaceArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::NamespaceArray, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::NamespaceArray, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::NamespaceArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
    }

    void ServerArray()
    {
      // Base Attributes
      AddValue(ObjectID::ServerArray, AttributeID::NODE_ID,      NodeID(ObjectID::ServerArray));
      AddValue(ObjectID::ServerArray, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ServerArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerArray));
      AddValue(ObjectID::ServerArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerArray));
      AddValue(ObjectID::ServerArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerArray));
      AddValue(ObjectID::ServerArray, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerArray, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      const std::vector<std::string> servers {"opcua.treww.com"};
      AddValue(ObjectID::ServerArray, AttributeID::VALUE, servers);
      AddValue(ObjectID::ServerArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::ServerArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(1));
      AddValue(ObjectID::ServerArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ServerArray, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ServerArray, AttributeID::VALUE_RANK, int32_t(1));

      // References
      AddReference(ObjectID::ServerArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
    }

    void ServerCapabilities()
    {
      // Base Attributes
      AddValue(ObjectID::ServerCapabilities, AttributeID::NODE_ID,      NodeID(ObjectID::ServerCapabilities));
      AddValue(ObjectID::ServerCapabilities, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ServerCapabilities, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerCapabilities));
      AddValue(ObjectID::ServerCapabilities, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerCapabilities));
      AddValue(ObjectID::ServerCapabilities, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerCapabilities));
      AddValue(ObjectID::ServerCapabilities, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerCapabilities, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerCapabilities, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::ServerCapabilities, forward, ReferenceID::HasTypeDefinition, ObjectID::ServerCapabilitiesType, Names::ServerCapabilitiesType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServerCapabilities, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void ServerDiagnostics()
    {
      // Base Attributes
      AddValue(ObjectID::ServerDiagnostics, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnostics));
      AddValue(ObjectID::ServerDiagnostics, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ServerDiagnostics, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerDiagnostics));
      AddValue(ObjectID::ServerDiagnostics, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerDiagnostics));
      AddValue(ObjectID::ServerDiagnostics, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerDiagnostics));
      AddValue(ObjectID::ServerDiagnostics, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerDiagnostics, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerDiagnostics, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::ServerDiagnostics, forward, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsType, Names::ServerDiagnosticsType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServerDiagnostics, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void ServerRedundancy()
    {
      // Base Attributes
      AddValue(ObjectID::ServerRedundancy, AttributeID::NODE_ID,      NodeID(ObjectID::ServerRedundancy));
      AddValue(ObjectID::ServerRedundancy, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::ServerRedundancy, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ServerRedundancy));
      AddValue(ObjectID::ServerRedundancy, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerRedundancy));
      AddValue(ObjectID::ServerRedundancy, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerRedundancy));
      AddValue(ObjectID::ServerRedundancy, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerRedundancy, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ServerRedundancy, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::ServerRedundancy, forward, ReferenceID::HasTypeDefinition, ObjectID::ServerRedundancyType, Names::ServerRedundancyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServerRedundancy, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void ServerStatus()
    {
      // Base Attributes
      AddValue(ObjectID::ServerStatus, AttributeID::NODE_ID,      NodeID(ObjectID::ServerStatus));
      AddValue(ObjectID::ServerStatus, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ServerStatus, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ServerStatus));
      AddValue(ObjectID::ServerStatus, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerStatus));
      AddValue(ObjectID::ServerStatus, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerStatus));
      AddValue(ObjectID::ServerStatus, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServerStatus, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ServerStatus, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::ServerStatus, AttributeID::DATA_TYPE, NodeID(ObjectID::StructureServerStatusDataType));
      AddValue(ObjectID::ServerStatus, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ServerStatus, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerStatus, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServerStatus, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ServerStatus, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ServerStatus, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasTypeDefinition, ObjectID::StructureServerStatusDataType, Names::ServerStatusDataType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasModellingRule,  ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasComponent,      ObjectID::StructureBuildInfo, Names::BuildInfo, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasComponent,      ObjectID::CurrentTime, Names::CurrentTime, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasComponent,      ObjectID::SecondsTillShutdown, Names::SecondsTillShutdown, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasComponent,      ObjectID::ShutdownReason, Names::ShutdownReason, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasComponent,      ObjectID::StartTime, Names::StartTime, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::ServerStatus, forward, ReferenceID::HasComponent,      ObjectID::State, Names::State, NodeClass::Variable, ObjectID::BaseDataVariableType);
    }

    void BuildInfo()
    {
      // Base Attributes
      AddValue(ObjectID::BuildInfo, AttributeID::NODE_ID,      NodeID(ObjectID::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::BuildInfo, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BuildInfo, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::BuildInfo, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::BuildInfo, AttributeID::DATA_TYPE, NodeID(ObjectID::StructureBuildInfo));
      AddValue(ObjectID::BuildInfo, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::BuildInfo, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BuildInfo, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BuildInfo, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::BuildInfo, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::BuildInfo, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasComponent,      ObjectID::BuildDate, Names::BuildDate, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasComponent,      ObjectID::BuildNumber, Names::BuildNumber, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasComponent,      ObjectID::ManufacturerName, Names::ManufacturerName, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasComponent,      ObjectID::ProductName, Names::ProductName, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasComponent,      ObjectID::ProductURI, Names::ProductURI, NodeClass::Variable, ObjectID::BaseDataVariableType);
      AddReference(ObjectID::BuildInfo, forward, ReferenceID::HasComponent,      ObjectID::SoftwareVersion, Names::SoftwareVersion, NodeClass::Variable, ObjectID::BaseDataVariableType);
    }

    void BuildDate()
    {
      // Base Attributes
      AddValue(ObjectID::BuildDate, AttributeID::NODE_ID,      NodeID(ObjectID::BuildDate));
      AddValue(ObjectID::BuildDate, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::BuildDate, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildDate));
      AddValue(ObjectID::BuildDate, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildDate));
      AddValue(ObjectID::BuildDate, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildDate));
      AddValue(ObjectID::BuildDate, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BuildDate, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::BuildDate, AttributeID::VALUE, DateTime());
      AddValue(ObjectID::BuildDate, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::BuildDate, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::BuildDate, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BuildDate, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BuildDate, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::BuildDate, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::BuildDate, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::BuildDate, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void BuildNumber()
    {
      // Base Attributes
      AddValue(ObjectID::BuildNumber, AttributeID::NODE_ID,      NodeID(ObjectID::BuildNumber));
      AddValue(ObjectID::BuildNumber, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::BuildNumber, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildNumber));
      AddValue(ObjectID::BuildNumber, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildNumber));
      AddValue(ObjectID::BuildNumber, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildNumber));
      AddValue(ObjectID::BuildNumber, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BuildNumber, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::BuildNumber, AttributeID::VALUE, std::string());
      AddValue(ObjectID::BuildNumber, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::BuildNumber, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::BuildNumber, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BuildNumber, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BuildNumber, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::BuildNumber, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::BuildNumber, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::BuildNumber, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void ManufacturerName()
    {
      // Base Attributes
      AddValue(ObjectID::ManufacturerName, AttributeID::NODE_ID,      NodeID(ObjectID::ManufacturerName));
      AddValue(ObjectID::ManufacturerName, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ManufacturerName, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildNumber));
      AddValue(ObjectID::ManufacturerName, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildNumber));
      AddValue(ObjectID::ManufacturerName, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildNumber));
      AddValue(ObjectID::ManufacturerName, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ManufacturerName, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ManufacturerName, AttributeID::VALUE, std::string());
      AddValue(ObjectID::ManufacturerName, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::ManufacturerName, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ManufacturerName, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ManufacturerName, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ManufacturerName, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ManufacturerName, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ManufacturerName, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ManufacturerName, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void ProductName()
    {
      // Base Attributes
      AddValue(ObjectID::ProductName, AttributeID::NODE_ID,      NodeID(ObjectID::ProductName));
      AddValue(ObjectID::ProductName, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ProductName, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ProductName));
      AddValue(ObjectID::ProductName, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ProductName));
      AddValue(ObjectID::ProductName, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ProductName));
      AddValue(ObjectID::ProductName, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ProductName, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ProductName, AttributeID::VALUE, std::string());
      AddValue(ObjectID::ProductName, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::ProductName, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ProductName, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ProductName, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ProductName, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ProductName, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ProductName, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ProductName, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void ProductURI()
    {
      // Base Attributes
      AddValue(ObjectID::ProductURI, AttributeID::NODE_ID,      NodeID(ObjectID::ProductURI));
      AddValue(ObjectID::ProductURI, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ProductURI, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ProductURI));
      AddValue(ObjectID::ProductURI, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ProductURI));
      AddValue(ObjectID::ProductURI, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ProductURI));
      AddValue(ObjectID::ProductURI, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ProductURI, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ProductURI, AttributeID::VALUE, std::string());
      AddValue(ObjectID::ProductURI, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::ProductURI, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ProductURI, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ProductURI, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ProductURI, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ProductURI, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ProductURI, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ProductURI, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void SoftwareVersion()
    {
      // Base Attributes
      AddValue(ObjectID::SoftwareVersion, AttributeID::NODE_ID,      NodeID(ObjectID::SoftwareVersion));
      AddValue(ObjectID::SoftwareVersion, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SoftwareVersion, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SoftwareVersion));
      AddValue(ObjectID::SoftwareVersion, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SoftwareVersion));
      AddValue(ObjectID::SoftwareVersion, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SoftwareVersion));
      AddValue(ObjectID::SoftwareVersion, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SoftwareVersion, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SoftwareVersion, AttributeID::VALUE, std::string());
      AddValue(ObjectID::SoftwareVersion, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
      AddValue(ObjectID::SoftwareVersion, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::SoftwareVersion, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SoftwareVersion, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SoftwareVersion, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SoftwareVersion, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SoftwareVersion, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::SoftwareVersion, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void CurrentTime()
    {
      // Base Attributes
      AddValue(ObjectID::CurrentTime, AttributeID::NODE_ID,      NodeID(ObjectID::CurrentTime));
      AddValue(ObjectID::CurrentTime, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::CurrentTime, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::CurrentTime));
      AddValue(ObjectID::CurrentTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::CurrentTime));
      AddValue(ObjectID::CurrentTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::CurrentTime));
      AddValue(ObjectID::CurrentTime, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::CurrentTime, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::CurrentTime, AttributeID::VALUE, DateTime());
      AddValue(ObjectID::CurrentTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::CurrentTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::CurrentTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::CurrentTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::CurrentTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::CurrentTime, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::CurrentTime, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::CurrentTime, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void SecondsTillShutdown()
    {
      // Base Attributes
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::NODE_ID,      NodeID(ObjectID::SecondsTillShutdown));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SecondsTillShutdown));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SecondsTillShutdown));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SecondsTillShutdown));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::VALUE, int32_t());
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::DATA_TYPE, NodeID(ObjectID::Int32));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::SecondsTillShutdown, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::SecondsTillShutdown, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void ShutdownReason()
    {
      // Base Attributes
      AddValue(ObjectID::ShutdownReason, AttributeID::NODE_ID,      NodeID(ObjectID::ShutdownReason));
      AddValue(ObjectID::ShutdownReason, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ShutdownReason, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ShutdownReason));
      AddValue(ObjectID::ShutdownReason, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ShutdownReason));
      AddValue(ObjectID::ShutdownReason, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ShutdownReason));
      AddValue(ObjectID::ShutdownReason, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ShutdownReason, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ShutdownReason, AttributeID::VALUE, LocalizedText());
      AddValue(ObjectID::ShutdownReason, AttributeID::DATA_TYPE, NodeID(ObjectID::LocalizedText));
      AddValue(ObjectID::ShutdownReason, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ShutdownReason, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ShutdownReason, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ShutdownReason, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ShutdownReason, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ShutdownReason, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ShutdownReason, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void StartTime()
    {
      // Base Attributes
      AddValue(ObjectID::StartTime, AttributeID::NODE_ID,      NodeID(ObjectID::StartTime));
      AddValue(ObjectID::StartTime, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::StartTime, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::StartTime));
      AddValue(ObjectID::StartTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::StartTime));
      AddValue(ObjectID::StartTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::StartTime));
      AddValue(ObjectID::StartTime, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StartTime, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::StartTime, AttributeID::VALUE, CurrentDateTime());
      AddValue(ObjectID::StartTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
      AddValue(ObjectID::StartTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::StartTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::StartTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::StartTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::StartTime, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::StartTime, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::StartTime, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void State()
    {
      // Base Attributes
      AddValue(ObjectID::State, AttributeID::NODE_ID,      NodeID(ObjectID::State));
      AddValue(ObjectID::State, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::State, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::State));
      AddValue(ObjectID::State, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::State));
      AddValue(ObjectID::State, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::State));
      AddValue(ObjectID::State, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::State, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::State, AttributeID::VALUE, uint32_t());
      AddValue(ObjectID::State, AttributeID::DATA_TYPE, NodeID(ObjectID::ServerState));
      AddValue(ObjectID::State, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::State, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::State, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::State, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::State, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::State, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::State, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
    }

    void ServiceLevel()
    {
      // Base Attributes
      AddValue(ObjectID::ServiceLevel, AttributeID::NODE_ID,      NodeID(ObjectID::ServiceLevel));
      AddValue(ObjectID::ServiceLevel, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::ServiceLevel, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ServiceLevel));
      AddValue(ObjectID::ServiceLevel, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServiceLevel));
      AddValue(ObjectID::ServiceLevel, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServiceLevel));
      AddValue(ObjectID::ServiceLevel, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ServiceLevel, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::ServiceLevel, AttributeID::VALUE, uint8_t());
      AddValue(ObjectID::ServiceLevel, AttributeID::DATA_TYPE, NodeID(ObjectID::Byte));
      AddValue(ObjectID::ServiceLevel, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::ServiceLevel, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServiceLevel, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::ServiceLevel, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::ServiceLevel, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::ServiceLevel, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::ServiceLevel, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::ServiceLevel, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void VendorServerInfo()
    {
      // Base Attributes
      AddValue(ObjectID::VendorServerInfo, AttributeID::NODE_ID,      NodeID(ObjectID::VendorServerInfo));
      AddValue(ObjectID::VendorServerInfo, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::VendorServerInfo, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::VendorServerInfo));
      AddValue(ObjectID::VendorServerInfo, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::VendorServerInfo));
      AddValue(ObjectID::VendorServerInfo, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::VendorServerInfo));
      AddValue(ObjectID::VendorServerInfo, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::VendorServerInfo, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::VendorServerInfo, AttributeID::IS_ABSTRACT, false);

      // References
      AddReference(ObjectID::VendorServerInfo, forward, ReferenceID::HasTypeDefinition, ObjectID::VendorServerInfoType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      AddReference(ObjectID::VendorServerInfo, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
    }

    void SessionDiagnosticsObjectType()
    {
      // Base Attributes
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::NODE_ID,      NodeID(ObjectID::SessionDiagnosticsObjectType));
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SessionDiagnosticsObjectType));
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SessionDiagnosticsObjectType));
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SessionDiagnosticsObjectType));
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::IS_ABSTRACT, false);
    }

    void SessionDiagnosticsSummaryType()
    {
      // Base Attributes
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::NODE_ID,      NodeID(ObjectID::SessionDiagnosticsSummaryType));
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SessionDiagnosticsSummaryType));
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SessionDiagnosticsSummaryType));
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SessionDiagnosticsSummaryType));
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::IS_ABSTRACT, false);
    }

    void StateType()
    {
      // Base Attributes
      AddValue(ObjectID::StateType, AttributeID::NODE_ID,      NodeID(ObjectID::StateType));
      AddValue(ObjectID::StateType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::StateType, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::StateType));
      AddValue(ObjectID::StateType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::StateType));
      AddValue(ObjectID::StateType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::StateType));
      AddValue(ObjectID::StateType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StateType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::StateType, AttributeID::IS_ABSTRACT, false);
      // References
      AddReference(ObjectID::StateType, forward, ReferenceID::HasProperty, ObjectID::StateNumber, Names::StateNumber, NodeClass::Variable, ObjectID::PropertyType);
    }

    void StateNumber()
    {
      // Base Attributes
      AddValue(ObjectID::StateNumber, AttributeID::NODE_ID,      NodeID(ObjectID::StateNumber));
      AddValue(ObjectID::StateNumber, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
      AddValue(ObjectID::StateNumber, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::StateNumber));
      AddValue(ObjectID::StateNumber, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::StateNumber));
      AddValue(ObjectID::StateNumber, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::StateNumber));
      AddValue(ObjectID::StateNumber, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::StateNumber, AttributeID::USER_WRITE_MASK, 0);
      // Variable Attributes
      AddValue(ObjectID::StateNumber, AttributeID::VALUE, uint32_t());
      AddValue(ObjectID::StateNumber, AttributeID::DATA_TYPE, NodeID(ObjectID::UInt32));
      AddValue(ObjectID::StateNumber, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::StateNumber, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::StateNumber, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::StateNumber, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::StateNumber, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::StateNumber, AttributeID::VALUE_RANK, int32_t(-1));

      // References
      AddReference(ObjectID::StateNumber, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
    }

    void TransitionType()
    {
      // Base Attributes
      AddValue(ObjectID::TransitionType, AttributeID::NODE_ID,      NodeID(ObjectID::TransitionType));
      AddValue(ObjectID::TransitionType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::TransitionType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::TransitionType));
      AddValue(ObjectID::TransitionType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::TransitionType));
      AddValue(ObjectID::TransitionType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::TransitionType));
      AddValue(ObjectID::TransitionType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::TransitionType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::TransitionType, AttributeID::IS_ABSTRACT, false);
    }

    void VendorServerInfoType()
    {
      // Base Attributes
      AddValue(ObjectID::VendorServerInfoType, AttributeID::NODE_ID,      NodeID(ObjectID::VendorServerInfoType));
      AddValue(ObjectID::VendorServerInfoType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ObjectType));
      AddValue(ObjectID::VendorServerInfoType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::VendorServerInfoType));
      AddValue(ObjectID::VendorServerInfoType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::VendorServerInfoType));
      AddValue(ObjectID::VendorServerInfoType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::VendorServerInfoType));
      AddValue(ObjectID::VendorServerInfoType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::VendorServerInfoType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::VendorServerInfoType, AttributeID::IS_ABSTRACT, false);
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
      // Type Attributes
      AddValue(ObjectID::References, AttributeID::IS_ABSTRACT, true);
      // Reference attributes
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
      // Type Attributes
      AddValue(ObjectID::HierarchicalReferences, AttributeID::IS_ABSTRACT, true);
      // Reference attributes
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
      // Type Attributes
      AddValue(ObjectID::HasChild, AttributeID::IS_ABSTRACT, true);
      // Reference attributes
      AddValue(ObjectID::HasChild, AttributeID::SYMMETRIC, false);

      // References
      AddReference(ObjectID::HasChild, forward, ReferenceID::HasSubtype, ObjectID::Aggregates, Names::Aggregates, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::HasChild, forward, ReferenceID::HasSubtype, ObjectID::HasSubtype, Names::HasSubtype, NodeClass::ReferenceType, ObjectID::Null);
    }

    void Aggregates()
    {
      // Attributes
      AddValue(ObjectID::Aggregates, AttributeID::NODE_ID,      NodeID(ObjectID::Aggregates));
      AddValue(ObjectID::Aggregates, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::Aggregates, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Aggregates));
      AddValue(ObjectID::Aggregates, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Aggregates));
      AddValue(ObjectID::Aggregates, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Aggregates));
      AddValue(ObjectID::Aggregates, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Aggregates, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Aggregates, AttributeID::IS_ABSTRACT, true);
      // Reference attributes
      AddValue(ObjectID::Aggregates, AttributeID::SYMMETRIC, false);

      // References
      AddReference(ObjectID::Aggregates, forward, ReferenceID::HasSubtype, ObjectID::HasHistoricalConfiguration, Names::HasHistoricalConfiguration, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::Aggregates, forward, ReferenceID::HasSubtype, ObjectID::HasComponent, Names::HasComponent, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::Aggregates, forward, ReferenceID::HasSubtype, ObjectID::HasProperty, Names::HasProperty, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HasComponent()
    {
      // Attributes
      AddValue(ObjectID::HasComponent, AttributeID::NODE_ID,      NodeID(ObjectID::HasComponent));
      AddValue(ObjectID::HasComponent, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasComponent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasComponent));
      AddValue(ObjectID::HasComponent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasComponent));
      AddValue(ObjectID::HasComponent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasComponent));
      AddValue(ObjectID::HasComponent, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasComponent, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasComponent, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasComponent, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasComponent, AttributeID::INVERSE_NAME, LocalizedText("ComponentOf"));

      // References
      AddReference(ObjectID::HasComponent, forward, ReferenceID::HasSubtype, ObjectID::HasOrderedComponent, Names::HasOrderedComponent, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HasOrderedComponent()
    {
      // Attributes
      AddValue(ObjectID::HasOrderedComponent, AttributeID::NODE_ID,      NodeID(ObjectID::HasOrderedComponent));
      AddValue(ObjectID::HasOrderedComponent, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasOrderedComponent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasOrderedComponent));
      AddValue(ObjectID::HasOrderedComponent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasOrderedComponent));
      AddValue(ObjectID::HasOrderedComponent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasOrderedComponent));
      AddValue(ObjectID::HasOrderedComponent, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasOrderedComponent, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasOrderedComponent, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasOrderedComponent, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasOrderedComponent, AttributeID::INVERSE_NAME, LocalizedText("OrederedComponentOf"));
    }

    void HasHistoricalConfiguration()
    {
      // Attributes
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::NODE_ID,      NodeID(ObjectID::HasHistoricalConfiguration));
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasHistoricalConfiguration));
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasHistoricalConfiguration));
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasHistoricalConfiguration));
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasHistoricalConfiguration, AttributeID::INVERSE_NAME, LocalizedText("HistoricalConfigurationOf"));
    }

    void HasProperty()
    {
      // Attributes
      AddValue(ObjectID::HasProperty, AttributeID::NODE_ID,      NodeID(ObjectID::HasHistoricalConfiguration));
      AddValue(ObjectID::HasProperty, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasProperty, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasHistoricalConfiguration));
      AddValue(ObjectID::HasProperty, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasHistoricalConfiguration));
      AddValue(ObjectID::HasProperty, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasHistoricalConfiguration));
      AddValue(ObjectID::HasProperty, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasProperty, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasProperty, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasProperty, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasProperty, AttributeID::INVERSE_NAME, LocalizedText("PropertyOf"));
    }

    void HasSubtype()
    {
      // Attributes
      AddValue(ObjectID::HasSubtype, AttributeID::NODE_ID,      NodeID(ObjectID::HasSubtype));
      AddValue(ObjectID::HasSubtype, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasSubtype, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasSubtype));
      AddValue(ObjectID::HasSubtype, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasSubtype));
      AddValue(ObjectID::HasSubtype, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasSubtype));
      AddValue(ObjectID::HasSubtype, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasSubtype, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasSubtype, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasSubtype, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasSubtype, AttributeID::INVERSE_NAME, LocalizedText("SubtypeOf"));
    }

    void HasEventSource()
    {
      // Attributes
      AddValue(ObjectID::HasEventSource, AttributeID::NODE_ID,      NodeID(ObjectID::HasEventSource));
      AddValue(ObjectID::HasEventSource, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasEventSource, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasEventSource));
      AddValue(ObjectID::HasEventSource, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasEventSource));
      AddValue(ObjectID::HasEventSource, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasEventSource));
      AddValue(ObjectID::HasEventSource, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasEventSource, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasEventSource, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasEventSource, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasEventSource, AttributeID::INVERSE_NAME, LocalizedText("EventSourceOf"));
      // References
      AddReference(ObjectID::HasEventSource, forward, ReferenceID::HasSubtype, ObjectID::HasNotifier, Names::HasNotifier, NodeClass::ReferenceType, ObjectID::Null);
    }

    void HasNotifier()
    {
      // Attributes
      AddValue(ObjectID::HasNotifier, AttributeID::NODE_ID,      NodeID(ObjectID::HasNotifier));
      AddValue(ObjectID::HasNotifier, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasNotifier, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasNotifier));
      AddValue(ObjectID::HasNotifier, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasNotifier));
      AddValue(ObjectID::HasNotifier, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasNotifier));
      AddValue(ObjectID::HasNotifier, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasNotifier, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasNotifier, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasNotifier, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasNotifier, AttributeID::INVERSE_NAME, LocalizedText("NotifierOf"));
    }

    void Organizes()
    {
      // Attributes
      AddValue(ObjectID::Organizes, AttributeID::NODE_ID,      NodeID(ObjectID::Organizes));
      AddValue(ObjectID::Organizes, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::Organizes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Organizes));
      AddValue(ObjectID::Organizes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Organizes));
      AddValue(ObjectID::Organizes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Organizes));
      AddValue(ObjectID::Organizes, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::Organizes, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::Organizes, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::Organizes, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::Organizes, AttributeID::INVERSE_NAME, LocalizedText("OrganizedBy"));
    }

    void NonHierarchicalReferences()
    {
      // Attributes
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::NODE_ID,      NodeID(ObjectID::NonHierarchicalReferences));
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NonHierarchicalReferences));
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::NonHierarchicalReferences));
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::NonHierarchicalReferences));
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::IS_ABSTRACT, true);
      // Reference attributes
      AddValue(ObjectID::NonHierarchicalReferences, AttributeID::SYMMETRIC, false);

      // References
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::FromState, Names::FromState, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::GeneratesEvent, Names::GeneratesEvent, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasCause, Names::HasCause, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasCondition, Names::HasCondition, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasDescription, Names::HasDescription, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasEffect, Names::HasEffect, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasEncoding, Names::HasEncoding, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasModelParent, Names::HasModelParent, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasModellingRule, Names::HasModellingRule, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasTypeDefinition, Names::HasTypeDefinition, NodeClass::ReferenceType, ObjectID::Null);
      AddReference(ObjectID::NonHierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::ToState, Names::ToState, NodeClass::ReferenceType, ObjectID::Null);
    }

    void FromState()
    {
      // Attributes
      AddValue(ObjectID::FromState, AttributeID::NODE_ID,      NodeID(ObjectID::FromState));
      AddValue(ObjectID::FromState, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::FromState, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::FromState));
      AddValue(ObjectID::FromState, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::FromState));
      AddValue(ObjectID::FromState, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::FromState));
      AddValue(ObjectID::FromState, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::FromState, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::FromState, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::FromState, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::FromState, AttributeID::INVERSE_NAME, LocalizedText("ToTransition"));
    }

    void GeneratesEvent()
    {
      // Attributes
      AddValue(ObjectID::GeneratesEvent, AttributeID::NODE_ID,      NodeID(ObjectID::GeneratesEvent));
      AddValue(ObjectID::GeneratesEvent, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::GeneratesEvent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::GeneratesEvent));
      AddValue(ObjectID::GeneratesEvent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::GeneratesEvent));
      AddValue(ObjectID::GeneratesEvent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::GeneratesEvent));
      AddValue(ObjectID::GeneratesEvent, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::GeneratesEvent, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::GeneratesEvent, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::GeneratesEvent, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::GeneratesEvent, AttributeID::INVERSE_NAME, LocalizedText("GeneratesBy"));
    }

    void HasCause()
    {
      // Attributes
      AddValue(ObjectID::HasCause, AttributeID::NODE_ID,      NodeID(ObjectID::HasCause));
      AddValue(ObjectID::HasCause, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasCause, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasCause));
      AddValue(ObjectID::HasCause, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasCause));
      AddValue(ObjectID::HasCause, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasCause));
      AddValue(ObjectID::HasCause, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasCause, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasCause, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasCause, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasCause, AttributeID::INVERSE_NAME, LocalizedText("MayBeCausedBy"));
    }

    void HasDescription()
    {
      // Attributes
      AddValue(ObjectID::HasDescription, AttributeID::NODE_ID,      NodeID(ObjectID::HasDescription));
      AddValue(ObjectID::HasDescription, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasDescription, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasDescription));
      AddValue(ObjectID::HasDescription, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasDescription));
      AddValue(ObjectID::HasDescription, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasDescription));
      AddValue(ObjectID::HasDescription, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasDescription, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasDescription, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasDescription, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasDescription, AttributeID::INVERSE_NAME, LocalizedText("DescriptionOf"));
    }

    void HasEffect()
    {
      // Attributes
      AddValue(ObjectID::HasEffect, AttributeID::NODE_ID,      NodeID(ObjectID::HasEffect));
      AddValue(ObjectID::HasEffect, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasEffect, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasEffect));
      AddValue(ObjectID::HasEffect, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasEffect));
      AddValue(ObjectID::HasEffect, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasEffect));
      AddValue(ObjectID::HasEffect, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasEffect, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasEffect, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasEffect, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasEffect, AttributeID::INVERSE_NAME, LocalizedText("MayBeEffectedBy"));
    }

    void HasEncoding()
    {
      // Attributes
      AddValue(ObjectID::HasEncoding, AttributeID::NODE_ID,      NodeID(ObjectID::HasEncoding));
      AddValue(ObjectID::HasEncoding, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasEncoding, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasEncoding));
      AddValue(ObjectID::HasEncoding, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasEncoding));
      AddValue(ObjectID::HasEncoding, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasEncoding));
      AddValue(ObjectID::HasEncoding, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasEncoding, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasEncoding, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasEncoding, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasEncoding, AttributeID::INVERSE_NAME, LocalizedText("EncodingOf"));
    }

    void HasModelParent()
    {
      // Attributes
      AddValue(ObjectID::HasModelParent, AttributeID::NODE_ID,      NodeID(ObjectID::HasModelParent));
      AddValue(ObjectID::HasModelParent, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasModelParent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasModelParent));
      AddValue(ObjectID::HasModelParent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasModelParent));
      AddValue(ObjectID::HasModelParent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasModelParent));
      AddValue(ObjectID::HasModelParent, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasModelParent, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasModelParent, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasModelParent, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasModelParent, AttributeID::INVERSE_NAME, LocalizedText("ModelParentOf"));
    }

    void HasModellingRule()
    {
      // Attributes
      AddValue(ObjectID::HasModellingRule, AttributeID::NODE_ID,      NodeID(ObjectID::HasModellingRule));
      AddValue(ObjectID::HasModellingRule, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasModellingRule, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasModellingRule));
      AddValue(ObjectID::HasModellingRule, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasModellingRule));
      AddValue(ObjectID::HasModellingRule, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasModellingRule));
      AddValue(ObjectID::HasModellingRule, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasModellingRule, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasModellingRule, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasModellingRule, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasModellingRule, AttributeID::INVERSE_NAME, LocalizedText("ModellingRuleOf"));
    }

    void HasTypeDefinition()
    {
      // Attributes
      AddValue(ObjectID::HasTypeDefinition, AttributeID::NODE_ID,      NodeID(ObjectID::HasTypeDefinition));
      AddValue(ObjectID::HasTypeDefinition, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::HasTypeDefinition, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasTypeDefinition));
      AddValue(ObjectID::HasTypeDefinition, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasTypeDefinition));
      AddValue(ObjectID::HasTypeDefinition, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasTypeDefinition));
      AddValue(ObjectID::HasTypeDefinition, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::HasTypeDefinition, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::HasTypeDefinition, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::HasTypeDefinition, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::HasTypeDefinition, AttributeID::INVERSE_NAME, LocalizedText("TypeDefinitionOf"));
    }

    void ToState()
    {
      // Attributes
      AddValue(ObjectID::ToState, AttributeID::NODE_ID,      NodeID(ObjectID::ToState));
      AddValue(ObjectID::ToState, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::ReferenceType));
      AddValue(ObjectID::ToState, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ToState));
      AddValue(ObjectID::ToState, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ToState));
      AddValue(ObjectID::ToState, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ToState));
      AddValue(ObjectID::ToState, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::ToState, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::ToState, AttributeID::IS_ABSTRACT, false);
      // Reference attributes
      AddValue(ObjectID::ToState, AttributeID::SYMMETRIC, false);
      AddValue(ObjectID::ToState, AttributeID::INVERSE_NAME, LocalizedText("FromTransition"));
    }

    void VariableTypes()
    {
      // Attributes
      AddValue(ObjectID::VariableTypes, AttributeID::NODE_ID,      NodeID(ObjectID::VariableTypes));
      AddValue(ObjectID::VariableTypes, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Object));
      AddValue(ObjectID::VariableTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::VariableTypes));
      AddValue(ObjectID::VariableTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::VariableTypes));
      AddValue(ObjectID::VariableTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::VariableTypes));
      AddValue(ObjectID::VariableTypes, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::VariableTypes, AttributeID::USER_WRITE_MASK, 0);
      // References
      AddReference(ObjectID::VariableTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::FolderType, NodeClass::Object, ObjectID::Null);
      AddReference(ObjectID::VariableTypes, forward, ReferenceID::Organizes, ObjectID::BaseVariableType, Names::BaseVariableType, NodeClass::VariableType, ObjectID::Null);
    }

    void BaseVariableType()
    {
      // Attributes
      AddValue(ObjectID::BaseVariableType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseVariableType));
      AddValue(ObjectID::BaseVariableType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::VariableType));
      AddValue(ObjectID::BaseVariableType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseVariableType));
      AddValue(ObjectID::BaseVariableType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseVariableType));
      AddValue(ObjectID::BaseVariableType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseVariableType));
      AddValue(ObjectID::BaseVariableType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BaseVariableType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::BaseVariableType, AttributeID::IS_ABSTRACT, false);
      // Variable Attributes
      AddValue(ObjectID::BaseVariableType, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::BaseVariableType, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
      AddValue(ObjectID::BaseVariableType, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::BaseVariableType, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BaseVariableType, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BaseVariableType, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::BaseVariableType, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::BaseVariableType, AttributeID::VALUE_RANK, int32_t(0));
      // References
      AddReference(ObjectID::BaseVariableType, forward, ReferenceID::HasSubtype, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseVariableType, forward, ReferenceID::HasSubtype, ObjectID::PropertyType, Names::PropertyType, NodeClass::VariableType, ObjectID::Null);
    }

    void BaseDataVariableType()
    {
      // Attributes
      AddValue(ObjectID::BaseDataVariableType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseDataVariableType));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::VariableType));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseDataVariableType));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseDataVariableType));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseDataVariableType));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::WRITE_MASK,   0);
      AddValue(ObjectID::BaseDataVariableType, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(ObjectID::BaseDataVariableType, AttributeID::IS_ABSTRACT, false);
      // Variable Attributes
      AddValue(ObjectID::BaseDataVariableType, AttributeID::VALUE, NodeID());
      AddValue(ObjectID::BaseDataVariableType, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(ObjectID::BaseDataVariableType, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(ObjectID::BaseDataVariableType, AttributeID::HISTORIZING, false);
      AddValue(ObjectID::BaseDataVariableType, AttributeID::VALUE_RANK, int32_t(0));
      // References
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::DataTypeDescriptionType, Names::DataTypeDescriptionType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::DataTypeDictionaryType,  Names::DataTypeDictionaryType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsArrayType, Names::SamplingIntervalDiagnosticsArrayType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsType, Names::SamplingIntervalDiagnosticsType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsSummaryType, Names::ServerDiagnosticsSummaryType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::ServerStatusType, Names::ServerStatusType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::ServerVendorCapabilityType, Names::ServerVendorCapabilityType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SessionsDiagnosticsArrayType, Names::SessionsDiagnosticsArrayType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsVariableType, Names::ServerDiagnosticsVariableType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsArrayType, Names::SessionSecurityDiagnosticsArrayType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsType, Names::SessionSecurityDiagnosticsType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsArrayType, Names::SubscriptionDiagnosticsArrayType, NodeClass::VariableType, ObjectID::Null);
      AddReference(ObjectID::BaseDataVariableType, forward, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsType, Names::SubscriptionDiagnosticsType, NodeClass::VariableType, ObjectID::Null);
    }

    void AddVariableType(ObjectID id, const char* name)
    {
      // Attributes
      AddValue(id, AttributeID::NODE_ID,      NodeID(id));
      AddValue(id, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::VariableType));
      AddValue(id, AttributeID::BROWSE_NAME,  QualifiedName(0, name));
      AddValue(id, AttributeID::DISPLAY_NAME, LocalizedText(name));
      AddValue(id, AttributeID::DESCRIPTION,  LocalizedText(name));
      AddValue(id, AttributeID::WRITE_MASK,   0);
      AddValue(id, AttributeID::USER_WRITE_MASK, 0);
      // Type Attributes
      AddValue(id, AttributeID::IS_ABSTRACT, false);
      // Variable Attributes
      AddValue(id, AttributeID::VALUE, NodeID());
      AddValue(id, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
      AddValue(id, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
      AddValue(id, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(id, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
      AddValue(id, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
      AddValue(id, AttributeID::HISTORIZING, false);
      AddValue(id, AttributeID::VALUE_RANK, int32_t(0));
    }


    void DataTypeDescriptionType()
    {
      AddVariableType(ObjectID::DataTypeDescriptionType, OpcUa::Names::DataTypeDescriptionType);
    }

    void DataTypeDictionaryType()
    {
      AddVariableType(ObjectID::DataTypeDictionaryType, OpcUa::Names::DataTypeDictionaryType);
    }

    void SamplingIntervalDiagnosticsArrayType()
    {
      AddVariableType(ObjectID::SamplingIntervalDiagnosticsArrayType, OpcUa::Names::SamplingIntervalDiagnosticsArrayType);
    }

    void SamplingIntervalDiagnosticsType()
    {
      AddVariableType(ObjectID::SamplingIntervalDiagnosticsType, OpcUa::Names::SamplingIntervalDiagnosticsType);
    }

    void ServerDiagnosticsSummaryType()
    {
      AddVariableType(ObjectID::ServerDiagnosticsSummaryType, OpcUa::Names::ServerDiagnosticsSummaryType);
    }

    void ServerStatusType()
    {
      AddVariableType(ObjectID::ServerStatusType, OpcUa::Names::ServerStatusType);
    }

    void ServerVendorCapabilityType()
    {
      AddVariableType(ObjectID::ServerVendorCapabilityType, OpcUa::Names::ServerVendorCapabilityType);
    }


    void SessionsDiagnosticsArrayType()
    {
      AddVariableType(ObjectID::SessionsDiagnosticsArrayType, OpcUa::Names::SessionsDiagnosticsArrayType);
    }

    void ServerDiagnosticsVariableType()
    {
      AddVariableType(ObjectID::ServerDiagnosticsVariableType, OpcUa::Names::ServerDiagnosticsVariableType);
    }

    void SessionSecurityDiagnosticsArrayType()
    {
      AddVariableType(ObjectID::SessionSecurityDiagnosticsArrayType, OpcUa::Names::SessionSecurityDiagnosticsArrayType);
    }

    void SessionSecurityDiagnosticsType()
    {
      AddVariableType(ObjectID::SessionSecurityDiagnosticsType, OpcUa::Names::SessionSecurityDiagnosticsType);
    }

    void SubscriptionDiagnosticsArrayType()
    {
      AddVariableType(ObjectID::SubscriptionDiagnosticsArrayType, OpcUa::Names::SubscriptionDiagnosticsArrayType);
    }

    void SubscriptionDiagnosticsType()
    {
      AddVariableType(ObjectID::SubscriptionDiagnosticsType, OpcUa::Names::SubscriptionDiagnosticsType);
    }

    void PropertyType()
    {
      AddVariableType(ObjectID::PropertyType, OpcUa::Names::PropertyType);
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
