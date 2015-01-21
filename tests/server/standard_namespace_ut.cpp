/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/status_codes.h>

#include <opc/ua/server/address_space.h>
#include <opc/ua/server/standard_address_space.h>

#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace OpcUa;


static Server::AddressSpace::SharedPtr NameSpace;


class StandardNamespaceStructure : public Test
{
protected:
  virtual void SetUp()
  {
    const bool debug = false;
    if (!NameSpace)
    {
      NameSpace = OpcUa::Server::CreateAddressSpace(debug);
      OpcUa::Server::FillStandardNamespace(*NameSpace, debug);
    }
  }

  virtual void TearDown()
  {
//    NameSpace.reset();
  }

protected:
  std::vector<ReferenceDescription> Browse(const NodeID& id) const
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = id;
    OpcUa::NodesQuery query;
    query.NodesToBrowse.push_back(description);
    auto result = NameSpace->Browse(query);
    if (result.empty())
    {
      return std::vector<ReferenceDescription>();
    }
    return result[0].Referencies;
  }

  bool HasReference(std::vector<ReferenceDescription> refs, ReferenceID referenceID,  NodeID targetNode) const
  {
    for (const ReferenceDescription ref : refs)
    {
      if (ref.TargetNodeID == targetNode && ref.ReferenceTypeID == referenceID)
      {
        return true;
      }
    }
    return false;
  }

  bool HasAttribute(OpcUa::ObjectID object, OpcUa::AttributeID attribute)
  {
    ReadParameters params;
    AttributeValueID id;
    id.Node = object;
    id.Attribute = attribute;
    params.AttributesToRead.push_back(id);
    std::vector<DataValue> values = NameSpace->Read(params);
    return values.size() == 1 && values[0].Status == StatusCode::Good;
  }

  void ExpectHasBaseAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::NodeId));
    EXPECT_TRUE(HasAttribute(id, AttributeID::NodeClass));
    EXPECT_TRUE(HasAttribute(id, AttributeID::BrowseName));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DisplayName));
    EXPECT_TRUE(HasAttribute(id, AttributeID::Description));
    EXPECT_TRUE(HasAttribute(id, AttributeID::WriteMask));
    EXPECT_TRUE(HasAttribute(id, AttributeID::UserWriteMask));
  }

  void ExpectHasObjectAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::EventNotifier));
  }

  void ExpectHasTypeAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::IsAbstract));
  }

  void ExpectHasDataTypeAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::IsAbstract));
  }

  void ExpectHasVariableAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::Value));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DataType));
    EXPECT_TRUE(HasAttribute(id, AttributeID::ValueRank));
    EXPECT_TRUE(HasAttribute(id, AttributeID::ArrayDimensions));
    EXPECT_TRUE(HasAttribute(id, AttributeID::AccessLevel));
    EXPECT_TRUE(HasAttribute(id, AttributeID::UserAccessLevel));
    EXPECT_TRUE(HasAttribute(id, AttributeID::MinimumSamplingInterval));
    EXPECT_TRUE(HasAttribute(id, AttributeID::Historizing));
  }

  void ExpectHasVariableTypeAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::Value));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DataType));
    EXPECT_TRUE(HasAttribute(id, AttributeID::ValueRank));
    EXPECT_TRUE(HasAttribute(id, AttributeID::ArrayDimensions));
    EXPECT_TRUE(HasAttribute(id, AttributeID::IsAbstract));
  }

  void ExpectHasReferenceTypeAttributes(ObjectID id)
  {
    ExpectHasBaseAttributes(id);
    ExpectHasTypeAttributes(id);
    EXPECT_TRUE(HasAttribute(id, AttributeID::Symmetric));
  }
};

template <typename T>
std::size_t SizeOf(std::vector<T> vec)
{
  return vec.size();
}

template <typename T>
inline NodeID Node(T value)
{
  return NodeID(value);
}

TEST_F(StandardNamespaceStructure, CanBrowseRootFolder_By_Organizes_RefType)
{
  OpcUa::BrowseDescription description;
  description.NodeToBrowse = ObjectID::RootFolder;
  description.Direction = BrowseDirection::Forward;
  description.ReferenceTypeID = ReferenceID::Organizes;
  description.IncludeSubtypes = true;
  description.NodeClasses = NODE_CLASS_OBJECT;
  description.ResultMask = REFERENCE_ALL;

  OpcUa::NodesQuery query;
  query.NodesToBrowse.push_back(description);
  std::vector<BrowseResult> results = NameSpace->Browse(query);
  ASSERT_EQ(results.size(), 1);
  ASSERT_EQ(results[0].Referencies.size(), 3);
}

TEST_F(StandardNamespaceStructure, CanBrowseRootFolder_By_HierarchicalReferencies_Subtypes)
{
  OpcUa::BrowseDescription description;
  description.NodeToBrowse = ObjectID::RootFolder;
  description.Direction = BrowseDirection::Forward;
  description.ReferenceTypeID = ReferenceID::HierarchicalReferences;
  description.IncludeSubtypes = true;
  description.NodeClasses = NODE_CLASS_OBJECT;
  description.ResultMask = REFERENCE_ALL;
  OpcUa::NodesQuery query;
  query.NodesToBrowse.push_back(description);
  std::vector<BrowseResult> results = NameSpace->Browse(query);
  ASSERT_EQ(results.size(), 1);
  ASSERT_EQ(results[0].Referencies.size(), 3);
}

TEST_F(StandardNamespaceStructure, CheckRoot)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RootFolder);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ObjectsFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::TypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ViewsFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));

  ExpectHasBaseAttributes(ObjectID::RootFolder);
}

TEST_F(StandardNamespaceStructure, Server)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_NamespaceArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServiceLevel));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerCapabilities));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerDiagnostics));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_VendorServerInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerRedundancy));

  ExpectHasBaseAttributes(ObjectID::Server);
}

