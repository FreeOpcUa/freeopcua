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
    spdlog::drop_all();
    Logger = spdlog::stderr_color_mt("test");
    Logger->set_level(spdlog::level::info);

    if (!NameSpace)
      {
        NameSpace = OpcUa::Server::CreateAddressSpace(Logger);
        OpcUa::Server::FillStandardNamespace(*NameSpace, Logger);
      }
  }

  virtual void TearDown()
  {
//    NameSpace.reset();
  }

protected:
  std::vector<ReferenceDescription> Browse(const NodeId & id) const
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

  bool HasReference(std::vector<ReferenceDescription> refs, ReferenceId referenceId,  NodeId targetNode) const
  {
    for (const ReferenceDescription ref : refs)
      {
        if (ref.TargetNodeId == targetNode && ref.ReferenceTypeId == referenceId)
          {
            return true;
          }
      }

    return false;
  }

  bool HasAttribute(OpcUa::ObjectId object, OpcUa::AttributeId attribute)
  {
    ReadParameters params;
    ReadValueId id;
    id.NodeId = object;
    id.AttributeId = attribute;
    params.AttributesToRead.push_back(id);
    std::vector<DataValue> values = NameSpace->Read(params);
    return values.size() == 1 && values[0].Status == StatusCode::Good;
  }

  void ExpectHasBaseAttributes(ObjectId id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeId::NodeId));
    EXPECT_TRUE(HasAttribute(id, AttributeId::NodeClass));
    EXPECT_TRUE(HasAttribute(id, AttributeId::BrowseName));
    EXPECT_TRUE(HasAttribute(id, AttributeId::DisplayName));
    EXPECT_TRUE(HasAttribute(id, AttributeId::Description));
    EXPECT_TRUE(HasAttribute(id, AttributeId::WriteMask));
    EXPECT_TRUE(HasAttribute(id, AttributeId::UserWriteMask));
  }

  void ExpectHasObjectAttributes(ObjectId id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeId::EventNotifier));
  }

  void ExpectHasTypeAttributes(ObjectId id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeId::IsAbstract));
  }

  void ExpectHasDataTypeAttributes(ObjectId id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeId::IsAbstract));
  }

  void ExpectHasVariableAttributes(ObjectId id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeId::Value));
    EXPECT_TRUE(HasAttribute(id, AttributeId::DataType));
    EXPECT_TRUE(HasAttribute(id, AttributeId::ValueRank));
    EXPECT_TRUE(HasAttribute(id, AttributeId::ArrayDimensions));
    EXPECT_TRUE(HasAttribute(id, AttributeId::AccessLevel));
    EXPECT_TRUE(HasAttribute(id, AttributeId::UserAccessLevel));
    EXPECT_TRUE(HasAttribute(id, AttributeId::MinimumSamplingInterval));
    EXPECT_TRUE(HasAttribute(id, AttributeId::Historizing));
  }

  void ExpectHasVariableTypeAttributes(ObjectId id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeId::Value));
    EXPECT_TRUE(HasAttribute(id, AttributeId::DataType));
    EXPECT_TRUE(HasAttribute(id, AttributeId::ValueRank));
    EXPECT_TRUE(HasAttribute(id, AttributeId::ArrayDimensions));
    EXPECT_TRUE(HasAttribute(id, AttributeId::IsAbstract));
  }

  void ExpectHasReferenceTypeAttributes(ObjectId id)
  {
    ExpectHasBaseAttributes(id);
    ExpectHasTypeAttributes(id);
    EXPECT_TRUE(HasAttribute(id, AttributeId::Symmetric));
  }

private:
  Common::Logger::SharedPtr Logger;
};

template <typename T>
std::size_t SizeOf(std::vector<T> vec)
{
  return vec.size();
}

template <typename T>
inline NodeId Node(T value)
{
  return NodeId(value);
}

TEST_F(StandardNamespaceStructure, CanBrowseRootFolder_By_Organizes_RefType)
{
  OpcUa::BrowseDescription description;
  description.NodeToBrowse = ObjectId::RootFolder;
  description.Direction = BrowseDirection::Forward;
  description.ReferenceTypeId = ReferenceId::Organizes;
  description.IncludeSubtypes = true;
  description.NodeClasses = NodeClass::Object;
  description.ResultMask = BrowseResultMask::All;

  OpcUa::NodesQuery query;
  query.NodesToBrowse.push_back(description);
  std::vector<BrowseResult> results = NameSpace->Browse(query);
  ASSERT_EQ(results.size(), 1);
  ASSERT_EQ(results[0].Referencies.size(), 3);
}

TEST_F(StandardNamespaceStructure, CanBrowseRootFolder_By_HierarchicalReferencies_Subtypes)
{
  OpcUa::BrowseDescription description;
  description.NodeToBrowse = ObjectId::RootFolder;
  description.Direction = BrowseDirection::Forward;
  description.ReferenceTypeId = ReferenceId::HierarchicalReferences;
  description.IncludeSubtypes = true;
  description.NodeClasses = NodeClass::Object;
  description.ResultMask = BrowseResultMask::All;
  OpcUa::NodesQuery query;
  query.NodesToBrowse.push_back(description);
  std::vector<BrowseResult> results = NameSpace->Browse(query);
  ASSERT_EQ(results.size(), 1);
  ASSERT_EQ(results[0].Referencies.size(), 3);
}

TEST_F(StandardNamespaceStructure, CheckRoot)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::RootFolder);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::ObjectsFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::TypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::ViewsFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));

  ExpectHasBaseAttributes(ObjectId::RootFolder);
}

TEST_F(StandardNamespaceStructure, Server)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_NamespaceArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServiceLevel));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerCapabilities));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerDiagnostics));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_VendorServerInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerRedundancy));

  ExpectHasBaseAttributes(ObjectId::Server);
}

TEST_F(StandardNamespaceStructure, Server_ServerArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerArray);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerArray);
  ExpectHasVariableAttributes(ObjectId::Server_ServerArray);
}

TEST_F(StandardNamespaceStructure, Server_NamespaceArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_NamespaceArray);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_NamespaceArray);
  ExpectHasVariableAttributes(ObjectId::Server_NamespaceArray);
}

