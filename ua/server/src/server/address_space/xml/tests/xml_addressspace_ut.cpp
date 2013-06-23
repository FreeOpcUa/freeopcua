/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/server/address_space/xml/xml_addressspace_loader.h>

#include <opc/ua/object_ids.h>
#include <opc/ua/attribute_ids.h>
#include <opc/ua/status_codes.h>

#include <src/server/address_space/address_space_internal.h>

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
    NameSpace = OpcUa::Internal::CreateAddressSpaceMultiplexor();
  }

  virtual void TearDown()
  {
    NameSpace.reset();
  }

protected:
  std::vector<ReferenceDescription> Browse(const NodeID& id) const
  {
    OpcUa::Remote::BrowseParameters params;
    params.Description.NodeToBrowse = id;
    return NameSpace->Browse(params);
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
/*
  void ExpectHasBaseAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::NODE_ID, id));
    EXPECT_TRUE(HasAttribute(id, AttributeID::NODE_CLASS, NodeClass::Object));
    EXPECT_TRUE(HasAttribute(id, AttributeID::BROWSE_NAME, std::string("Root")));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DISPLAY_NAME, LocalizedText("Root")));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DESCRIPTION, std::string("Root")));
    EXPECT_TRUE(HasAttribute(id, AttributeID::WRITE_MASK, 0));
    EXPECT_TRUE(HasAttribute(id, AttributeID::USER_WRITE_MASK, 0));
  }

  void ExpectHasTypeAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::IS_ABSTRACT, ));
  }

  void ExpectHasVariableAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::VALUE));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DATA_TYPE));
    EXPECT_TRUE(HasAttribute(id, AttributeID::VALUE_RANK));
    EXPECT_TRUE(HasAttribute(id, AttributeID::ARRAY_DIMENSIONS));
    EXPECT_TRUE(HasAttribute(id, AttributeID::ACCESS_LEVEL));
    EXPECT_TRUE(HasAttribute(id, AttributeID::USER_ACCESS_LEVEL));
    EXPECT_TRUE(HasAttribute(id, AttributeID::MINIMUM_SAMPLING_INTERVAL));
    EXPECT_TRUE(HasAttribute(id, AttributeID::HISTORIZING));
  }

  void ExpectHasReferenceTypeAttributes(ObjectID id)
  {
    ExpectHasBaseAttributes(id);
    ExpectHasTypeAttributes(id);
    EXPECT_TRUE(HasAttribute(id, AttributeID::SYMMETRIC));
  }
*/

  std::string ConfigPath(const char* name)
  {
    return std::string("tests/") + name;
  }

protected:
  Internal::AddressSpaceMultiplexor::UniquePtr NameSpace;
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

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::NODE_CLASS, (uint32_t)NodeClass::Object));
}

TEST_F(XmlAddressSpace, BaseNodeHasBrowseName)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::BROWSE_NAME, std::string("Root")));
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

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::EVENT_NOTIFIER, std::string("notifier")));
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

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::DATA_TYPE, (uint32_t)VariantType::UINT32));
}

TEST_F(XmlAddressSpace, BaseNodeHasValueRank)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::VALUE_RANK, (uint32_t)0));
}

TEST_F(XmlAddressSpace, BaseNodeHasArrayDimensions)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::ARRAY_DIMENSIONS, std::string("0")));
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

  ASSERT_TRUE(HasAttribute(NumericNodeID(84, 10), AttributeID::HISTORIZING, false));
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