TEST_F(StandardNamespaceStructure, Server_ServerArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerArray);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerArray);
  ExpectHasVariableAttributes(ObjectID::Server_ServerArray);
}

TEST_F(StandardNamespaceStructure, Server_NamespaceArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_NamespaceArray);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_NamespaceArray);
  ExpectHasVariableAttributes(ObjectID::Server_NamespaceArray);
}

TEST_F(StandardNamespaceStructure, Server_ServiceLevel)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServiceLevel);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServiceLevel);
  ExpectHasVariableAttributes(ObjectID::Server_ServiceLevel);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerCapabilitiesType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerCapabilities_ServerProfileArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerCapabilities_MinSupportedSampleRate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerCapabilities_MaxBrowseContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerCapabilities_MaxQueryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerCapabilities_MaxHistoryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerCapabilities_ModellingRules));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerCapabilities_SoftwareCertificates));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_ServerProfileArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_ServerProfileArray);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_ServerProfileArray);
  ExpectHasVariableAttributes(ObjectID::Server_ServerCapabilities_ServerProfileArray);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MinSupportedSampleRate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_MinSupportedSampleRate);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_MinSupportedSampleRate);
  ExpectHasVariableAttributes(ObjectID::Server_ServerCapabilities_MinSupportedSampleRate);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MaxBrowseContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_MaxBrowseContinuationPoints);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_MaxBrowseContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::Server_ServerCapabilities_MaxBrowseContinuationPoints);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MaxQueryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_MaxQueryContinuationPoints);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_MaxQueryContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::Server_ServerCapabilities_MaxQueryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MaxHistoryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_MaxHistoryContinuationPoints);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_MaxHistoryContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::Server_ServerCapabilities_MaxHistoryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_SoftwareCertificates)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_SoftwareCertificates);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_SoftwareCertificates);
  ExpectHasVariableAttributes(ObjectID::Server_ServerCapabilities_SoftwareCertificates);
}

TEST_F(StandardNamespaceStructure, Server_ModellingRules)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerCapabilities_ModellingRules);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ModellingRule_ExposesItsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ModellingRule_Mandatory));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ModellingRule_MandatoryShared));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ModellingRule_Optional));

  ExpectHasBaseAttributes(ObjectID::Server_ServerCapabilities_ModellingRules);
}

TEST_F(StandardNamespaceStructure, ModellingRule_ExposesItsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_ExposesItsArray);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ModellingRule_ExposesItsArray_NamingRule));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_ExposesItsArray);
}

TEST_F(StandardNamespaceStructure, ModellingRule_ExposesItsArray_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_ExposesItsArray_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_ExposesItsArray_NamingRule);
  ExpectHasVariableAttributes(ObjectID::ModellingRule_ExposesItsArray_NamingRule);
}


TEST_F(StandardNamespaceStructure, ModellingRule_Mandatory)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_Mandatory);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ModellingRule_Mandatory_NamingRule));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_Mandatory);
}

TEST_F(StandardNamespaceStructure, ModellingRule_Mandatory_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_Mandatory_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_Mandatory_NamingRule);
  ExpectHasVariableAttributes(ObjectID::ModellingRule_Mandatory_NamingRule);
}

TEST_F(StandardNamespaceStructure, ModellingRule_MandatoryShared)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_MandatoryShared);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ModellingRule_MandatoryShared_NamingRule));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_MandatoryShared);
}

TEST_F(StandardNamespaceStructure, ModellingRule_MandatoryShared_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_MandatoryShared_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_MandatoryShared_NamingRule);
  ExpectHasVariableAttributes(ObjectID::ModellingRule_MandatoryShared_NamingRule);
}

TEST_F(StandardNamespaceStructure, ModellingRule_Optional)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_Optional);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ModellingRule_Optional_NamingRule));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_Optional);
}

TEST_F(StandardNamespaceStructure, ModellingRule_Optional_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRule_Optional_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ModellingRule_Optional_NamingRule);
  ExpectHasVariableAttributes(ObjectID::ModellingRule_Optional_NamingRule);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerDiagnostics);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Server_ServerDiagnostics_EnabledFlag));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerDiagnostics_SessionsDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerDiagnostics_ServerDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerDiagnostics_SubscriptionDiagnosticsArray));

  ExpectHasBaseAttributes(ObjectID::Server_ServerDiagnostics);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_EnabledFlag)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerDiagnostics_EnabledFlag);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerDiagnostics_EnabledFlag);
  ExpectHasVariableAttributes(ObjectID::Server_ServerDiagnostics_EnabledFlag);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SamplingIntervalDiagnosticsArrayType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectID::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_SessionsDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerDiagnostics_SessionsDiagnosticsSummary);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SessionsDiagnosticsSummaryType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerDiagnostics_SessionsDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_ServerDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerDiagnostics_ServerDiagnosticsSummary);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsSummaryType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerDiagnostics_ServerDiagnosticsSummary);
  ExpectHasVariableAttributes(ObjectID::Server_ServerDiagnostics_ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_SubscriptionRateDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerDiagnostics_SubscriptionDiagnosticsArray);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SubscriptionDiagnosticsArrayType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerDiagnostics_SubscriptionDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectID::Server_ServerDiagnostics_SubscriptionDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, Server_ServerRedundancy)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerRedundancy);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerRedundancyType));

  //EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerRedundancy_RedundancySupport));

  ExpectHasBaseAttributes(ObjectID::Server_ServerRedundancy);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerStatusType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_CurrentTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_StartTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_State));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BuildInfoType));

  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo_BuildDate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo_BuildNumber));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo_ManufacturerName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo_ProductName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo_ProductUri));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::Server_ServerStatus_BuildInfo_SoftwareVersion));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_BuildDate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo_BuildDate);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_BuildNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo_BuildNumber);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo_BuildNumber);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo_BuildNumber);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_ManufacturerName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo_ManufacturerName);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo_ManufacturerName);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo_ManufacturerName);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_ProductName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo_ProductName);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo_ProductName);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo_ProductName);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_ProductUri)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo_ProductUri);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo_ProductUri);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo_ProductUri);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_SoftwareVersion)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_BuildInfo_SoftwareVersion);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_BuildInfo_SoftwareVersion);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_BuildInfo_SoftwareVersion);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus_CurrentTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_CurrentTime);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_CurrentTime);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_CurrentTime);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus_StartTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_StartTime);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_StartTime);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_StartTime);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus_State)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_ServerStatus_State);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::Server_ServerStatus_State);
  ExpectHasVariableAttributes(ObjectID::Server_ServerStatus_State);
}