TEST_F(StandardNamespaceStructure, Server_ServiceLevel)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServiceLevel);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServiceLevel);
  ExpectHasVariableAttributes(ObjectId::Server_ServiceLevel);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerCapabilitiesType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerCapabilities_ServerProfileArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerCapabilities_MinSupportedSampleRate));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerCapabilities_MaxBrowseContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerCapabilities_MaxQueryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerCapabilities_MaxHistoryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerCapabilities_ModellingRules));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerCapabilities_SoftwareCertificates));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_ServerProfileArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_ServerProfileArray);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_ServerProfileArray);
  ExpectHasVariableAttributes(ObjectId::Server_ServerCapabilities_ServerProfileArray);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MinSupportedSampleRate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_MinSupportedSampleRate);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_MinSupportedSampleRate);
  ExpectHasVariableAttributes(ObjectId::Server_ServerCapabilities_MinSupportedSampleRate);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MaxBrowseContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_MaxBrowseContinuationPoints);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_MaxBrowseContinuationPoints);
  ExpectHasVariableAttributes(ObjectId::Server_ServerCapabilities_MaxBrowseContinuationPoints);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MaxQueryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_MaxQueryContinuationPoints);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_MaxQueryContinuationPoints);
  ExpectHasVariableAttributes(ObjectId::Server_ServerCapabilities_MaxQueryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_MaxHistoryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_MaxHistoryContinuationPoints);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_MaxHistoryContinuationPoints);
  ExpectHasVariableAttributes(ObjectId::Server_ServerCapabilities_MaxHistoryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, Server_ServerCapabilities_SoftwareCertificates)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_SoftwareCertificates);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_SoftwareCertificates);
  ExpectHasVariableAttributes(ObjectId::Server_ServerCapabilities_SoftwareCertificates);
}

TEST_F(StandardNamespaceStructure, Server_ModellingRules)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerCapabilities_ModellingRules);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));

  ////EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ModellingRule_ExposesItsArray));
  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ModellingRule_Mandatory));
  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ModellingRule_MandatoryShared));
  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ModellingRule_Optional));

  ExpectHasBaseAttributes(ObjectId::Server_ServerCapabilities_ModellingRules);
}

TEST_F(StandardNamespaceStructure, ModellingRule_ExposesItsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_ExposesItsArray);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::ModellingRule_ExposesItsArray_NamingRule));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_ExposesItsArray);
}

TEST_F(StandardNamespaceStructure, ModellingRule_ExposesItsArray_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_ExposesItsArray_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_ExposesItsArray_NamingRule);
  ExpectHasVariableAttributes(ObjectId::ModellingRule_ExposesItsArray_NamingRule);
}


TEST_F(StandardNamespaceStructure, ModellingRule_Mandatory)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_Mandatory);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::ModellingRule_Mandatory_NamingRule));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_Mandatory);
}

TEST_F(StandardNamespaceStructure, ModellingRule_Mandatory_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_Mandatory_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_Mandatory_NamingRule);
  ExpectHasVariableAttributes(ObjectId::ModellingRule_Mandatory_NamingRule);
}

TEST_F(StandardNamespaceStructure, ModellingRule_MandatoryShared)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_MandatoryShared);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::ModellingRule_MandatoryShared_NamingRule));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_MandatoryShared);
}

TEST_F(StandardNamespaceStructure, ModellingRule_MandatoryShared_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_MandatoryShared_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_MandatoryShared_NamingRule);
  ExpectHasVariableAttributes(ObjectId::ModellingRule_MandatoryShared_NamingRule);
}

TEST_F(StandardNamespaceStructure, ModellingRule_Optional)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_Optional);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ModellingRuleType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::ModellingRule_Optional_NamingRule));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_Optional);
}

TEST_F(StandardNamespaceStructure, ModellingRule_Optional_NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRule_Optional_NamingRule);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ModellingRule_Optional_NamingRule);
  ExpectHasVariableAttributes(ObjectId::ModellingRule_Optional_NamingRule);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerDiagnostics);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerDiagnosticsType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::Server_ServerDiagnostics_EnabledFlag));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerDiagnostics_SessionsDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerDiagnostics_ServerDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerDiagnostics_SubscriptionDiagnosticsArray));

  ExpectHasBaseAttributes(ObjectId::Server_ServerDiagnostics);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_EnabledFlag)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerDiagnostics_EnabledFlag);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerDiagnostics_EnabledFlag);
  ExpectHasVariableAttributes(ObjectId::Server_ServerDiagnostics_EnabledFlag);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::SamplingIntervalDiagnosticsArrayType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectId::Server_ServerDiagnostics_SamplingIntervalDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_SessionsDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerDiagnostics_SessionsDiagnosticsSummary);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::SessionsDiagnosticsSummaryType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerDiagnostics_SessionsDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_ServerDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerDiagnostics_ServerDiagnosticsSummary);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerDiagnosticsSummaryType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerDiagnostics_ServerDiagnosticsSummary);
  ExpectHasVariableAttributes(ObjectId::Server_ServerDiagnostics_ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, Server_ServerDiagnostics_SubscriptionRateDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerDiagnostics_SubscriptionDiagnosticsArray);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::SubscriptionDiagnosticsArrayType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerDiagnostics_SubscriptionDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectId::Server_ServerDiagnostics_SubscriptionDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, Server_ServerRedundancy)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerRedundancy);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerRedundancyType));

  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerRedundancy_RedundancySupport));

  ExpectHasBaseAttributes(ObjectId::Server_ServerRedundancy);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerStatusType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_CurrentTime));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_StartTime));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_State));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BuildInfoType));

  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo_BuildDate));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo_BuildNumber));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo_ManufacturerName));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo_ProductName));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo_ProductUri));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::Server_ServerStatus_BuildInfo_SoftwareVersion));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_BuildDate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo_BuildDate);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_BuildNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo_BuildNumber);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo_BuildNumber);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo_BuildNumber);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_ManufacturerName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo_ManufacturerName);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo_ManufacturerName);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo_ManufacturerName);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_ProductName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo_ProductName);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo_ProductName);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo_ProductName);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_ProductUri)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo_ProductUri);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo_ProductUri);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo_ProductUri);
}

TEST_F(StandardNamespaceStructure, Server_BuildInfo_SoftwareVersion)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_BuildInfo_SoftwareVersion);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_BuildInfo_SoftwareVersion);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_BuildInfo_SoftwareVersion);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus_CurrentTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_CurrentTime);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_CurrentTime);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_CurrentTime);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus_StartTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_StartTime);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_StartTime);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_StartTime);
}

