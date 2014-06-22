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
    NameSpace = OpcUa::UaServer::CreateAddressSpace();
  }

  virtual void TearDown()
  {
    NameSpace.reset();
  }

protected:
  std::vector<ReferenceDescription> Browse(const NodeID& id) const
  {
    OpcUa::BrowseDescription description;
    description.NodeToBrowse = id;
    OpcUa::NodesQuery query;
    query.NodesToBrowse.push_back(description);
    return NameSpace->Browse(query);
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

  template <typename T>
  bool HasAttribute(const NodeID& object, OpcUa::AttributeID attribute, T value)
  {
    ReadParameters params;
    AttributeValueID id;
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

  std::string ConfigPath(const char* name)
  {
    return std::string("tests/server/") + name;
  }

protected:
  UaServer::AddressSpace::UniquePtr NameSpace;
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

TEST_F(XmlAddressSpace, BaseNodeHasID)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::NODE_ID, NumericNodeID((uint32_t)ObjectID::RootFolder, 10)));
}

TEST_F(XmlAddressSpace, BaseNodeHasClass)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::NODE_CLASS, (int32_t)NodeClass::Object));
}

TEST_F(XmlAddressSpace, BaseNodeHasBrowseName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::BROWSE_NAME, QualifiedName("Root")));
}

TEST_F(XmlAddressSpace, BaseNodeHasDiaplayName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::DISPLAY_NAME, LocalizedText("Root")));
}

TEST_F(XmlAddressSpace, BaseNodeHasDescription)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::DESCRIPTION, std::string("Root")));
}

TEST_F(XmlAddressSpace, BaseNodeHasWriteMask)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::WRITE_MASK, (uint32_t)10));
}

TEST_F(XmlAddressSpace, BaseNodeHasUserWriteMask)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::USER_WRITE_MASK, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasIsAbstract)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::IS_ABSTRACT, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasSymmetric)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::SYMMETRIC, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasInverseName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::INVERSE_NAME, std::string("inverse_name")));
}

TEST_F(XmlAddressSpace, BaseNodeHasContainsNoLoops)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::CONTAINS_NO_LOOPS, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasEventNotifier)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::EVENT_NOTIFIER, std::vector<uint8_t>{0}));
}

TEST_F(XmlAddressSpace, BaseNodeHasValue)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::VALUE, (uint32_t)55));
}

TEST_F(XmlAddressSpace, BaseNodeHasDataType)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::DATA_TYPE, NodeID(ObjectID::UInt32)));
}

TEST_F(XmlAddressSpace, BaseNodeHasValueRank)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::VALUE_RANK, (int32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasArrayDimensions)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::ARRAY_DIMENSIONS, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasAccessLevel)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::ACCESS_LEVEL, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasUserAccessLevel)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::USER_ACCESS_LEVEL, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasMinimumSamplingInterval)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::MINIMUM_SAMPLING_INTERVAL, (uint32_t)100));
}

TEST_F(XmlAddressSpace, BaseNodeHasHistorizing)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::HISTORIZING, true));
}

TEST_F(XmlAddressSpace, BaseNodeHasExecutable)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::EXECUTABLE, false));
}

TEST_F(XmlAddressSpace, BaseNodeHasUserExecutable)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::USER_EXECUTABLE, false));
}

TEST_F(XmlAddressSpace, InternalReference)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("predefined_references.xml")));

  const NodeID targetNode = NumericNodeID(100, 10);
  std::vector<ReferenceDescription> references = Browse(NumericNodeID(99, 10));
  ASSERT_TRUE(HasReference(references, ReferenceID::References, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::NonHierarchicalReferences, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HierarchicalReferences, targetNode));

  ASSERT_TRUE(HasReference(references, ReferenceID::References, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::NonHierarchicalReferences, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HierarchicalReferences, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasChild, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::Organizes, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasEventSource, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasModellingRule, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasEncoding, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasDescription, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasTypeDefinition, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::GeneratesEvent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::Aggregates, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasSubtype, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasProperty, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasComponent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasNotifier, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasOrderedComponent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasModelParent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::FromState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::ToState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasCause, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasEffect, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasHistoricalConfiguration, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasHistoricalEventConfiguration, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasSubStateMachine, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasEventHistory, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::AlwaysGeneratesEvent, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasTrueSubState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasFalseSubState, targetNode));
  ASSERT_TRUE(HasReference(references, ReferenceID::HasCondition, targetNode));
  ASSERT_FALSE(HasReference(references, ReferenceID::Unknown, targetNode));
}

TEST_F(XmlAddressSpace, DISABLED_ExternalReference)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("predefined_references.xml")));

  const NodeID targetNode = NumericNodeID(99, 10);
  std::vector<ReferenceDescription> references = Browse(NumericNodeID(84));
  ASSERT_TRUE(HasReference(references, ReferenceID::Organizes, targetNode));
}