TEST_F(StandardNamespaceStructure, Server_VendorServerInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Server_VendorServerInfo);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::VendorServerInfoType));

  ExpectHasBaseAttributes(ObjectID::Server_VendorServerInfo);
}

TEST_F(StandardNamespaceStructure, CheckTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::TypesFolder);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::DataTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::EventTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ObjectTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ReferenceTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::VariableTypesFolder));

  ExpectHasBaseAttributes(ObjectID::TypesFolder);
}

TEST_F(StandardNamespaceStructure, DataTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseDataType));

  ExpectHasBaseAttributes(ObjectID::DataTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseDataType);
  EXPECT_EQ(SizeOf(refs), 16);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Boolean));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ByteString));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataValue));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DateTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DiagnosticInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Enumeration));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ExpandedNodeId));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Guid));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::LocalizedText));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NodeId));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Number));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::QualifiedName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StatusCode));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::String));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Structure));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::XmlElement));

  ExpectHasBaseAttributes(ObjectID::BaseDataType);
  ExpectHasTypeAttributes(ObjectID::BaseDataType);
}

TEST_F(StandardNamespaceStructure, Boolean)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Boolean);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Boolean);
  ExpectHasTypeAttributes(ObjectID::Boolean);
}

TEST_F(StandardNamespaceStructure, ByteString)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ByteString);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Image));

  ExpectHasBaseAttributes(ObjectID::ByteString);
  ExpectHasTypeAttributes(ObjectID::ByteString);
}

TEST_F(StandardNamespaceStructure, Image)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Image);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageBMP));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageGIF));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageJPG));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImagePNG));

  ExpectHasBaseAttributes(ObjectID::Image);
  ExpectHasTypeAttributes(ObjectID::Image);
}

TEST_F(StandardNamespaceStructure, ImageBmp)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageBMP);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageBMP);
  ExpectHasTypeAttributes(ObjectID::ImageBMP);
}

TEST_F(StandardNamespaceStructure, ImageGif)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageGIF);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageGIF);
  ExpectHasTypeAttributes(ObjectID::ImageGIF);
}

TEST_F(StandardNamespaceStructure, ImageJPG)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageJPG);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageJPG);
  ExpectHasTypeAttributes(ObjectID::ImageJPG);
}

TEST_F(StandardNamespaceStructure, ImagePng)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImagePNG);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImagePNG);
  ExpectHasTypeAttributes(ObjectID::ImagePNG);
}

TEST_F(StandardNamespaceStructure, DateTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DateTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UtcTime));

  ExpectHasBaseAttributes(ObjectID::DateTime);
  ExpectHasTypeAttributes(ObjectID::DateTime);
}

TEST_F(StandardNamespaceStructure, UtcTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UtcTime);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::UtcTime);
  ExpectHasTypeAttributes(ObjectID::UtcTime);
}

TEST_F(StandardNamespaceStructure, DiagnosticInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DiagnosticInfo);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DiagnosticInfo);
  ExpectHasTypeAttributes(ObjectID::DiagnosticInfo);
}

TEST_F(StandardNamespaceStructure, Enumeration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Enumeration);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::IdType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::MessageSecurityMode));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NodeClass));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::RedundancySupport));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SecurityTokenRequestType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerState));

  ExpectHasBaseAttributes(ObjectID::Enumeration);
  ExpectHasTypeAttributes(ObjectID::Enumeration);
}

TEST_F(StandardNamespaceStructure, IdType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::IdType);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectID::IdType);
  ExpectHasTypeAttributes(ObjectID::IdType);
}

TEST_F(StandardNamespaceStructure, MessageSecurityMode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MessageSecurityMode);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectID::MessageSecurityMode);
  ExpectHasTypeAttributes(ObjectID::MessageSecurityMode);
}

TEST_F(StandardNamespaceStructure, NodeClass)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NodeClass);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::NodeClass);
  ExpectHasTypeAttributes(ObjectID::NodeClass);
}

TEST_F(StandardNamespaceStructure, RedundancySupport)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RedundancySupport);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectID::RedundancySupport);
  ExpectHasTypeAttributes(ObjectID::RedundancySupport);
}

TEST_F(StandardNamespaceStructure, SecurityTokenRequestType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SecurityTokenRequestType);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectID::SecurityTokenRequestType);
  ExpectHasTypeAttributes(ObjectID::SecurityTokenRequestType);
}

TEST_F(StandardNamespaceStructure, ServerState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerState);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectID::ServerState);
  ExpectHasTypeAttributes(ObjectID::ServerState);
}

TEST_F(StandardNamespaceStructure, ExpandedNodeID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ExpandedNodeId);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ExpandedNodeId);
  ExpectHasTypeAttributes(ObjectID::ExpandedNodeId);
}

TEST_F(StandardNamespaceStructure, Guid)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Guid);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Guid);
  ExpectHasTypeAttributes(ObjectID::Guid);
}

TEST_F(StandardNamespaceStructure, LocalizedText)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::LocalizedText);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::LocalizedText);
  ExpectHasTypeAttributes(ObjectID::LocalizedText);
}