TEST_F(StandardNamespaceStructure, Server_ServerStatus_State)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_ServerStatus_State);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::Server_ServerStatus_State);
  ExpectHasVariableAttributes(ObjectId::Server_ServerStatus_State);
}

TEST_F(StandardNamespaceStructure, Server_VendorServerInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Server_VendorServerInfo);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::VendorServerInfoType));

  ExpectHasBaseAttributes(ObjectId::Server_VendorServerInfo);
}

TEST_F(StandardNamespaceStructure, CheckTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::TypesFolder);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::DataTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::EventTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::ObjectTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::ReferenceTypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::VariableTypesFolder));

  ExpectHasBaseAttributes(ObjectId::TypesFolder);
}

TEST_F(StandardNamespaceStructure, DataTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DataTypesFolder);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::BaseDataType));

  ExpectHasBaseAttributes(ObjectId::DataTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseDataType);
  EXPECT_EQ(SizeOf(refs), 16);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Boolean));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ByteString));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DataValue));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DateTime));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DiagnosticInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Enumeration));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ExpandedNodeId));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Guid));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::LocalizedText));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::NodeId));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Number));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::QualifiedName));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::StatusCode));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::String));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Structure));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::XmlElement));

  ExpectHasBaseAttributes(ObjectId::BaseDataType);
  ExpectHasTypeAttributes(ObjectId::BaseDataType);
}

TEST_F(StandardNamespaceStructure, Boolean)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Boolean);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Boolean);
  ExpectHasTypeAttributes(ObjectId::Boolean);
}

TEST_F(StandardNamespaceStructure, ByteString)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ByteString);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Image));

  ExpectHasBaseAttributes(ObjectId::ByteString);
  ExpectHasTypeAttributes(ObjectId::ByteString);
}

TEST_F(StandardNamespaceStructure, Image)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Image);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ImageBMP));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ImageGIF));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ImageJPG));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ImagePNG));

  ExpectHasBaseAttributes(ObjectId::Image);
  ExpectHasTypeAttributes(ObjectId::Image);
}

TEST_F(StandardNamespaceStructure, ImageBmp)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ImageBMP);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ImageBMP);
  ExpectHasTypeAttributes(ObjectId::ImageBMP);
}

TEST_F(StandardNamespaceStructure, ImageGif)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ImageGIF);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ImageGIF);
  ExpectHasTypeAttributes(ObjectId::ImageGIF);
}

TEST_F(StandardNamespaceStructure, ImageJPG)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ImageJPG);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ImageJPG);
  ExpectHasTypeAttributes(ObjectId::ImageJPG);
}

TEST_F(StandardNamespaceStructure, ImagePng)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ImagePNG);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ImagePNG);
  ExpectHasTypeAttributes(ObjectId::ImagePNG);
}

TEST_F(StandardNamespaceStructure, DateTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DateTime);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UtcTime));

  ExpectHasBaseAttributes(ObjectId::DateTime);
  ExpectHasTypeAttributes(ObjectId::DateTime);
}

TEST_F(StandardNamespaceStructure, UtcTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::UtcTime);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::UtcTime);
  ExpectHasTypeAttributes(ObjectId::UtcTime);
}

TEST_F(StandardNamespaceStructure, DiagnosticInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DiagnosticInfo);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::DiagnosticInfo);
  ExpectHasTypeAttributes(ObjectId::DiagnosticInfo);
}

TEST_F(StandardNamespaceStructure, Enumeration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Enumeration);
  EXPECT_EQ(SizeOf(refs), 17);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::IdType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::MessageSecurityMode));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::NodeClass));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::RedundancySupport));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SecurityTokenRequestType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerState));

  ExpectHasBaseAttributes(ObjectId::Enumeration);
  ExpectHasTypeAttributes(ObjectId::Enumeration);
}

TEST_F(StandardNamespaceStructure, IdType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::IdType);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::IdType);
  ExpectHasTypeAttributes(ObjectId::IdType);
}

TEST_F(StandardNamespaceStructure, MessageSecurityMode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::MessageSecurityMode);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::MessageSecurityMode);
  ExpectHasTypeAttributes(ObjectId::MessageSecurityMode);
}

TEST_F(StandardNamespaceStructure, NodeClass)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::NodeClass);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::NodeClass);
  ExpectHasTypeAttributes(ObjectId::NodeClass);
}

TEST_F(StandardNamespaceStructure, RedundancySupport)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::RedundancySupport);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::RedundancySupport);
  ExpectHasTypeAttributes(ObjectId::RedundancySupport);
}

TEST_F(StandardNamespaceStructure, SecurityTokenRequestType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SecurityTokenRequestType);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::SecurityTokenRequestType);
  ExpectHasTypeAttributes(ObjectId::SecurityTokenRequestType);
}

TEST_F(StandardNamespaceStructure, ServerState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerState);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::ServerState);
  ExpectHasTypeAttributes(ObjectId::ServerState);
}

TEST_F(StandardNamespaceStructure, ExpandedNodeId)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ExpandedNodeId);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ExpandedNodeId);
  ExpectHasTypeAttributes(ObjectId::ExpandedNodeId);
}

TEST_F(StandardNamespaceStructure, Guid)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Guid);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Guid);
  ExpectHasTypeAttributes(ObjectId::Guid);
}

TEST_F(StandardNamespaceStructure, LocalizedText)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::LocalizedText);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::LocalizedText);
  ExpectHasTypeAttributes(ObjectId::LocalizedText);
}

TEST_F(StandardNamespaceStructure, NodeId)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::NodeId);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::NodeId);
  ExpectHasTypeAttributes(ObjectId::NodeId);
}

TEST_F(StandardNamespaceStructure, Number)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Number);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Double));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Float));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Integer));
  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UInteger));

  ExpectHasBaseAttributes(ObjectId::Number);
  ExpectHasTypeAttributes(ObjectId::Number);
}

TEST_F(StandardNamespaceStructure, Double)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Double);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Duration));

  ExpectHasBaseAttributes(ObjectId::Double);
  ExpectHasTypeAttributes(ObjectId::Double);
}

