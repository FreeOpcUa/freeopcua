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

#include <opc/ua/server/standard_namespace.h>
#include <opc/common/addons_core/addon.h>
#include <opc/ua/node_management.h>
#include <opc/ua/protocol/node_classes.h>
#include <opc/ua/protocol/variable_access_level.h>
#include <opc/ua/protocol/strings.h>

#include <algorithm>
#include <map>
#include <iostream>

namespace
{
    using namespace OpcUa;
    using namespace OpcUa::Remote;

    const bool forward = true;
    const bool reverse = true;

    class StandardNamespace
    {
    public:
      StandardNamespace(OpcUa::Remote::NodeManagementServices& registry, bool debug)
        : Registry(registry)
        , Debug(debug)
      {
      }

      void Fill()
      {
        if (Debug) std::clog << "Programmatically fillig address space" << std::endl;

       Root();
         Objects();
         Views();
         Server();
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

    private:
      void NewValue(NodeID node, AttributeID attribute, Variant value)
      {
        Registry.AddAttribute(node, attribute, value);
      }

      void NewValue(ObjectID node, AttributeID attribute, Variant value)
      {
        NewValue(NodeID(node), attribute, value);
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
        desc.BrowseName = QualifiedName(name);
        desc.DisplayName = LocalizedText(name);
        desc.TargetNodeClass = targetNodeClass;
        desc.TargetNodeTypeDefinition = targetNodeTypeDefinition;

        Registry.AddReference(NodeID(sourceNode), desc);
      }


      void Root()
      {
        // Attributes
        NewValue(ObjectID::RootFolder, AttributeID::NODE_ID,      NodeID(ObjectID::RootFolder));
        NewValue(ObjectID::RootFolder, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::RootFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Root));
        NewValue(ObjectID::RootFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Root));
        NewValue(ObjectID::RootFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Root));
        NewValue(ObjectID::RootFolder, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::RootFolder, AttributeID::USER_WRITE_MASK, 0);
        NewValue(ObjectID::RootFolder, AttributeID::EVENT_NOTIFIER, (uint8_t)0);

        // Referencies
        AddReference(ObjectID::RootFolder,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ObjectsFolder, Names::Objects,    NodeClass::Object,     ObjectID::FolderType);
        AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::TypesFolder,   Names::Types,      NodeClass::Object,     ObjectID::FolderType);
        AddReference(ObjectID::RootFolder,  forward, ReferenceID::Organizes, ObjectID::ViewsFolder,   Names::Views,      NodeClass::Object,     ObjectID::FolderType);
      }

      void Objects()
      {
        // Attributes
        NewValue(ObjectID::ObjectsFolder, AttributeID::NODE_ID,      NodeID(ObjectID::ObjectsFolder));
        NewValue(ObjectID::ObjectsFolder, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ObjectsFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Objects));
        NewValue(ObjectID::ObjectsFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Objects));
        NewValue(ObjectID::ObjectsFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Objects));
        NewValue(ObjectID::ObjectsFolder, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ObjectsFolder, AttributeID::USER_WRITE_MASK, 0);
        NewValue(ObjectID::ObjectsFolder, AttributeID::EVENT_NOTIFIER, (uint8_t)0);

        AddReference(ObjectID::ObjectsFolder, forward, ReferenceID::Organizes, ObjectID::Server, Names::Server, NodeClass::Object, ObjectID::ServerType);
        AddReference(ObjectID::ObjectsFolder,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      }

      void Views()
      {
        // Attributes
        NewValue(ObjectID::ViewsFolder, AttributeID::NODE_ID,      NodeID(ObjectID::ViewsFolder));
        NewValue(ObjectID::ViewsFolder, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ViewsFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Views));
        NewValue(ObjectID::ViewsFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Views));
        NewValue(ObjectID::ViewsFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Views));
        NewValue(ObjectID::ViewsFolder, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ViewsFolder, AttributeID::USER_WRITE_MASK, 0);
        NewValue(ObjectID::ViewsFolder, AttributeID::EVENT_NOTIFIER, (uint8_t)0);

        AddReference(ObjectID::ViewsFolder,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
      }


    void Server()
      {
        // Attributes
        NewValue(ObjectID::Server, AttributeID::NODE_ID,      NodeID(ObjectID::Server));
        NewValue(ObjectID::Server, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::Server, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Server));
        NewValue(ObjectID::Server, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Server));
        NewValue(ObjectID::Server, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Server));
        NewValue(ObjectID::Server, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Server, AttributeID::USER_WRITE_MASK, 0);
        NewValue(ObjectID::Server, AttributeID::EVENT_NOTIFIER, (uint8_t)0);
        AddReference(ObjectID::Server, forward, ReferenceID::HasComponent, ObjectID::ServerCapabilities, Names::ServerCapabilities, NodeClass::Variable, ObjectID::PropertyType);
        AddReference(ObjectID::Server, forward, ReferenceID::HasComponent, ObjectID::NamespaceArray, Names::NamespaceArray, NodeClass::Variable, ObjectID::PropertyType);
        AddReference(ObjectID::Server, forward, ReferenceID::HasProperty,  ObjectID::ServerStatus, Names::ServerStatus, NodeClass::Variable, ObjectID::ServerStatusType);
        AddReference(ObjectID::Server, forward, ReferenceID::HasProperty,  ObjectID::ServerArray, Names::ServerArray, NodeClass::Variable, ObjectID::PropertyType);
      }