TEST_F(StandardNamespaceStructure, NodeID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NodeId);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::NodeId);
  ExpectHasTypeAttributes(ObjectID::NodeId);
}

TEST_F(StandardNamespaceStructure, Number)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Number);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Double));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Float));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Integer));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UInteger));

  ExpectHasBaseAttributes(ObjectID::Number);
  ExpectHasTypeAttributes(ObjectID::Number);
}

TEST_F(StandardNamespaceStructure, Double)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Double);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Duration));

  ExpectHasBaseAttributes(ObjectID::Double);
  ExpectHasTypeAttributes(ObjectID::Double);
}

TEST_F(StandardNamespaceStructure, Duration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Duration);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Duration);
  ExpectHasTypeAttributes(ObjectID::Duration);
}

TEST_F(StandardNamespaceStructure, Float)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Float);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Float);
  ExpectHasTypeAttributes(ObjectID::Float);
}

TEST_F(StandardNamespaceStructure, Integer)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Integer);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Int16));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Int32));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Int64));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SByte));

  ExpectHasBaseAttributes(ObjectID::Integer);
  ExpectHasTypeAttributes(ObjectID::Integer);
}

TEST_F(StandardNamespaceStructure, Int16)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Int16);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Int16);
  ExpectHasTypeAttributes(ObjectID::Int16);
}

TEST_F(StandardNamespaceStructure, Int32)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Int32);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Int32);
  ExpectHasTypeAttributes(ObjectID::Int32);
}

TEST_F(StandardNamespaceStructure, Int64)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Int64);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Int64);
  ExpectHasTypeAttributes(ObjectID::Int64);
}

TEST_F(StandardNamespaceStructure, SByte)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SByte);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SByte);
  ExpectHasTypeAttributes(ObjectID::SByte);
}

TEST_F(StandardNamespaceStructure, UInteger)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UInteger);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UInt16));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UInt32));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UInt64));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Byte));

  ExpectHasBaseAttributes(ObjectID::UInteger);
  ExpectHasTypeAttributes(ObjectID::UInteger);
}

TEST_F(StandardNamespaceStructure, UInt16)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UInt16);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::UInt16);
  ExpectHasTypeAttributes(ObjectID::UInt16);
}

TEST_F(StandardNamespaceStructure, UInt32)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UInt32);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::UInt32);
  ExpectHasTypeAttributes(ObjectID::UInt32);
}

TEST_F(StandardNamespaceStructure, UInt64)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UInt64);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::UInt64);
  ExpectHasTypeAttributes(ObjectID::UInt64);
}

TEST_F(StandardNamespaceStructure, Byte)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Byte);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Byte);
  ExpectHasTypeAttributes(ObjectID::Byte);
}

TEST_F(StandardNamespaceStructure, QualifiedName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::QualifiedName);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::QualifiedName);
  ExpectHasTypeAttributes(ObjectID::QualifiedName);
}

TEST_F(StandardNamespaceStructure, StatusCode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StatusCode);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StatusCode);
  ExpectHasTypeAttributes(ObjectID::StatusCode);
}

TEST_F(StandardNamespaceStructure, String)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::String);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::LocaleId));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NumericRange));

  ExpectHasBaseAttributes(ObjectID::String);
  ExpectHasTypeAttributes(ObjectID::String);
}

TEST_F(StandardNamespaceStructure, LocaleID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::LocaleId);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::LocaleId);
  ExpectHasTypeAttributes(ObjectID::LocaleId);
}

TEST_F(StandardNamespaceStructure, NumericRange)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NumericRange);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::NumericRange);
  ExpectHasTypeAttributes(ObjectID::NumericRange);
}

TEST_F(StandardNamespaceStructure, Structure)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Structure);
  EXPECT_EQ(SizeOf(refs), 21);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::AddNodesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::AddReferencesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ApplicationDescription));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Argument));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BuildInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DeleteNodesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DeleteReferencesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::EUInformation));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ModelChangeStructureDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Range));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SemanticChangeStructureDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsSummaryDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerStatusDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServiceCounterDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SignedSoftwareCertificate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StatusResult));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UserIdentityToken));

  ExpectHasBaseAttributes(ObjectID::Structure);
  ExpectHasTypeAttributes(ObjectID::Structure);
}

TEST_F(StandardNamespaceStructure, StructureAddNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::AddNodesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::AddNodesItem);
  ExpectHasTypeAttributes(ObjectID::AddNodesItem);
}

TEST_F(StandardNamespaceStructure, StructureAddReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::AddReferencesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::AddReferencesItem);
  ExpectHasTypeAttributes(ObjectID::AddReferencesItem);
}

TEST_F(StandardNamespaceStructure, StructureApplicationDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ApplicationDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ApplicationDescription);
  ExpectHasTypeAttributes(ObjectID::ApplicationDescription);
}

TEST_F(StandardNamespaceStructure, StructureArgument)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Argument);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Argument);
  ExpectHasTypeAttributes(ObjectID::Argument);
}

TEST_F(StandardNamespaceStructure, StructureBuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfo);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::BuildInfo);
  ExpectHasTypeAttributes(ObjectID::BuildInfo);
}

TEST_F(StandardNamespaceStructure, StructureDeleteNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DeleteNodesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DeleteNodesItem);
  ExpectHasTypeAttributes(ObjectID::DeleteNodesItem);
}

TEST_F(StandardNamespaceStructure, StructureDeleteReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DeleteReferencesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DeleteReferencesItem);
  ExpectHasTypeAttributes(ObjectID::DeleteReferencesItem);
}

TEST_F(StandardNamespaceStructure, StructureEUInformation)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::EUInformation);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::EUInformation);
  ExpectHasTypeAttributes(ObjectID::EUInformation);
}

TEST_F(StandardNamespaceStructure, EUModelChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModelChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ModelChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectID::ModelChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, StructureRange)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Range);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Range);
  ExpectHasTypeAttributes(ObjectID::Range);
}