TEST_F(StandardNamespaceStructure, Duration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Duration);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Duration);
  ExpectHasTypeAttributes(ObjectId::Duration);
}

TEST_F(StandardNamespaceStructure, Float)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Float);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Float);
  ExpectHasTypeAttributes(ObjectId::Float);
}

TEST_F(StandardNamespaceStructure, Integer)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Integer);
  EXPECT_EQ(SizeOf(refs), 5);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Int16));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Int32));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Int64));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SByte));

  ExpectHasBaseAttributes(ObjectId::Integer);
  ExpectHasTypeAttributes(ObjectId::Integer);
}

TEST_F(StandardNamespaceStructure, Int16)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Int16);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Int16);
  ExpectHasTypeAttributes(ObjectId::Int16);
}

TEST_F(StandardNamespaceStructure, Int32)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Int32);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Int32);
  ExpectHasTypeAttributes(ObjectId::Int32);
}

TEST_F(StandardNamespaceStructure, Int64)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Int64);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Int64);
  ExpectHasTypeAttributes(ObjectId::Int64);
}

TEST_F(StandardNamespaceStructure, SByte)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SByte);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SByte);
  ExpectHasTypeAttributes(ObjectId::SByte);
}

TEST_F(StandardNamespaceStructure, UInteger)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::UInteger);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UInt16));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UInt32));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UInt64));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Byte));

  ExpectHasBaseAttributes(ObjectId::UInteger);
  ExpectHasTypeAttributes(ObjectId::UInteger);
}

TEST_F(StandardNamespaceStructure, UInt16)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::UInt16);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::UInt16);
  ExpectHasTypeAttributes(ObjectId::UInt16);
}

TEST_F(StandardNamespaceStructure, UInt32)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::UInt32);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::UInt32);
  ExpectHasTypeAttributes(ObjectId::UInt32);
}

TEST_F(StandardNamespaceStructure, UInt64)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::UInt64);
  EXPECT_EQ(SizeOf(refs), 1);

  ExpectHasBaseAttributes(ObjectId::UInt64);
  ExpectHasTypeAttributes(ObjectId::UInt64);
}

TEST_F(StandardNamespaceStructure, Byte)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Byte);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Byte);
  ExpectHasTypeAttributes(ObjectId::Byte);
}

TEST_F(StandardNamespaceStructure, QualifiedName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::QualifiedName);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::QualifiedName);
  ExpectHasTypeAttributes(ObjectId::QualifiedName);
}

TEST_F(StandardNamespaceStructure, StatusCode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::StatusCode);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::StatusCode);
  ExpectHasTypeAttributes(ObjectId::StatusCode);
}

TEST_F(StandardNamespaceStructure, String)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::String);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::LocaleId));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::NumericRange));

  ExpectHasBaseAttributes(ObjectId::String);
  ExpectHasTypeAttributes(ObjectId::String);
}

TEST_F(StandardNamespaceStructure, LocaleId)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::LocaleId);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::LocaleId);
  ExpectHasTypeAttributes(ObjectId::LocaleId);
}

TEST_F(StandardNamespaceStructure, NumericRange)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::NumericRange);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::NumericRange);
  ExpectHasTypeAttributes(ObjectId::NumericRange);
}

TEST_F(StandardNamespaceStructure, Structure)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Structure);
  EXPECT_EQ(SizeOf(refs), 45);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::AddNodesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::AddReferencesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ApplicationDescription));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Argument));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::BuildInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DeleteNodesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DeleteReferencesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::EUInformation));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ModelChangeStructureDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Range));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SamplingIntervalDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SemanticChangeStructureDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerDiagnosticsSummaryDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerStatusDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServiceCounterDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionSecurityDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SignedSoftwareCertificate));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::StatusResult));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SubscriptionDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UserIdentityToken));

  ExpectHasBaseAttributes(ObjectId::Structure);
  ExpectHasTypeAttributes(ObjectId::Structure);
}

TEST_F(StandardNamespaceStructure, StructureAddNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::AddNodesItem);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::AddNodesItem);
  ExpectHasTypeAttributes(ObjectId::AddNodesItem);
}

TEST_F(StandardNamespaceStructure, StructureAddReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::AddReferencesItem);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::AddReferencesItem);
  ExpectHasTypeAttributes(ObjectId::AddReferencesItem);
}

TEST_F(StandardNamespaceStructure, StructureApplicationDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ApplicationDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ApplicationDescription);
  ExpectHasTypeAttributes(ObjectId::ApplicationDescription);
}

TEST_F(StandardNamespaceStructure, StructureArgument)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Argument);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::Argument);
  ExpectHasTypeAttributes(ObjectId::Argument);
}

TEST_F(StandardNamespaceStructure, StructureBuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfo);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::BuildInfo);
  ExpectHasTypeAttributes(ObjectId::BuildInfo);
}

TEST_F(StandardNamespaceStructure, StructureDeleteNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DeleteNodesItem);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::DeleteNodesItem);
  ExpectHasTypeAttributes(ObjectId::DeleteNodesItem);
}

TEST_F(StandardNamespaceStructure, StructureDeleteReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DeleteReferencesItem);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::DeleteReferencesItem);
  ExpectHasTypeAttributes(ObjectId::DeleteReferencesItem);
}

TEST_F(StandardNamespaceStructure, StructureEUInformation)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::EUInformation);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::EUInformation);
  ExpectHasTypeAttributes(ObjectId::EUInformation);
}

TEST_F(StandardNamespaceStructure, EUModelChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModelChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::ModelChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectId::ModelChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, StructureRange)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Range);
  EXPECT_EQ(SizeOf(refs), 2);

  ExpectHasBaseAttributes(ObjectId::Range);
  ExpectHasTypeAttributes(ObjectId::Range);
}

TEST_F(StandardNamespaceStructure, StructureSamplingIntervalDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SamplingIntervalDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SamplingIntervalDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectId::SamplingIntervalDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSemanticChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SemanticChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SemanticChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectId::SemanticChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, StructureServerDiagnosticsSummaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsSummaryType);
  ExpectHasTypeAttributes(ObjectId::ServerDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, StructureServerStatusDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerStatusDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ServerStatusDataType);
  ExpectHasTypeAttributes(ObjectId::ServerStatusDataType);
}

