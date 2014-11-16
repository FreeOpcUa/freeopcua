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

    const bool forward = true;
    const bool reverse = true;


    class StandardNamespace
    {
    public:
      StandardNamespace(OpcUa::NodeManagementServices& registry, bool debug)
        : Registry(registry)
        , Debug(debug)
      {
      }

      void Fill()
      {
        if (Debug) std::clog << "Programmatically fillig address space" << std::endl;

         Objects();
         Views();
         Types();
      }

    private:
      void AddReference(ObjectID sourceNode, bool isForward, ReferenceID referenceType, ObjectID targetNode, NodeClass targetNodeClass)
      {
        AddReferencesItem ref;
        ref.IsForward = isForward;
        ref.ReferenceTypeId = referenceType;
        ref.SourceNodeID = sourceNode;
        ref.TargetNodeClass = targetNodeClass;
        ref.TargetNodeID = targetNode;

        Registry.AddReferences(std::vector<AddReferencesItem>{ref});
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

        Server();
      }

      void Server()
      {
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

        //Server_Auditing();
        NamespaceArray(ObjectID::Server_NamespaceArray, ObjectID::Server);
        ServerArray(ObjectID::Server_ServerArray, ObjectID::Server);
        ServerCapabilities();
        ServerDiagnostics();
        ServerRedundancy();
        ServerStatus();
        VendorServerInfo();
        ServiceLevel();
      }
      /*
      void Server_Auditing()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_Auditing;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Auditing);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server;
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
      }
      */
      void ServerCapabilities()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerCapabilities);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerCapabilitiesType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerCapabilities);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerCapabilities);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        LocaleIdArray();
        MaxBrowseContinuationPoints();
        MaxHistoryContinuationPoints();
        MaxQueryContinuationPoints();
        MinSupportedSampleRate();
        ModellingRules();
        ServerProfileArray();
        SoftwareCertificates();
      }

      void LocaleIdArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_LocaleIdArray;
        node.BrowseName = QualifiedName(0, Names::LocaleIDArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::LocaleIDArray);
        attrs.DisplayName = LocalizedText(Names::LocaleIDArray);
        attrs.AccessLevel = VariableAccessLevel::CurrentRead;
        attrs.UserAccessLevel = VariableAccessLevel::CurrentRead;
        attrs.Dimensions = {0};
        attrs.Historizing = false;
        attrs.MinimumSamplingInterval = Duration(100);
        attrs.Rank = 1;
        attrs.Value = std::vector<std::string>({ "en" });
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void MaxBrowseContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_MaxBrowseContinuationPoints;
        node.BrowseName = QualifiedName(0, Names::MaxBrowseContinuationPoints);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::MaxBrowseContinuationPoints);
        attrs.DisplayName = LocalizedText(Names::MaxBrowseContinuationPoints);
        attrs.AccessLevel = VariableAccessLevel::CurrentRead;
        attrs.UserAccessLevel = VariableAccessLevel::CurrentRead;
        attrs.Historizing = false;
        attrs.MinimumSamplingInterval = 0;
        attrs.Rank = -1;
        attrs.Value = uint16_t(0);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void MaxHistoryContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_MaxHistoryContinuationPoints;
        node.BrowseName = QualifiedName(0, Names::MaxHistoryContinuationPoints);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::MaxHistoryContinuationPoints);
        attrs.DisplayName = LocalizedText(Names::MaxHistoryContinuationPoints);
        attrs.AccessLevel = VariableAccessLevel::CurrentRead;
        attrs.UserAccessLevel = VariableAccessLevel::CurrentRead;
        attrs.Historizing = false;
        attrs.MinimumSamplingInterval = 0;
        attrs.Rank = -1;
        attrs.Value = uint16_t(0);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void MaxQueryContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_MaxQueryContinuationPoints;
        node.BrowseName = QualifiedName(0, Names::MaxQueryContinuationPoints);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::MaxQueryContinuationPoints);
        attrs.DisplayName = LocalizedText(Names::MaxQueryContinuationPoints);
        attrs.AccessLevel = VariableAccessLevel::CurrentRead;
        attrs.UserAccessLevel = VariableAccessLevel::CurrentRead;
        attrs.Historizing = false;
        attrs.MinimumSamplingInterval = 0;
        attrs.Rank = -1;
        attrs.Value = uint16_t(0);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void MinSupportedSampleRate()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_MinSupportedSampleRate;
        node.BrowseName = QualifiedName(0, OpcUa::Names::MinSupportedSampleRate);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
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
      }

      void ModellingRules()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_ModellingRules;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRules);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::FolderType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRules);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRules);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        ModellingRuleExposesItsArray();
        ModellingRule_Mandatory();
        ModellingRule_MandatoryShared();
        ModellingRuleOptional();
      }

      void ModellingRuleExposesItsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ModellingRule_ExposesItsArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRule_ExposesItsArray);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities_ModellingRules;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ModellingRuleType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRules);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRules);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        NamingRule(ObjectID::ModellingRule_ExposesItsArray_NamingRule, ObjectID::ModellingRule_ExposesItsArray, 6);
      }

      void ModellingRule_Mandatory()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ModellingRule_Mandatory;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRule_Mandatory);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities_ModellingRules;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ModellingRuleType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRule_Mandatory);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRule_Mandatory);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        NamingRule(ObjectID::ModellingRule_Mandatory_NamingRule, ObjectID::ModellingRule_Mandatory, 1);
      }

      void ModellingRule_MandatoryShared()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ModellingRule_MandatoryShared;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRule_MandatoryShared);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities_ModellingRules;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ModellingRuleType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRule_MandatoryShared);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRule_MandatoryShared);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        NamingRule(ObjectID::ModellingRule_MandatoryShared_NamingRule, ObjectID::ModellingRule_MandatoryShared, 1);
      }

      void ModellingRuleOptional()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ModellingRule_Optional;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ModellingRule_Optional);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities_ModellingRules;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ModellingRuleType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ModellingRule_Optional);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ModellingRule_Optional);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        NamingRule(ObjectID::ModellingRule_Optional_NamingRule, ObjectID::ModellingRule_Optional, 2);
      }
      
      void ServerProfileArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_ServerProfileArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerProfileArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerProfileArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerProfileArray);
        attrs.Type = ObjectID::Double;
        attrs.Value = std::vector<std::string>();
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      
      void SoftwareCertificates()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerCapabilities_SoftwareCertificates;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SoftwareCertificates);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerCapabilities;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SoftwareCertificates);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SoftwareCertificates);
        attrs.Type = NodeID(ObjectID::SoftwareCertificate);
        attrs.Value;
        attrs.Dimensions = {0};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerDiagnostics()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerDiagnostics;
        node.BrowseName = QualifiedName(0, Names::ServerDiagnostics);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerDiagnosticsType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerDiagnostics);
        attrs.DisplayName = LocalizedText(Names::ServerDiagnostics);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        EnableFlag();
        SamplingIntervalDiagnosticsArray();
        ServerDiagnosticsSummary();
        SessionsDiagnosticsSummary();
        SubscriptionDiagnosticsArray();
      }

      void EnableFlag()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerDiagnostics_EnabledFlag;
        node.BrowseName = QualifiedName(0, OpcUa::Names::EnableFlag);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerDiagnostics;
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
        AddReference(ObjectID::Server_ServerDiagnostics_EnabledFlag, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void SamplingIntervalDiagnosticsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerDiagnostics_SamplingRateDiagnosticsArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerDiagnostics;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::SamplingRateDiagnosticsArrayType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray);
        attrs.Type = ObjectID::Double;
        attrs.Value = std::vector<double>();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::Server_ServerDiagnostics_SamplingRateDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerDiagnosticsSummary()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerDiagnostics_ServerDiagnosticsSummary;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerDiagnosticsSummary);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerDiagnostics;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerDiagnosticsSummaryType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerDiagnosticsSummary);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerDiagnosticsSummary);
        attrs.Type = ObjectID::BaseDataType;
        attrs.Value = Variant();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SessionsDiagnosticsSummary()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerDiagnostics_SessionDiagnosticsSummary;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SessionDiagnosticsSummary);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server_ServerDiagnostics;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::SessionsDiagnosticsSummaryType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SessionDiagnosticsSummary);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SessionDiagnosticsSummary);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SubscriptionDiagnosticsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerDiagnostics_SubscriptionDiagnosticsArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SubscriptionDiagnosticsArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerDiagnostics;
        node.ReferenceTypeId = ReferenceID::HasComponent;
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
      }

      void ServerRedundancy()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerRedundancy;
        node.BrowseName = QualifiedName(0, Names::ServerRedundancy);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerRedundancyType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerRedundancy);
        attrs.DisplayName = LocalizedText(Names::ServerRedundancy);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        RedundancySupport();
      }

      void RedundancySupport()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Null;
        node.BrowseName = QualifiedName(0, OpcUa::Names::RedundancySupport);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerRedundancy;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::RedundancySupport);
        attrs.DisplayName = LocalizedText(OpcUa::Names::RedundancySupport);
        attrs.Type = ObjectID::RedundancySupport;
        attrs.Value;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerStatus()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus;
        node.BrowseName = QualifiedName(0, Names::ServerStatus);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerStatusType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerStatus);
        attrs.DisplayName = LocalizedText(Names::ServerStatus);
        attrs.Type = ObjectID::ServerStatusDataType;
        attrs.Value = NodeID(); // TODO
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BuildInfo();
        CurrentTime();
        //SecondsTillShutdown();
        //ShutdownReason();
        StartTime();
        State();
      }

      void BuildInfo()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo;
        node.BrowseName = QualifiedName(0, Names::BuildInfo);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BuildInfoVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildInfo);
        attrs.DisplayName = LocalizedText(Names::BuildInfo);
        attrs.Type = ObjectID::BuildInfo;
        attrs.Value = ObjectID::Null; // TODO
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BuildDate();
        BuildNumber();
        ManufacturerName();
        ProductName();
        ProductURI();
        SoftwareVersion();
      }
      void BuildDate()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo_BuildDate;
        node.BrowseName = QualifiedName(0, Names::BuildDate);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_BuildInfo;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildDate);
        attrs.DisplayName = LocalizedText(Names::BuildDate);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime(); // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildNumber()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo_BuildNumber;
        node.BrowseName = QualifiedName(0, Names::BuildNumber);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_BuildInfo;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildNumber);
        attrs.DisplayName = LocalizedText(Names::BuildNumber);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("0.1.3");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ManufacturerName()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo_ManufacturerName;
        node.BrowseName = QualifiedName(0, Names::ManufacturerName);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_BuildInfo;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ManufacturerName);
        attrs.DisplayName = LocalizedText(Names::ManufacturerName);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("freeopcua");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ProductName()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo_ProductName;
        node.BrowseName = QualifiedName(0, Names::ProductName);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_BuildInfo;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ProductName);
        attrs.DisplayName = LocalizedText(Names::ProductName);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("freeopcua");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ProductURI()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo_ProductUri;
        node.BrowseName = QualifiedName(0, Names::ProductURI);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_BuildInfo;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ProductURI);
        attrs.DisplayName = LocalizedText(Names::ProductURI);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("https://github.com/FreeOpcUa/freeopcua");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SoftwareVersion()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_BuildInfo_SoftwareVersion;
        node.BrowseName = QualifiedName(0, Names::SoftwareVersion);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_BuildInfo;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::SoftwareVersion);
        attrs.DisplayName = LocalizedText(Names::SoftwareVersion);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("0.1.3");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void CurrentTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_CurrentTime;
        node.BrowseName = QualifiedName(0, Names::CurrentTime);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::CurrentTime);
        attrs.DisplayName = LocalizedText(Names::CurrentTime);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      /*
      void SecondsTillShutdown()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_SecondsTillShutdown;
        node.BrowseName = QualifiedName(0, Names::SecondsTillShutdown);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus_ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::SecondsTillShutdown);
        attrs.DisplayName = LocalizedText(Names::SecondsTillShutdown);
        attrs.Type = ObjectID::Int32;
        attrs.Value;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      */
      /*
      void ShutdownReason()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ShutdownReason;
        node.BrowseName = QualifiedName(0, Names::ShutdownReason);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ShutdownReason);
        attrs.DisplayName = LocalizedText(Names::ShutdownReason);
        attrs.Type = ObjectID::String;
        attrs.Value;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      */
      void StartTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_StartTime;
        node.BrowseName = QualifiedName(0, Names::StartTime);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::StartTime);
        attrs.DisplayName = LocalizedText(Names::StartTime);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void State()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServerStatus_State;
        node.BrowseName = QualifiedName(0, Names::State);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server_ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::State);
        attrs.DisplayName = LocalizedText(Names::State);
        attrs.Type = ObjectID::ServerState;
        attrs.Value; // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServiceLevel()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_ServiceLevel;
        node.BrowseName = QualifiedName(0, Names::ServiceLevel);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ServiceLevel);
        attrs.DisplayName = LocalizedText(Names::ServiceLevel);
        attrs.Type = ObjectID::Byte;
        attrs.Value; // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void VendorServerInfo()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Server_VendorServerInfo;
        node.BrowseName = QualifiedName(0, OpcUa::Names::VendorServerInfo);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::Server;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::VendorServerInfoType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::VendorServerInfo);
        attrs.DisplayName = LocalizedText(OpcUa::Names::VendorServerInfo);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
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
      }

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

        DataTypes();
        EventTypesFolder();
        ObjectTypes();
        ReferenceTypes();
        VariableTypes();
      }

      void DataTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::DataTypesFolder;
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

        BaseDataType();
      }

      void BaseDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseDataType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::DataTypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseDataType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseDataType);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BooleanType();
        ByteStringType();
        DataValueType();
        DateTimeType();
        DiagnosticInfoType();
        EnumerationType();
        ExpandedNodeID();
        Guid();
        LocalizedTextType();
        NodeIDType();
        NumberType();
        QualifiedNameType();
        StatusCodeType();
        StringType();
        StructureType();
        XmlElement();

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

        ImageType();
      }

      void ImageType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Image;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Image);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::ByteString;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Image);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Image);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        ImageBmpType();
        ImageGifType();
        ImageJpgType();
        ImagePngType();
      }

      void ImageBmpType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ImageBMP;
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
        node.RequestedNewNodeID = ObjectID::ImageGIF;
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
        node.RequestedNewNodeID = ObjectID::ImageJPG;
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
        node.RequestedNewNodeID = ObjectID::ImagePNG;
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

        UtcTimeType();
      }

      void UtcTimeType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UtcTime;
        node.BrowseName = QualifiedName(0, Names::UtcTime);
        node.Class = NodeClass::DataType;
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

        IdType();
        MessageSecurityModeType();
        NodeClassType();
        RedundancySupportType();
        SecurityTokenRequestType();
        ServerStateType();
      }

      void EnumStrings(ObjectID nodeID, ObjectID parentNode, const std::vector<LocalizedText>& values)
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = nodeID;
        node.BrowseName = QualifiedName(0, Names::EnumStrings);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = parentNode;
        node.ReferenceTypeId = ReferenceID::HasProperty;
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

        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Numeric"));
        values.push_back(LocalizedText("String"));
        values.push_back(LocalizedText("Guid"));
        values.push_back(LocalizedText("Opaque"));
        EnumStrings(ObjectID::Null, ObjectID::IdType, values);
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

        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Invalid"));
        values.push_back(LocalizedText("None"));
        values.push_back(LocalizedText("Sign"));
        values.push_back(LocalizedText("SignAndEncrypt"));
        EnumStrings(ObjectID::Null, ObjectID::MessageSecurityMode, values);
      }

      void NodeClassType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::NodeClass;
        node.BrowseName = QualifiedName(0, Names::NodeClass);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Enumeration;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::NodeClass);
        attrs.DisplayName = LocalizedText(Names::NodeClass);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
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

        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("None"));
        values.push_back(LocalizedText("Cold"));
        values.push_back(LocalizedText("Warm"));
        values.push_back(LocalizedText("Hot"));
        values.push_back(LocalizedText("Transparent"));
        EnumStrings(ObjectID::Null, ObjectID::RedundancySupport, values);
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

        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Issue"));
        values.push_back(LocalizedText("Renew"));
        EnumStrings(ObjectID::Null, ObjectID::SecurityTokenRequestType, values);
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

        std::vector<OpcUa::LocalizedText> values;
        values.push_back(LocalizedText("Running"));
        values.push_back(LocalizedText("Failed"));
        values.push_back(LocalizedText("NoConfiguration"));
        values.push_back(LocalizedText("Suspended"));
        values.push_back(LocalizedText("Shutdown"));
        values.push_back(LocalizedText("Test"));
        values.push_back(LocalizedText("CommunicationFault"));
        values.push_back(LocalizedText("Unknown"));
        EnumStrings(ObjectID::Null, ObjectID::ServerState, values);
      }

      void ExpandedNodeID()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ExpandedNodeId;
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
        node.RequestedNewNodeID = ObjectID::NodeId;
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

        DoubleType();
        FloatType();
        IntegerType();
        UIntegerType();
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

        DurationType();
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

        Int16Type();
        Int32Type();
        Int64Type();
        SByteType();
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
        node.RequestedNewNodeID = ObjectID::UnsignedInteger;
        node.BrowseName = QualifiedName(0, Names::UInteger);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Number;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UInteger);
        attrs.DisplayName = LocalizedText(Names::UInteger);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        ByteType();
        UInt16Type();
        UInt32Type();
        UInt64Type();
      }

      void UInt16Type()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::UInt16;
        node.BrowseName = QualifiedName(0, Names::UInt16);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UnsignedInteger;
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
        node.ParentNodeId = ObjectID::UnsignedInteger;
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
        node.ParentNodeId = ObjectID::UnsignedInteger;
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
        node.ParentNodeId = ObjectID::UnsignedInteger;
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

        LocaleIDType();
        NumericRangeType();
      }

      void LocaleIDType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::LocaleId;
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
      }

      void StructureAddNodesItem()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::AddNodesItem;
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
        node.RequestedNewNodeID = ObjectID::AddReferencesItem;
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
        node.RequestedNewNodeID = ObjectID::ApplicationDescription;
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
        node.RequestedNewNodeID = ObjectID::Argument;
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
        node.RequestedNewNodeID = ObjectID::BuildInfo;
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
        node.RequestedNewNodeID = ObjectID::DeleteNodesItem;
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
        node.RequestedNewNodeID = ObjectID::DeleteReferencesItem;
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
        node.RequestedNewNodeID = ObjectID::EUInformation;
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
        node.RequestedNewNodeID = ObjectID::ModelChangeStructureDataType;
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
        node.RequestedNewNodeID = ObjectID::Range;
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
        node.RequestedNewNodeID = ObjectID::SamplingRateDiagnosticsDataType;
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
        node.RequestedNewNodeID = ObjectID::SemanticChangeStructureDataType;
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
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsSummaryDataType;
        node.BrowseName = QualifiedName(0, Names::ServerDiagnosticsDataType);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerDiagnosticsDataType);
        attrs.DisplayName = LocalizedText(Names::ServerDiagnosticsDataType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StructureServerStatusDataType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerStatusDataType;
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
        node.RequestedNewNodeID = ObjectID::ServiceCounterDataType;
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
        node.RequestedNewNodeID = ObjectID::SessionDiagnosticsDataType;
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
        node.RequestedNewNodeID = ObjectID::SessionSecurityDiagnosticsDataType;
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
        node.RequestedNewNodeID = ObjectID::SignedSoftwareCertificate;
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
        node.RequestedNewNodeID = ObjectID::StatusResult;
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
        node.RequestedNewNodeID = ObjectID::SubscriptionDiagnosticsDataType;
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
        node.RequestedNewNodeID = ObjectID::UserIdentityToken;
        node.BrowseName = QualifiedName(0, Names::UserIdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::Structure;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::UserIdentifyToken);
        attrs.DisplayName = LocalizedText(Names::UserIdentifyToken);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AnonymousIdentifyToken();
        UserNameIdentifyToken();
        X509IdentifyToken();
      }

      void AnonymousIdentifyToken()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::AnonymousIdentityToken;
        node.BrowseName = QualifiedName(0, Names::AnonymousIdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UserIdentityToken;
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
        node.RequestedNewNodeID = ObjectID::UserNameIdentityToken;
        node.BrowseName = QualifiedName(0, Names::UserNameIdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UserIdentityToken;
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
        node.RequestedNewNodeID = ObjectID::X509IdentityToken;
        node.BrowseName = QualifiedName(0, Names::X509IdentifyToken);
        node.Class = NodeClass::DataType;
        node.ParentNodeId = ObjectID::UserIdentityToken;
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
        node.ParentNodeId = ObjectID::BaseDataType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::XmlElement);
        attrs.DisplayName = LocalizedText(Names::XmlElement);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void EventTypesFolder()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::EventTypesFolder;
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

        BaseEventType();
      }

      void BaseEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::EventTypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        DataTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseEventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseEventType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BaseModelChangeEventType();
        SemanticChangeEventType();
        SystemEventType();
        EventID();
        EventType();
        //LocalTime();
        Message();
        ReceiveTime();
        Severity();
        SourceName();
        SourceNode();
        Time();
      }

      void BaseModelChangeEventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseModelChangeEventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseModelChangeEventType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
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

        DeviceFailureEventType();
        RefreshEndEventType();
        RefreshRequiredEventType();
        RefreshStartEventType();
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
        node.RequestedNewNodeID = ObjectID::BaseEventType_EventId;
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
        AddReference(ObjectID::BaseEventType_EventId, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void EventType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_EventType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::EventType);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::EventType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::EventType);
        attrs.Value = NodeID();
        attrs.Type = ObjectID::NodeId;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::BaseEventType_EventType, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }
      /*
      void LocalTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_Time;
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
        AddReference(ObjectID::BaseEventType_Time, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }
      */
      void Message()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_Message;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Message);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Message);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Message);
        attrs.Value = LocalizedText();
        attrs.Type = ObjectID::LocalizedText;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::BaseEventType_Message, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ReceiveTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_ReceiveTime;
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
        AddReference(ObjectID::BaseEventType_ReceiveTime, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void Severity()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_Severity;
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
        AddReference(ObjectID::BaseEventType_Severity, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void SourceName()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_SourceName;
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
        AddReference(ObjectID::BaseEventType_SourceName, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void SourceNode()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_SourceNode;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SourceNode);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BaseEventType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SourceNode);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SourceNode);
        attrs.Value = NodeID();
        attrs.Type = ObjectID::NodeId;
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::BaseEventType_SourceNode, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void Time()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseEventType_Time;
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
        AddReference(ObjectID::BaseEventType_Time, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ObjectTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ObjectTypesFolder;
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

        BaseObjectType();
      }

      void BaseObjectType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseObjectType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseObjectType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::ObjectTypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseObjectType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseObjectType);
        attrs.IsAbstract = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AddReference(ObjectID::BaseObjectType, forward, ReferenceID::HasSubtype, ObjectID::BaseEventType, NodeClass::ObjectType);

        DataTypeEncodingType();
        DataTypeSystemType();
        FolderType();
        HistoricalEventConfigurationType();
        ModellingRuleType();
        ServerCapabilitiesType();
        ServerDiagnosticsType();
        ServerRedundancyType();
        ServerType();
        SessionDiagnosticsObjectType();
        SessionDiagnosticsSummaryType();
        StateType();
        TransitionType();
        VendorServerInfoType();
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
        node.RequestedNewNodeID = ObjectID::HistoricalEventConfigurationType;
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
        node.RequestedNewNodeID = ObjectID::ModellingRuleType;
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

        NamingRule(ObjectID::ModellingRuleType_NamingRule, ObjectID::ModellingRuleType, 0);
      }

      void NamingRule(ObjectID id, ObjectID parent, uint32_t value)
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = id;
        node.BrowseName = QualifiedName(0, OpcUa::Names::NamingRule);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = parent;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::NamingRule);
        attrs.DisplayName = LocalizedText(OpcUa::Names::NamingRule);
        attrs.Type = ObjectID::Int32;
        attrs.Value = value;
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

        //ServerTypeAggregateFunctions();
        ServerTypeModelingRules();
        ServerTypeLocaleIDArray();
        ServerTypeMaxBrowseContinuationPoints();
        ServerTypeMaxHistoryContinuationPoints();
        ServerTypeMinSupportedSampleRate();
        ServerTypeServerProfileArray();
        //ServerTypeSoftwareCertificates();
      }
      /*
      void ServerTypeAggregateFunctions()
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
        AddReference(ObjectID::AggregateFunctions, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }
      */
      void ServerTypeModelingRules()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType_ModellingRules;
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
        AddReference(ObjectID::ServerCapabilitiesType_ModellingRules, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeLocaleIDArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType_LocaleIdArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::LocaleIDArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerCapabilitiesType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::LocaleIDArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::LocaleIDArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::vector<std::string>();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerCapabilitiesType_LocaleIdArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeMaxBrowseContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType_MaxBrowseContinuationPoints;
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
        AddReference(ObjectID::ServerCapabilitiesType_MaxBrowseContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeMaxHistoryContinuationPoints()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType_MaxHistoryContinuationPoints;
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
        AddReference(ObjectID::ServerCapabilitiesType_MaxHistoryContinuationPoints, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeMinSupportedSampleRate()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType_MinSupportedSampleRate;
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
        AddReference(ObjectID::ServerCapabilitiesType_MinSupportedSampleRate, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeServerProfileArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerCapabilitiesType_ServerProfileArray;
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
        AddReference(ObjectID::ServerCapabilitiesType_ServerProfileArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }
      /*
      void ServerTypeSoftwareCertificates()
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
        AddReference(ObjectID::SoftwareCertificates, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }
      */
      void ServerDiagnosticsType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerDiagnosticsType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerDiagnosticsType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerDiagnosticsType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        ServerTypeEnableFlag();
        ServerTypeSamplingIntervalDiagnosticsArray();
        ServerTypeServerDiagnosticsSummary();
        ServerTypeSessionsDiagnosticsSummary();
        ServerTypeSubscriptionDiagnosticsArray();
      }

      void ServerTypeEnableFlag()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType_EnabledFlag;
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
        AddReference(ObjectID::ServerDiagnosticsType_EnabledFlag, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeSamplingIntervalDiagnosticsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType_SamplingRateDiagnosticsArray;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SamplingIntervalDiagnosticsArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::SamplingRateDiagnosticsArrayType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SamplingIntervalDiagnosticsArray);
        attrs.Type = ObjectID::Double;
        attrs.Value = std::vector<double>();
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerDiagnosticsType_SamplingRateDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeServerDiagnosticsSummary()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerDiagnosticsSummary);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::ServerDiagnosticsSummaryType;
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
        AddReference(ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeSessionsDiagnosticsSummary()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType_SessionDiagnosticsSummary;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SessionDiagnosticsSummary);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerDiagnosticsType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::SessionsDiagnosticsSummaryType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SessionDiagnosticsSummary);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SessionDiagnosticsSummary);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerDiagnosticsType_SessionDiagnosticsSummary, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeSubscriptionDiagnosticsArray()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerDiagnosticsType_SubscriptionDiagnosticsArray;
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
        AddReference(ObjectID::ServerDiagnosticsType_SubscriptionDiagnosticsArray, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
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

        RedundancySupportTypeRedundancySupport();
      }

      void RedundancySupportTypeRedundancySupport()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerRedundancyType_RedundancySupport;
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
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerType);
        attrs.DisplayName = LocalizedText(Names::ServerType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        //ServerType_Auditing();
        NamespaceArray(ObjectID::ServerType_NamespaceArray, ObjectID::ServerType);
        ServerArray(ObjectID::ServerType_ServerArray, ObjectID::ServerType);
        ServerTypeCapabilities(ObjectID::ServerType);
        ServerTypeDiagnostics();
        ServerTypeServerRedundancy();
        ServerTypeServerStatus();
        ServerTypeServiceLevel();
        ServerTypeVendorServerInfo();
      }
      /*
      void ServerType_Auditing()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_Auditing;
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
        AddReference(ObjectID::Auditing, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }
      */
      void NamespaceArray(ObjectID id, ObjectID parent)
      {
        if (Debug) std::cout << "Creating namepsace array" << std::endl;
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = id;
        node.BrowseName = QualifiedName(0, OpcUa::Names::NamespaceArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = parent;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::NamespaceArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::NamespaceArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::vector<std::string>{"http://opcfoundation.org/UA/", "http://freeopcua.github.io"};
        attrs.Dimensions = {};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerArray(ObjectID id, ObjectID parent)
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = id;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerArray);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = parent;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerArray);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerArray);
        attrs.Type = ObjectID::String;
        attrs.Value = std::vector<std::string>{"http://treww.github.com"}; // TODO
        attrs.Dimensions = {};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }


      void ServerTypeCapabilities(ObjectID parent)
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_ServerCapabilities;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerCapabilities);
        node.Class = NodeClass::Object;
        node.ParentNodeId = parent;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerCapabilitiesType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerCapabilities);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerCapabilities);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerType_ServerCapabilities, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeDiagnostics()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_ServerDiagnostics;
        node.BrowseName = QualifiedName(0, Names::ServerDiagnostics);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerDiagnosticsType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerDiagnostics);
        attrs.DisplayName = LocalizedText(Names::ServerDiagnostics);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerType_ServerDiagnostics, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeServerRedundancy()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_ServerRedundancy;
        node.BrowseName = QualifiedName(0, Names::ServerRedundancy);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::ServerRedundancyType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerRedundancy);
        attrs.DisplayName = LocalizedText(Names::ServerRedundancy);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerType_ServerRedundancy, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeServerStatus()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_ServerStatus;
        node.BrowseName = QualifiedName(0, Names::ServerStatus);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::ServerStatusType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ServerStatus);
        attrs.DisplayName = LocalizedText(Names::ServerStatus);
        attrs.Type = ObjectID::ServerStatusDataType;
        attrs.Value = NodeID(); // TODO
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        AddReference(ObjectID::ServerType_ServerStatus, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeBuildInfo()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerStatusType_BuildInfo;
        node.BrowseName = QualifiedName(0, Names::BuildInfo);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatusType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BuildInfoVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildInfo);
        attrs.DisplayName = LocalizedText(Names::BuildInfo);
        attrs.Type = ObjectID::BuildInfo;
        attrs.Value = ObjectID::Null; // TODO
        attrs.Dimensions = {0};
        attrs.Rank = 1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildInfoVariableType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType;
        node.BrowseName = QualifiedName(0, Names::BuildInfoVariableType);
        node.Class = NodeClass::VariableType;
        node.ParentNodeId = ObjectID::BaseDataVariableType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        VariableTypeAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildInfoVariableType);
        attrs.DisplayName = LocalizedText(Names::BuildInfoVariableType);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime(); // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BuildInfoVariableTypeBuildDate();
        BuildInfoVariableTypeBuildNumber();
        BuildInfoVariableTypeManufacturerName();
        BuildInfoVariableTypeProductName();
        BuildInfoVariableTypeProductURI();
        BuildInfoVariableTypeSoftwareVersion();
      }

      void BuildInfoVariableTypeBuildDate()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType_BuildDate;
        node.BrowseName = QualifiedName(0, Names::BuildDate);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BuildInfoVariableType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildDate);
        attrs.DisplayName = LocalizedText(Names::BuildDate);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime(); // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildInfoVariableTypeBuildNumber()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType_BuildNumber;
        node.BrowseName = QualifiedName(0, Names::BuildNumber);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BuildInfoVariableType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::BuildNumber);
        attrs.DisplayName = LocalizedText(Names::BuildNumber);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("0.1.3");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildInfoVariableTypeManufacturerName()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType_ManufacturerName;
        node.BrowseName = QualifiedName(0, Names::ManufacturerName);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BuildInfoVariableType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ManufacturerName);
        attrs.DisplayName = LocalizedText(Names::ManufacturerName);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("freeopcua");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildInfoVariableTypeProductName()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType_ProductName;
        node.BrowseName = QualifiedName(0, Names::ProductName);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BuildInfoVariableType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ProductName);
        attrs.DisplayName = LocalizedText(Names::ProductName);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("freeopcua");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildInfoVariableTypeProductURI()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType_ProductUri;
        node.BrowseName = QualifiedName(0, Names::ProductURI);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BuildInfoVariableType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ProductURI);
        attrs.DisplayName = LocalizedText(Names::ProductURI);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("https://github.com/FreeOpcUa/freeopcua");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void BuildInfoVariableTypeSoftwareVersion()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BuildInfoVariableType_SoftwareVersion;
        node.BrowseName = QualifiedName(0, Names::SoftwareVersion);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::BuildInfoVariableType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::SoftwareVersion);
        attrs.DisplayName = LocalizedText(Names::SoftwareVersion);
        attrs.Type = ObjectID::String;
        attrs.Value = std::string("0.1.3");
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerTypeCurrentTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerStatusType_CurrentTime;
        node.BrowseName = QualifiedName(0, Names::CurrentTime);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatusType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::CurrentTime);
        attrs.DisplayName = LocalizedText(Names::CurrentTime);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      /*
      void ServerTypeSecondsTillShutdown()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SecondsTillShutdown;
        node.BrowseName = QualifiedName(0, Names::SecondsTillShutdown);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::SecondsTillShutdown);
        attrs.DisplayName = LocalizedText(Names::SecondsTillShutdown);
        attrs.Type = ObjectID::Int32;
        attrs.Value;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      */
      /*
      void ServerTypeShutdownReason()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ShutdownReason;
        node.BrowseName = QualifiedName(0, Names::ShutdownReason);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatus;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ShutdownReason);
        attrs.DisplayName = LocalizedText(Names::ShutdownReason);
        attrs.Type = ObjectID::String;
        attrs.Value;
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      */
      void ServerTypeStartTime()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerStatusType_StartTime;
        node.BrowseName = QualifiedName(0, Names::StartTime);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatusType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::StartTime);
        attrs.DisplayName = LocalizedText(Names::StartTime);
        attrs.Type = ObjectID::DateTime;
        attrs.Value = OpcUa::CurrentDateTime();
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerTypeState()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerStatusType_State;
        node.BrowseName = QualifiedName(0, Names::State);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerStatusType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::BaseDataVariableType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::State);
        attrs.DisplayName = LocalizedText(Names::State);
        attrs.Type = ObjectID::ServerState;
        attrs.Value; // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ServerTypeServiceLevel()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_ServiceLevel;
        node.BrowseName = QualifiedName(0, Names::ServiceLevel);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::ServiceLevel);
        attrs.DisplayName = LocalizedText(Names::ServiceLevel);
        attrs.Type = ObjectID::Byte;
        attrs.Value; // TODO
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerType_ServiceLevel, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void ServerTypeVendorServerInfo()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerType_VendorServerInfo;
        node.BrowseName = QualifiedName(0, OpcUa::Names::VendorServerInfo);
        node.Class = NodeClass::Object;
        node.ParentNodeId = ObjectID::ServerType;
        node.ReferenceTypeId = ReferenceID::HasComponent;
        node.TypeDefinition = ObjectID::VendorServerInfoType;
        ObjectAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::VendorServerInfo);
        attrs.DisplayName = LocalizedText(OpcUa::Names::VendorServerInfo);
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        // References
        AddReference(ObjectID::ServerType_VendorServerInfo, forward, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory, NodeClass::DataType);
      }

      void SessionDiagnosticsObjectType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SessionDiagnosticsObjectType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SessionDiagnosticsObjectType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SessionDiagnosticsObjectType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SessionDiagnosticsObjectType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void SessionDiagnosticsSummaryType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::SessionsDiagnosticsSummaryType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::SessionDiagnosticsSummaryType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::SessionDiagnosticsSummaryType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::SessionDiagnosticsSummaryType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void StateType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StateType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::StateType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::StateType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::StateType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        StateNumber();
      }

      void StateNumber()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::StateType_StateNumber;
        node.BrowseName = QualifiedName(0, Names::StateNumber);
        node.Class = NodeClass::Variable;
        node.ParentNodeId = ObjectID::StateType;
        node.ReferenceTypeId = ReferenceID::HasProperty;
        node.TypeDefinition = ObjectID::PropertyType;
        VariableAttributes attrs;
        attrs.Description = LocalizedText(Names::StateNumber);
        attrs.DisplayName = LocalizedText(Names::StateNumber);
        attrs.Type = ObjectID::UInt32;
        attrs.Value = uint32_t(0);
        attrs.Dimensions = {};
        attrs.Rank = -1;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void TransitionType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::TransitionType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::TransitionType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::TransitionType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::TransitionType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void VendorServerInfoType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::VendorServerInfoType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::VendorServerInfoType);
        node.Class = NodeClass::ObjectType;
        node.ParentNodeId = ObjectID::BaseObjectType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        ObjectTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::VendorServerInfoType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::VendorServerInfoType);
        attrs.IsAbstract = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ReferenceTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ReferenceTypesFolder;
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

        Refs();
      }

      void Refs()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::References;
        node.BrowseName = QualifiedName(0, OpcUa::Names::References);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::ReferenceTypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::References);
        attrs.DisplayName = LocalizedText(OpcUa::Names::References);
        attrs.IsAbstract = true;
        attrs.Symmetric = true;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        HierarchicalReferences();
        NonHierarchicalReferences();
      }

      void HierarchicalReferences()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HierarchicalReferences;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HierarchicalReferences);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::References;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HierarchicalReferences);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HierarchicalReferences);
        attrs.IsAbstract = true;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        HasChild();
        HasEventSource();
        Organizes();
      }

      void HasChild()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasChild;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasChild);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasChild);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasChild);
        attrs.IsAbstract = true;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        Aggregates();
        HasSubtype();
      }

      void Aggregates()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Aggregates;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Aggregates);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HasChild;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Aggregates);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Aggregates);
        attrs.IsAbstract = true;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        HasComponent();
        HasHistoricalConfiguration();
        HasProperty();
      }

      void HasComponent()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasComponent;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasComponent);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::Aggregates;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasComponent);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasComponent);
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        HasOrderedComponent();
      }

      void HasOrderedComponent()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasOrderedComponent;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasOrderedComponent);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HasComponent;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasOrderedComponent);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasOrderedComponent);
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasHistoricalConfiguration()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasHistoricalConfiguration;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasHistoricalConfiguration);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::Aggregates;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasHistoricalConfiguration);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasHistoricalConfiguration);
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasProperty()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasProperty;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasProperty);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::Aggregates;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasProperty);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasProperty);
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasSubtype()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasSubtype;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasSubtype);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HasChild;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasSubtype);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasSubtype);
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasEventSource()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasEventSource;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasEventSource);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasEventSource);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasEventSource);
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        HasNotifier();
      }

      void HasNotifier()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasNotifier;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasNotifier);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HasEventSource;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasNotifier);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasNotifier);
        attrs.InverseName = LocalizedText("NotifierOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void Organizes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::Organizes;
        node.BrowseName = QualifiedName(0, OpcUa::Names::Organizes);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::HierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::Organizes);
        attrs.DisplayName = LocalizedText(OpcUa::Names::Organizes);
        attrs.InverseName = LocalizedText("OrganizedBy");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void NonHierarchicalReferences()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::NonHierarchicalReferences;
        node.BrowseName = QualifiedName(0, OpcUa::Names::NonHierarchicalReferences);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::References;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::NonHierarchicalReferences);
        attrs.DisplayName = LocalizedText(OpcUa::Names::NonHierarchicalReferences);
        attrs.IsAbstract = true;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        FromState();
        GeneratesEvent();
        HasCause();
        //HasCondition();
        HasDescription();
        HasEffect();
        HasEncoding();
        HasModelParent();
        HasModellingRule();
        HasTypeDefinition();
        ToState();
      }

      void FromState()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::FromState;
        node.BrowseName = QualifiedName(0, OpcUa::Names::FromState);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::FromState);
        attrs.DisplayName = LocalizedText(OpcUa::Names::FromState);
        attrs.InverseName = LocalizedText("ToTransition");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void GeneratesEvent()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::GeneratesEvent;
        node.BrowseName = QualifiedName(0, OpcUa::Names::GeneratesEvent);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::GeneratesEvent);
        attrs.DisplayName = LocalizedText(OpcUa::Names::GeneratesEvent);
        attrs.InverseName = LocalizedText("GeneratesBy");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasCause()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasCause;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasCause);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasCause);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasCause);
        attrs.InverseName = LocalizedText("MayBeCausedBy");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      /*
      void HasCondition()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasCondition;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasCondition);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasCondition);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasCondition);
        attrs.InverseName = LocalizedText("IsConditionOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }
      */
      void HasDescription()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasDescription;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasDescription);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasDescription);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasDescription);
        attrs.InverseName = LocalizedText("DescriptionOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasEffect()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasEffect;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasEffect);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasEffect);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasEffect);
        attrs.InverseName = LocalizedText("MayBeEffectedBy");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasEncoding()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasEncoding;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasEncoding);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasEncoding);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasEncoding);
        attrs.InverseName = LocalizedText("EncodingOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasModelParent()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasModelParent;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasModelParent);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasModelParent);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasModelParent);
        attrs.InverseName = LocalizedText("ModelParentOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasModellingRule()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasModellingRule;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasModellingRule);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasModellingRule);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasModellingRule);
        attrs.InverseName = LocalizedText("ModellingRuleOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void HasTypeDefinition()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::HasTypeDefinition;
        node.BrowseName = QualifiedName(0, OpcUa::Names::HasTypeDefinition);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::HasTypeDefinition);
        attrs.DisplayName = LocalizedText(OpcUa::Names::HasTypeDefinition);
        attrs.InverseName = LocalizedText("TypeDefinitionOf");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void ToState()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ToState;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ToState);
        node.Class = NodeClass::ReferenceType;
        node.ParentNodeId = ObjectID::NonHierarchicalReferences;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        ReferenceTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ToState);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ToState);
        attrs.InverseName = LocalizedText("FromTransition");
        attrs.IsAbstract = false;
        attrs.Symmetric = false;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }

      void VariableTypes()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::VariableTypesFolder;
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

        BaseVariableType();
      }

      void BaseVariableType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseVariableType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseVariableType);
        node.Class = NodeClass::VariableType;
        node.ParentNodeId = ObjectID::VariableTypesFolder;
        node.ReferenceTypeId = ReferenceID::Organizes;
        node.TypeDefinition = ObjectID::Null;
        VariableTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseVariableType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseVariableType);
        attrs.IsAbstract = false;
        attrs.Type = ObjectID::BaseDataType;
        attrs.Rank = 0;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BaseDataVariableType();
        PropertyType();
      }

      void BaseDataVariableType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::BaseDataVariableType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::BaseDataVariableType);
        node.Class = NodeClass::VariableType;
        node.ParentNodeId = ObjectID::BaseVariableType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        VariableTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::BaseDataVariableType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::BaseDataVariableType);
        attrs.IsAbstract = false;
        attrs.Type = ObjectID::BaseDataType;
        attrs.Rank = 0;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        BuildInfoVariableType();
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
      }

      void AddVariableType(ObjectID id, const char* name, ObjectID parentId)
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = id;
        node.BrowseName = QualifiedName(0, name);
        node.Class = NodeClass::VariableType;
        node.ParentNodeId = parentId;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        VariableTypeAttributes attrs;
        attrs.Description = LocalizedText(name);
        attrs.DisplayName = LocalizedText(name);
        attrs.IsAbstract = false;
        attrs.Type = ObjectID::BaseDataType;
        attrs.Rank = 0;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});
      }


      void DataTypeDescriptionType()
      {
        AddVariableType(ObjectID::DataTypeDescriptionType, OpcUa::Names::DataTypeDescriptionType, ObjectID::BaseDataVariableType);
      }

      void DataTypeDictionaryType()
      {
        AddVariableType(ObjectID::DataTypeDictionaryType, OpcUa::Names::DataTypeDictionaryType, ObjectID::BaseDataVariableType);
      }

      void SamplingIntervalDiagnosticsArrayType()
      {
        AddVariableType(ObjectID::SamplingRateDiagnosticsArrayType, OpcUa::Names::SamplingIntervalDiagnosticsArrayType, ObjectID::BaseDataVariableType);
      }

      void SamplingIntervalDiagnosticsType()
      {
        AddVariableType(ObjectID::SamplingRateDiagnosticsType, OpcUa::Names::SamplingIntervalDiagnosticsType, ObjectID::BaseDataVariableType);
      }

      void ServerDiagnosticsSummaryType()
      {
        AddVariableType(ObjectID::ServerDiagnosticsSummaryType, OpcUa::Names::ServerDiagnosticsSummaryType, ObjectID::BaseDataVariableType);
      }

      void ServerStatusType()
      {
        // Attributes
        AddNodesItem node;
        node.RequestedNewNodeID = ObjectID::ServerStatusType;
        node.BrowseName = QualifiedName(0, OpcUa::Names::ServerStatusType);
        node.Class = NodeClass::VariableType;
        node.ParentNodeId = ObjectID::BaseDataVariableType;
        node.ReferenceTypeId = ReferenceID::HasSubtype;
        node.TypeDefinition = ObjectID::Null;
        VariableTypeAttributes attrs;
        attrs.Description = LocalizedText(OpcUa::Names::ServerStatusType);
        attrs.DisplayName = LocalizedText(OpcUa::Names::ServerStatusType);
        attrs.IsAbstract = false;
        attrs.Type = ObjectID::ServerStatusDataType;
        attrs.Rank = 0;
        node.Attributes = attrs;
        Registry.AddNodes(std::vector<AddNodesItem>{node});

        ServerTypeBuildInfo();
        ServerTypeCurrentTime();
        ServerTypeStartTime();
        ServerTypeState();
      }

      void ServerVendorCapabilityType()
      {
        AddVariableType(ObjectID::ServerVendorCapabilityType, OpcUa::Names::ServerVendorCapabilityType, ObjectID::BaseDataVariableType);
      }


      void SessionsDiagnosticsArrayType()
      {
        AddVariableType(ObjectID::SessionDiagnosticsArrayType, OpcUa::Names::SessionsDiagnosticsArrayType, ObjectID::BaseDataVariableType);
      }

      void ServerDiagnosticsVariableType()
      {
        AddVariableType(ObjectID::SessionDiagnosticsVariableType, OpcUa::Names::ServerDiagnosticsVariableType, ObjectID::BaseDataVariableType);
      }

      void SessionSecurityDiagnosticsArrayType()
      {
        AddVariableType(ObjectID::SessionSecurityDiagnosticsArrayType, OpcUa::Names::SessionSecurityDiagnosticsArrayType, ObjectID::BaseDataVariableType);
      }

      void SessionSecurityDiagnosticsType()
      {
        AddVariableType(ObjectID::SessionSecurityDiagnosticsType, OpcUa::Names::SessionSecurityDiagnosticsType, ObjectID::BaseDataVariableType);
      }

      void SubscriptionDiagnosticsArrayType()
      {
        AddVariableType(ObjectID::SubscriptionDiagnosticsArrayType, OpcUa::Names::SubscriptionDiagnosticsArrayType, ObjectID::BaseDataVariableType);
      }

      void SubscriptionDiagnosticsType()
      {
        AddVariableType(ObjectID::SubscriptionDiagnosticsType, OpcUa::Names::SubscriptionDiagnosticsType, ObjectID::BaseDataVariableType);
      }

      void PropertyType()
      {
        AddVariableType(ObjectID::PropertyType, OpcUa::Names::PropertyType, ObjectID::BaseVariableType);
      }

    private:
      OpcUa::NodeManagementServices& Registry;
      const bool Debug;
    };

} // namespace

namespace OpcUa
{
  namespace Server
  {

    void FillStandardNamespace(OpcUa::NodeManagementServices& registry, bool debug)
    {
      StandardNamespace ns(registry, debug);
      ns.Fill();
    }

  } // namespace UaServer
} // namespace OpcUa