TEST_F(StandardNamespaceStructure, StructureSamplingIntervalDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SamplingIntervalDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSemanticChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SemanticChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SemanticChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectID::SemanticChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, StructureServerDiagnosticsSummaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsSummaryType);
  ExpectHasTypeAttributes(ObjectID::ServerDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, StructureServerStatusDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerStatusDataType);
  ExpectHasTypeAttributes(ObjectID::ServerStatusDataType);
}

TEST_F(StandardNamespaceStructure, StructureServiceCounterDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServiceCounterDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServiceCounterDataType);
  ExpectHasTypeAttributes(ObjectID::ServiceCounterDataType);
}

TEST_F(StandardNamespaceStructure, StructureSessionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SessionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSessionSecurityDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionSecurityDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionSecurityDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SessionSecurityDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSignedSoftwareCertificate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SignedSoftwareCertificate);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SignedSoftwareCertificate);
  ExpectHasTypeAttributes(ObjectID::SignedSoftwareCertificate);
}

TEST_F(StandardNamespaceStructure, StructureStatusResult)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StatusResult);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StatusResult);
  ExpectHasTypeAttributes(ObjectID::StatusResult);
}

TEST_F(StandardNamespaceStructure, StructureSubscriptionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SubscriptionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureUserIdentifyToken)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UserIdentityToken);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::AnonymousIdentityToken));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UserNameIdentityToken));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::X509IdentityToken));

  ExpectHasBaseAttributes(ObjectID::UserIdentityToken);
  ExpectHasTypeAttributes(ObjectID::UserIdentityToken);
}

TEST_F(StandardNamespaceStructure, XmlElement)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::XmlElement);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::XmlElement);
  ExpectHasTypeAttributes(ObjectID::XmlElement);
}

TEST_F(StandardNamespaceStructure, EventTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::EventTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));

  ExpectHasBaseAttributes(ObjectID::EventTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType);
  EXPECT_EQ(SizeOf(refs), 11);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BaseModelChangeEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SemanticChangeEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SystemEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_EventId));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_EventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_Message));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_ReceiveTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_Severity));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_SourceName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_SourceNode));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::BaseEventType_Time));

  ExpectHasBaseAttributes(ObjectID::BaseEventType);
  ExpectHasTypeAttributes(ObjectID::BaseEventType);
}

TEST_F(StandardNamespaceStructure, BaseModelChangeEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseModelChangeEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::BaseModelChangeEventType);
  ExpectHasTypeAttributes(ObjectID::BaseModelChangeEventType);
}

TEST_F(StandardNamespaceStructure, SemanticChangeEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SemanticChangeEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SemanticChangeEventType);
  ExpectHasTypeAttributes(ObjectID::SemanticChangeEventType);
}

TEST_F(StandardNamespaceStructure, SystemEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SystemEventType);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DeviceFailureEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::RefreshEndEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::RefreshRequiredEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::RefreshStartEventType));

  ExpectHasBaseAttributes(ObjectID::SystemEventType);
  ExpectHasTypeAttributes(ObjectID::SystemEventType);
}

TEST_F(StandardNamespaceStructure, DeviceFailureEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DeviceFailureEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DeviceFailureEventType);
  ExpectHasTypeAttributes(ObjectID::DeviceFailureEventType);
}

TEST_F(StandardNamespaceStructure, RefreshEndEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RefreshEndEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::RefreshEndEventType);
  ExpectHasTypeAttributes(ObjectID::RefreshEndEventType);
}

TEST_F(StandardNamespaceStructure, RefreshRequiredEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RefreshRequiredEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::RefreshRequiredEventType);
  ExpectHasTypeAttributes(ObjectID::RefreshRequiredEventType);
}

TEST_F(StandardNamespaceStructure, RefreshStartEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RefreshStartEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::RefreshStartEventType);
  ExpectHasTypeAttributes(ObjectID::RefreshStartEventType);
}

TEST_F(StandardNamespaceStructure, EventID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_EventId);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_EventId);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_EventId);
}

TEST_F(StandardNamespaceStructure, EventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_EventType);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_EventType);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_EventType);
}

TEST_F(StandardNamespaceStructure, Message)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_Message);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_Message);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_Message);
}

TEST_F(StandardNamespaceStructure, ReceiveTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_ReceiveTime);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_ReceiveTime);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_ReceiveTime);
}

TEST_F(StandardNamespaceStructure, Severity)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_Severity);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_Severity);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_Severity);
}

TEST_F(StandardNamespaceStructure, SourceName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_SourceName);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_SourceName);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_SourceName);
}

TEST_F(StandardNamespaceStructure, SourceNode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_SourceNode);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_SourceNode);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_SourceNode);
}

TEST_F(StandardNamespaceStructure, Time)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType_Time);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::BaseEventType_Time);
  ExpectHasVariableAttributes(ObjectID::BaseEventType_Time);
}

TEST_F(StandardNamespaceStructure, ObjectTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ObjectTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseObjectType));

  ExpectHasBaseAttributes(ObjectID::ObjectTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseObjectType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseObjectType);
  EXPECT_EQ(SizeOf(refs), 15-1); // XXX
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BaseEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeEncodingType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeSystemType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::FolderType));
  //EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HistoricalEventConfigurationType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ModellingRuleType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerCapabilitiesType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerRedundancyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsObjectType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionsDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StateType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::TransitionType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::VendorServerInfoType));

  ExpectHasBaseAttributes(ObjectID::BaseObjectType);
  ExpectHasTypeAttributes(ObjectID::BaseObjectType);
}

TEST_F(StandardNamespaceStructure, DataTypeEncodingType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypeEncodingType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DataTypeEncodingType);
  ExpectHasTypeAttributes(ObjectID::DataTypeEncodingType);
}