TEST_F(StandardNamespaceStructure, StructureServiceCounterDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServiceCounterDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ServiceCounterDataType);
  ExpectHasTypeAttributes(ObjectId::ServiceCounterDataType);
}

TEST_F(StandardNamespaceStructure, StructureSessionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SessionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectId::SessionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSessionSecurityDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionSecurityDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SessionSecurityDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectId::SessionSecurityDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSignedSoftwareCertificate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SignedSoftwareCertificate);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SignedSoftwareCertificate);
  ExpectHasTypeAttributes(ObjectId::SignedSoftwareCertificate);
}

TEST_F(StandardNamespaceStructure, StructureStatusResult)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::StatusResult);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::StatusResult);
  ExpectHasTypeAttributes(ObjectId::StatusResult);
}

TEST_F(StandardNamespaceStructure, StructureSubscriptionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SubscriptionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SubscriptionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectId::SubscriptionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureUserIdentifyToken)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::UserIdentityToken);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::AnonymousIdentityToken));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::UserNameIdentityToken));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::X509IdentityToken));

  ExpectHasBaseAttributes(ObjectId::UserIdentityToken);
  ExpectHasTypeAttributes(ObjectId::UserIdentityToken);
}

TEST_F(StandardNamespaceStructure, XmlElement)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::XmlElement);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::XmlElement);
  ExpectHasTypeAttributes(ObjectId::XmlElement);
}

TEST_F(StandardNamespaceStructure, EventTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::EventTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::BaseEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));

  ExpectHasBaseAttributes(ObjectId::EventTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType);
  EXPECT_EQ(SizeOf(refs), 11);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::BaseModelChangeEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SemanticChangeEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SystemEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_EventId));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_EventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_Message));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_ReceiveTime));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_Severity));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_SourceName));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_SourceNode));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::BaseEventType_Time));

  ExpectHasBaseAttributes(ObjectId::BaseEventType);
  ExpectHasTypeAttributes(ObjectId::BaseEventType);
}

TEST_F(StandardNamespaceStructure, BaseModelChangeEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseModelChangeEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::BaseModelChangeEventType);
  ExpectHasTypeAttributes(ObjectId::BaseModelChangeEventType);
}

TEST_F(StandardNamespaceStructure, SemanticChangeEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SemanticChangeEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SemanticChangeEventType);
  ExpectHasTypeAttributes(ObjectId::SemanticChangeEventType);
}

TEST_F(StandardNamespaceStructure, SystemEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SystemEventType);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DeviceFailureEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::RefreshEndEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::RefreshRequiredEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::RefreshStartEventType));

  ExpectHasBaseAttributes(ObjectId::SystemEventType);
  ExpectHasTypeAttributes(ObjectId::SystemEventType);
}

TEST_F(StandardNamespaceStructure, DeviceFailureEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DeviceFailureEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::DeviceFailureEventType);
  ExpectHasTypeAttributes(ObjectId::DeviceFailureEventType);
}

TEST_F(StandardNamespaceStructure, RefreshEndEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::RefreshEndEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::RefreshEndEventType);
  ExpectHasTypeAttributes(ObjectId::RefreshEndEventType);
}

TEST_F(StandardNamespaceStructure, RefreshRequiredEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::RefreshRequiredEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::RefreshRequiredEventType);
  ExpectHasTypeAttributes(ObjectId::RefreshRequiredEventType);
}

TEST_F(StandardNamespaceStructure, RefreshStartEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::RefreshStartEventType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::RefreshStartEventType);
  ExpectHasTypeAttributes(ObjectId::RefreshStartEventType);
}

TEST_F(StandardNamespaceStructure, EventId)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_EventId);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_EventId);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_EventId);
}

TEST_F(StandardNamespaceStructure, EventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_EventType);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_EventType);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_EventType);
}

TEST_F(StandardNamespaceStructure, Message)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_Message);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_Message);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_Message);
}

TEST_F(StandardNamespaceStructure, ReceiveTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_ReceiveTime);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_ReceiveTime);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_ReceiveTime);
}

TEST_F(StandardNamespaceStructure, Severity)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_Severity);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_Severity);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_Severity);
}

TEST_F(StandardNamespaceStructure, SourceName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_SourceName);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_SourceName);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_SourceName);
}

TEST_F(StandardNamespaceStructure, SourceNode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_SourceNode);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_SourceNode);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_SourceNode);
}

TEST_F(StandardNamespaceStructure, Time)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseEventType_Time);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::BaseEventType_Time);
  ExpectHasVariableAttributes(ObjectId::BaseEventType_Time);
}

TEST_F(StandardNamespaceStructure, ObjectTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ObjectTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::BaseObjectType));

  ExpectHasBaseAttributes(ObjectId::ObjectTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseObjectType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseObjectType);
  EXPECT_EQ(SizeOf(refs), 24);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::BaseEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DataTypeEncodingType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DataTypeSystemType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::FolderType));
  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HistoricalEventConfigurationType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ModellingRuleType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerCapabilitiesType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerRedundancyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionDiagnosticsObjectType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionsDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::StateType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::TransitionType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::VendorServerInfoType));

  ExpectHasBaseAttributes(ObjectId::BaseObjectType);
  ExpectHasTypeAttributes(ObjectId::BaseObjectType);
}

TEST_F(StandardNamespaceStructure, DataTypeEncodingType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DataTypeEncodingType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::DataTypeEncodingType);
  ExpectHasTypeAttributes(ObjectId::DataTypeEncodingType);
}

TEST_F(StandardNamespaceStructure, DataTypeSystemType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DataTypeSystemType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::DataTypeSystemType);
  ExpectHasTypeAttributes(ObjectId::DataTypeSystemType);
}

TEST_F(StandardNamespaceStructure, FolderType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::FolderType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::FolderType);
  ExpectHasTypeAttributes(ObjectId::FolderType);
}

//TEST_F(StandardNamespaceStructure, HistoricalEventConfigurationType)
//{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HistoricalEventConfigurationType);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasBaseAttributes(ObjectId::HistoricalEventConfigurationType);
//  ExpectHasTypeAttributes(ObjectId::HistoricalEventConfigurationType);
//}

TEST_F(StandardNamespaceStructure, ModellingRuleType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRuleType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::ModellingRuleType_NamingRule));

  ExpectHasBaseAttributes(ObjectId::ModellingRuleType);
  ExpectHasTypeAttributes(ObjectId::ModellingRuleType);
}