/*
    void ServerCapabilities()
      {
        // Attributes
        NewValue(ObjectID::ServerCapabilities, AttributeID::NODE_ID,      NodeID(ObjectID::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ServerCapabilities, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerCapabilities, AttributeID::USER_WRITE_MASK, 0);
      }

*/

      void Types()
      {
        // Attributes
        NewValue(ObjectID::TypesFolder, AttributeID::NODE_ID,      NodeID(ObjectID::TypesFolder));
        NewValue(ObjectID::TypesFolder, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::TypesFolder, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Types));
        NewValue(ObjectID::TypesFolder, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Types));
        NewValue(ObjectID::TypesFolder, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Types));
        NewValue(ObjectID::TypesFolder, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::TypesFolder, AttributeID::USER_WRITE_MASK, 0);

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
        NewValue(ObjectID::DataTypes, AttributeID::NODE_ID,      NodeID(ObjectID::DataTypes));
        NewValue(ObjectID::DataTypes, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::DataTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataTypes));
        NewValue(ObjectID::DataTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataTypes));
        NewValue(ObjectID::DataTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataTypes));
        NewValue(ObjectID::DataTypes, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DataTypes, AttributeID::USER_WRITE_MASK, 0);

        // References
        AddReference(ObjectID::DataTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::DataTypes, forward, ReferenceID::Organizes, ObjectID::BaseDataType, Names::BaseDataType, NodeClass::DataType, ObjectID::Null);
      }

      void BaseDataType()
      {
        // Base Attributes
        NewValue(ObjectID::BaseDataType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseDataType));
        NewValue(ObjectID::BaseDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::BaseDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseDataType));
        NewValue(ObjectID::BaseDataType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseDataType));
        NewValue(ObjectID::BaseDataType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseDataType));
        NewValue(ObjectID::BaseDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BaseDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::BaseDataType, AttributeID::IS_ABSTRACT, true);

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
        NewValue(ObjectID::Boolean, AttributeID::NODE_ID,      NodeID(ObjectID::Boolean));
        NewValue(ObjectID::Boolean, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Boolean, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Boolean));
        NewValue(ObjectID::Boolean, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Boolean));
        NewValue(ObjectID::Boolean, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Boolean));
        NewValue(ObjectID::Boolean, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Boolean, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::Boolean, AttributeID::IS_ABSTRACT, false);
      }

      void ByteStringType()
      {
        // Base Attributes
        NewValue(ObjectID::ByteString, AttributeID::NODE_ID,      NodeID(ObjectID::ByteString));
        NewValue(ObjectID::ByteString, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ByteString, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ByteString));
        NewValue(ObjectID::ByteString, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ByteString));
        NewValue(ObjectID::ByteString, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ByteString));
        NewValue(ObjectID::ByteString, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ByteString, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::ByteString, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::ByteString, forward, ReferenceID::HasSubtype, ObjectID::Image, Names::Image, NodeClass::DataType, ObjectID::Null);
      }

      void ImageType()
      {
        // Base Attributes
        NewValue(ObjectID::Image, AttributeID::NODE_ID,      NodeID(ObjectID::Image));
        NewValue(ObjectID::Image, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Image, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Image));
        NewValue(ObjectID::Image, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Image));
        NewValue(ObjectID::Image, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Image));
        NewValue(ObjectID::Image, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Image, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::Image, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImageBmp, Names::ImageBmp, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImageGif, Names::ImageGif, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImageJpg, Names::ImageJpg, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Image, forward, ReferenceID::HasSubtype, ObjectID::ImagePng, Names::ImagePng, NodeClass::DataType, ObjectID::Null);
      }

      void ImageBmpType()
      {
        // Base Attributes
        NewValue(ObjectID::ImageBmp, AttributeID::NODE_ID,      NodeID(ObjectID::ImageBmp));
        NewValue(ObjectID::ImageBmp, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ImageBmp, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImageBmp));
        NewValue(ObjectID::ImageBmp, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImageBmp));
        NewValue(ObjectID::ImageBmp, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImageBmp));
        NewValue(ObjectID::ImageBmp, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ImageBmp, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::ImageBmp, AttributeID::IS_ABSTRACT, false);
      }

      void ImageGifType()
      {
        // Base Attributes
        NewValue(ObjectID::ImageGif, AttributeID::NODE_ID,      NodeID(ObjectID::ImageGif));
        NewValue(ObjectID::ImageGif, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ImageGif, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImageGif));
        NewValue(ObjectID::ImageGif, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImageGif));
        NewValue(ObjectID::ImageGif, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImageGif));
        NewValue(ObjectID::ImageGif, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ImageGif, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::ImageGif, AttributeID::IS_ABSTRACT, false);
      }

      void ImageJpgType()
      {
        // Base Attributes
        NewValue(ObjectID::ImageJpg, AttributeID::NODE_ID,      NodeID(ObjectID::ImageJpg));
        NewValue(ObjectID::ImageJpg, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ImageJpg, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImageJpg));
        NewValue(ObjectID::ImageJpg, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImageJpg));
        NewValue(ObjectID::ImageJpg, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImageJpg));
        NewValue(ObjectID::ImageJpg, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ImageJpg, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::ImageJpg, AttributeID::IS_ABSTRACT, false);
      }

      void ImagePngType()
      {
        // Base Attributes
        NewValue(ObjectID::ImagePng, AttributeID::NODE_ID,      NodeID(ObjectID::ImagePng));
        NewValue(ObjectID::ImagePng, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ImagePng, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ImagePng));
        NewValue(ObjectID::ImagePng, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ImagePng));
        NewValue(ObjectID::ImagePng, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ImagePng));
        NewValue(ObjectID::ImagePng, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ImagePng, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::ImagePng, AttributeID::IS_ABSTRACT, false);
      }

      void DataValueType()
      {
        // Base Attributes
        NewValue(ObjectID::DataValue, AttributeID::NODE_ID,      NodeID(ObjectID::DataValue));
        NewValue(ObjectID::DataValue, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::DataValue, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataValue));
        NewValue(ObjectID::DataValue, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataValue));
        NewValue(ObjectID::DataValue, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataValue));
        NewValue(ObjectID::DataValue, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DataValue, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::DataValue, AttributeID::IS_ABSTRACT, false);
      }

      void DateTimeType()
      {
        // Attributes
        NewValue(ObjectID::DateTime, AttributeID::NODE_ID,      NodeID(ObjectID::DateTime));
        NewValue(ObjectID::DateTime, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::DateTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DateTime));
        NewValue(ObjectID::DateTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DateTime));
        NewValue(ObjectID::DateTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DateTime));
        NewValue(ObjectID::DateTime, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DateTime, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::DateTime, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::DateTime, forward, ReferenceID::HasSubtype, ObjectID::UtcTime, Names::UtcTime, NodeClass::ObjectType, ObjectID::Null);
      }

      void UtcTimeType()
      {
        // Attributes
        NewValue(ObjectID::UtcTime, AttributeID::NODE_ID,      NodeID(ObjectID::UtcTime));
        NewValue(ObjectID::UtcTime, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::UtcTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UtcTime));
        NewValue(ObjectID::UtcTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::UtcTime));
        NewValue(ObjectID::UtcTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::UtcTime));
        NewValue(ObjectID::UtcTime, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::UtcTime, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::UtcTime, AttributeID::IS_ABSTRACT, false);
      }

      void DiagnosticInfoType()
      {
        // Attributes
        NewValue(ObjectID::DiagnosticInfo, AttributeID::NODE_ID,      NodeID(ObjectID::DiagnosticInfo));
        NewValue(ObjectID::DiagnosticInfo, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::DiagnosticInfo, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DiagnosticInfo));
        NewValue(ObjectID::DiagnosticInfo, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DiagnosticInfo));
        NewValue(ObjectID::DiagnosticInfo, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DiagnosticInfo));
        NewValue(ObjectID::DiagnosticInfo, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DiagnosticInfo, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::DiagnosticInfo, AttributeID::IS_ABSTRACT, false);
      }

      void EnumerationType()
      {
        // Base Attributes
        NewValue(ObjectID::Enumeration, AttributeID::NODE_ID,      NodeID(ObjectID::Enumeration));
        NewValue(ObjectID::Enumeration, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Enumeration, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Enumeration));
        NewValue(ObjectID::Enumeration, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Enumeration));
        NewValue(ObjectID::Enumeration, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Enumeration));
        NewValue(ObjectID::Enumeration, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Enumeration, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::Enumeration, AttributeID::IS_ABSTRACT, true);

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
        NewValue(ObjectID::IdType, AttributeID::NODE_ID,      NodeID(ObjectID::IdType));
        NewValue(ObjectID::IdType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::IdType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::IdType));
        NewValue(ObjectID::IdType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::IdType));
        NewValue(ObjectID::IdType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::IdType));
        NewValue(ObjectID::IdType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::IdType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::IdType, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::IdType, forward, ReferenceID::HasProperty, ObjectID::IdTypeEnumStrings, Names::EnumStrings, NodeClass::Variable, ObjectID::PropertyType);
      }

      void EnumStrings(ObjectID nodeID, const std::vector<LocalizedText>& values)
      {
        // Base Attributes
        NewValue(nodeID, AttributeID::NODE_ID,      NodeID(nodeID));
        NewValue(nodeID, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(nodeID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EnumStrings));
        NewValue(nodeID, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EnumStrings));
        NewValue(nodeID, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EnumStrings));
        NewValue(nodeID, AttributeID::WRITE_MASK,   0);
        NewValue(nodeID, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(nodeID, AttributeID::VALUE, values);
        NewValue(nodeID, AttributeID::DATA_TYPE, NodeID(ObjectID::LocalizedText));
        NewValue(nodeID, AttributeID::ARRAY_DIMENSIONS, std::vector<int32_t>(1,0));
        NewValue(nodeID, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(nodeID, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(nodeID, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(100));
        NewValue(nodeID, AttributeID::HISTORIZING, false);
        NewValue(nodeID, AttributeID::VALUE_RANK, int32_t(1));
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
        NewValue(ObjectID::MessageSecurityMode, AttributeID::NODE_ID,      NodeID(ObjectID::MessageSecurityMode));
        NewValue(ObjectID::MessageSecurityMode, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::MessageSecurityMode, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MessageSecurityMode));
        NewValue(ObjectID::MessageSecurityMode, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MessageSecurityMode));
        NewValue(ObjectID::MessageSecurityMode, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MessageSecurityMode));
        NewValue(ObjectID::MessageSecurityMode, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::MessageSecurityMode, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::MessageSecurityMode, AttributeID::IS_ABSTRACT, false);
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
        NewValue(ObjectID::NodeClass, AttributeID::NODE_ID,      NodeID(ObjectID::NodeClass));
        NewValue(ObjectID::NodeClass, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::NodeClass, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NodeClass));
        NewValue(ObjectID::NodeClass, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::NodeClass));
        NewValue(ObjectID::NodeClass, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::NodeClass));
        NewValue(ObjectID::NodeClass, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::NodeClass, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::NodeClass, AttributeID::IS_ABSTRACT, false);
      }

      void RedundancySupportType()
      {
        // Base Attributes
        NewValue(ObjectID::RedundancySupport, AttributeID::NODE_ID,      NodeID(ObjectID::RedundancySupport));
        NewValue(ObjectID::RedundancySupport, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::RedundancySupport, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::RedundancySupport));
        NewValue(ObjectID::RedundancySupport, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::RedundancySupport));
        NewValue(ObjectID::RedundancySupport, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::RedundancySupport));
        NewValue(ObjectID::RedundancySupport, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::RedundancySupport, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::RedundancySupport, AttributeID::IS_ABSTRACT, false);
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
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::NODE_ID,      NodeID(ObjectID::SecurityTokenRequestType));
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SecurityTokenRequestType));
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SecurityTokenRequestType));
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SecurityTokenRequestType));
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attribute
        NewValue(ObjectID::SecurityTokenRequestType, AttributeID::IS_ABSTRACT, false);
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
        NewValue(ObjectID::ServerState, AttributeID::NODE_ID,      NodeID(ObjectID::ServerState));
        NewValue(ObjectID::ServerState, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ServerState, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerState));
        NewValue(ObjectID::ServerState, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerState));
        NewValue(ObjectID::ServerState, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerState));
        NewValue(ObjectID::ServerState, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerState, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerState, AttributeID::IS_ABSTRACT, false);
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
        NewValue(ObjectID::ExpandedNodeID, AttributeID::NODE_ID,      NodeID(ObjectID::ExpandedNodeID));
        NewValue(ObjectID::ExpandedNodeID, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::ExpandedNodeID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ExpandedNodeID));
        NewValue(ObjectID::ExpandedNodeID, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ExpandedNodeID));
        NewValue(ObjectID::ExpandedNodeID, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ExpandedNodeID));
        NewValue(ObjectID::ExpandedNodeID, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ExpandedNodeID, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ExpandedNodeID, AttributeID::IS_ABSTRACT, false);
      }

      void Guid()
      {
        // Base Attributes
        NewValue(ObjectID::Guid, AttributeID::NODE_ID,      NodeID(ObjectID::Guid));
        NewValue(ObjectID::Guid, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Guid, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Guid));
        NewValue(ObjectID::Guid, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Guid));
        NewValue(ObjectID::Guid, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Guid));
        NewValue(ObjectID::Guid, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Guid, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Guid, AttributeID::IS_ABSTRACT, false);
      }

      void LocalizedTextType()
      {
        // Base Attributes
        NewValue(ObjectID::LocalizedText, AttributeID::NODE_ID,      NodeID(ObjectID::LocalizedText));
        NewValue(ObjectID::LocalizedText, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::LocalizedText, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocalizedText));
        NewValue(ObjectID::LocalizedText, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::LocalizedText));
        NewValue(ObjectID::LocalizedText, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::LocalizedText));
        NewValue(ObjectID::LocalizedText, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::LocalizedText, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::LocalizedText, AttributeID::IS_ABSTRACT, false);
      }

      void NodeIDType()
      {
        // Base Attributes
        NewValue(ObjectID::NodeID, AttributeID::NODE_ID,      NodeID(ObjectID::NodeID));
        NewValue(ObjectID::NodeID, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::NodeID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NodeID));
        NewValue(ObjectID::NodeID, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::NodeID));
        NewValue(ObjectID::NodeID, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::NodeID));
        NewValue(ObjectID::NodeID, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::NodeID, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::NodeID, AttributeID::IS_ABSTRACT, false);
      }

      void NumberType()
      {
        // Base Attributes
        NewValue(ObjectID::Number, AttributeID::NODE_ID,      NodeID(ObjectID::Number));
        NewValue(ObjectID::Number, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Number, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Number));
        NewValue(ObjectID::Number, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Number));
        NewValue(ObjectID::Number, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Number));
        NewValue(ObjectID::Number, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Number, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Number, AttributeID::IS_ABSTRACT, true);
        // References
        AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::Double,   Names::Double,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::Float,    Names::Float,    NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::Integer,  Names::Integer,  NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Number, forward, ReferenceID::HasSubtype, ObjectID::UInteger, Names::UInteger, NodeClass::DataType, ObjectID::Null);
      }

      void DoubleType()
      {
        // Base Attributes
        NewValue(ObjectID::Double, AttributeID::NODE_ID,      NodeID(ObjectID::Double));
        NewValue(ObjectID::Double, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Double, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Double));
        NewValue(ObjectID::Double, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Double));
        NewValue(ObjectID::Double, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Double));
        NewValue(ObjectID::Double, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Double, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Double, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::Double, forward, ReferenceID::HasSubtype, ObjectID::Duration,   Names::Duration,   NodeClass::DataType, ObjectID::Null);
      }

      void DurationType()
      {
        // Base Attributes
        NewValue(ObjectID::Duration, AttributeID::NODE_ID,      NodeID(ObjectID::Duration));
        NewValue(ObjectID::Duration, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Duration, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Duration));
        NewValue(ObjectID::Duration, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Duration));
        NewValue(ObjectID::Duration, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Duration));
        NewValue(ObjectID::Duration, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Duration, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Duration, AttributeID::IS_ABSTRACT, false);
      }

      void FloatType()
      {
        // Base Attributes
        NewValue(ObjectID::Float, AttributeID::NODE_ID,      NodeID(ObjectID::Float));
        NewValue(ObjectID::Float, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Float, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Float));
        NewValue(ObjectID::Float, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Float));
        NewValue(ObjectID::Float, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Float));
        NewValue(ObjectID::Float, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Float, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Float, AttributeID::IS_ABSTRACT, false);
      }

      void IntegerType()
      {
        // Base Attributes
        NewValue(ObjectID::Integer, AttributeID::NODE_ID,      NodeID(ObjectID::Integer));
        NewValue(ObjectID::Integer, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Integer, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Integer));
        NewValue(ObjectID::Integer, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Integer));
        NewValue(ObjectID::Integer, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Integer));
        NewValue(ObjectID::Integer, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Integer, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Integer, AttributeID::IS_ABSTRACT, true);
        // References
        AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::Int16,   Names::Int16,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::Int32,   Names::Int32,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::Int64,   Names::Int64,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Integer, forward, ReferenceID::HasSubtype, ObjectID::SByte,   Names::SByte,   NodeClass::DataType, ObjectID::Null);
      }

      void Int16Type()
      {
        // Base Attributes
        NewValue(ObjectID::Int16, AttributeID::NODE_ID,      NodeID(ObjectID::Int16));
        NewValue(ObjectID::Int16, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Int16, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Int16));
        NewValue(ObjectID::Int16, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Int16));
        NewValue(ObjectID::Int16, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Int16));
        NewValue(ObjectID::Int16, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Int16, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Int16, AttributeID::IS_ABSTRACT, false);
      }

      void Int32Type()
      {
        // Base Attributes
        NewValue(ObjectID::Int32, AttributeID::NODE_ID,      NodeID(ObjectID::Int32));
        NewValue(ObjectID::Int32, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Int32, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Int32));
        NewValue(ObjectID::Int32, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Int32));
        NewValue(ObjectID::Int32, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Int32));
        NewValue(ObjectID::Int32, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Int32, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Int32, AttributeID::IS_ABSTRACT, false);
      }

      void Int64Type()
      {
        // Base Attributes
        NewValue(ObjectID::Int64, AttributeID::NODE_ID,      NodeID(ObjectID::Int64));
        NewValue(ObjectID::Int64, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Int64, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Int64));
        NewValue(ObjectID::Int64, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Int64));
        NewValue(ObjectID::Int64, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Int64));
        NewValue(ObjectID::Int64, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Int64, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Int64, AttributeID::IS_ABSTRACT, false);
      }

      void SByteType()
      {
        // Base Attributes
        NewValue(ObjectID::SByte, AttributeID::NODE_ID,      NodeID(ObjectID::SByte));
        NewValue(ObjectID::SByte, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::SByte, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SByte));
        NewValue(ObjectID::SByte, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SByte));
        NewValue(ObjectID::SByte, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SByte));
        NewValue(ObjectID::SByte, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SByte, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::SByte, AttributeID::IS_ABSTRACT, false);
      }

      void UIntegerType()
      {
        // Base Attributes
        NewValue(ObjectID::UInteger, AttributeID::NODE_ID,      NodeID(ObjectID::UInteger));
        NewValue(ObjectID::UInteger, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::UInteger, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInteger));
        NewValue(ObjectID::UInteger, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInteger));
        NewValue(ObjectID::UInteger, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInteger));
        NewValue(ObjectID::UInteger, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::UInteger, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::UInteger, AttributeID::IS_ABSTRACT, true);
        // References
        AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::Byte,     Names::SByte,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::UInt16,   Names::Int16,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::UInt32,   Names::Int32,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::UInteger, forward, ReferenceID::HasSubtype, ObjectID::UInt64,   Names::Int64,   NodeClass::DataType, ObjectID::Null);
      }

      void UInt16Type()
      {
        // Base Attributes
        NewValue(ObjectID::UInt16, AttributeID::NODE_ID,      NodeID(ObjectID::UInt16));
        NewValue(ObjectID::UInt16, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::UInt16, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInt16));
        NewValue(ObjectID::UInt16, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInt16));
        NewValue(ObjectID::UInt16, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInt16));
        NewValue(ObjectID::UInt16, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::UInt16, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::UInt16, AttributeID::IS_ABSTRACT, false);
      }

      void UInt32Type()
      {
        // Base Attributes
        NewValue(ObjectID::UInt32, AttributeID::NODE_ID,      NodeID(ObjectID::UInt32));
        NewValue(ObjectID::UInt32, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::UInt32, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInt32));
        NewValue(ObjectID::UInt32, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInt32));
        NewValue(ObjectID::UInt32, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInt32));
        NewValue(ObjectID::UInt32, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::UInt32, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::UInt32, AttributeID::IS_ABSTRACT, false);
      }

      void UInt64Type()
      {
        // Base Attributes
        NewValue(ObjectID::UInt64, AttributeID::NODE_ID,      NodeID(ObjectID::UInt64));
        NewValue(ObjectID::UInt64, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::UInt64, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::UInt64));
        NewValue(ObjectID::UInt64, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UInt64));
        NewValue(ObjectID::UInt64, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UInt64));
        NewValue(ObjectID::UInt64, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::UInt64, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::UInt64, AttributeID::IS_ABSTRACT, false);
      }

      void ByteType()
      {
        // Base Attributes
        NewValue(ObjectID::Byte, AttributeID::NODE_ID,      NodeID(ObjectID::Byte));
        NewValue(ObjectID::Byte, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Byte, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Byte));
        NewValue(ObjectID::Byte, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Byte));
        NewValue(ObjectID::Byte, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Byte));
        NewValue(ObjectID::Byte, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Byte, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Byte, AttributeID::IS_ABSTRACT, false);
      }

      void QualifiedNameType()
      {
        // Base Attributes
        NewValue(ObjectID::QualifiedName, AttributeID::NODE_ID,      NodeID(ObjectID::QualifiedName));
        NewValue(ObjectID::QualifiedName, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::QualifiedName, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::QualifiedName));
        NewValue(ObjectID::QualifiedName, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::QualifiedName));
        NewValue(ObjectID::QualifiedName, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::QualifiedName));
        NewValue(ObjectID::QualifiedName, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::QualifiedName, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::QualifiedName, AttributeID::IS_ABSTRACT, false);
      }

      void StatusCodeType()
      {
        // Base Attributes
        NewValue(ObjectID::StatusCode, AttributeID::NODE_ID,      NodeID(ObjectID::StatusCode));
        NewValue(ObjectID::StatusCode, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StatusCode, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::StatusCode));
        NewValue(ObjectID::StatusCode, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::StatusCode));
        NewValue(ObjectID::StatusCode, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::StatusCode));
        NewValue(ObjectID::StatusCode, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StatusCode, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StatusCode, AttributeID::IS_ABSTRACT, false);
      }

      void StringType()
      {
        // Base Attributes
        NewValue(ObjectID::String, AttributeID::NODE_ID,      NodeID(ObjectID::String));
        NewValue(ObjectID::String, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::String, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::String));
        NewValue(ObjectID::String, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::String));
        NewValue(ObjectID::String, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::String));
        NewValue(ObjectID::String, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::String, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::String, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::String, forward, ReferenceID::HasSubtype, ObjectID::LocaleID,   Names::LocaleID,   NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::String, forward, ReferenceID::HasSubtype, ObjectID::NumericRange,   Names::NumericRange,   NodeClass::DataType, ObjectID::Null);
      }

      void LocaleIDType()
      {
        // Base Attributes
        NewValue(ObjectID::LocaleID, AttributeID::NODE_ID,      NodeID(ObjectID::LocaleID));
        NewValue(ObjectID::LocaleID, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::LocaleID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocaleID));
        NewValue(ObjectID::LocaleID, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::LocaleID));
        NewValue(ObjectID::LocaleID, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::LocaleID));
        NewValue(ObjectID::LocaleID, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::LocaleID, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::LocaleID, AttributeID::IS_ABSTRACT, false);
      }

      void NumericRangeType()
      {
        // Base Attributes
        NewValue(ObjectID::NumericRange, AttributeID::NODE_ID,      NodeID(ObjectID::NumericRange));
        NewValue(ObjectID::NumericRange, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::NumericRange, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NumericRange));
        NewValue(ObjectID::NumericRange, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::NumericRange));
        NewValue(ObjectID::NumericRange, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::NumericRange));
        NewValue(ObjectID::NumericRange, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::NumericRange, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::NumericRange, AttributeID::IS_ABSTRACT, false);
      }

      void StructureType()
      {
        // Base Attributes
        NewValue(ObjectID::Structure, AttributeID::NODE_ID,      NodeID(ObjectID::Structure));
        NewValue(ObjectID::Structure, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::Structure, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Structure));
        NewValue(ObjectID::Structure, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Structure));
        NewValue(ObjectID::Structure, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Structure));
        NewValue(ObjectID::Structure, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Structure, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Structure, AttributeID::IS_ABSTRACT, true);
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
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureAddNodesItem));
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AddNodesItem));
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AddNodesItem));
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AddNodesItem));
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureAddNodesItem, AttributeID::IS_ABSTRACT, false);
      }

      void StructureAddReferencesItem()
      {
        // Base Attributes
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureAddReferencesItem));
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AddReferencesItem));
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AddReferencesItem));
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AddReferencesItem));
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureAddReferencesItem, AttributeID::IS_ABSTRACT, false);
      }

      void StructureApplicationDescriptionType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::NODE_ID,      NodeID(ObjectID::StructureApplicationDescription));
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ApplicationDescription));
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ApplicationDescription));
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ApplicationDescription));
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureApplicationDescription, AttributeID::IS_ABSTRACT, false);
      }

      void StructureArgument()
      {
        // Base Attributes
        NewValue(ObjectID::StructureArgument, AttributeID::NODE_ID,      NodeID(ObjectID::StructureArgument));
        NewValue(ObjectID::StructureArgument, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureArgument, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Argument));
        NewValue(ObjectID::StructureArgument, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Argument));
        NewValue(ObjectID::StructureArgument, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Argument));
        NewValue(ObjectID::StructureArgument, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureArgument, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureArgument, AttributeID::IS_ABSTRACT, false);
      }

      void StructureBuildInfo()
      {
        // Base Attributes
        NewValue(ObjectID::StructureBuildInfo, AttributeID::NODE_ID,      NodeID(ObjectID::StructureBuildInfo));
        NewValue(ObjectID::StructureBuildInfo, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureBuildInfo, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BuildInfo));
        NewValue(ObjectID::StructureBuildInfo, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::BuildInfo));
        NewValue(ObjectID::StructureBuildInfo, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::BuildInfo));
        NewValue(ObjectID::StructureBuildInfo, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureBuildInfo, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureBuildInfo, AttributeID::IS_ABSTRACT, false);
      }

      void StructureDeleteNodesItem()
      {
        // Base Attributes
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureDeleteNodesItem));
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DeleteNodesItem));
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::DeleteNodesItem));
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::DeleteNodesItem));
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureDeleteNodesItem, AttributeID::IS_ABSTRACT, false);
      }

      void StructureDeleteReferencesItem()
      {
        // Base Attributes
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::NODE_ID,      NodeID(ObjectID::StructureDeleteReferencesItem));
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DeleteReferencesItem));
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::DeleteReferencesItem));
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::DeleteReferencesItem));
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureDeleteReferencesItem, AttributeID::IS_ABSTRACT, false);
      }

      void StructureEUInformation()
      {
        // Base Attributes
        NewValue(ObjectID::StructureEUInformation, AttributeID::NODE_ID,      NodeID(ObjectID::StructureEUInformation));
        NewValue(ObjectID::StructureEUInformation, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureEUInformation, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EUInformation));
        NewValue(ObjectID::StructureEUInformation, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::EUInformation));
        NewValue(ObjectID::StructureEUInformation, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::EUInformation));
        NewValue(ObjectID::StructureEUInformation, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureEUInformation, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureEUInformation, AttributeID::IS_ABSTRACT, false);
      }

      void StructureModelChangeStructureDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureModelChangeStructureDataType));
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModelChangeStructureDataType));
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ModelChangeStructureDataType));
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ModelChangeStructureDataType));
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureModelChangeStructureDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureRange()
      {
        // Base Attributes
        NewValue(ObjectID::StructureRange, AttributeID::NODE_ID,      NodeID(ObjectID::StructureRange));
        NewValue(ObjectID::StructureRange, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureRange, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Range));
        NewValue(ObjectID::StructureRange, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::Range));
        NewValue(ObjectID::StructureRange, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::Range));
        NewValue(ObjectID::StructureRange, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureRange, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureRange, AttributeID::IS_ABSTRACT, false);
      }

      void StructureSamplingIntervalDiagnosticsDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSamplingIntervalDiagnosticsDataType));
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsDataType));
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsDataType));
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsDataType));
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureSamplingIntervalDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureSemanticChangeStructureDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSemanticChangeStructureDataType));
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SemanticChangeStructureDataType));
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SemanticChangeStructureDataType));
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SemanticChangeStructureDataType));
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureSemanticChangeStructureDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureServerDiagnosticsSummaryDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureServerDiagnosticsSummaryType));
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServerDiagnosticsSummaryType));
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServerDiagnosticsSummaryType));
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServerDiagnosticsSummaryType));
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureServerDiagnosticsSummaryType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureServerStatusDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureServerStatusDataType));
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServerStatusDataType));
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServerStatusDataType));
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServerStatusDataType));
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureServerStatusDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureServiceCounterDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureServiceCounterDataType));
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::ServiceCounterDataType));
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::ServiceCounterDataType));
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::ServiceCounterDataType));
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureServiceCounterDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureSessionDiagnosticsDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSessionDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SessionDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SessionDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SessionDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureSessionDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureSessionSecurityDiagnosticsDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSessionSecurityDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SessionSecurityDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SessionSecurityDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SessionSecurityDiagnosticsDataType));
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureSessionSecurityDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureSignedSoftwareCertificate()
      {
        // Base Attributes
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSignedSoftwareCertificate));
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SignedSoftwareCertificate));
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SignedSoftwareCertificate));
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SignedSoftwareCertificate));
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureSignedSoftwareCertificate, AttributeID::IS_ABSTRACT, false);
      }

      void StructureStatusResult()
      {
        // Base Attributes
        NewValue(ObjectID::StructureStatusResult, AttributeID::NODE_ID,      NodeID(ObjectID::StructureStatusResult));
        NewValue(ObjectID::StructureStatusResult, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureStatusResult, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::StatusResult));
        NewValue(ObjectID::StructureStatusResult, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::StatusResult));
        NewValue(ObjectID::StructureStatusResult, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::StatusResult));
        NewValue(ObjectID::StructureStatusResult, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureStatusResult, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureStatusResult, AttributeID::IS_ABSTRACT, false);
      }

      void StructureSubscriptionDiagnosticsDataType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::NODE_ID,      NodeID(ObjectID::StructureSubscriptionDiagnosticsDataType));
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SubscriptionDiagnosticsDataType));
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SubscriptionDiagnosticsDataType));
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SubscriptionDiagnosticsDataType));
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureSubscriptionDiagnosticsDataType, AttributeID::IS_ABSTRACT, false);
      }

      void StructureUserIdentifyTokenType()
      {
        // Base Attributes
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::StructureUserIdentifyToken));
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::UserIdentifyToken));
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UserIdentifyToken));
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UserIdentifyToken));
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StructureUserIdentifyToken, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::StructureUserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::AnonymousIdentifyToken, Names::AnonymousIdentifyToken, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::StructureUserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::UserNameIdentifyToken,  Names::UserNameIdentifyToken,  NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::StructureUserIdentifyToken, forward, ReferenceID::HasSubtype, ObjectID::X509IdentifyToken,      Names::X509IdentifyToken,      NodeClass::DataType, ObjectID::Null);
      }

      void AnonymousIdentifyToken()
      {
        // Base Attributes
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::AnonymousIdentifyToken));
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::AnonymousIdentifyToken));
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::AnonymousIdentifyToken));
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::AnonymousIdentifyToken));
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::AnonymousIdentifyToken, AttributeID::IS_ABSTRACT, false);
      }

      void UserNameIdentifyToken()
      {
        // Base Attributes
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::UserNameIdentifyToken));
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::UserNameIdentifyToken));
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::UserNameIdentifyToken));
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::UserNameIdentifyToken));
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::UserNameIdentifyToken, AttributeID::IS_ABSTRACT, false);
      }

      void X509IdentifyToken()
      {
        // Base Attributes
        NewValue(ObjectID::X509IdentifyToken, AttributeID::NODE_ID,      NodeID(ObjectID::X509IdentifyToken));
        NewValue(ObjectID::X509IdentifyToken, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::X509IdentifyToken, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::X509IdentifyToken));
        NewValue(ObjectID::X509IdentifyToken, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::X509IdentifyToken));
        NewValue(ObjectID::X509IdentifyToken, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::X509IdentifyToken));
        NewValue(ObjectID::X509IdentifyToken, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::X509IdentifyToken, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::X509IdentifyToken, AttributeID::IS_ABSTRACT, false);
      }

      void XmlElement()
      {
        // Base Attributes
        NewValue(ObjectID::XmlElement, AttributeID::NODE_ID,      NodeID(ObjectID::XmlElement));
        NewValue(ObjectID::XmlElement, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::DataType));
        NewValue(ObjectID::XmlElement, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::XmlElement));
        NewValue(ObjectID::XmlElement, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::XmlElement));
        NewValue(ObjectID::XmlElement, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::XmlElement));
        NewValue(ObjectID::XmlElement, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::XmlElement, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::XmlElement, AttributeID::IS_ABSTRACT, false);
      }

      void EventTypes()
      {
        // Base Attributes
        NewValue(ObjectID::EventTypes, AttributeID::NODE_ID,      NodeID(ObjectID::EventTypes));
        NewValue(ObjectID::EventTypes, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::EventTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EventTypes));
        NewValue(ObjectID::EventTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EventTypes));
        NewValue(ObjectID::EventTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EventTypes));
        NewValue(ObjectID::EventTypes, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::EventTypes, AttributeID::USER_WRITE_MASK, 0);

        // References
        AddReference(ObjectID::EventTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::EventTypes, forward, ReferenceID::Organizes, ObjectID::BaseEventType, Names::BaseEventType, NodeClass::ObjectType, ObjectID::Null);
      }

      void BaseEventType()
      {
        // Base Attributes
        NewValue(ObjectID::BaseEventType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseEventType));
        NewValue(ObjectID::BaseEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::BaseEventType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseEventType));
        NewValue(ObjectID::BaseEventType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseEventType));
        NewValue(ObjectID::BaseEventType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseEventType));
        NewValue(ObjectID::BaseEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BaseEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::BaseEventType, AttributeID::IS_ABSTRACT, true);

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
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseModelChangeEventType));
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::BaseModelChangeEventType));
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::BaseModelChangeEventType));
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::BaseModelChangeEventType));
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::BaseModelChangeEventType, AttributeID::IS_ABSTRACT, false);
      }

      void SemanticChangeEventType()
      {
        // Base Attributes
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::NODE_ID,      NodeID(ObjectID::SemanticChangeEventType));
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SemanticChangeEventType));
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SemanticChangeEventType));
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SemanticChangeEventType));
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::SemanticChangeEventType, AttributeID::IS_ABSTRACT, false);
      }

      void SystemEventType()
      {
        // Base Attributes
        NewValue(ObjectID::SystemEventType, AttributeID::NODE_ID,      NodeID(ObjectID::SystemEventType));
        NewValue(ObjectID::SystemEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::SystemEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::SystemEventType));
        NewValue(ObjectID::SystemEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::SystemEventType));
        NewValue(ObjectID::SystemEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::SystemEventType));
        NewValue(ObjectID::SystemEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SystemEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::SystemEventType, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::DeviceFailureEventType,   Names::DeviceFailureEventType,   NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::RefreshEndEventType,      Names::RefreshEndEventType,      NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::RefreshRequiredEventType, Names::RefreshRequiredEventType, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::SystemEventType, forward, ReferenceID::HasSubtype, ObjectID::RefreshStartEventType,    Names::RefreshStartEventType,    NodeClass::ObjectType, ObjectID::Null);
      }

      void DeviceFailureEventType()
      {
        // Base Attributes
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::NODE_ID,      NodeID(ObjectID::DeviceFailureEventType));
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::DeviceFailureEventType));
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::DeviceFailureEventType));
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::DeviceFailureEventType));
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::DeviceFailureEventType, AttributeID::IS_ABSTRACT, false);
      }

      void RefreshEndEventType()
      {
        // Base Attributes
        NewValue(ObjectID::RefreshEndEventType, AttributeID::NODE_ID,      NodeID(ObjectID::RefreshEndEventType));
        NewValue(ObjectID::RefreshEndEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::RefreshEndEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::RefreshEndEventType));
        NewValue(ObjectID::RefreshEndEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::RefreshEndEventType));
        NewValue(ObjectID::RefreshEndEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::RefreshEndEventType));
        NewValue(ObjectID::RefreshEndEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::RefreshEndEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::RefreshEndEventType, AttributeID::IS_ABSTRACT, false);
      }

      void RefreshRequiredEventType()
      {
        // Base Attributes
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::NODE_ID,      NodeID(ObjectID::RefreshRequiredEventType));
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::RefreshRequiredEventType));
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::RefreshRequiredEventType));
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::RefreshRequiredEventType));
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::RefreshRequiredEventType, AttributeID::IS_ABSTRACT, false);
      }

      void RefreshStartEventType()
      {
        // Base Attributes
        NewValue(ObjectID::RefreshStartEventType, AttributeID::NODE_ID,      NodeID(ObjectID::RefreshStartEventType));
        NewValue(ObjectID::RefreshStartEventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::RefreshStartEventType, AttributeID::BROWSE_NAME,  QualifiedName(0,     OpcUa::Names::RefreshStartEventType));
        NewValue(ObjectID::RefreshStartEventType, AttributeID::DISPLAY_NAME, OpcUa::LocalizedText(OpcUa::Names::RefreshStartEventType));
        NewValue(ObjectID::RefreshStartEventType, AttributeID::DESCRIPTION,  OpcUa::LocalizedText(OpcUa::Names::RefreshStartEventType));
        NewValue(ObjectID::RefreshStartEventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::RefreshStartEventType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::RefreshStartEventType, AttributeID::IS_ABSTRACT, false);
      }

      void EventID()
      {
        // Base Attributes
        NewValue(ObjectID::EventID, AttributeID::NODE_ID,      NodeID(ObjectID::EventID));
        NewValue(ObjectID::EventID, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::EventID, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EventID));
        NewValue(ObjectID::EventID, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EventID));
        NewValue(ObjectID::EventID, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EventID));
        NewValue(ObjectID::EventID, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::EventID, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes

        NewValue(ObjectID::EventID, AttributeID::VALUE, ByteString());
        NewValue(ObjectID::EventID, AttributeID::DATA_TYPE, NodeID(ObjectID::ByteString));
        NewValue(ObjectID::EventID, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::EventID, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::EventID, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::EventID, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::EventID, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::EventID, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::EventID, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::EventID, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void EventType()
      {
        // Base Attributes
        NewValue(ObjectID::EventType, AttributeID::NODE_ID,      NodeID(ObjectID::EventType));
        NewValue(ObjectID::EventType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::EventType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EventType));
        NewValue(ObjectID::EventType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EventType));
        NewValue(ObjectID::EventType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EventType));
        NewValue(ObjectID::EventType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::EventType, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes

        NewValue(ObjectID::EventType, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::EventType, AttributeID::DATA_TYPE, NodeID(ObjectID::NodeID));
        NewValue(ObjectID::EventType, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::EventType, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::EventType, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::EventType, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::EventType, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::EventType, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::EventType, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::EventType, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }
      void LocalTime()
      {
        // Base Attributes
        NewValue(ObjectID::LocalTime, AttributeID::NODE_ID,      NodeID(ObjectID::LocalTime));
        NewValue(ObjectID::LocalTime, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::LocalTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocalTime));
        NewValue(ObjectID::LocalTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::LocalTime));
        NewValue(ObjectID::LocalTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::LocalTime));
        NewValue(ObjectID::LocalTime, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::LocalTime, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes

        NewValue(ObjectID::LocalTime, AttributeID::VALUE, DateTime());
        NewValue(ObjectID::LocalTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::LocalTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::LocalTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::LocalTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::LocalTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::LocalTime, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::LocalTime, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::LocalTime, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::LocalTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void Message()
      {
        // Base Attributes
        NewValue(ObjectID::Message, AttributeID::NODE_ID,      NodeID(ObjectID::Message));
        NewValue(ObjectID::Message, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::Message, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Message));
        NewValue(ObjectID::Message, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Message));
        NewValue(ObjectID::Message, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Message));
        NewValue(ObjectID::Message, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Message, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::Message, AttributeID::VALUE, LocalizedText());
        NewValue(ObjectID::Message, AttributeID::DATA_TYPE, NodeID(ObjectID::LocalizedText));
        NewValue(ObjectID::Message, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::Message, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Message, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Message, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::Message, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::Message, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::Message, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Message, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void ReceiveTime()
      {
        // Base Attributes
        NewValue(ObjectID::ReceiveTime, AttributeID::NODE_ID,      NodeID(ObjectID::ReceiveTime));
        NewValue(ObjectID::ReceiveTime, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ReceiveTime, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ReceiveTime));
        NewValue(ObjectID::ReceiveTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ReceiveTime));
        NewValue(ObjectID::ReceiveTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ReceiveTime));
        NewValue(ObjectID::ReceiveTime, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ReceiveTime, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ReceiveTime, AttributeID::VALUE, DateTime());
        NewValue(ObjectID::ReceiveTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::ReceiveTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ReceiveTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ReceiveTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ReceiveTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ReceiveTime, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ReceiveTime, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ReceiveTime, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ReceiveTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void Severity()
      {
        // Base Attributes
        NewValue(ObjectID::Severity, AttributeID::NODE_ID,      NodeID(ObjectID::Severity));
        NewValue(ObjectID::Severity, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::Severity, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Severity));
        NewValue(ObjectID::Severity, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Severity));
        NewValue(ObjectID::Severity, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Severity));
        NewValue(ObjectID::Severity, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Severity, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::Severity, AttributeID::VALUE, uint16_t());
        NewValue(ObjectID::Severity, AttributeID::DATA_TYPE, NodeID(ObjectID::UInt16));
        NewValue(ObjectID::Severity, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::Severity, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Severity, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Severity, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::Severity, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::Severity, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::Severity, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Severity, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void SourceName()
      {
        // Base Attributes
        NewValue(ObjectID::SourceName, AttributeID::NODE_ID,      NodeID(ObjectID::SourceName));
        NewValue(ObjectID::SourceName, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SourceName, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SourceName));
        NewValue(ObjectID::SourceName, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SourceName));
        NewValue(ObjectID::SourceName, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SourceName));
        NewValue(ObjectID::SourceName, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SourceName, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SourceName, AttributeID::VALUE, std::string());
        NewValue(ObjectID::SourceName, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::SourceName, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::SourceName, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SourceName, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SourceName, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SourceName, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SourceName, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::SourceName, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::SourceName, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void SourceNode()
      {
        // Base Attributes
        NewValue(ObjectID::SourceNode, AttributeID::NODE_ID,      NodeID(ObjectID::SourceNode));
        NewValue(ObjectID::SourceNode, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SourceNode, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SourceNode));
        NewValue(ObjectID::SourceNode, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SourceNode));
        NewValue(ObjectID::SourceNode, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SourceNode));
        NewValue(ObjectID::SourceNode, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SourceNode, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SourceNode, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::SourceNode, AttributeID::DATA_TYPE, NodeID(ObjectID::NodeID));
        NewValue(ObjectID::SourceNode, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::SourceNode, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SourceNode, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SourceNode, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SourceNode, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SourceNode, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::SourceNode, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::SourceNode, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void Time()
      {
        // Base Attributes
        NewValue(ObjectID::Time, AttributeID::NODE_ID,      NodeID(ObjectID::Time));
        NewValue(ObjectID::Time, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::Time, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Time));
        NewValue(ObjectID::Time, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Time));
        NewValue(ObjectID::Time, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Time));
        NewValue(ObjectID::Time, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Time, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::Time, AttributeID::VALUE, DateTime());
        NewValue(ObjectID::Time, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::Time, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::Time, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Time, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Time, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::Time, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::Time, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::Time, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Time, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void ObjectTypes()
      {
        // Attributes
        NewValue(ObjectID::ObjectTypes, AttributeID::NODE_ID,      NodeID(ObjectID::ObjectTypes));
        NewValue(ObjectID::ObjectTypes, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ObjectTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ObjectTypes));
        NewValue(ObjectID::ObjectTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ObjectTypes));
        NewValue(ObjectID::ObjectTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ObjectTypes));
        NewValue(ObjectID::ObjectTypes, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ObjectTypes, AttributeID::USER_WRITE_MASK, 0);

        // References
        AddReference(ObjectID::ObjectTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::ObjectTypes, forward, ReferenceID::Organizes, ObjectID::BaseObjectType, Names::BaseObjectType, NodeClass::ObjectType, ObjectID::Null);
      }

      void BaseObjectType()
      {
        // Base Attributes
        NewValue(ObjectID::BaseObjectType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseObjectType));
        NewValue(ObjectID::BaseObjectType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::BaseObjectType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseObjectType));
        NewValue(ObjectID::BaseObjectType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseObjectType));
        NewValue(ObjectID::BaseObjectType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseObjectType));
        NewValue(ObjectID::BaseObjectType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BaseObjectType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::BaseObjectType, AttributeID::IS_ABSTRACT, true);

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
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::NODE_ID,      NodeID(ObjectID::DataTypeEncodingType));
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataTypeEncodingType));
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataTypeEncodingType));
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataTypeEncodingType));
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::DataTypeEncodingType, AttributeID::IS_ABSTRACT, false);
      }

      void DataTypeSystemType()
      {
        // Base Attributes
        NewValue(ObjectID::DataTypeSystemType, AttributeID::NODE_ID,      NodeID(ObjectID::DataTypeSystemType));
        NewValue(ObjectID::DataTypeSystemType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::DataTypeSystemType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::DataTypeSystemType));
        NewValue(ObjectID::DataTypeSystemType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::DataTypeSystemType));
        NewValue(ObjectID::DataTypeSystemType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::DataTypeSystemType));
        NewValue(ObjectID::DataTypeSystemType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::DataTypeSystemType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::DataTypeSystemType, AttributeID::IS_ABSTRACT, false);
      }

      void FolderType()
      {
        // Base Attributes
        NewValue(ObjectID::FolderType, AttributeID::NODE_ID,      NodeID(ObjectID::FolderType));
        NewValue(ObjectID::FolderType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::FolderType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::FolderType));
        NewValue(ObjectID::FolderType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::FolderType));
        NewValue(ObjectID::FolderType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::FolderType));
        NewValue(ObjectID::FolderType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::FolderType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::FolderType, AttributeID::IS_ABSTRACT, false);
      }

      void HistoricalEventConfigurationType()
      {
        // Base Attributes
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::NODE_ID,      NodeID(ObjectID::HistoricalEventConfigurationType));
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HistoricalEventConfigurationType));
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HistoricalEventConfigurationType));
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HistoricalEventConfigurationType));
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HistoricalEventConfigurationType, AttributeID::IS_ABSTRACT, false);
      }

      void ModellingRuleType()
      {
        // Base Attributes
        NewValue(ObjectID::ModellingRuleType, AttributeID::NODE_ID,      NodeID(ObjectID::ModellingRuleType));
        NewValue(ObjectID::ModellingRuleType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::ModellingRuleType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModellingRuleType));
        NewValue(ObjectID::ModellingRuleType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ModellingRuleType));
        NewValue(ObjectID::ModellingRuleType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ModellingRuleType));
        NewValue(ObjectID::ModellingRuleType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ModellingRuleType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ModellingRuleType, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::ModellingRuleType, forward, ReferenceID::HasProperty, ObjectID::NamingRule, Names::NamingRule, NodeClass::Variable, ObjectID::Null);
      }

      void NamingRule()
      {
        // Base Attributes
        NewValue(ObjectID::NamingRule, AttributeID::NODE_ID,      NodeID(ObjectID::Time));
        NewValue(ObjectID::NamingRule, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::NamingRule, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Time));
        NewValue(ObjectID::NamingRule, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Time));
        NewValue(ObjectID::NamingRule, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Time));
        NewValue(ObjectID::NamingRule, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::NamingRule, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::NamingRule, AttributeID::VALUE, DateTime());
        NewValue(ObjectID::NamingRule, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::NamingRule, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::NamingRule, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::NamingRule, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::NamingRule, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::NamingRule, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::NamingRule, AttributeID::VALUE_RANK, int32_t(-1));
        // References
        AddReference(ObjectID::NamingRule, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      }

      void ServerCapabilitiesType()
      {
        // Base Attributes
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::NODE_ID,      NodeID(ObjectID::ModellingRuleType));
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerCapabilitiesType));
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerCapabilitiesType));
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerCapabilitiesType));
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerCapabilitiesType, AttributeID::IS_ABSTRACT, false);
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
        NewValue(ObjectID::AggregateFunctions, AttributeID::NODE_ID,      NodeID(ObjectID::AggregateFunctions));
        NewValue(ObjectID::AggregateFunctions, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::AggregateFunctions, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::AggregateFunctions));
        NewValue(ObjectID::AggregateFunctions, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::AggregateFunctions));
        NewValue(ObjectID::AggregateFunctions, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::AggregateFunctions));
        NewValue(ObjectID::AggregateFunctions, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::AggregateFunctions, AttributeID::USER_WRITE_MASK, 0);
        //References
        AddReference(ObjectID::AggregateFunctions,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::AggregateFunctions, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void ModelingRules()
      {
        // Attributes
        NewValue(ObjectID::ModellingRules, AttributeID::NODE_ID,      NodeID(ObjectID::ModellingRules));
        NewValue(ObjectID::ModellingRules, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ModellingRules, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ModellingRules));
        NewValue(ObjectID::ModellingRules, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ModellingRules));
        NewValue(ObjectID::ModellingRules, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ModellingRules));
        NewValue(ObjectID::ModellingRules, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ModellingRules, AttributeID::USER_WRITE_MASK, 0);
        //References
        AddReference(ObjectID::ModellingRules,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,    Names::FolderType, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::ModellingRules, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void LocaleIDArray()
      {
        // Base Attributes
        NewValue(ObjectID::LocaleIDArray, AttributeID::NODE_ID,      NodeID(ObjectID::LocaleIDArray));
        NewValue(ObjectID::LocaleIDArray, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::LocaleIDArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::LocaleIDArray));
        NewValue(ObjectID::LocaleIDArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::LocaleIDArray));
        NewValue(ObjectID::LocaleIDArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::LocaleIDArray));
        NewValue(ObjectID::LocaleIDArray, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::LocaleIDArray, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::LocaleIDArray, AttributeID::VALUE, std::string());
        NewValue(ObjectID::LocaleIDArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::LocaleIDArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>{0});
        NewValue(ObjectID::LocaleIDArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::LocaleIDArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::LocaleIDArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::LocaleIDArray, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::LocaleIDArray, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::LocaleIDArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::LocaleIDArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void MaxBrowseContinuationPoints()
      {
        // Base Attributes
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::NODE_ID,      NodeID(ObjectID::MaxBrowseContinuationPoints));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MaxBrowseContinuationPoints));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MaxBrowseContinuationPoints));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MaxBrowseContinuationPoints));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::VALUE, double());
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::DATA_TYPE, NodeID(ObjectID::Double));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::MaxBrowseContinuationPoints, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::MaxBrowseContinuationPoints, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::MaxBrowseContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void MaxHistoryContinuationPoints()
      {
        // Base Attributes
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::NODE_ID,      NodeID(ObjectID::MaxHistoryContinuationPoints));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MaxHistoryContinuationPoints));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MaxHistoryContinuationPoints));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MaxHistoryContinuationPoints));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::VALUE, double());
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::DATA_TYPE, NodeID(ObjectID::Double));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::MaxHistoryContinuationPoints, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::MaxHistoryContinuationPoints, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::MaxHistoryContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void MinSupportedSampleRate()
      {
        // Base Attributes
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::NODE_ID,      NodeID(ObjectID::MinSupportedSampleRate));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::MinSupportedSampleRate));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::MinSupportedSampleRate));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::MinSupportedSampleRate));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::VALUE, double());
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::DATA_TYPE, NodeID(ObjectID::Double));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::ARRAY_DIMENSIONS, std::vector<int32_t>());
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::MinSupportedSampleRate, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::MinSupportedSampleRate, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::MinSupportedSampleRate, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void ServerProfileArray()
      {
        // Base Attributes
        NewValue(ObjectID::ServerProfileArray, AttributeID::NODE_ID,      NodeID(ObjectID::ServerProfileArray));
        NewValue(ObjectID::ServerProfileArray, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ServerProfileArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerProfileArray));
        NewValue(ObjectID::ServerProfileArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerProfileArray));
        NewValue(ObjectID::ServerProfileArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerProfileArray));
        NewValue(ObjectID::ServerProfileArray, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerProfileArray, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ServerProfileArray, AttributeID::VALUE, std::string());
        NewValue(ObjectID::ServerProfileArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::ServerProfileArray, AttributeID::ARRAY_DIMENSIONS, std::vector<int32_t>{0});
        NewValue(ObjectID::ServerProfileArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerProfileArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerProfileArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ServerProfileArray, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ServerProfileArray, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::ServerProfileArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ServerProfileArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void SoftwareCertificates()
      {
        // Base Attributes
        NewValue(ObjectID::SoftwareCertificates, AttributeID::NODE_ID,      NodeID(ObjectID::SoftwareCertificates));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SoftwareCertificates));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SoftwareCertificates));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SoftwareCertificates));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SoftwareCertificates, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SoftwareCertificates, AttributeID::VALUE, Variant());
        NewValue(ObjectID::SoftwareCertificates, AttributeID::DATA_TYPE, NodeID(ObjectID::SoftwareCertificate));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::ARRAY_DIMENSIONS, std::vector<int32_t>{0});
        NewValue(ObjectID::SoftwareCertificates, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SoftwareCertificates, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SoftwareCertificates, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::SoftwareCertificates, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::SoftwareCertificates, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::Null);
      }

      void ServerDiagnosticsType()
      {
        // Base Attributes
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnosticsType));
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerDiagnosticsType));
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerDiagnosticsType));
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerDiagnosticsType));
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerDiagnosticsType, AttributeID::IS_ABSTRACT, false);
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
        NewValue(ObjectID::EnableFlag, AttributeID::NODE_ID,      NodeID(ObjectID::EnableFlag));
        NewValue(ObjectID::EnableFlag, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::EnableFlag, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::EnableFlag));
        NewValue(ObjectID::EnableFlag, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::EnableFlag));
        NewValue(ObjectID::EnableFlag, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::EnableFlag));
        NewValue(ObjectID::EnableFlag, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::EnableFlag, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::EnableFlag, AttributeID::VALUE, false);
        NewValue(ObjectID::EnableFlag, AttributeID::DATA_TYPE, NodeID(ObjectID::Boolean));
        NewValue(ObjectID::EnableFlag, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::EnableFlag, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::EnableFlag, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::EnableFlag, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::EnableFlag, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::EnableFlag, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::EnableFlag, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::EnableFlag, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void SamplingIntervalDiagnosticsArray()
      {
        // Base Attributes
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::NODE_ID,      NodeID(ObjectID::SamplingIntervalDiagnosticsArray));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsArray));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>{0});
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SamplingIntervalDiagnosticsArray, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::SamplingIntervalDiagnosticsArray, forward, ReferenceID::HasTypeDefinition, ObjectID::SamplingIntervalDiagnosticsArrayType, Names::SamplingIntervalDiagnosticsArrayType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::SamplingIntervalDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void ServerDiagnosticsSummary()
      {
        // Base Attributes
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnosticsSummary));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerDiagnosticsSummary));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerDiagnosticsSummary));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerDiagnosticsSummary));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ServerDiagnosticsSummary, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ServerDiagnosticsSummary, forward, ReferenceID::HasTypeDefinition, ObjectID::StructureServerDiagnosticsSummaryType, Names::ServerDiagnosticsSummaryType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ServerDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void SessionsDiagnosticsSummary()
      {
        // Base Attributes
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::NODE_ID,      NodeID(ObjectID::SessionsDiagnosticsSummary));
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SessionDiagnosticsSummary));
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SessionDiagnosticsSummary));
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SessionDiagnosticsSummary));
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SessionsDiagnosticsSummary, AttributeID::USER_WRITE_MASK, 0);

        // References
        AddReference(ObjectID::SessionsDiagnosticsSummary, forward, ReferenceID::HasTypeDefinition, ObjectID::SessionDiagnosticsSummaryType, Names::SessionDiagnosticsSummaryType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::SessionsDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void SubscriptionDiagnosticsArray()
      {
        // Base Attributes
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::NODE_ID,      NodeID(ObjectID::SubscriptionDiagnosticsArray));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::SubscriptionDiagnosticsArray));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SubscriptionDiagnosticsArray));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SubscriptionDiagnosticsArray));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::DATA_TYPE, NodeID(ObjectID::StructureSubscriptionDiagnosticsDataType));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::ARRAY_DIMENSIONS, std::vector<int32_t>(0));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SubscriptionDiagnosticsArray, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::SubscriptionDiagnosticsArray, forward, ReferenceID::HasTypeDefinition, ObjectID::SubscriptionDiagnosticsArrayType, Names::SubscriptionDiagnosticsArrayType, NodeClass::VariableType, ObjectID::Null);
        AddReference(ObjectID::SubscriptionDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void ServerRedundancyType()
      {
        // Base Attributes
        NewValue(ObjectID::ServerRedundancyType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerRedundancyType));
        NewValue(ObjectID::ServerRedundancyType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::ServerRedundancyType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerRedundancyType));
        NewValue(ObjectID::ServerRedundancyType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerRedundancyType));
        NewValue(ObjectID::ServerRedundancyType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerRedundancyType));
        NewValue(ObjectID::ServerRedundancyType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerRedundancyType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerRedundancyType, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::ServerRedundancyType, forward, ReferenceID::HasProperty, ObjectID::RedundancySupport, Names::RedundancySupport, NodeClass::Variable, ObjectID::PropertyType);
      }

      void RedundancySupportTypeRedundancySupport()
      {
        // Base Attributes
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::NODE_ID,      NodeID(ObjectID::RedundancySupportTypeRedundancySupport));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::NODE_CLASS,   static_cast<uint32_t>(NodeClass::Variable));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::RedundancySupport));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::RedundancySupport));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::RedundancySupport));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::VALUE, int32_t(0));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::DATA_TYPE, NodeID(ObjectID::Int32));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::RedundancySupportTypeRedundancySupport, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::RedundancySupportTypeRedundancySupport, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      }

      void ServerType()
      {
        // Base Attributes
        NewValue(ObjectID::ServerType, AttributeID::NODE_ID,      NodeID(ObjectID::ServerType));
        NewValue(ObjectID::ServerType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::ServerType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerType));
        NewValue(ObjectID::ServerType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerType));
        NewValue(ObjectID::ServerType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerType));
        NewValue(ObjectID::ServerType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerType, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::Auditing, Names::Auditing, NodeClass::Variable, ObjectID::PropertyType);
        AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::NamespaceArray, Names::NamespaceArray, NodeClass::Variable, ObjectID::PropertyType);
        AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::ServerArray, Names::ServerArray, NodeClass::Variable, ObjectID::PropertyType);
        AddReference(ObjectID::ServerType, forward, ReferenceID::HasProperty,  ObjectID::ServerProfileArray, Names::ServerProfileArray, NodeClass::Variable, ObjectID::PropertyType);
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
        NewValue(ObjectID::Auditing, AttributeID::NODE_ID,      NodeID(ObjectID::Auditing));
        NewValue(ObjectID::Auditing, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::Auditing, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Auditing));
        NewValue(ObjectID::Auditing, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Auditing));
        NewValue(ObjectID::Auditing, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Auditing));
        NewValue(ObjectID::Auditing, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Auditing, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::Auditing, AttributeID::VALUE, false);
        NewValue(ObjectID::Auditing, AttributeID::DATA_TYPE, NodeID(ObjectID::Boolean));
        NewValue(ObjectID::Auditing, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(0));
        NewValue(ObjectID::Auditing, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Auditing, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::Auditing, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::Auditing, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::Auditing, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::Auditing, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::Auditing, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void NamespaceArray()
      {
        // Base Attributes
        NewValue(ObjectID::NamespaceArray, AttributeID::NODE_ID,      NodeID(ObjectID::NamespaceArray));
        NewValue(ObjectID::NamespaceArray, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::NamespaceArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NamespaceArray));
        NewValue(ObjectID::NamespaceArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::NamespaceArray));
        NewValue(ObjectID::NamespaceArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::NamespaceArray));
        NewValue(ObjectID::NamespaceArray, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::NamespaceArray, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        const std::vector<std::string> namespaces {"http://opcfoundation.org/UA/", "http://treww.github.com"};
        NewValue(ObjectID::NamespaceArray, AttributeID::VALUE, namespaces);
        NewValue(ObjectID::NamespaceArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::NamespaceArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(2));
        NewValue(ObjectID::NamespaceArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::NamespaceArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::NamespaceArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::NamespaceArray, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::NamespaceArray, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::NamespaceArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      }

      void ServerArray()
      {
        // Base Attributes
        NewValue(ObjectID::ServerArray, AttributeID::NODE_ID,      NodeID(ObjectID::ServerArray));
        NewValue(ObjectID::ServerArray, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ServerArray, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerArray));
        NewValue(ObjectID::ServerArray, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerArray));
        NewValue(ObjectID::ServerArray, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerArray));
        NewValue(ObjectID::ServerArray, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerArray, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        const std::vector<std::string> servers {"opcua.treww.com"};
        NewValue(ObjectID::ServerArray, AttributeID::VALUE, servers);
        NewValue(ObjectID::ServerArray, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::ServerArray, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>(1));
        NewValue(ObjectID::ServerArray, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerArray, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerArray, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ServerArray, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ServerArray, AttributeID::VALUE_RANK, int32_t(1));

        // References
        AddReference(ObjectID::ServerArray, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      }


      void ServerCapabilities()
      {
        // Base Attributes
        NewValue(ObjectID::ServerCapabilities, AttributeID::NODE_ID,      NodeID(ObjectID::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ServerCapabilities, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerCapabilities, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerCapabilities, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::ServerCapabilities, forward, ReferenceID::HasTypeDefinition, ObjectID::ServerCapabilitiesType, Names::ServerCapabilitiesType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ServerCapabilities, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
        AddReference(ObjectID::ServerCapabilities, forward, ReferenceID::HasProperty,  ObjectID::ServerProfileArray, Names::ServerProfileArray, NodeClass::Variable, ObjectID::PropertyType);
      }

      void ServerDiagnostics()
      {
        // Base Attributes
        NewValue(ObjectID::ServerDiagnostics, AttributeID::NODE_ID,      NodeID(ObjectID::ServerDiagnostics));
        NewValue(ObjectID::ServerDiagnostics, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ServerDiagnostics, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerDiagnostics));
        NewValue(ObjectID::ServerDiagnostics, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerDiagnostics));
        NewValue(ObjectID::ServerDiagnostics, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerDiagnostics));
        NewValue(ObjectID::ServerDiagnostics, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerDiagnostics, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerDiagnostics, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::ServerDiagnostics, forward, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsType, Names::ServerDiagnosticsType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ServerDiagnostics, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void ServerRedundancy()
      {
        // Base Attributes
        NewValue(ObjectID::ServerRedundancy, AttributeID::NODE_ID,      NodeID(ObjectID::ServerRedundancy));
        NewValue(ObjectID::ServerRedundancy, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ServerRedundancy, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ServerRedundancy));
        NewValue(ObjectID::ServerRedundancy, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerRedundancy));
        NewValue(ObjectID::ServerRedundancy, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerRedundancy));
        NewValue(ObjectID::ServerRedundancy, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerRedundancy, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ServerRedundancy, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::ServerRedundancy, forward, ReferenceID::HasTypeDefinition, ObjectID::ServerRedundancyType, Names::ServerRedundancyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ServerRedundancy, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void ServerStatus()
      {
        // Base Attributes
        NewValue(ObjectID::ServerStatus, AttributeID::NODE_ID,      NodeID(ObjectID::ServerStatus));
        NewValue(ObjectID::ServerStatus, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ServerStatus, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ServerStatus));
        NewValue(ObjectID::ServerStatus, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerStatus));
        NewValue(ObjectID::ServerStatus, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerStatus));
        NewValue(ObjectID::ServerStatus, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerStatus, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ServerStatus, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::ServerStatus, AttributeID::DATA_TYPE, NodeID(ObjectID::StructureServerStatusDataType));
        NewValue(ObjectID::ServerStatus, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ServerStatus, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerStatus, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServerStatus, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ServerStatus, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ServerStatus, AttributeID::VALUE_RANK, int32_t(-1));

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
        NewValue(ObjectID::BuildInfo, AttributeID::NODE_ID,      NodeID(ObjectID::BuildInfo));
        NewValue(ObjectID::BuildInfo, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::BuildInfo, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildInfo));
        NewValue(ObjectID::BuildInfo, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildInfo));
        NewValue(ObjectID::BuildInfo, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildInfo));
        NewValue(ObjectID::BuildInfo, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BuildInfo, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::BuildInfo, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::BuildInfo, AttributeID::DATA_TYPE, NodeID(ObjectID::StructureBuildInfo));
        NewValue(ObjectID::BuildInfo, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::BuildInfo, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BuildInfo, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BuildInfo, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::BuildInfo, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::BuildInfo, AttributeID::VALUE_RANK, int32_t(-1));

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
        NewValue(ObjectID::BuildDate, AttributeID::NODE_ID,      NodeID(ObjectID::BuildDate));
        NewValue(ObjectID::BuildDate, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::BuildDate, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildDate));
        NewValue(ObjectID::BuildDate, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildDate));
        NewValue(ObjectID::BuildDate, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildDate));
        NewValue(ObjectID::BuildDate, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BuildDate, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::BuildDate, AttributeID::VALUE, DateTime());
        NewValue(ObjectID::BuildDate, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::BuildDate, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::BuildDate, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BuildDate, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BuildDate, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::BuildDate, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::BuildDate, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::BuildDate, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void BuildNumber()
      {
        // Base Attributes
        NewValue(ObjectID::BuildNumber, AttributeID::NODE_ID,      NodeID(ObjectID::BuildNumber));
        NewValue(ObjectID::BuildNumber, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::BuildNumber, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildNumber));
        NewValue(ObjectID::BuildNumber, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildNumber));
        NewValue(ObjectID::BuildNumber, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildNumber));
        NewValue(ObjectID::BuildNumber, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BuildNumber, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::BuildNumber, AttributeID::VALUE, std::string());
        NewValue(ObjectID::BuildNumber, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::BuildNumber, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::BuildNumber, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BuildNumber, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BuildNumber, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::BuildNumber, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::BuildNumber, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::BuildNumber, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void ManufacturerName()
      {
        // Base Attributes
        NewValue(ObjectID::ManufacturerName, AttributeID::NODE_ID,      NodeID(ObjectID::ManufacturerName));
        NewValue(ObjectID::ManufacturerName, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ManufacturerName, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::BuildNumber));
        NewValue(ObjectID::ManufacturerName, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BuildNumber));
        NewValue(ObjectID::ManufacturerName, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BuildNumber));
        NewValue(ObjectID::ManufacturerName, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ManufacturerName, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ManufacturerName, AttributeID::VALUE, std::string());
        NewValue(ObjectID::ManufacturerName, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::ManufacturerName, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ManufacturerName, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ManufacturerName, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ManufacturerName, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ManufacturerName, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ManufacturerName, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ManufacturerName, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void ProductName()
      {
        // Base Attributes
        NewValue(ObjectID::ProductName, AttributeID::NODE_ID,      NodeID(ObjectID::ProductName));
        NewValue(ObjectID::ProductName, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ProductName, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ProductName));
        NewValue(ObjectID::ProductName, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ProductName));
        NewValue(ObjectID::ProductName, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ProductName));
        NewValue(ObjectID::ProductName, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ProductName, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ProductName, AttributeID::VALUE, std::string());
        NewValue(ObjectID::ProductName, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::ProductName, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ProductName, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ProductName, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ProductName, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ProductName, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ProductName, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ProductName, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void ProductURI()
      {
        // Base Attributes
        NewValue(ObjectID::ProductURI, AttributeID::NODE_ID,      NodeID(ObjectID::ProductURI));
        NewValue(ObjectID::ProductURI, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ProductURI, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ProductURI));
        NewValue(ObjectID::ProductURI, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ProductURI));
        NewValue(ObjectID::ProductURI, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ProductURI));
        NewValue(ObjectID::ProductURI, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ProductURI, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ProductURI, AttributeID::VALUE, std::string());
        NewValue(ObjectID::ProductURI, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::ProductURI, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ProductURI, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ProductURI, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ProductURI, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ProductURI, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ProductURI, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ProductURI, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void SoftwareVersion()
      {
        // Base Attributes
        NewValue(ObjectID::SoftwareVersion, AttributeID::NODE_ID,      NodeID(ObjectID::SoftwareVersion));
        NewValue(ObjectID::SoftwareVersion, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SoftwareVersion, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SoftwareVersion));
        NewValue(ObjectID::SoftwareVersion, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SoftwareVersion));
        NewValue(ObjectID::SoftwareVersion, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SoftwareVersion));
        NewValue(ObjectID::SoftwareVersion, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SoftwareVersion, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SoftwareVersion, AttributeID::VALUE, std::string());
        NewValue(ObjectID::SoftwareVersion, AttributeID::DATA_TYPE, NodeID(ObjectID::String));
        NewValue(ObjectID::SoftwareVersion, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::SoftwareVersion, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SoftwareVersion, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SoftwareVersion, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SoftwareVersion, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SoftwareVersion, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::SoftwareVersion, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void CurrentTime()
      {
        // Base Attributes
        NewValue(ObjectID::CurrentTime, AttributeID::NODE_ID,      NodeID(ObjectID::CurrentTime));
        NewValue(ObjectID::CurrentTime, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::CurrentTime, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::CurrentTime));
        NewValue(ObjectID::CurrentTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::CurrentTime));
        NewValue(ObjectID::CurrentTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::CurrentTime));
        NewValue(ObjectID::CurrentTime, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::CurrentTime, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::CurrentTime, AttributeID::VALUE, DateTime());
        NewValue(ObjectID::CurrentTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::CurrentTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::CurrentTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::CurrentTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::CurrentTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::CurrentTime, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::CurrentTime, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::CurrentTime, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void SecondsTillShutdown()
      {
        // Base Attributes
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::NODE_ID,      NodeID(ObjectID::SecondsTillShutdown));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SecondsTillShutdown));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SecondsTillShutdown));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SecondsTillShutdown));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::VALUE, uint32_t());
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::DATA_TYPE, NodeID(ObjectID::Int32));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::SecondsTillShutdown, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::SecondsTillShutdown, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void ShutdownReason()
      {
        // Base Attributes
        NewValue(ObjectID::ShutdownReason, AttributeID::NODE_ID,      NodeID(ObjectID::ShutdownReason));
        NewValue(ObjectID::ShutdownReason, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ShutdownReason, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ShutdownReason));
        NewValue(ObjectID::ShutdownReason, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ShutdownReason));
        NewValue(ObjectID::ShutdownReason, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ShutdownReason));
        NewValue(ObjectID::ShutdownReason, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ShutdownReason, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ShutdownReason, AttributeID::VALUE, LocalizedText());
        NewValue(ObjectID::ShutdownReason, AttributeID::DATA_TYPE, NodeID(ObjectID::LocalizedText));
        NewValue(ObjectID::ShutdownReason, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ShutdownReason, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ShutdownReason, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ShutdownReason, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ShutdownReason, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ShutdownReason, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ShutdownReason, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void StartTime()
      {
        // Base Attributes
        NewValue(ObjectID::StartTime, AttributeID::NODE_ID,      NodeID(ObjectID::StartTime));
        NewValue(ObjectID::StartTime, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::StartTime, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::StartTime));
        NewValue(ObjectID::StartTime, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::StartTime));
        NewValue(ObjectID::StartTime, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::StartTime));
        NewValue(ObjectID::StartTime, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StartTime, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::StartTime, AttributeID::VALUE, CurrentDateTime());
        NewValue(ObjectID::StartTime, AttributeID::DATA_TYPE, NodeID(ObjectID::DateTime));
        NewValue(ObjectID::StartTime, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::StartTime, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::StartTime, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::StartTime, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::StartTime, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::StartTime, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::StartTime, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void State()
      {
        // Base Attributes
        NewValue(ObjectID::State, AttributeID::NODE_ID,      NodeID(ObjectID::State));
        NewValue(ObjectID::State, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::State, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::State));
        NewValue(ObjectID::State, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::State));
        NewValue(ObjectID::State, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::State));
        NewValue(ObjectID::State, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::State, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::State, AttributeID::VALUE, uint32_t(0));
        NewValue(ObjectID::State, AttributeID::DATA_TYPE, NodeID(ObjectID::ServerState));
        NewValue(ObjectID::State, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::State, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::State, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::State, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::State, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::State, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::State, forward, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::DataType, ObjectID::Null);
      }

      void ServiceLevel()
      {
        // Base Attributes
        NewValue(ObjectID::ServiceLevel, AttributeID::NODE_ID,      NodeID(ObjectID::ServiceLevel));
        NewValue(ObjectID::ServiceLevel, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::ServiceLevel, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::ServiceLevel));
        NewValue(ObjectID::ServiceLevel, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServiceLevel));
        NewValue(ObjectID::ServiceLevel, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServiceLevel));
        NewValue(ObjectID::ServiceLevel, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServiceLevel, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::ServiceLevel, AttributeID::VALUE, uint8_t());
        NewValue(ObjectID::ServiceLevel, AttributeID::DATA_TYPE, NodeID(ObjectID::Byte));
        NewValue(ObjectID::ServiceLevel, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::ServiceLevel, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServiceLevel, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::ServiceLevel, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::ServiceLevel, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::ServiceLevel, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::ServiceLevel, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::ServiceLevel, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void VendorServerInfo()
      {
        // Base Attributes
        NewValue(ObjectID::VendorServerInfo, AttributeID::NODE_ID,      NodeID(ObjectID::VendorServerInfo));
        NewValue(ObjectID::VendorServerInfo, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::VendorServerInfo, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::VendorServerInfo));
        NewValue(ObjectID::VendorServerInfo, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::VendorServerInfo));
        NewValue(ObjectID::VendorServerInfo, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::VendorServerInfo));
        NewValue(ObjectID::VendorServerInfo, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::VendorServerInfo, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::VendorServerInfo, AttributeID::IS_ABSTRACT, false);

        // References
        AddReference(ObjectID::VendorServerInfo, forward, ReferenceID::HasTypeDefinition, ObjectID::VendorServerInfoType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
        AddReference(ObjectID::VendorServerInfo, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, Names::ModellingRuleMandatory, NodeClass::DataType, ObjectID::ModellingRuleType);
      }

      void SessionDiagnosticsObjectType()
      {
        // Base Attributes
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::NODE_ID,      NodeID(ObjectID::SessionDiagnosticsObjectType));
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SessionDiagnosticsObjectType));
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SessionDiagnosticsObjectType));
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SessionDiagnosticsObjectType));
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::SessionDiagnosticsObjectType, AttributeID::IS_ABSTRACT, false);
      }

      void SessionDiagnosticsSummaryType()
      {
        // Base Attributes
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::NODE_ID,      NodeID(ObjectID::SessionDiagnosticsSummaryType));
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::SessionDiagnosticsSummaryType));
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::SessionDiagnosticsSummaryType));
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::SessionDiagnosticsSummaryType));
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::SessionDiagnosticsSummaryType, AttributeID::IS_ABSTRACT, false);
      }

      void StateType()
      {
        // Base Attributes
        NewValue(ObjectID::StateType, AttributeID::NODE_ID,      NodeID(ObjectID::StateType));
        NewValue(ObjectID::StateType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::StateType, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::StateType));
        NewValue(ObjectID::StateType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::StateType));
        NewValue(ObjectID::StateType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::StateType));
        NewValue(ObjectID::StateType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StateType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::StateType, AttributeID::IS_ABSTRACT, false);
        // References
        AddReference(ObjectID::StateType, forward, ReferenceID::HasProperty, ObjectID::StateNumber, Names::StateNumber, NodeClass::Variable, ObjectID::PropertyType);
      }

      void StateNumber()
      {
        // Base Attributes
        NewValue(ObjectID::StateNumber, AttributeID::NODE_ID,      NodeID(ObjectID::StateNumber));
        NewValue(ObjectID::StateNumber, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Variable));
        NewValue(ObjectID::StateNumber, AttributeID::BROWSE_NAME,  QualifiedName(1, OpcUa::Names::StateNumber));
        NewValue(ObjectID::StateNumber, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::StateNumber));
        NewValue(ObjectID::StateNumber, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::StateNumber));
        NewValue(ObjectID::StateNumber, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::StateNumber, AttributeID::USER_WRITE_MASK, 0);
        // Variable Attributes
        NewValue(ObjectID::StateNumber, AttributeID::VALUE, uint32_t());
        NewValue(ObjectID::StateNumber, AttributeID::DATA_TYPE, NodeID(ObjectID::UInt32));
        NewValue(ObjectID::StateNumber, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::StateNumber, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::StateNumber, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::StateNumber, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::StateNumber, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::StateNumber, AttributeID::VALUE_RANK, int32_t(-1));

        // References
        AddReference(ObjectID::StateNumber, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, Names::PropertyType, NodeClass::DataType, ObjectID::Null);
      }

      void TransitionType()
      {
        // Base Attributes
        NewValue(ObjectID::TransitionType, AttributeID::NODE_ID,      NodeID(ObjectID::TransitionType));
        NewValue(ObjectID::TransitionType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::TransitionType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::TransitionType));
        NewValue(ObjectID::TransitionType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::TransitionType));
        NewValue(ObjectID::TransitionType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::TransitionType));
        NewValue(ObjectID::TransitionType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::TransitionType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::TransitionType, AttributeID::IS_ABSTRACT, false);
      }

      void VendorServerInfoType()
      {
        // Base Attributes
        NewValue(ObjectID::VendorServerInfoType, AttributeID::NODE_ID,      NodeID(ObjectID::VendorServerInfoType));
        NewValue(ObjectID::VendorServerInfoType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ObjectType));
        NewValue(ObjectID::VendorServerInfoType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::VendorServerInfoType));
        NewValue(ObjectID::VendorServerInfoType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::VendorServerInfoType));
        NewValue(ObjectID::VendorServerInfoType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::VendorServerInfoType));
        NewValue(ObjectID::VendorServerInfoType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::VendorServerInfoType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::VendorServerInfoType, AttributeID::IS_ABSTRACT, false);
      }

      void ReferenceTypes()
      {
        // Attributes
        NewValue(ObjectID::ReferenceTypes, AttributeID::NODE_ID,      NodeID(ObjectID::ReferenceTypes));
        NewValue(ObjectID::ReferenceTypes, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ReferenceTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ReferenceTypes));
        NewValue(ObjectID::ReferenceTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ReferenceTypes));
        NewValue(ObjectID::ReferenceTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ReferenceTypes));
        NewValue(ObjectID::ReferenceTypes, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ReferenceTypes, AttributeID::USER_WRITE_MASK, 0);

        // References
        AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::ReferenceTypes, NodeClass::ObjectType, ObjectID::Null);
        AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::Organizes, ObjectID::References, Names::References, NodeClass::ReferenceType, ObjectID::Null);
      }

      void Refs()
      {
        // Attributes
        NewValue(ObjectID::References, AttributeID::NODE_ID,      NodeID(ObjectID::References));
        NewValue(ObjectID::References, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::References, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::References));
        NewValue(ObjectID::References, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::References));
        NewValue(ObjectID::References, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::References));
        NewValue(ObjectID::References, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::References, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::References, AttributeID::IS_ABSTRACT, true);
        // Reference attributes
        NewValue(ObjectID::References, AttributeID::SYMMETRIC, true);

        // References
        AddReference(ObjectID::References, forward, ReferenceID::HasSubtype, ObjectID::HierarchicalReferences, Names::HierarchicalReferences, NodeClass::ReferenceType, ObjectID::Null);
        AddReference(ObjectID::References, forward, ReferenceID::HasSubtype, ObjectID::NonHierarchicalReferences, Names::NonHierarchicalReferences, NodeClass::ReferenceType, ObjectID::Null);
      }

      void HierarchicalReferences()
      {
        // Attributes
        NewValue(ObjectID::HierarchicalReferences, AttributeID::NODE_ID,      NodeID(ObjectID::HierarchicalReferences));
        NewValue(ObjectID::HierarchicalReferences, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HierarchicalReferences, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HierarchicalReferences));
        NewValue(ObjectID::HierarchicalReferences, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HierarchicalReferences));
        NewValue(ObjectID::HierarchicalReferences, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HierarchicalReferences));
        NewValue(ObjectID::HierarchicalReferences, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HierarchicalReferences, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HierarchicalReferences, AttributeID::IS_ABSTRACT, true);
        // Reference attributes
        NewValue(ObjectID::HierarchicalReferences, AttributeID::SYMMETRIC, false);

        // References
        AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasChild, Names::HasChild, NodeClass::ReferenceType, ObjectID::Null);
        AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::HasEventSource, Names::HasEventSource, NodeClass::ReferenceType, ObjectID::Null);
        AddReference(ObjectID::HierarchicalReferences, forward, ReferenceID::HasSubtype, ObjectID::Organizes, Names::Organizes, NodeClass::ReferenceType, ObjectID::Null);
      }

      void HasChild()
      {
        // Attributes
        NewValue(ObjectID::HasChild, AttributeID::NODE_ID,      NodeID(ObjectID::HasChild));
        NewValue(ObjectID::HasChild, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasChild, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasChild));
        NewValue(ObjectID::HasChild, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasChild));
        NewValue(ObjectID::HasChild, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasChild));
        NewValue(ObjectID::HasChild, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasChild, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasChild, AttributeID::IS_ABSTRACT, true);
        // Reference attributes
        NewValue(ObjectID::HasChild, AttributeID::SYMMETRIC, false);

        // References
        AddReference(ObjectID::HasChild, forward, ReferenceID::HasSubtype, ObjectID::Aggregates, Names::Aggregates, NodeClass::ReferenceType, ObjectID::Null);
        AddReference(ObjectID::HasChild, forward, ReferenceID::HasSubtype, ObjectID::HasSubtype, Names::HasSubtype, NodeClass::ReferenceType, ObjectID::Null);
      }

      void Aggregates()
      {
        // Attributes
        NewValue(ObjectID::Aggregates, AttributeID::NODE_ID,      NodeID(ObjectID::Aggregates));
        NewValue(ObjectID::Aggregates, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::Aggregates, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Aggregates));
        NewValue(ObjectID::Aggregates, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Aggregates));
        NewValue(ObjectID::Aggregates, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Aggregates));
        NewValue(ObjectID::Aggregates, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Aggregates, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Aggregates, AttributeID::IS_ABSTRACT, true);
        // Reference attributes
        NewValue(ObjectID::Aggregates, AttributeID::SYMMETRIC, false);

        // References
        AddReference(ObjectID::Aggregates, forward, ReferenceID::HasSubtype, ObjectID::HasHistoricalConfiguration, Names::HasHistoricalConfiguration, NodeClass::ReferenceType, ObjectID::Null);
        AddReference(ObjectID::Aggregates, forward, ReferenceID::HasSubtype, ObjectID::HasComponent, Names::HasComponent, NodeClass::ReferenceType, ObjectID::Null);
        AddReference(ObjectID::Aggregates, forward, ReferenceID::HasSubtype, ObjectID::HasProperty, Names::HasProperty, NodeClass::ReferenceType, ObjectID::Null);
      }

      void HasComponent()
      {
        // Attributes
        NewValue(ObjectID::HasComponent, AttributeID::NODE_ID,      NodeID(ObjectID::HasComponent));
        NewValue(ObjectID::HasComponent, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasComponent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasComponent));
        NewValue(ObjectID::HasComponent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasComponent));
        NewValue(ObjectID::HasComponent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasComponent));
        NewValue(ObjectID::HasComponent, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasComponent, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasComponent, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasComponent, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasComponent, AttributeID::INVERSE_NAME, LocalizedText("ComponentOf"));

        // References
        AddReference(ObjectID::HasComponent, forward, ReferenceID::HasSubtype, ObjectID::HasOrderedComponent, Names::HasOrderedComponent, NodeClass::ReferenceType, ObjectID::Null);
      }

      void HasOrderedComponent()
      {
        // Attributes
        NewValue(ObjectID::HasOrderedComponent, AttributeID::NODE_ID,      NodeID(ObjectID::HasOrderedComponent));
        NewValue(ObjectID::HasOrderedComponent, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasOrderedComponent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasOrderedComponent));
        NewValue(ObjectID::HasOrderedComponent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasOrderedComponent));
        NewValue(ObjectID::HasOrderedComponent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasOrderedComponent));
        NewValue(ObjectID::HasOrderedComponent, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasOrderedComponent, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasOrderedComponent, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasOrderedComponent, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasOrderedComponent, AttributeID::INVERSE_NAME, LocalizedText("OrederedComponentOf"));
      }

      void HasHistoricalConfiguration()
      {
        // Attributes
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::NODE_ID,      NodeID(ObjectID::HasHistoricalConfiguration));
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasHistoricalConfiguration));
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasHistoricalConfiguration));
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasHistoricalConfiguration));
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasHistoricalConfiguration, AttributeID::INVERSE_NAME, LocalizedText("HistoricalConfigurationOf"));
      }

      void HasProperty()
      {
        // Attributes
        NewValue(ObjectID::HasProperty, AttributeID::NODE_ID,      NodeID(ObjectID::HasProperty));
        NewValue(ObjectID::HasProperty, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasProperty, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasProperty));
        NewValue(ObjectID::HasProperty, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasProperty));
        NewValue(ObjectID::HasProperty, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasProperty));
        NewValue(ObjectID::HasProperty, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasProperty, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasProperty, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasProperty, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasProperty, AttributeID::INVERSE_NAME, LocalizedText("PropertyOf"));
      }

      void HasSubtype()
      {
        // Attributes
        NewValue(ObjectID::HasSubtype, AttributeID::NODE_ID,      NodeID(ObjectID::HasSubtype));
        NewValue(ObjectID::HasSubtype, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasSubtype, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasSubtype));
        NewValue(ObjectID::HasSubtype, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasSubtype));
        NewValue(ObjectID::HasSubtype, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasSubtype));
        NewValue(ObjectID::HasSubtype, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasSubtype, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasSubtype, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasSubtype, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasSubtype, AttributeID::INVERSE_NAME, LocalizedText("SubtypeOf"));
      }

      void HasEventSource()
      {
        // Attributes
        NewValue(ObjectID::HasEventSource, AttributeID::NODE_ID,      NodeID(ObjectID::HasEventSource));
        NewValue(ObjectID::HasEventSource, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasEventSource, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasEventSource));
        NewValue(ObjectID::HasEventSource, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasEventSource));
        NewValue(ObjectID::HasEventSource, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasEventSource));
        NewValue(ObjectID::HasEventSource, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasEventSource, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasEventSource, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasEventSource, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasEventSource, AttributeID::INVERSE_NAME, LocalizedText("EventSourceOf"));
        // References
        AddReference(ObjectID::HasEventSource, forward, ReferenceID::HasSubtype, ObjectID::HasNotifier, Names::HasNotifier, NodeClass::ReferenceType, ObjectID::Null);
      }

      void HasNotifier()
      {
        // Attributes
        NewValue(ObjectID::HasNotifier, AttributeID::NODE_ID,      NodeID(ObjectID::HasNotifier));
        NewValue(ObjectID::HasNotifier, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasNotifier, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasNotifier));
        NewValue(ObjectID::HasNotifier, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasNotifier));
        NewValue(ObjectID::HasNotifier, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasNotifier));
        NewValue(ObjectID::HasNotifier, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasNotifier, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasNotifier, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasNotifier, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasNotifier, AttributeID::INVERSE_NAME, LocalizedText("NotifierOf"));
      }

      void Organizes()
      {
        // Attributes
        NewValue(ObjectID::Organizes, AttributeID::NODE_ID,      NodeID(ObjectID::Organizes));
        NewValue(ObjectID::Organizes, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::Organizes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::Organizes));
        NewValue(ObjectID::Organizes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::Organizes));
        NewValue(ObjectID::Organizes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::Organizes));
        NewValue(ObjectID::Organizes, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::Organizes, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::Organizes, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::Organizes, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::Organizes, AttributeID::INVERSE_NAME, LocalizedText("OrganizedBy"));
      }

      void NonHierarchicalReferences()
      {
        // Attributes
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::NODE_ID,      NodeID(ObjectID::NonHierarchicalReferences));
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::NonHierarchicalReferences));
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::NonHierarchicalReferences));
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::NonHierarchicalReferences));
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::IS_ABSTRACT, true);
        // Reference attributes
        NewValue(ObjectID::NonHierarchicalReferences, AttributeID::SYMMETRIC, false);

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
        NewValue(ObjectID::FromState, AttributeID::NODE_ID,      NodeID(ObjectID::FromState));
        NewValue(ObjectID::FromState, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::FromState, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::FromState));
        NewValue(ObjectID::FromState, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::FromState));
        NewValue(ObjectID::FromState, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::FromState));
        NewValue(ObjectID::FromState, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::FromState, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::FromState, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::FromState, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::FromState, AttributeID::INVERSE_NAME, LocalizedText("ToTransition"));
      }

      void GeneratesEvent()
      {
        // Attributes
        NewValue(ObjectID::GeneratesEvent, AttributeID::NODE_ID,      NodeID(ObjectID::GeneratesEvent));
        NewValue(ObjectID::GeneratesEvent, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::GeneratesEvent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::GeneratesEvent));
        NewValue(ObjectID::GeneratesEvent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::GeneratesEvent));
        NewValue(ObjectID::GeneratesEvent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::GeneratesEvent));
        NewValue(ObjectID::GeneratesEvent, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::GeneratesEvent, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::GeneratesEvent, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::GeneratesEvent, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::GeneratesEvent, AttributeID::INVERSE_NAME, LocalizedText("GeneratesBy"));
      }

      void HasCause()
      {
        // Attributes
        NewValue(ObjectID::HasCause, AttributeID::NODE_ID,      NodeID(ObjectID::HasCause));
        NewValue(ObjectID::HasCause, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasCause, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasCause));
        NewValue(ObjectID::HasCause, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasCause));
        NewValue(ObjectID::HasCause, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasCause));
        NewValue(ObjectID::HasCause, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasCause, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasCause, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasCause, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasCause, AttributeID::INVERSE_NAME, LocalizedText("MayBeCausedBy"));
      }

      void HasDescription()
      {
        // Attributes
        NewValue(ObjectID::HasDescription, AttributeID::NODE_ID,      NodeID(ObjectID::HasDescription));
        NewValue(ObjectID::HasDescription, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasDescription, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasDescription));
        NewValue(ObjectID::HasDescription, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasDescription));
        NewValue(ObjectID::HasDescription, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasDescription));
        NewValue(ObjectID::HasDescription, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasDescription, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasDescription, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasDescription, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasDescription, AttributeID::INVERSE_NAME, LocalizedText("DescriptionOf"));
      }

      void HasEffect()
      {
        // Attributes
        NewValue(ObjectID::HasEffect, AttributeID::NODE_ID,      NodeID(ObjectID::HasEffect));
        NewValue(ObjectID::HasEffect, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasEffect, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasEffect));
        NewValue(ObjectID::HasEffect, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasEffect));
        NewValue(ObjectID::HasEffect, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasEffect));
        NewValue(ObjectID::HasEffect, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasEffect, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasEffect, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasEffect, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasEffect, AttributeID::INVERSE_NAME, LocalizedText("MayBeEffectedBy"));
      }

      void HasEncoding()
      {
        // Attributes
        NewValue(ObjectID::HasEncoding, AttributeID::NODE_ID,      NodeID(ObjectID::HasEncoding));
        NewValue(ObjectID::HasEncoding, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasEncoding, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasEncoding));
        NewValue(ObjectID::HasEncoding, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasEncoding));
        NewValue(ObjectID::HasEncoding, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasEncoding));
        NewValue(ObjectID::HasEncoding, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasEncoding, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasEncoding, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasEncoding, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasEncoding, AttributeID::INVERSE_NAME, LocalizedText("EncodingOf"));
      }

      void HasModelParent()
      {
        // Attributes
        NewValue(ObjectID::HasModelParent, AttributeID::NODE_ID,      NodeID(ObjectID::HasModelParent));
        NewValue(ObjectID::HasModelParent, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasModelParent, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasModelParent));
        NewValue(ObjectID::HasModelParent, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasModelParent));
        NewValue(ObjectID::HasModelParent, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasModelParent));
        NewValue(ObjectID::HasModelParent, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasModelParent, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasModelParent, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasModelParent, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasModelParent, AttributeID::INVERSE_NAME, LocalizedText("ModelParentOf"));
      }

      void HasModellingRule()
      {
        // Attributes
        NewValue(ObjectID::HasModellingRule, AttributeID::NODE_ID,      NodeID(ObjectID::HasModellingRule));
        NewValue(ObjectID::HasModellingRule, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasModellingRule, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasModellingRule));
        NewValue(ObjectID::HasModellingRule, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasModellingRule));
        NewValue(ObjectID::HasModellingRule, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasModellingRule));
        NewValue(ObjectID::HasModellingRule, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasModellingRule, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasModellingRule, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasModellingRule, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasModellingRule, AttributeID::INVERSE_NAME, LocalizedText("ModellingRuleOf"));
      }

      void HasTypeDefinition()
      {
        // Attributes
        NewValue(ObjectID::HasTypeDefinition, AttributeID::NODE_ID,      NodeID(ObjectID::HasTypeDefinition));
        NewValue(ObjectID::HasTypeDefinition, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::HasTypeDefinition, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::HasTypeDefinition));
        NewValue(ObjectID::HasTypeDefinition, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::HasTypeDefinition));
        NewValue(ObjectID::HasTypeDefinition, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::HasTypeDefinition));
        NewValue(ObjectID::HasTypeDefinition, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::HasTypeDefinition, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::HasTypeDefinition, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::HasTypeDefinition, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::HasTypeDefinition, AttributeID::INVERSE_NAME, LocalizedText("TypeDefinitionOf"));
      }

      void ToState()
      {
        // Attributes
        NewValue(ObjectID::ToState, AttributeID::NODE_ID,      NodeID(ObjectID::ToState));
        NewValue(ObjectID::ToState, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::ReferenceType));
        NewValue(ObjectID::ToState, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ToState));
        NewValue(ObjectID::ToState, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ToState));
        NewValue(ObjectID::ToState, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ToState));
        NewValue(ObjectID::ToState, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ToState, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::ToState, AttributeID::IS_ABSTRACT, false);
        // Reference attributes
        NewValue(ObjectID::ToState, AttributeID::SYMMETRIC, false);
        NewValue(ObjectID::ToState, AttributeID::INVERSE_NAME, LocalizedText("FromTransition"));
      }

      void VariableTypes()
      {
        // Attributes
        NewValue(ObjectID::VariableTypes, AttributeID::NODE_ID,      NodeID(ObjectID::VariableTypes));
        NewValue(ObjectID::VariableTypes, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::VariableTypes, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::VariableTypes));
        NewValue(ObjectID::VariableTypes, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::VariableTypes));
        NewValue(ObjectID::VariableTypes, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::VariableTypes));
        NewValue(ObjectID::VariableTypes, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::VariableTypes, AttributeID::USER_WRITE_MASK, 0);
        // References
        AddReference(ObjectID::VariableTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, Names::FolderType, NodeClass::Object, ObjectID::Null);
        AddReference(ObjectID::VariableTypes, forward, ReferenceID::Organizes, ObjectID::BaseVariableType, Names::BaseVariableType, NodeClass::VariableType, ObjectID::Null);
      }

      void BaseVariableType()
      {
        // Attributes
        NewValue(ObjectID::BaseVariableType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseVariableType));
        NewValue(ObjectID::BaseVariableType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::VariableType));
        NewValue(ObjectID::BaseVariableType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseVariableType));
        NewValue(ObjectID::BaseVariableType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseVariableType));
        NewValue(ObjectID::BaseVariableType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseVariableType));
        NewValue(ObjectID::BaseVariableType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BaseVariableType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::BaseVariableType, AttributeID::IS_ABSTRACT, false);
        // Variable Attributes
        NewValue(ObjectID::BaseVariableType, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::BaseVariableType, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
        NewValue(ObjectID::BaseVariableType, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::BaseVariableType, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BaseVariableType, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BaseVariableType, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::BaseVariableType, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::BaseVariableType, AttributeID::VALUE_RANK, int32_t(0));
        // References
        AddReference(ObjectID::BaseVariableType, forward, ReferenceID::HasSubtype, ObjectID::BaseDataVariableType, Names::BaseDataVariableType, NodeClass::VariableType, ObjectID::Null);
        AddReference(ObjectID::BaseVariableType, forward, ReferenceID::HasSubtype, ObjectID::PropertyType, Names::PropertyType, NodeClass::VariableType, ObjectID::Null);
      }

      void BaseDataVariableType()
      {
        // Attributes
        NewValue(ObjectID::BaseDataVariableType, AttributeID::NODE_ID,      NodeID(ObjectID::BaseDataVariableType));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::VariableType));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::BaseDataVariableType));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::BaseDataVariableType));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::BaseDataVariableType));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::BaseDataVariableType, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(ObjectID::BaseDataVariableType, AttributeID::IS_ABSTRACT, false);
        // Variable Attributes
        NewValue(ObjectID::BaseDataVariableType, AttributeID::VALUE, NodeID());
        NewValue(ObjectID::BaseDataVariableType, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(ObjectID::BaseDataVariableType, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(ObjectID::BaseDataVariableType, AttributeID::HISTORIZING, false);
        NewValue(ObjectID::BaseDataVariableType, AttributeID::VALUE_RANK, int32_t(0));
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
        NewValue(id, AttributeID::NODE_ID,      NodeID(id));
        NewValue(id, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::VariableType));
        NewValue(id, AttributeID::BROWSE_NAME,  QualifiedName(0, name));
        NewValue(id, AttributeID::DISPLAY_NAME, LocalizedText(name));
        NewValue(id, AttributeID::DESCRIPTION,  LocalizedText(name));
        NewValue(id, AttributeID::WRITE_MASK,   0);
        NewValue(id, AttributeID::USER_WRITE_MASK, 0);
        // Type Attributes
        NewValue(id, AttributeID::IS_ABSTRACT, false);
        // Variable Attributes
        NewValue(id, AttributeID::VALUE, NodeID());
        NewValue(id, AttributeID::DATA_TYPE, NodeID(ObjectID::BaseDataType));
        NewValue(id, AttributeID::ARRAY_DIMENSIONS, std::vector<uint32_t>());
        NewValue(id, AttributeID::ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(id, AttributeID::USER_ACCESS_LEVEL, static_cast<uint8_t>(VariableAccessLevel::CurrentRead));
        NewValue(id, AttributeID::MINIMUM_SAMPLING_INTERVAL, Duration(0));
        NewValue(id, AttributeID::HISTORIZING, false);
        NewValue(id, AttributeID::VALUE_RANK, int32_t(0));
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
      OpcUa::Remote::NodeManagementServices& Registry;
      const bool Debug;
    };

} // namespace

namespace OpcUa
{
  namespace UaServer
  {

    void FillStandardNamespace(OpcUa::Remote::NodeManagementServices& registry, bool debug)
    {
      StandardNamespace ns(registry, debug);
      ns.Fill();
    }

  } // namespace UaServer
} // namespace OpcUa