TEST_F(StandardNamespaceStructure, DataTypeSystemType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypeSystemType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DataTypeSystemType);
  ExpectHasTypeAttributes(ObjectID::DataTypeSystemType);
}

TEST_F(StandardNamespaceStructure, FolderType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::FolderType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::FolderType);
  ExpectHasTypeAttributes(ObjectID::FolderType);
}

//TEST_F(StandardNamespaceStructure, HistoricalEventConfigurationType)
//{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HistoricalEventConfigurationType);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasBaseAttributes(ObjectID::HistoricalEventConfigurationType);
//  ExpectHasTypeAttributes(ObjectID::HistoricalEventConfigurationType);
//}

TEST_F(StandardNamespaceStructure, ModellingRuleType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRuleType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ModellingRuleType_NamingRule));

  ExpectHasBaseAttributes(ObjectID::ModellingRuleType);
  ExpectHasTypeAttributes(ObjectID::ModellingRuleType);
}

TEST_F(StandardNamespaceStructure, NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRuleType_NamingRule);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ModellingRuleType_NamingRule);
  ExpectHasVariableAttributes(ObjectID::ModellingRuleType_NamingRule);
}

TEST_F(StandardNamespaceStructure, ServerCapabilitiesType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerCapabilitiesType_LocaleIdArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerCapabilitiesType_MaxBrowseContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerCapabilitiesType_MaxHistoryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerCapabilitiesType_MinSupportedSampleRate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerCapabilitiesType_ModellingRules));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerCapabilitiesType_ServerProfileArray));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType);
  ExpectHasTypeAttributes(ObjectID::ServerCapabilitiesType);
}

TEST_F(StandardNamespaceStructure, ModellingRules)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType_ModellingRules);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType_ModellingRules);
}

TEST_F(StandardNamespaceStructure, LocaleIDArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType_LocaleIdArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType_LocaleIdArray);
  ExpectHasVariableAttributes(ObjectID::ServerCapabilitiesType_LocaleIdArray);
}

TEST_F(StandardNamespaceStructure, MaxBrowseContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType_MaxBrowseContinuationPoints);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType_MaxBrowseContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::ServerCapabilitiesType_MaxBrowseContinuationPoints);
}

TEST_F(StandardNamespaceStructure, MaxHistoryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType_MaxHistoryContinuationPoints);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType_MaxHistoryContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::ServerCapabilitiesType_MaxHistoryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, MinSupportedSampleRate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType_MinSupportedSampleRate);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType_MinSupportedSampleRate);
  ExpectHasVariableAttributes(ObjectID::ServerCapabilitiesType_MinSupportedSampleRate);
}

TEST_F(StandardNamespaceStructure, ServerProfileArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType_ServerProfileArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType_ServerProfileArray);
  ExpectHasVariableAttributes(ObjectID::ServerCapabilitiesType_ServerProfileArray);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 5);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerDiagnosticsType_EnabledFlag));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerDiagnosticsType_SubscriptionDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerDiagnosticsType_SessionsDiagnosticsSummary));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType);
  ExpectHasTypeAttributes(ObjectID::ServerDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, EnableFlag)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType_EnabledFlag);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType_EnabledFlag);
  ExpectHasVariableAttributes(ObjectID::ServerDiagnosticsType_EnabledFlag);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SamplingIntervalDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectID::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary);
  ExpectHasVariableAttributes(ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType_SessionsDiagnosticsSummary);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SessionsDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType_ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType_SubscriptionDiagnosticsArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SubscriptionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType_SubscriptionDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, ServerRedundancyType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerRedundancyType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ServerRedundancyType_RedundancySupport));

  ExpectHasBaseAttributes(ObjectID::ServerRedundancyType);
  ExpectHasTypeAttributes(ObjectID::ServerRedundancyType);
}

TEST_F(StandardNamespaceStructure, RedundancySupportTypeRedundancySupport)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerRedundancyType_RedundancySupport);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ServerRedundancyType_RedundancySupport);
  ExpectHasVariableAttributes(ObjectID::ServerRedundancyType_RedundancySupport);
}

TEST_F(StandardNamespaceStructure, ServerType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType);
  EXPECT_EQ(SizeOf(refs), 8);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerType_NamespaceArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerType_ServerArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerType_ServerCapabilities));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerType_ServerDiagnostics));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerType_ServerRedundancy));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerType_ServerStatus));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerType_ServiceLevel));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerType_VendorServerInfo));

  ExpectHasBaseAttributes(ObjectID::ServerType);
  ExpectHasTypeAttributes(ObjectID::ServerType);
}

TEST_F(StandardNamespaceStructure, NamespaceArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_NamespaceArray);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition,  ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ServerType_NamespaceArray);
  ExpectHasVariableAttributes(ObjectID::ServerType_NamespaceArray);
}

TEST_F(StandardNamespaceStructure, ServerArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_ServerArray);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition,  ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ServerType_ServerArray);
  ExpectHasVariableAttributes(ObjectID::ServerType_ServerArray);
}

TEST_F(StandardNamespaceStructure, ServerCapabilities)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_ServerCapabilities);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerCapabilitiesType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerType_ServerCapabilities);
  ExpectHasObjectAttributes(ObjectID::ServerType_ServerCapabilities);
}

TEST_F(StandardNamespaceStructure, ServerDiagnostics)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_ServerDiagnostics);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerType_ServerDiagnostics);
  ExpectHasObjectAttributes(ObjectID::ServerType_ServerDiagnostics);
}

TEST_F(StandardNamespaceStructure, ServerRedundancy)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_ServerRedundancy);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerRedundancyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerType_ServerRedundancy);
  ExpectHasObjectAttributes(ObjectID::ServerType_ServerRedundancy);
}

TEST_F(StandardNamespaceStructure, ServerStatus)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_ServerStatus);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerStatusType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerType_ServerStatus);
  ExpectHasVariableAttributes(ObjectID::ServerType_ServerStatus);
}