TEST_F(StandardNamespaceStructure, NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ModellingRuleType_NamingRule);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ModellingRuleType_NamingRule);
  ExpectHasVariableAttributes(ObjectId::ModellingRuleType_NamingRule);
}

TEST_F(StandardNamespaceStructure, ServerCapabilitiesType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType);
  EXPECT_EQ(SizeOf(refs), 13);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerCapabilitiesType_LocaleIdArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerCapabilitiesType_MaxBrowseContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerCapabilitiesType_MaxHistoryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerCapabilitiesType_MinSupportedSampleRate));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerCapabilitiesType_ModellingRules));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerCapabilitiesType_ServerProfileArray));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType);
  ExpectHasTypeAttributes(ObjectId::ServerCapabilitiesType);
}

TEST_F(StandardNamespaceStructure, ModellingRules)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType_ModellingRules);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType_ModellingRules);
}

TEST_F(StandardNamespaceStructure, LocaleIdArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType_LocaleIdArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType_LocaleIdArray);
  ExpectHasVariableAttributes(ObjectId::ServerCapabilitiesType_LocaleIdArray);
}

TEST_F(StandardNamespaceStructure, MaxBrowseContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType_MaxBrowseContinuationPoints);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType_MaxBrowseContinuationPoints);
  ExpectHasVariableAttributes(ObjectId::ServerCapabilitiesType_MaxBrowseContinuationPoints);
}

TEST_F(StandardNamespaceStructure, MaxHistoryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType_MaxHistoryContinuationPoints);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType_MaxHistoryContinuationPoints);
  ExpectHasVariableAttributes(ObjectId::ServerCapabilitiesType_MaxHistoryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, MinSupportedSampleRate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType_MinSupportedSampleRate);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType_MinSupportedSampleRate);
  ExpectHasVariableAttributes(ObjectId::ServerCapabilitiesType_MinSupportedSampleRate);
}

TEST_F(StandardNamespaceStructure, ServerProfileArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerCapabilitiesType_ServerProfileArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerCapabilitiesType_ServerProfileArray);
  ExpectHasVariableAttributes(ObjectId::ServerCapabilitiesType_ServerProfileArray);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 5);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerDiagnosticsType_EnabledFlag));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerDiagnosticsType_ServerDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerDiagnosticsType_SubscriptionDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerDiagnosticsType_SessionsDiagnosticsSummary));

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsType);
  ExpectHasTypeAttributes(ObjectId::ServerDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, EnableFlag)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsType_EnabledFlag);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsType_EnabledFlag);
  ExpectHasVariableAttributes(ObjectId::ServerDiagnosticsType_EnabledFlag);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::SamplingIntervalDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectId::ServerDiagnosticsType_SamplingIntervalDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsType_ServerDiagnosticsSummary);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsType_ServerDiagnosticsSummary);
  ExpectHasVariableAttributes(ObjectId::ServerDiagnosticsType_ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsType_SessionsDiagnosticsSummary);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::SessionsDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsType_ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsType_SubscriptionDiagnosticsArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::SubscriptionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsType_SubscriptionDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, ServerRedundancyType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerRedundancyType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::ServerRedundancyType_RedundancySupport));

  ExpectHasBaseAttributes(ObjectId::ServerRedundancyType);
  ExpectHasTypeAttributes(ObjectId::ServerRedundancyType);
}

TEST_F(StandardNamespaceStructure, RedundancySupportTypeRedundancySupport)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerRedundancyType_RedundancySupport);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ServerRedundancyType_RedundancySupport);
  ExpectHasVariableAttributes(ObjectId::ServerRedundancyType_RedundancySupport);
}

TEST_F(StandardNamespaceStructure, ServerType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType);
  EXPECT_EQ(SizeOf(refs), 8);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerType_NamespaceArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerType_ServerArray));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerType_ServerCapabilities));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerType_ServerDiagnostics));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerType_ServerRedundancy));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerType_ServerStatus));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty,  ObjectId::ServerType_ServiceLevel));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerType_VendorServerInfo));

  ExpectHasBaseAttributes(ObjectId::ServerType);
  ExpectHasTypeAttributes(ObjectId::ServerType);
}

TEST_F(StandardNamespaceStructure, NamespaceArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_NamespaceArray);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition,  ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ServerType_NamespaceArray);
  ExpectHasVariableAttributes(ObjectId::ServerType_NamespaceArray);
}

TEST_F(StandardNamespaceStructure, ServerArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_ServerArray);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition,  ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::ServerType_ServerArray);
  ExpectHasVariableAttributes(ObjectId::ServerType_ServerArray);
}

TEST_F(StandardNamespaceStructure, ServerCapabilities)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_ServerCapabilities);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerCapabilitiesType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule,  ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerType_ServerCapabilities);
  ExpectHasObjectAttributes(ObjectId::ServerType_ServerCapabilities);
}

TEST_F(StandardNamespaceStructure, ServerDiagnostics)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_ServerDiagnostics);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule,  ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerType_ServerDiagnostics);
  ExpectHasObjectAttributes(ObjectId::ServerType_ServerDiagnostics);
}

TEST_F(StandardNamespaceStructure, ServerRedundancy)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_ServerRedundancy);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerRedundancyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule,  ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerType_ServerRedundancy);
  ExpectHasObjectAttributes(ObjectId::ServerType_ServerRedundancy);
}

TEST_F(StandardNamespaceStructure, ServerStatus)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_ServerStatus);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::ServerStatusType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerType_ServerStatus);
  ExpectHasVariableAttributes(ObjectId::ServerType_ServerStatus);
}

TEST_F(StandardNamespaceStructure, BuildInfoType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent,      ObjectId::BuildInfoType_BuildDate));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent,      ObjectId::BuildInfoType_BuildNumber));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent,      ObjectId::BuildInfoType_ManufacturerName));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent,      ObjectId::BuildInfoType_ProductName));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent,      ObjectId::BuildInfoType_ProductUri));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent,      ObjectId::BuildInfoType_SoftwareVersion));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType);
  ExpectHasVariableTypeAttributes(ObjectId::BuildInfoType);
}

