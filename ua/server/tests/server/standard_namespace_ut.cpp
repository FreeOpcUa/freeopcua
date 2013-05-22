/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <src/server/standard_namespace.h>

#include <functional>

#include "common.h"

using namespace testing;
using namespace OpcUa;

class StandardNamespaceTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    NameSpace = CreateStandardNamespace();
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
    for (auto ref : refs)
    {
      if (ref.TargetNodeID == targetNode && ref.ReferenceTypeID == referenceID)
      {
        return true;
      }
    }
    return false;
  }

  bool HasAttribute(ObjectID object, AttributeID attribute)
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

protected:
  std::unique_ptr<OpcUa::StandardNamespace> NameSpace;
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

TEST(StandardNamespace, CanBeCreated)
{
  std::unique_ptr<Remote::ViewServices> ns = CreateStandardNamespace();
  ASSERT_TRUE(static_cast<bool>(ns));
}

TEST_F(StandardNamespaceTest, CanBrowseRootFolder_By_Organizes_RefType)
{
  OpcUa::Remote::BrowseParameters params;
  params.Description.NodeToBrowse = ObjectID::RootFolder;
  params.Description.Direction = BrowseDirection::Forward;
  params.Description.ReferenceTypeID = ReferenceID::Organizes;
  params.Description.IncludeSubtypes = true;
  params.Description.NodeClasses = NODE_CLASS_OBJECT;
  params.Description.ResultMask = REFERENCE_ALL;
  std::vector<ReferenceDescription> referencies = NameSpace->Browse(params);
  ASSERT_EQ(referencies.size(), 3);
}

TEST_F(StandardNamespaceTest, CanBrowseRootFolder_By_HierarchicalReferencies_Subtypes)
{
  std::unique_ptr<Remote::ViewServices> ns = CreateStandardNamespace();
  OpcUa::Remote::BrowseParameters params;
  params.Description.NodeToBrowse = ObjectID::RootFolder;
  params.Description.Direction = BrowseDirection::Forward;
  params.Description.ReferenceTypeID = ReferenceID::HierarchicalReferences;
  params.Description.IncludeSubtypes = true;
  params.Description.NodeClasses = NODE_CLASS_OBJECT;
  params.Description.ResultMask = REFERENCE_ALL;
  std::vector<ReferenceDescription> referencies = NameSpace->Browse(params);
  ASSERT_EQ(referencies.size(), 3);
}

TEST_F(StandardNamespaceTest, CheckRoot)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RootFolder);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ObjectsFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::TypesFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ViewsFolder));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));

  ExpectHasBaseAttributes(ObjectID::RootFolder);
}

TEST_F(StandardNamespaceTest, CheckObjects)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ObjectsFolder);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ObjectsFolder);
}

TEST_F(StandardNamespaceTest, CheckTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::TypesFolder);
  EXPECT_EQ(SizeOf(refs), 6);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::DataTypes));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::EventTypes));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ObjectTypes));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::ReferenceTypes));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::VariableTypes));

  ExpectHasBaseAttributes(ObjectID::TypesFolder);
}

TEST_F(StandardNamespaceTest, DataTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypes);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseDataType));

  ExpectHasBaseAttributes(ObjectID::DataTypes);
}

TEST_F(StandardNamespaceTest, BaseDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseDataType);
  EXPECT_EQ(SizeOf(refs), 16);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Boolean));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ByteString));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataValue));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DateTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DiagnosticInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Enumeration));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ExpandedNodeID));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Guid));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::LocalizedText));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NodeID));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Number));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::QualifiedName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StatusCode));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::String));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Structure));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::XmlElement));

  ExpectHasBaseAttributes(ObjectID::BaseDataType);
  ExpectHasTypeAttributes(ObjectID::BaseDataType);
}

TEST_F(StandardNamespaceTest, Boolean)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Boolean);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Boolean);
  ExpectHasTypeAttributes(ObjectID::Boolean);
}