TEST_F(StandardNamespaceStructure, BuildInfoType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildInfoType_BuildDate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildInfoType_BuildNumber));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildInfoType_ManufacturerName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildInfoType_ProductName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildInfoType_ProductUri));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildInfoType_SoftwareVersion));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType);
  ExpectHasVariableTypeAttributes(ObjectID::BuildInfoType);
}

TEST_F(StandardNamespaceStructure, BuildDate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType_BuildDate);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType_BuildDate);
  ExpectHasVariableAttributes(ObjectID::BuildInfoType_BuildDate);
}

TEST_F(StandardNamespaceStructure, BuildNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType_BuildNumber);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType_BuildNumber);
  ExpectHasVariableAttributes(ObjectID::BuildInfoType_BuildNumber);
}

TEST_F(StandardNamespaceStructure, ManufacturerName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType_ManufacturerName);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType_ManufacturerName);
  ExpectHasVariableAttributes(ObjectID::BuildInfoType_ManufacturerName);
}

TEST_F(StandardNamespaceStructure, ProductName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType_ProductName);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType_ProductName);
  ExpectHasVariableAttributes(ObjectID::BuildInfoType_ProductName);
}

TEST_F(StandardNamespaceStructure, ProductURI)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType_ProductUri);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType_ProductUri);
  ExpectHasVariableAttributes(ObjectID::BuildInfoType_ProductUri);
}

TEST_F(StandardNamespaceStructure, SoftwareVersion)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfoType_SoftwareVersion);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildInfoType_SoftwareVersion);
  ExpectHasVariableAttributes(ObjectID::BuildInfoType_SoftwareVersion);
}

TEST_F(StandardNamespaceStructure, CurrentTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusType_CurrentTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ServerStatusType_CurrentTime);
  ExpectHasVariableAttributes(ObjectID::ServerStatusType_CurrentTime);
}

TEST_F(StandardNamespaceStructure, StartTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusType_StartTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ServerStatusType_StartTime);
  ExpectHasVariableAttributes(ObjectID::ServerStatusType_StartTime);
}

TEST_F(StandardNamespaceStructure, State)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusType_State);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ServerStatusType_State);
  ExpectHasVariableAttributes(ObjectID::ServerStatusType_State);
}

TEST_F(StandardNamespaceStructure, ServiceLevel)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_ServiceLevel);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerType_ServiceLevel);
  ExpectHasVariableAttributes(ObjectID::ServerType_ServiceLevel);
}

TEST_F(StandardNamespaceStructure, VendorServerInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType_VendorServerInfo);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::VendorServerInfoType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectID::ServerType_VendorServerInfo);
  ExpectHasObjectAttributes(ObjectID::ServerType_VendorServerInfo);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsObjectType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionDiagnosticsObjectType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionDiagnosticsObjectType);
  ExpectHasTypeAttributes(ObjectID::SessionDiagnosticsObjectType);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsSummaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionsDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionsDiagnosticsSummaryType);
  ExpectHasTypeAttributes(ObjectID::SessionsDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, StateType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StateType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::StateType_StateNumber));

  ExpectHasBaseAttributes(ObjectID::StateType);
  ExpectHasTypeAttributes(ObjectID::StateType);
}

TEST_F(StandardNamespaceStructure, StateNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StateType_StateNumber);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::StateType_StateNumber);
  ExpectHasVariableAttributes(ObjectID::StateType_StateNumber);
}

TEST_F(StandardNamespaceStructure, TransitionType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::TransitionType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::TransitionType);
  ExpectHasTypeAttributes(ObjectID::TransitionType);
}

TEST_F(StandardNamespaceStructure, VendorServerInfoType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::VendorServerInfoType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::VendorServerInfoType);
  ExpectHasTypeAttributes(ObjectID::VendorServerInfoType);
}

TEST_F(StandardNamespaceStructure, ReferenceTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ReferenceTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::References));

  ExpectHasBaseAttributes(ObjectID::ReferenceTypesFolder);
}