TEST_F(StandardNamespaceStructure, BuildDate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType_BuildDate);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType_BuildDate);
  ExpectHasVariableAttributes(ObjectId::BuildInfoType_BuildDate);
}

TEST_F(StandardNamespaceStructure, BuildNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType_BuildNumber);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType_BuildNumber);
  ExpectHasVariableAttributes(ObjectId::BuildInfoType_BuildNumber);
}

TEST_F(StandardNamespaceStructure, ManufacturerName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType_ManufacturerName);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType_ManufacturerName);
  ExpectHasVariableAttributes(ObjectId::BuildInfoType_ManufacturerName);
}

TEST_F(StandardNamespaceStructure, ProductName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType_ProductName);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType_ProductName);
  ExpectHasVariableAttributes(ObjectId::BuildInfoType_ProductName);
}

TEST_F(StandardNamespaceStructure, ProductUri)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType_ProductUri);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType_ProductUri);
  ExpectHasVariableAttributes(ObjectId::BuildInfoType_ProductUri);
}

TEST_F(StandardNamespaceStructure, SoftwareVersion)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BuildInfoType_SoftwareVersion);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::BuildInfoType_SoftwareVersion);
  ExpectHasVariableAttributes(ObjectId::BuildInfoType_SoftwareVersion);
}

TEST_F(StandardNamespaceStructure, CurrentTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerStatusType_CurrentTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::ServerStatusType_CurrentTime);
  ExpectHasVariableAttributes(ObjectId::ServerStatusType_CurrentTime);
}

TEST_F(StandardNamespaceStructure, StartTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerStatusType_StartTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::ServerStatusType_StartTime);
  ExpectHasVariableAttributes(ObjectId::ServerStatusType_StartTime);
}

TEST_F(StandardNamespaceStructure, State)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerStatusType_State);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectId::ServerStatusType_State);
  ExpectHasVariableAttributes(ObjectId::ServerStatusType_State);
}

TEST_F(StandardNamespaceStructure, ServiceLevel)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_ServiceLevel);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerType_ServiceLevel);
  ExpectHasVariableAttributes(ObjectId::ServerType_ServiceLevel);
}

TEST_F(StandardNamespaceStructure, VendorServerInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerType_VendorServerInfo);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::VendorServerInfoType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasModellingRule, ObjectId::ModellingRule_Mandatory));

  ExpectHasBaseAttributes(ObjectId::ServerType_VendorServerInfo);
  ExpectHasObjectAttributes(ObjectId::ServerType_VendorServerInfo);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsObjectType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionDiagnosticsObjectType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SessionDiagnosticsObjectType);
  ExpectHasTypeAttributes(ObjectId::SessionDiagnosticsObjectType);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsSummaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionsDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SessionsDiagnosticsSummaryType);
  ExpectHasTypeAttributes(ObjectId::SessionsDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, StateType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::StateType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasProperty, ObjectId::StateType_StateNumber));

  ExpectHasBaseAttributes(ObjectId::StateType);
  ExpectHasTypeAttributes(ObjectId::StateType);
}

TEST_F(StandardNamespaceStructure, StateNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::StateType_StateNumber);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::StateType_StateNumber);
  ExpectHasVariableAttributes(ObjectId::StateType_StateNumber);
}

TEST_F(StandardNamespaceStructure, TransitionType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::TransitionType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::TransitionType);
  ExpectHasTypeAttributes(ObjectId::TransitionType);
}

TEST_F(StandardNamespaceStructure, VendorServerInfoType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::VendorServerInfoType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::VendorServerInfoType);
  ExpectHasTypeAttributes(ObjectId::VendorServerInfoType);
}

TEST_F(StandardNamespaceStructure, ReferenceTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ReferenceTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::References));

  ExpectHasBaseAttributes(ObjectId::ReferenceTypesFolder);
}

TEST_F(StandardNamespaceStructure, References)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::References);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HierarchicalReferences));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::NonHierarchicalReferences));

  ExpectHasReferenceTypeAttributes(ObjectId::References);
  EXPECT_FALSE(HasAttribute(ObjectId::HierarchicalReferences, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HierarchicalReferences)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HierarchicalReferences);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasChild));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasEventSource));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Organizes));

  ExpectHasReferenceTypeAttributes(ObjectId::HierarchicalReferences);
  EXPECT_FALSE(HasAttribute(ObjectId::HierarchicalReferences, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasChild)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasChild);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasSubtype));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::Aggregates));

  ExpectHasReferenceTypeAttributes(ObjectId::HasChild);
  EXPECT_FALSE(HasAttribute(ObjectId::HasChild, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, Aggregates)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Aggregates);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasHistoricalConfiguration));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasComponent));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasProperty));

  ExpectHasReferenceTypeAttributes(ObjectId::Aggregates);
  EXPECT_FALSE(HasAttribute(ObjectId::Aggregates, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasComponent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasComponent);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasOrderedComponent));

  ExpectHasReferenceTypeAttributes(ObjectId::HasComponent);
  EXPECT_TRUE(HasAttribute(ObjectId::HasComponent, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasOrderedComponent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasOrderedComponent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasOrderedComponent);
  EXPECT_TRUE(HasAttribute(ObjectId::HasOrderedComponent, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasHistoricalConfiguration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasHistoricalConfiguration);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasHistoricalConfiguration);
  EXPECT_TRUE(HasAttribute(ObjectId::HasHistoricalConfiguration, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasProperty)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasProperty);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasProperty);
  EXPECT_TRUE(HasAttribute(ObjectId::HasProperty, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasSubtype)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasSubtype);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasSubtype);
  EXPECT_TRUE(HasAttribute(ObjectId::HasSubtype, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasEventSource)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasEventSource);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasNotifier));

  ExpectHasReferenceTypeAttributes(ObjectId::HasEventSource);
  EXPECT_TRUE(HasAttribute(ObjectId::HasEventSource, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasNotifier)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasNotifier);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasNotifier);
  EXPECT_TRUE(HasAttribute(ObjectId::HasNotifier, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, Organizes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::Organizes);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::Organizes);
  EXPECT_TRUE(HasAttribute(ObjectId::Organizes, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, NonHierarchicalReferences)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::NonHierarchicalReferences);
  EXPECT_EQ(SizeOf(refs), 10 - 1); // XXX
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::FromState));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::GeneratesEvent));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasCause));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasDescription));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasEffect));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasEncoding));
  //EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasModelParent));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasModellingRule));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::HasTypeDefinition));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ToState));

  ExpectHasReferenceTypeAttributes(ObjectId::NonHierarchicalReferences);
  EXPECT_FALSE(HasAttribute(ObjectId::NonHierarchicalReferences, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, FromState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::FromState);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::FromState);
  EXPECT_TRUE(HasAttribute(ObjectId::FromState, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, GeneratesEvent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::GeneratesEvent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::GeneratesEvent);
  EXPECT_TRUE(HasAttribute(ObjectId::GeneratesEvent, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasCause)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasCause);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasCause);
  EXPECT_TRUE(HasAttribute(ObjectId::HasCause, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasDescription);
  EXPECT_TRUE(HasAttribute(ObjectId::HasDescription, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasEffect)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasEffect);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasEffect);
  EXPECT_TRUE(HasAttribute(ObjectId::HasEffect, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasEncoding)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasEncoding);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasEncoding);
  EXPECT_TRUE(HasAttribute(ObjectId::HasEncoding, AttributeId::InverseName));
}

