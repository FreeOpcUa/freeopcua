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
#include <opc/ua/protocol/node_classes.h>
#include <opc/ua/protocol/strings.h>
#include <opc/ua/protocol/variable_access_level.h>
#include <opc/ua/services/node_management.h>

#include <algorithm>
#include <iostream>
#include <map>

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

       //Root();
         Objects();
         Views();
         //Server();
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
/*
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
*/
      }

    private:

      void NewValue(NodeID node, AttributeID attribute, Variant value)
      {
        //Registry.AddAttribute(node, attribute, value);
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
        NodeClass targetNodeClass)
      {
        AddReferencesItem ref;
        ref.IsForward = isForward;
        ref.ReferenceTypeId = referenceType;
        ref.SourceNodeID = sourceNode;
        ref.TargetNodeClass = targetNodeClass;
        ref.TargetNodeID = targetNode;

        Registry.AddReferences(std::vector<AddReferencesItem>{ref});
      }


      void Root()
      {
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Root);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Root);

        AddNodesItem node;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Root);
        node.Class = NodeClass::Object;
        node.RequestedNewNodeID = ObjectID::RootFolder;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // Referencies
        AddReference(ObjectID::RootFolder,  forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType,  NodeClass::ObjectType);
      }

      void Objects()
      {
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ObjectsFolder;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Objects);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::RootFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Objects);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Objects);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AddReference(ObjectID::ObjectsFolder, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, NodeClass::ObjectType);
      }

      void Views()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ViewsFolder;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Views);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::RootFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Views);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Views);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AddReference(ObjectID::ViewsFolder, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, NodeClass::ObjectType);
      }

