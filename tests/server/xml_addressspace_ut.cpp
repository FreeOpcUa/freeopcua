/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/server/xml_address_space_loader.h>

#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/status_codes.h>
#include <opc/ua/server/addons/address_space.h>
#include <opc/ua/server/address_space.h>

#include <functional>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// TODO Add tests for several appearing nodes in xml.
// TODO Add tests for all node classes and for invalid classe names.

using namespace testing;
using namespace OpcUa;
using namespace OpcUa::Internal;

class XmlAddressSpace : public testing::Test
{
protected:
  virtual void SetUp()
  {
    const bool debug = false;
    NameSpace = OpcUa::Server::CreateAddressSpace(debug);
  }

  virtual void TearDown()
  {
    NameSpace.reset();
  }

protected:
  std::vector<ReferenceDescription> Browse(const NodeId & id) const
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = id;
    OpcUa::NodesQuery query;
    query.NodesToBrowse.push_back(description);
    return NameSpace->Browse(query);
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

  template <typename T>
  bool HasAttribute(const NodeId & object, OpcUa::AttributeId attribute, T value)
  {
    ReadParameters params;
    ReadValueId id;
    id.Node = object;
    id.Attribute = attribute;
    params.AttributesToRead.push_back(id);
    std::vector<DataValue> values = NameSpace->Read(params);
    EXPECT_EQ(values.size(), 1);

    if (values.size() != 1)
      {
        return false;
      }

    const DataValue var = values.front();
    EXPECT_EQ(var.Status, StatusCode::Good);
    return var == value;
  }

  std::string ConfigPath(const char * name)
  {
    return std::string("tests/server/") + name;
  }

protected:
  Server::AddressSpace::UniquePtr NameSpace;
};

TEST_F(XmlAddressSpace, ExceptionIfCannotLoadDocument)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_THROW(loader.Load(ConfigPath("not_found.xml")), std::exception);
}

TEST_F(XmlAddressSpace, ExceptionIfRootNodeInvalid)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_THROW(loader.Load(ConfigPath("invalid_root.xml")), std::exception);
}

TEST_F(XmlAddressSpace, ExceptionIfNodeVersionOfAddressSpace)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_THROW(loader.Load(ConfigPath("no_version.xml")), std::exception);
}

TEST_F(XmlAddressSpace, ExceptionIfUnknownVersion)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_THROW(loader.Load(ConfigPath("invalid_version.xml")), std::exception);
}

TEST_F(XmlAddressSpace, NoExceptionIfDocumentValid)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("empty.xml")));
}

TEST_F(XmlAddressSpace, BaseNodeHasId)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::NodeId, NumericNodeId((uint32_t)ObjectId::RootFolder, 10)));
}

TEST_F(XmlAddressSpace, BaseNodeHasClass)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::NodeClass, (int32_t)NodeClass::Object));
}

TEST_F(XmlAddressSpace, BaseNodeHasBrowseName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::BrowseName, QualifiedName("Root")));
}

TEST_F(XmlAddressSpace, BaseNodeHasDiaplayName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::DisplayName, LocalizedText("Root")));
}

TEST_F(XmlAddressSpace, BaseNodeHasDescription)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::Description, std::string("Root")));
}

TEST_F(XmlAddressSpace, BaseNodeHasWriteMask)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::WriteMask, (uint32_t)10));
}

TEST_F(XmlAddressSpace, BaseNodeHasUserWriteMask)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::UserWriteMask, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasIsAbstract)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::IsAbstract, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasSymmetric)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::Symmetric, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasInverseName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::InverseName, std::string("inverse_name")));
}

TEST_F(XmlAddressSpace, BaseNodeHasContainsNoLoops)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::ContainsNoLoops, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasEventNotifier)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::EventNotifier, std::vector<uint8_t> {0}));
}

TEST_F(XmlAddressSpace, BaseNodeHasValue)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::Value, (uint32_t)55));
}

TEST_F(XmlAddressSpace, BaseNodeHasDataType)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::DataType, NodeId(ObjectId::UInt32)));
}

TEST_F(XmlAddressSpace, BaseNodeHasValueRank)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::ValueRank, (int32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasArrayDimensions)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::ArrayDimensions, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasAccessLevel)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::AccessLevel, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasUserAccessLevel)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::UserAccessLevel, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasMinimumSamplingInterval)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::MinimumSamplingInterval, (uint32_t)100));
}

TEST_F(XmlAddressSpace, BaseNodeHasHistorizing)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::Historizing, true));
}

TEST_F(XmlAddressSpace, BaseNodeHasExecutable)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::Executable, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasUserExecutable)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeId(84, 10), AttributeId::UserExecutable, false));
}

TEST_F(XmlAddressSpace, InternalReference)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("predefined_references.xml")));

  const NodeId targetNode = NumericNodeId(100, 10);
  std::vector<ReferenceDescription> references = Browse(NumericNodeId(99, 10));
  ASSERT_TRUE(HasReference(references, ReferenceId::References, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::NonHierarchicalReferences, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HierarchicalReferences, targetNode));

  ASSERT_TRUE(HasReference(references, ReferenceId::References, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::NonHierarchicalReferences, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HierarchicalReferences, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasChild, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::Organizes, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasEventSource, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasModellingRule, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasEncoding, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasDescription, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasTypeDefinition, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::GeneratesEvent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::Aggregates, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasSubtype, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasProperty, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasComponent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasNotifier, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasOrderedComponent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasModelParent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::FromState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::ToState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasCause, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasEffect, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasHistoricalConfiguration, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasHistoricalEventConfiguration, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasSubStateMachine, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasEventHistory, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::AlwaysGeneratesEvent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasTrueSubState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasFalseSubState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceId::HasCondition, targetNode));
  ASSERT_FALSE(HasReference(references, ReferenceId::Unknown, targetNode));
}

TEST_F(XmlAddressSpace, DISABLED_ExternalReference)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("predefined_references.xml")));

  const NodeId targetNode = NumericNodeId(99, 10);
  std::vector<ReferenceDescription> references = Browse(NumericNodeId(84));
  ASSERT_TRUE(HasReference(references, ReferenceId::Organizes, targetNode));
}