//TEST_F(StandardNamespaceStructure, HasModelParent)
//{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasModelParent);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasReferenceTypeAttributes(ObjectId::HasModelParent);
//  EXPECT_TRUE(HasAttribute(ObjectId::HasModelParent, AttributeId::InverseName));
//}

TEST_F(StandardNamespaceStructure, HasModellingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasModellingRule);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasModellingRule);
  EXPECT_TRUE(HasAttribute(ObjectId::HasModellingRule, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, HasTypeDefinition)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::HasTypeDefinition);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::HasTypeDefinition);
  EXPECT_TRUE(HasAttribute(ObjectId::HasTypeDefinition, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, ToState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ToState);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectId::ToState);
  EXPECT_TRUE(HasAttribute(ObjectId::ToState, AttributeId::InverseName));
}

TEST_F(StandardNamespaceStructure, VariableTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::VariableTypesFolder);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasTypeDefinition, ObjectId::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::Organizes, ObjectId::BaseVariableType));

  ExpectHasBaseAttributes(ObjectId::VariableTypesFolder);
}

TEST_F(StandardNamespaceStructure, BaseVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseVariableType);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::BaseDataVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::PropertyType));

  ExpectHasBaseAttributes(ObjectId::BaseVariableType);
  ExpectHasVariableTypeAttributes(ObjectId::BaseVariableType);
}

TEST_F(StandardNamespaceStructure, BaseDataVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::BaseDataVariableType);
  EXPECT_EQ(SizeOf(refs), 14);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::BuildInfoType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DataTypeDescriptionType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::DataTypeDictionaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SamplingIntervalDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SamplingIntervalDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerStatusType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::ServerVendorCapabilityType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionDiagnosticsVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionSecurityDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SessionSecurityDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SubscriptionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasSubtype, ObjectId::SubscriptionDiagnosticsType));

  ExpectHasBaseAttributes(ObjectId::BaseDataVariableType);
  ExpectHasVariableTypeAttributes(ObjectId::BaseDataVariableType);
}

TEST_F(StandardNamespaceStructure, DataTypeDescriptionType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DataTypeDescriptionType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::DataTypeDescriptionType);
  ExpectHasVariableTypeAttributes(ObjectId::DataTypeDescriptionType);
}

TEST_F(StandardNamespaceStructure, DataTypeDictionaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::DataTypeDictionaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::DataTypeDictionaryType);
  ExpectHasVariableTypeAttributes(ObjectId::DataTypeDictionaryType);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SamplingIntervalDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SamplingIntervalDiagnosticsArrayType);
  ExpectHasVariableTypeAttributes(ObjectId::SamplingIntervalDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SamplingIntervalDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SamplingIntervalDiagnosticsType);
  ExpectHasVariableTypeAttributes(ObjectId::SamplingIntervalDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummaryDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsSummaryDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsSummaryDataType);
  ExpectHasDataTypeAttributes(ObjectId::ServerDiagnosticsSummaryDataType);
}

TEST_F(StandardNamespaceStructure, ServerStatusType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerStatusType);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerStatusType_StartTime));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerStatusType_CurrentTime));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerStatusType_State));
  EXPECT_TRUE(HasReference(refs, ReferenceId::HasComponent, ObjectId::ServerStatusType_BuildInfo));

  ExpectHasBaseAttributes(ObjectId::ServerStatusType);
  ExpectHasVariableTypeAttributes(ObjectId::ServerStatusType);
}

TEST_F(StandardNamespaceStructure, ServerVendorCapabilityType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerVendorCapabilityType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::ServerVendorCapabilityType);
  ExpectHasVariableTypeAttributes(ObjectId::ServerVendorCapabilityType);
}

TEST_F(StandardNamespaceStructure, DISABLED_SessionsDiagnosticsArrayType)
{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionsDiagnosticsDataType);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasBaseAttributes(ObjectId::SessionsDiagnosticsArrayType);
//  ExpectHasVariableTypeAttributes(ObjectId::SessionsDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, DISABLED_ServerDiagnosticsVariableType)
{
//  const std::vector<ReferenceDescription> refs = Browse(ObjectId::ServerDiagnosticsVariableType);
//  EXPECT_EQ(SizeOf(refs), 0);
//
//  ExpectHasBaseAttributes(ObjectId::ServerDiagnosticsVariableType);
//  ExpectHasVariableTypeAttributes(ObjectId::ServerDiagnosticsVariableType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionSecurityDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SessionSecurityDiagnosticsArrayType);
  ExpectHasVariableTypeAttributes(ObjectId::SessionSecurityDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SessionSecurityDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SessionSecurityDiagnosticsType);
  ExpectHasVariableTypeAttributes(ObjectId::SessionSecurityDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SubscriptionDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SubscriptionDiagnosticsArrayType);
  ExpectHasVariableTypeAttributes(ObjectId::SubscriptionDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::SubscriptionDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::SubscriptionDiagnosticsType);
  ExpectHasVariableTypeAttributes(ObjectId::SubscriptionDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, PropertyType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectId::PropertyType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectId::PropertyType);
  ExpectHasVariableTypeAttributes(ObjectId::PropertyType);
}
