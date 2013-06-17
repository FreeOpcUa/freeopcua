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
    EXPECT_TRUE(HasAttribute(id, AttributeID::NODE_ID));
    EXPECT_TRUE(HasAttribute(id, AttributeID::NODE_CLASS));
    EXPECT_TRUE(HasAttribute(id, AttributeID::BROWSE_NAME));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DISPLAY_NAME));
    EXPECT_TRUE(HasAttribute(id, AttributeID::DESCRIPTION));
    EXPECT_TRUE(HasAttribute(id, AttributeID::WRITE_MASK));
    EXPECT_TRUE(HasAttribute(id, AttributeID::USER_WRITE_MASK));
  }

  void ExpectHasTypeAttributes(ObjectID id)
  {
    EXPECT_TRUE(HasAttribute(id, AttributeID::IS_ABSTRACT));
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

TEST_F(XmlAddressSpace, NodeWithBaseAttributes)
{
  XmlAddressSpaceLoader loader(*NameSpace);
  ASSERT_NO_THROW(loader.Load(ConfigPath("base_node.xml")));

  ASSERT_TRUE(HasAttribute(ObjectID::RootFolder, AttributeID::NODE_ID));
  ASSERT_TRUE(HasAttribute(ObjectID::RootFolder, AttributeID::NODE_CLASS));
  ASSERT_TRUE(HasAttribute(ObjectID::RootFolder, AttributeID::BROWSE_NAME));
}