TEST_F(StandardNamespaceStructure, References)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::References);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HierarchicalReferences));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NonHierarchicalReferences));

  ExpectHasReferenceTypeAttributes(ObjectID::References);
  EXPECT_FALSE(HasAttribute(ObjectID::HierarchicalReferences, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HierarchicalReferences)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HierarchicalReferences);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasChild));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasEventSource));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Organizes));

  ExpectHasReferenceTypeAttributes(ObjectID::HierarchicalReferences);
  EXPECT_FALSE(HasAttribute(ObjectID::HierarchicalReferences, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasChild)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasChild);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasSubtype));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Aggregates));

  ExpectHasReferenceTypeAttributes(ObjectID::HasChild);
  EXPECT_FALSE(HasAttribute(ObjectID::HasChild, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, Aggregates)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Aggregates);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasHistoricalConfiguration));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasComponent));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasProperty));

  ExpectHasReferenceTypeAttributes(ObjectID::Aggregates);
  EXPECT_FALSE(HasAttribute(ObjectID::Aggregates, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasComponent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasComponent);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasOrderedComponent));

  ExpectHasReferenceTypeAttributes(ObjectID::HasComponent);
  EXPECT_TRUE(HasAttribute(ObjectID::HasComponent, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasOrderedComponent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasOrderedComponent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasOrderedComponent);
  EXPECT_TRUE(HasAttribute(ObjectID::HasOrderedComponent, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasHistoricalConfiguration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasHistoricalConfiguration);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasHistoricalConfiguration);
  EXPECT_TRUE(HasAttribute(ObjectID::HasHistoricalConfiguration, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasProperty)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasProperty);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasProperty);
  EXPECT_TRUE(HasAttribute(ObjectID::HasProperty, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasSubtype)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasSubtype);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasSubtype);
  EXPECT_TRUE(HasAttribute(ObjectID::HasSubtype, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasEventSource)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasEventSource);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasNotifier));

  ExpectHasReferenceTypeAttributes(ObjectID::HasEventSource);
  EXPECT_TRUE(HasAttribute(ObjectID::HasEventSource, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasNotifier)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasNotifier);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasNotifier);
  EXPECT_TRUE(HasAttribute(ObjectID::HasNotifier, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, Organizes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Organizes);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::Organizes);
  EXPECT_TRUE(HasAttribute(ObjectID::Organizes, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, NonHierarchicalReferences)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NonHierarchicalReferences);
  EXPECT_EQ(SizeOf(refs), 10-1); // XXX
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::FromState));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::GeneratesEvent));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasCause));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasDescription));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasEffect));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasEncoding));
  //EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasModelParent));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasModellingRule));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasTypeDefinition));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ToState));

  ExpectHasReferenceTypeAttributes(ObjectID::NonHierarchicalReferences);
  EXPECT_FALSE(HasAttribute(ObjectID::NonHierarchicalReferences, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, FromState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::FromState);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::FromState);
  EXPECT_TRUE(HasAttribute(ObjectID::FromState, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, GeneratesEvent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::GeneratesEvent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::GeneratesEvent);
  EXPECT_TRUE(HasAttribute(ObjectID::GeneratesEvent, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasCause)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasCause);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasCause);
  EXPECT_TRUE(HasAttribute(ObjectID::HasCause, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasDescription);
  EXPECT_TRUE(HasAttribute(ObjectID::HasDescription, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasEffect)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasEffect);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasEffect);
  EXPECT_TRUE(HasAttribute(ObjectID::HasEffect, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasEncoding)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasEncoding);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasEncoding);
  EXPECT_TRUE(HasAttribute(ObjectID::HasEncoding, AttributeID::InverseName));
}

//TEST_F(StandardNamespaceStructure, HasModelParent)
//{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasModelParent);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasReferenceTypeAttributes(ObjectID::HasModelParent);
//  EXPECT_TRUE(HasAttribute(ObjectID::HasModelParent, AttributeID::InverseName));
//}

TEST_F(StandardNamespaceStructure, HasModellingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasModellingRule);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasModellingRule);
  EXPECT_TRUE(HasAttribute(ObjectID::HasModellingRule, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, HasTypeDefinition)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasTypeDefinition);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasTypeDefinition);
  EXPECT_TRUE(HasAttribute(ObjectID::HasTypeDefinition, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, ToState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ToState);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::ToState);
  EXPECT_TRUE(HasAttribute(ObjectID::ToState, AttributeID::InverseName));
}

TEST_F(StandardNamespaceStructure, VariableTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::VariableTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseVariableType));

  ExpectHasBaseAttributes(ObjectID::VariableTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseVariableType);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BaseDataVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::BaseVariableType);
  ExpectHasVariableTypeAttributes(ObjectID::BaseVariableType);
}

TEST_F(StandardNamespaceStructure, BaseDataVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseDataVariableType);
  EXPECT_EQ(SizeOf(refs), 14);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BuildInfoType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeDescriptionType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeDictionaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerStatusType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerVendorCapabilityType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsType));

  ExpectHasBaseAttributes(ObjectID::BaseDataVariableType);
  ExpectHasVariableTypeAttributes(ObjectID::BaseDataVariableType);
}

TEST_F(StandardNamespaceStructure, DataTypeDescriptionType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypeDescriptionType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DataTypeDescriptionType);
  ExpectHasVariableTypeAttributes(ObjectID::DataTypeDescriptionType);
}

TEST_F(StandardNamespaceStructure, DataTypeDictionaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypeDictionaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DataTypeDictionaryType);
  ExpectHasVariableTypeAttributes(ObjectID::DataTypeDictionaryType);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsArrayType);
  ExpectHasVariableTypeAttributes(ObjectID::SamplingIntervalDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsType);
  ExpectHasVariableTypeAttributes(ObjectID::SamplingIntervalDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummaryDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsSummaryDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsSummaryDataType);
  ExpectHasDataTypeAttributes(ObjectID::ServerDiagnosticsSummaryDataType);
}

TEST_F(StandardNamespaceStructure, ServerStatusType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusType);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerStatusType_StartTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerStatusType_CurrentTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerStatusType_State));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerStatusType_BuildInfo));

  ExpectHasBaseAttributes(ObjectID::ServerStatusType);
  ExpectHasVariableTypeAttributes(ObjectID::ServerStatusType);
}

TEST_F(StandardNamespaceStructure, ServerVendorCapabilityType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerVendorCapabilityType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerVendorCapabilityType);
  ExpectHasVariableTypeAttributes(ObjectID::ServerVendorCapabilityType);
}

TEST_F(StandardNamespaceStructure, DISABLED_SessionsDiagnosticsArrayType)
{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionsDiagnosticsDataType);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasBaseAttributes(ObjectID::SessionsDiagnosticsArrayType);
//  ExpectHasVariableTypeAttributes(ObjectID::SessionsDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, DISABLED_ServerDiagnosticsVariableType)
{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsVariableType);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsVariableType);
//  ExpectHasVariableTypeAttributes(ObjectID::ServerDiagnosticsVariableType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionSecurityDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionSecurityDiagnosticsArrayType);
  ExpectHasVariableTypeAttributes(ObjectID::SessionSecurityDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionSecurityDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionSecurityDiagnosticsType);
  ExpectHasVariableTypeAttributes(ObjectID::SessionSecurityDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsArrayType);
  ExpectHasVariableTypeAttributes(ObjectID::SubscriptionDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsType);
  ExpectHasVariableTypeAttributes(ObjectID::SubscriptionDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, PropertyType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::PropertyType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::PropertyType);
  ExpectHasVariableTypeAttributes(ObjectID::PropertyType);
}