/*
      // Server object have to be created based on Server type in the some addon;
      void Server()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Server);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ObjectsFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::ServerType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Server);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Server);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AddReference(ObjectID::Server, forward, ReferenceID::HasComponent, ObjectID::NamespaceArray, Names::NamespaceArray, NodeClass::Variable, ObjectID::PropertyType);
        AddReference(ObjectID::Server, forward, ReferenceID::HasProperty,  ObjectID::ServerStatus, Names::ServerStatus, NodeClass::Variable, ObjectID::ServerStatusType);
        AddReference(ObjectID::Server, forward, ReferenceID::HasProperty,  ObjectID::ServerArray, Names::ServerArray, NodeClass::Variable, ObjectID::PropertyType);
      }
/*
    void ServerCapabilities()
      {
        // Attributes
        NewValue(ObjectID::ServerCapabilities, AttributeID::NODE_ID,      NodeID(ObjectID::));
        NewValue(ObjectID::ServerCapabilities, AttributeID::NODE_CLASS,   static_cast<int32_t>(NodeClass::Object));
        NewValue(ObjectID::ServerCapabilities, AttributeID::BROWSE_NAME,  QualifiedName(0, OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::DISPLAY_NAME, LocalizedText(OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::DESCRIPTION,  LocalizedText(OpcUa::Names::ServerCapabilities));
        NewValue(ObjectID::ServerCapabilities, AttributeID::WRITE_MASK,   0);
        NewValue(ObjectID::ServerCapabilities, AttributeID::USER_WRITE_MASK, 0);

        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilities;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerCapabilities);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::PropertyType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerCapabilities);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerCapabilities);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

*/

      void Types()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::TypesFolder;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Types);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::RootFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Types);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Types);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::TypesFolder, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, NodeClass::ObjectType);
      }

      void DataTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DataTypes;
        node.BrowseName = QualifiedName(0, OpcUa::Names::DataTypes);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::TypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::DataTypes);
        attrs.DisplayName = LocalizedText(OpcUa::Names::DataTypes);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::DataTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, NodeClass::ObjectType);
      }

      void BaseDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseDataType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::DataTypes;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseDataType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseDataType);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::ExpandedNodeID, NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Guid,           NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::LocalizedText,  NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::NodeID,         NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Number,         NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::QualifiedName,  NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::StatusCode,     NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::String,         NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::Structure,      NodeClass::DataType);
        AddReference(ObjectID::BaseDataType, forward, ReferenceID::HasSubtype, ObjectID::XmlElement,     NodeClass::DataType);
      }

      void BooleanType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Boolean;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Boolean);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Boolean);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Boolean);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ByteStringType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ByteString;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ByteString);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ByteString);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ByteString);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ImageType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Image;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Image);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Image);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Image);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ImageBmpType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ImageBmp;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ImageBmp);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Image;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ImageBmp);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ImageBmp);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ImageGifType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ImageGif;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ImageGif);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Image;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ImageGif);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ImageGif);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ImageJpgType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ImageJpg;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ImageJpg);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Image;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ImageJpg);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ImageJpg);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ImagePngType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ImagePng;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ImagePng);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Image;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ImagePng);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ImagePng);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DataValueType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DataValue;
        node.BrowseName = QualifiedName(0, OpcUa::Names::DataValue);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::DataValue);
        attrs.DisplayName = LocalizedText(OpcUa::Names::DataValue);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DateTimeType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DateTime;
        node.BrowseName = QualifiedName(0, OpcUa::Names::DateTime);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::DateTime);
        attrs.DisplayName = LocalizedText(OpcUa::Names::DateTime);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void UtcTimeType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UtcTime;
        node.BrowseName = QualifiedName(0, Names::UtcTime);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::DateTime;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UtcTime);
        attrs.DisplayName = LocalizedText(Names::UtcTime);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DiagnosticInfoType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DiagnosticInfo;
        node.BrowseName = QualifiedName(0, Names::DiagnosticInfo);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::DiagnosticInfo);
        attrs.DisplayName = LocalizedText(Names::DiagnosticInfo);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void EnumerationType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Enumeration;
        node.BrowseName = QualifiedName(0, Names::Enumeration);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Enumeration);
        attrs.DisplayName = LocalizedText(Names::Enumeration);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void IdType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::IdType;
        node.BrowseName = QualifiedName(0, Names::IdType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Enumeration;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::IdType);
        attrs.DisplayName = LocalizedText(Names::IdType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

      }

      void EnumStrings(ObjectID nodeID, ObjectID parentNode, const std::vector<LocalizedText>& values)
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = nodeID;
        node.BrowseName = QualifiedName(0, Names::EnumStrings);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = parentNode;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::EnumStrings);
        attrs.DisplayName = LocalizedText(Names::EnumStrings);
        attrs.AccessLevel = VariableAccessLevel::CurrentRead;
        attrs.UserAccessLevel = VariableAccessLevel::CurrentRead;
        attrs.Dimensions = {0};
        attrs.Historizing = false;
        attrs.MinimumSamplingInterval = Duration(100);
        attrs.Rank = 1;
        attrs.Value = values;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(nodeID, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
      }

      void IdTypeEnumStrings()
      {
        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Numeric"));
        values.push_back(LocalizedText("String"));
        values.push_back(LocalizedText("Guid"));
        values.push_back(LocalizedText("Opaque"));
        EnumStrings(ObjectID::IdTypeEnumStrings, ObjectID::IdType, values);
      }

      void MessageSecurityModeType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::MessageSecurityMode;
        node.BrowseName = QualifiedName(0, Names::MessageSecurityMode);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Enumeration;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::MessageSecurityMode);
        attrs.DisplayName = LocalizedText(Names::MessageSecurityMode);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void MessageSecurityModeTypeEnumStrings()
      {
        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Invalid"));
        values.push_back(LocalizedText("None"));
        values.push_back(LocalizedText("Sign"));
        values.push_back(LocalizedText("SignAndEncrypt"));
        EnumStrings(ObjectID::MessageSecurityModeEnumStrings, ObjectID::MessageSecurityMode, values);
      }

      void NodeClassType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::NodeClass;
        node.BrowseName = QualifiedName(0, Names::NodeClass);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::NodeClass);
        attrs.DisplayName = LocalizedText(Names::NodeClass);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::Enumeration, forward, ReferenceID::HasSubtype, ObjectID::NodeClass, NodeClass::DataType);
      }

      void RedundancySupportType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::RedundancySupport;
        node.BrowseName = QualifiedName(0, Names::RedundancySupport);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Enumeration;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::RedundancySupport);
        attrs.DisplayName = LocalizedText(Names::RedundancySupport);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void RedundancySupportTypeEnumStrings()
      {
        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("None"));
        values.push_back(LocalizedText("Cold"));
        values.push_back(LocalizedText("Warm"));
        values.push_back(LocalizedText("Hot"));
        values.push_back(LocalizedText("Transparent"));
        EnumStrings(ObjectID::RedundancySupportEnumStrings, ObjectID::RedundancySupport, values);
      }

      void SecurityTokenRequestType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SecurityTokenRequestType;
        node.BrowseName = QualifiedName(0, Names::SecurityTokenRequestType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Enumeration;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SecurityTokenRequestType);
        attrs.DisplayName = LocalizedText(Names::SecurityTokenRequestType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SecurityTokenRequestTypeEnumStrings()
      {
        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Issue"));
        values.push_back(LocalizedText("Renew"));
        EnumStrings(ObjectID::SecurityTokenRequestTypeEnumStrings, ObjectID::SecurityTokenRequestType, values);
      }

      void ServerStateType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerState;
        node.BrowseName = QualifiedName(0, Names::ServerState);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Enumeration;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerState);
        attrs.DisplayName = LocalizedText(Names::ServerState);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
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
        EnumStrings(ObjectID::ServerStateEnumStrings, ObjectID::ServerState, values);
      }

      void ExpandedNodeID()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ExpandedNodeID;
        node.BrowseName = QualifiedName(0, Names::ExpandedNodeID);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ExpandedNodeID);
        attrs.DisplayName = LocalizedText(Names::ExpandedNodeID);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void Guid()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Guid;
        node.BrowseName = QualifiedName(0, Names::Guid);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Guid);
        attrs.DisplayName = LocalizedText(Names::Guid);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void LocalizedTextType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::LocalizedText;
        node.BrowseName = QualifiedName(0, Names::LocalizedText);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::LocalizedText);
        attrs.DisplayName = LocalizedText(Names::LocalizedText);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void NodeIDType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::NodeID;
        node.BrowseName = QualifiedName(0, Names::NodeID);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::NodeID);
        attrs.DisplayName = LocalizedText(Names::NodeID);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void NumberType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Number;
        node.BrowseName = QualifiedName(0, Names::Number);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Number);
        attrs.DisplayName = LocalizedText(Names::Number);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DoubleType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Double;
        node.BrowseName = QualifiedName(0, Names::Double);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Number;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Double);
        attrs.DisplayName = LocalizedText(Names::Double);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DurationType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Duration;
        node.BrowseName = QualifiedName(0, Names::Duration);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Double;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Duration);
        attrs.DisplayName = LocalizedText(Names::Duration);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void FloatType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Float;
        node.BrowseName = QualifiedName(0, Names::Float);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Number;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Float);
        attrs.DisplayName = LocalizedText(Names::Float);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void IntegerType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Integer;
        node.BrowseName = QualifiedName(0, Names::Integer);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Number;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Integer);
        attrs.DisplayName = LocalizedText(Names::Integer);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void Int16Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Int16;
        node.BrowseName = QualifiedName(0, Names::Int16);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Integer;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Int16);
        attrs.DisplayName = LocalizedText(Names::Int16);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void Int32Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Int32;
        node.BrowseName = QualifiedName(0, Names::Int32);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Integer;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Int32);
        attrs.DisplayName = LocalizedText(Names::Int32);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void Int64Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Int64;
        node.BrowseName = QualifiedName(0, Names::Int64);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Integer;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Int64);
        attrs.DisplayName = LocalizedText(Names::Int64);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SByteType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SByte;
        node.BrowseName = QualifiedName(0, Names::SByte);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Integer;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SByte);
        attrs.DisplayName = LocalizedText(Names::SByte);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void UIntegerType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UInteger;
        node.BrowseName = QualifiedName(0, Names::UInteger);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Integer;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UInteger);
        attrs.DisplayName = LocalizedText(Names::UInteger);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void UInt16Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UInt16;
        node.BrowseName = QualifiedName(0, Names::UInt16);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UInteger;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UInt16);
        attrs.DisplayName = LocalizedText(Names::UInt16);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void UInt32Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UInt32;
        node.BrowseName = QualifiedName(0, Names::UInt32);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UInteger;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UInt32);
        attrs.DisplayName = LocalizedText(Names::UInt32);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void UInt64Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UInt64;
        node.BrowseName = QualifiedName(0, Names::UInt64);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UInteger;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UInt64);
        attrs.DisplayName = LocalizedText(Names::UInt64);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ByteType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Byte;
        node.BrowseName = QualifiedName(0, Names::Byte);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UInteger;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Byte);
        attrs.DisplayName = LocalizedText(Names::Byte);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void QualifiedNameType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::QualifiedName;
        node.BrowseName = QualifiedName(0, Names::QualifiedName);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::QualifiedName);
        attrs.DisplayName = LocalizedText(Names::QualifiedName);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StatusCodeType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StatusCode;
        node.BrowseName = QualifiedName(0, Names::StatusCode);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::StatusCode);
        attrs.DisplayName = LocalizedText(Names::StatusCode);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StringType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::String;
        node.BrowseName = QualifiedName(0, Names::String);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::String);
        attrs.DisplayName = LocalizedText(Names::String);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void LocaleIDType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::LocaleID;
        node.BrowseName = QualifiedName(0, Names::LocaleID);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::String;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::LocaleID);
        attrs.DisplayName = LocalizedText(Names::LocaleID);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void NumericRangeType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::NumericRange;
        node.BrowseName = QualifiedName(0, Names::NumericRange);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::String;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::NumericRange);
        attrs.DisplayName = LocalizedText(Names::NumericRange);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Structure;
        node.BrowseName = QualifiedName(0, Names::Structure);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Structure);
        attrs.DisplayName = LocalizedText(Names::Structure);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureAddNodesItem()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureAddNodesItem;
        node.BrowseName = QualifiedName(0, Names::AddNodesItem);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::AddNodesItem);
        attrs.DisplayName = LocalizedText(Names::AddNodesItem);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureAddReferencesItem()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureAddReferencesItem;
        node.BrowseName = QualifiedName(0, Names::AddReferencesItem);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::AddReferencesItem);
        attrs.DisplayName = LocalizedText(Names::AddReferencesItem);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureApplicationDescriptionType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureApplicationDescription;
        node.BrowseName = QualifiedName(0, Names::ApplicationDescription);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ApplicationDescription);
        attrs.DisplayName = LocalizedText(Names::ApplicationDescription);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureArgument()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureArgument;
        node.BrowseName = QualifiedName(0, Names::Argument);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Argument);
        attrs.DisplayName = LocalizedText(Names::Argument);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureBuildInfo()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureBuildInfo;
        node.BrowseName = QualifiedName(0, Names::BuildInfo);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildInfo);
        attrs.DisplayName = LocalizedText(Names::BuildInfo);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureDeleteNodesItem()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureDeleteNodesItem;
        node.BrowseName = QualifiedName(0, Names::DeleteNodesItem);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::DeleteNodesItem);
        attrs.DisplayName = LocalizedText(Names::DeleteNodesItem);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureDeleteReferencesItem()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureDeleteReferencesItem;
        node.BrowseName = QualifiedName(0, Names::DeleteReferencesItem);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::DeleteReferencesItem);
        attrs.DisplayName = LocalizedText(Names::DeleteReferencesItem);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureEUInformation()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureEUInformation;
        node.BrowseName = QualifiedName(0, Names::EUInformation);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::EUInformation);
        attrs.DisplayName = LocalizedText(Names::EUInformation);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureModelChangeStructureDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureModelChangeStructureDataType;
        node.BrowseName = QualifiedName(0, Names::ModelChangeStructureDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ModelChangeStructureDataType);
        attrs.DisplayName = LocalizedText(Names::ModelChangeStructureDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureRange()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureRange;
        node.BrowseName = QualifiedName(0, Names::Range);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::Range);
        attrs.DisplayName = LocalizedText(Names::Range);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureSamplingIntervalDiagnosticsDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureSamplingIntervalDiagnosticsDataType;
        node.BrowseName = QualifiedName(0, Names::SamplingIntervalDiagnosticsDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SamplingIntervalDiagnosticsDataType);
        attrs.DisplayName = LocalizedText(Names::SamplingIntervalDiagnosticsDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureSemanticChangeStructureDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureSemanticChangeStructureDataType;
        node.BrowseName = QualifiedName(0, Names::SemanticChangeStructureDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SemanticChangeStructureDataType);
        attrs.DisplayName = LocalizedText(Names::SemanticChangeStructureDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureServerDiagnosticsSummaryDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureServerDiagnosticsSummaryType;
        node.BrowseName = QualifiedName(0, Names::ServerDiagnosticsSummaryType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerDiagnosticsSummaryType);
        attrs.DisplayName = LocalizedText(Names::ServerDiagnosticsSummaryType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureServerStatusDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureServerStatusDataType;
        node.BrowseName = QualifiedName(0, Names::ServerStatusDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerStatusDataType);
        attrs.DisplayName = LocalizedText(Names::ServerStatusDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureServiceCounterDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureServiceCounterDataType;
        node.BrowseName = QualifiedName(0, Names::ServiceCounterDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServiceCounterDataType);
        attrs.DisplayName = LocalizedText(Names::ServiceCounterDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureSessionDiagnosticsDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureSessionDiagnosticsDataType;
        node.BrowseName = QualifiedName(0, Names::SessionDiagnosticsDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SessionDiagnosticsDataType);
        attrs.DisplayName = LocalizedText(Names::SessionDiagnosticsDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureSessionSecurityDiagnosticsDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureSessionSecurityDiagnosticsDataType;
        node.BrowseName = QualifiedName(0, Names::SessionSecurityDiagnosticsDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SessionSecurityDiagnosticsDataType);
        attrs.DisplayName = LocalizedText(Names::SessionSecurityDiagnosticsDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureSignedSoftwareCertificate()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureSignedSoftwareCertificate;
        node.BrowseName = QualifiedName(0, Names::SignedSoftwareCertificate);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SignedSoftwareCertificate);
        attrs.DisplayName = LocalizedText(Names::SignedSoftwareCertificate);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureStatusResult()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureStatusResult;
        node.BrowseName = QualifiedName(0, Names::StatusResult);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::StatusResult);
        attrs.DisplayName = LocalizedText(Names::StatusResult);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureSubscriptionDiagnosticsDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureSubscriptionDiagnosticsDataType;
        node.BrowseName = QualifiedName(0, Names::SubscriptionDiagnosticsDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::SubscriptionDiagnosticsDataType);
        attrs.DisplayName = LocalizedText(Names::SubscriptionDiagnosticsDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureUserIdentifyTokenType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StructureUserIdentifyToken;
        node.BrowseName = QualifiedName(0, Names::UserIdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UserIdentifyToken);
        attrs.DisplayName = LocalizedText(Names::UserIdentifyToken);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void AnonymousIdentifyToken()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::AnonymousIdentifyToken;
        node.BrowseName = QualifiedName(0, Names::AnonymousIdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::StructureUserIdentifyToken;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::AnonymousIdentifyToken);
        attrs.DisplayName = LocalizedText(Names::AnonymousIdentifyToken);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void UserNameIdentifyToken()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UserNameIdentifyToken;
        node.BrowseName = QualifiedName(0, Names::UserNameIdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::StructureUserIdentifyToken;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UserNameIdentifyToken);
        attrs.DisplayName = LocalizedText(Names::UserNameIdentifyToken);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void X509IdentifyToken()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::X509IdentifyToken;
        node.BrowseName = QualifiedName(0, Names::X509IdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::StructureUserIdentifyToken;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::X509IdentifyToken);
        attrs.DisplayName = LocalizedText(Names::X509IdentifyToken);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void XmlElement()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::XmlElement;
        node.BrowseName = QualifiedName(0, Names::XmlElement);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::XmlElement);
        attrs.DisplayName = LocalizedText(Names::XmlElement);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void EventTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::EventTypes;
        node.BrowseName = QualifiedName(0, OpcUa::Names::EventTypes);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::TypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::EventTypes);
        attrs.DisplayName = LocalizedText(OpcUa::Names::EventTypes);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BaseEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::EventTypes;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BaseModelChangeEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseModelChangeEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseModelChangeEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseModelChangeEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseModelChangeEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SemanticChangeEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SemanticChangeEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SemanticChangeEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SemanticChangeEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SemanticChangeEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SystemEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SystemEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SystemEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SystemEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SystemEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DeviceFailureEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DeviceFailureEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::DeviceFailureEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::SystemEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SystemEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SystemEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void RefreshEndEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::RefreshEndEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::RefreshEndEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::SystemEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::RefreshEndEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::RefreshEndEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void RefreshRequiredEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::RefreshRequiredEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::RefreshRequiredEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::SystemEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::RefreshRequiredEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::RefreshRequiredEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void RefreshStartEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::RefreshStartEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::RefreshStartEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::SystemEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::RefreshStartEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::RefreshStartEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void EventID()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::EventID;
        node.BrowseName = QualifiedName(0, OpcUa::Names::EventID);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::RefreshStartEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::RefreshStartEventType);
        attrs.Value = ByteString();
        attrs.Type = ObjectID::ByteString;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::EventID, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::EventID, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void EventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::EventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::EventType);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::EventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::EventType);
        attrs.Value = NodeID();
        attrs.Type = ObjectID::NodeID;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::EventType, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::EventType, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }
      void LocalTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::LocalTime;
        node.BrowseName = QualifiedName(0, OpcUa::Names::LocalTime);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::LocalTime);
        attrs.DisplayName = LocalizedText(OpcUa::Names::LocalTime);
        attrs.Value = DateTime();
        attrs.Type = ObjectID::DateTime;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::LocalTime, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::LocalTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void Message()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Message;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Message);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Message);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Message);
        attrs.Value = DateTime();
        attrs.Type = ObjectID::DateTime;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::Message, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::Message, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ReceiveTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ReceiveTime;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ReceiveTime);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ReceiveTime);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ReceiveTime);
        attrs.Value = DateTime();
        attrs.Type = ObjectID::DateTime;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ReceiveTime, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::ReceiveTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void Severity()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Severity;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Severity);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Severity);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Severity);
        attrs.Value = uint16_t();
        attrs.Type = ObjectID::UInt16;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::Severity, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::Severity, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void SourceName()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SourceName;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SourceName);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SourceName);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SourceName);
        attrs.Value = std::string();
        attrs.Type = ObjectID::String;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::SourceName, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::SourceName, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void SourceNode()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SourceNode;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SourceNode);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SourceNode);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SourceNode);
        attrs.Value = NodeID();
        attrs.Type = ObjectID::NodeID;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::SourceNode, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::SourceNode, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void Time()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Time;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Time);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Time);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Time);
        attrs.Value = DateTime();
        attrs.Type = ObjectID::DateTime;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::Time, forward, ReferenceID::HasTypeDefinition, ObjectID::PropertyType, NodeClass::DataType);
        AddReference(ObjectID::Time, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ObjectTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ObjectTypes;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ObjectTypes);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::TypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ObjectTypes);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ObjectTypes);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ObjectTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, NodeClass::ObjectType);
      }

      void BaseObjectType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseObjectType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseObjectType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::ObjectTypes;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseObjectType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseObjectType);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DataTypeEncodingType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DataTypeEncodingType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::DataTypeEncodingType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::DataTypeEncodingType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::DataTypeEncodingType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void DataTypeSystemType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DataTypeSystemType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::DataTypeSystemType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::DataTypeSystemType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::DataTypeSystemType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void FolderType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::FolderType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::FolderType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::FolderType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::FolderType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HistoricalEventConfigurationType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::FolderType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HistoricalEventConfigurationType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HistoricalEventConfigurationType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HistoricalEventConfigurationType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ModellingRuleType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::FolderType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRuleType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRuleType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRuleType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void NamingRule()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::FolderType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::NamingRule);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::NamingRule);
        attrs.DisplayName = LocalizedText(OpcUa::Names::NamingRule);
        attrs.Type = ObjectID::Int32;
        attrs.Value = int32_t();
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerCapabilitiesType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerCapabilitiesType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerCapabilitiesType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerCapabilitiesType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void AggregateFunctions()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::AggregateFunctions;
        node.BrowseName = QualifiedName(0, OpcUa::Names::AggregateFunctions);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::AggregateFunctions);
        attrs.DisplayName = LocalizedText(OpcUa::Names::AggregateFunctions);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
        //References
        AddReference(ObjectID::AggregateFunctions, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ModelingRules()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ModellingRules;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRules);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRules);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRules);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
        //References
        AddReference(ObjectID::ModellingRules, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void LocaleIDArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::LocaleIDArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::LocaleIDArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::LocaleIDArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::LocaleIDArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::LocaleIDArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void MaxBrowseContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::MaxBrowseContinuationPoints;
        node.BrowseName = QualifiedName(0, OpcUa::Names::MaxBrowseContinuationPoints);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::MaxBrowseContinuationPoints);
        attrs.DisplayName = LocalizedText(OpcUa::Names::MaxBrowseContinuationPoints);
        attrs.Type = ObjectID::Double;
        attrs.Value = double();
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::MaxBrowseContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void MaxHistoryContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::MaxHistoryContinuationPoints;
        node.BrowseName = QualifiedName(0, OpcUa::Names::MaxHistoryContinuationPoints);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::MaxHistoryContinuationPoints);
        attrs.DisplayName = LocalizedText(OpcUa::Names::MaxHistoryContinuationPoints);
        attrs.Type = ObjectID::Double;
        attrs.Value = double();
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::MaxHistoryContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void MinSupportedSampleRate()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::MinSupportedSampleRate;
        node.BrowseName = QualifiedName(0, OpcUa::Names::MinSupportedSampleRate);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::MinSupportedSampleRate);
        attrs.DisplayName = LocalizedText(OpcUa::Names::MinSupportedSampleRate);
        attrs.Type = ObjectID::Double;
        attrs.Value = double();
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::MinSupportedSampleRate, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ServerProfileArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerProfileArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerProfileArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerProfileArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerProfileArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::vector<std::string>();
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerProfileArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void SoftwareCertificates()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SoftwareCertificates;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SoftwareCertificates);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SoftwareCertificates);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SoftwareCertificates);
        attrs.Type = ObjectID::SoftwareCertificate;
        attrs.Value = Variant();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::SoftwareCertificates, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ServerDiagnosticsType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseObjectType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerDiagnosticsType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerDiagnosticsType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void EnableFlag()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::EnableFlag;
        node.BrowseName = QualifiedName(0, OpcUa::Names::EnableFlag);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::EnableFlag);
        attrs.DisplayName = LocalizedText(OpcUa::Names::EnableFlag);
        attrs.Type = ObjectID::Boolean;
        attrs.Value = false;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::EnableFlag, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void SamplingIntervalDiagnosticsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::EnableFlag;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::SamplingIntervalDiagnosticsArrayType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray);
        attrs.Type = ObjectID::BaseDataType;
        attrs.Value = NodeID();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::SamplingIntervalDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ServerDiagnosticsSummary()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsSummary;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerDiagnosticsSummary);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::StructureServerDiagnosticsSummaryType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerDiagnosticsSummary);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerDiagnosticsSummary);
        attrs.Type = ObjectID::BaseDataType;
        attrs.Value = Variant();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void SessionsDiagnosticsSummary()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SessionsDiagnosticsSummary;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SessionDiagnosticsSummary);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::SessionDiagnosticsSummaryType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SessionDiagnosticsSummary);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SessionDiagnosticsSummary);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::SessionsDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void SubscriptionDiagnosticsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SubscriptionDiagnosticsArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SubscriptionDiagnosticsArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::SubscriptionDiagnosticsArrayType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SubscriptionDiagnosticsArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SubscriptionDiagnosticsArray);
        attrs.Type = ObjectID::BaseDataType;
        attrs.Value = Variant();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::SubscriptionDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void ServerRedundancyType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerRedundancyType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerRedundancyType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerRedundancyType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerRedundancyType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void RedundancySupportTypeRedundancySupport()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::RedundancySupportTypeRedundancySupport;
        node.BrowseName = QualifiedName(0, OpcUa::Names::RedundancySupport);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerRedundancyType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::RedundancySupport);
        attrs.DisplayName = LocalizedText(OpcUa::Names::RedundancySupport);
        attrs.Type = ObjectID::Int32;
        attrs.Value = int32_t();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType;
        node.BrowseName = QualifiedName(0, Names::ServerType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerType);
        attrs.DisplayName = LocalizedText(Names::ServerType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void Auditing()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Auditing;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Auditing);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Auditing);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Auditing);
        attrs.Type = ObjectID::Boolean;
        attrs.Value = false;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::Auditing, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory, NodeClass::DataType);
      }

      void NamespaceArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::NamespaceArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::NamespaceArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::NamespaceArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::NamespaceArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::vector<std::string>{"http://opcfoundation.org/UA/", "http://treww.github.com"};
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::vector<std::string>{"http://treww.github.com"}; // TODO
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }


      void ServerCapabilities()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilities;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerCapabilities);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerCapabilitiesType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerCapabilities);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerCapabilities);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        //AddReference(ObjectID::ServerCapabilities, forward, ObjectID::ModellingRuleMandatory);
      }
/*
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
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ReferenceTypes;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ReferenceTypes);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::TypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ReferenceTypes);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ReferenceTypes);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AddReference(ObjectID::ReferenceTypes, forward, ReferenceID::HasTypeDefinition, ObjectID::FolderType, NodeClass::ObjectType);
      }

      void Refs()
      {
        // Attributes
        // Type Attributes
        NewValue(ObjectID::References, AttributeID::IS_ABSTRACT, true);
        // Reference attributes
        NewValue(ObjectID::References, AttributeID::SYMMETRIC, true);


        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::References;
        node.BrowseName = QualifiedName(0, OpcUa::Names::References);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::ReferenceTypes;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::References);
        attrs.DisplayName = LocalizedText(OpcUa::Names::References);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

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
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::VariableTypes;
        node.BrowseName = QualifiedName(0, OpcUa::Names::VariableTypes);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::TypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::VariableTypes);
        attrs.DisplayName = LocalizedText(OpcUa::Names::VariableTypes);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

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
*/
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