TEST_F(StandardNamespaceTest, ByteString)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ByteString);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Image));

  ExpectHasBaseAttributes(ObjectID::ByteString);
  ExpectHasTypeAttributes(ObjectID::ByteString);
}

TEST_F(StandardNamespaceTest, Image)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Image);
  EXPECT_EQ(SizeOf(refs), 4);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageBmp));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageGif));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageJpg));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImagePng));

  ExpectHasBaseAttributes(ObjectID::Image);
  ExpectHasTypeAttributes(ObjectID::Image);
}

TEST_F(StandardNamespaceTest, ImageBmp)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageBmp);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageBmp);
  ExpectHasTypeAttributes(ObjectID::ImageBmp);
}

TEST_F(StandardNamespaceTest, ImageGif)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageGif);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageGif);
  ExpectHasTypeAttributes(ObjectID::ImageGif);
}

TEST_F(StandardNamespaceTest, ImageJpg)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageJpg);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageJpg);
  ExpectHasTypeAttributes(ObjectID::ImageJpg);
}

TEST_F(StandardNamespaceTest, ImagePng)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImagePng);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImagePng);
  ExpectHasTypeAttributes(ObjectID::ImagePng);
}

TEST_F(StandardNamespaceTest, DateTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DateTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UtcTime));

  ExpectHasBaseAttributes(ObjectID::DateTime);
  ExpectHasTypeAttributes(ObjectID::DateTime);
}

TEST_F(StandardNamespaceTest, UtcTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UtcTime);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::UtcTime);
  ExpectHasTypeAttributes(ObjectID::UtcTime);
}

TEST_F(StandardNamespaceTest, DiagnosticInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DiagnosticInfo);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DiagnosticInfo);
  ExpectHasTypeAttributes(ObjectID::DiagnosticInfo);
}

TEST_F(StandardNamespaceTest, Enumeration)
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

TEST_F(StandardNamespaceTest, IdType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::IdType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::IdTypeEnumStrings));

  ExpectHasBaseAttributes(ObjectID::IdType);
  ExpectHasTypeAttributes(ObjectID::IdType);
}

TEST_F(StandardNamespaceTest, IdTypeEnuStrings)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::IdTypeEnumStrings);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::IdTypeEnumStrings);
  ExpectHasVariableAttributes(ObjectID::IdTypeEnumStrings);
}

TEST_F(StandardNamespaceTest, MessageSecurityMode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MessageSecurityMode);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::MessageSecurityModeEnumStrings));

  ExpectHasBaseAttributes(ObjectID::MessageSecurityMode);
  ExpectHasTypeAttributes(ObjectID::MessageSecurityMode);
}

TEST_F(StandardNamespaceTest, MessageSecurityModeEnumStrings)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MessageSecurityModeEnumStrings);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::IdTypeEnumStrings);
  ExpectHasVariableAttributes(ObjectID::IdTypeEnumStrings);
}

TEST_F(StandardNamespaceTest, NodeClass)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NodeClass);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::NodeClass);
  ExpectHasTypeAttributes(ObjectID::NodeClass);
}

TEST_F(StandardNamespaceTest, RedundancySupport)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RedundancySupport);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::RedundancySupportEnumStrings));

  ExpectHasBaseAttributes(ObjectID::RedundancySupport);
  ExpectHasTypeAttributes(ObjectID::RedundancySupport);
}

TEST_F(StandardNamespaceTest, SecurityTokenRequestType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SecurityTokenRequestType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SecurityTokenRequestTypeEnumStrings));

  ExpectHasBaseAttributes(ObjectID::SecurityTokenRequestType);
  ExpectHasTypeAttributes(ObjectID::SecurityTokenRequestType);
}

TEST_F(StandardNamespaceTest, ServerState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerState);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ServerStateEnumStrings));

  ExpectHasBaseAttributes(ObjectID::ServerState);
  ExpectHasTypeAttributes(ObjectID::ServerState);
}
