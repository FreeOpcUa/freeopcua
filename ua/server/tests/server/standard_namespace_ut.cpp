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

class StandardNamespaceStructure : public testing::Test
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

TEST_F(StandardNamespaceStructure, CanBrowseRootFolder_By_Organizes_RefType)
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

TEST_F(StandardNamespaceStructure, CanBrowseRootFolder_By_HierarchicalReferencies_Subtypes)
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

TEST_F(StandardNamespaceStructure, CheckObjects)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ObjectsFolder);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ObjectsFolder);
}

TEST_F(StandardNamespaceStructure, CheckTypes)
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

TEST_F(StandardNamespaceStructure, DataTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypes);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseDataType));

  ExpectHasBaseAttributes(ObjectID::DataTypes);
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
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageBmp));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageGif));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImageJpg));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ImagePng));

  ExpectHasBaseAttributes(ObjectID::Image);
  ExpectHasTypeAttributes(ObjectID::Image);
}

TEST_F(StandardNamespaceStructure, ImageBmp)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageBmp);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageBmp);
  ExpectHasTypeAttributes(ObjectID::ImageBmp);
}

TEST_F(StandardNamespaceStructure, ImageGif)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageGif);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageGif);
  ExpectHasTypeAttributes(ObjectID::ImageGif);
}

TEST_F(StandardNamespaceStructure, ImageJpg)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImageJpg);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImageJpg);
  ExpectHasTypeAttributes(ObjectID::ImageJpg);
}

TEST_F(StandardNamespaceStructure, ImagePng)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ImagePng);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ImagePng);
  ExpectHasTypeAttributes(ObjectID::ImagePng);
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
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::IdTypeEnumStrings));

  ExpectHasBaseAttributes(ObjectID::IdType);
  ExpectHasTypeAttributes(ObjectID::IdType);
}

TEST_F(StandardNamespaceStructure, IdTypeEnuStrings)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::IdTypeEnumStrings);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::IdTypeEnumStrings);
  ExpectHasVariableAttributes(ObjectID::IdTypeEnumStrings);
}

TEST_F(StandardNamespaceStructure, MessageSecurityMode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MessageSecurityMode);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::MessageSecurityModeEnumStrings));

  ExpectHasBaseAttributes(ObjectID::MessageSecurityMode);
  ExpectHasTypeAttributes(ObjectID::MessageSecurityMode);
}

TEST_F(StandardNamespaceStructure, MessageSecurityModeEnumStrings)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MessageSecurityModeEnumStrings);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::IdTypeEnumStrings);
  ExpectHasVariableAttributes(ObjectID::IdTypeEnumStrings);
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
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::RedundancySupportEnumStrings));

  ExpectHasBaseAttributes(ObjectID::RedundancySupport);
  ExpectHasTypeAttributes(ObjectID::RedundancySupport);
}

TEST_F(StandardNamespaceStructure, SecurityTokenRequestType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SecurityTokenRequestType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SecurityTokenRequestTypeEnumStrings));

  ExpectHasBaseAttributes(ObjectID::SecurityTokenRequestType);
  ExpectHasTypeAttributes(ObjectID::SecurityTokenRequestType);
}

TEST_F(StandardNamespaceStructure, ServerState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerState);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ServerStateEnumStrings));

  ExpectHasBaseAttributes(ObjectID::ServerState);
  ExpectHasTypeAttributes(ObjectID::ServerState);
}

TEST_F(StandardNamespaceStructure, ExpandedNodeID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ExpandedNodeID);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ExpandedNodeID);
  ExpectHasTypeAttributes(ObjectID::ExpandedNodeID);
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
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NodeID);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::NodeID);
  ExpectHasTypeAttributes(ObjectID::NodeID);
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
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::LocaleID));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NumericRange));

  ExpectHasBaseAttributes(ObjectID::String);
  ExpectHasTypeAttributes(ObjectID::String);
}

TEST_F(StandardNamespaceStructure, LocaleID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::LocaleID);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::LocaleID);
  ExpectHasTypeAttributes(ObjectID::LocaleID);
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
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UserIdentifyToken));

  ExpectHasBaseAttributes(ObjectID::Structure);
  ExpectHasTypeAttributes(ObjectID::Structure);
}

TEST_F(StandardNamespaceStructure, AddNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::AddNodesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::AddNodesItem);
  ExpectHasTypeAttributes(ObjectID::AddNodesItem);
}

TEST_F(StandardNamespaceStructure, AddReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::AddReferencesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::AddReferencesItem);
  ExpectHasTypeAttributes(ObjectID::AddReferencesItem);
}

TEST_F(StandardNamespaceStructure, ApplicationDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ApplicationDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ApplicationDescription);
  ExpectHasTypeAttributes(ObjectID::ApplicationDescription);
}

TEST_F(StandardNamespaceStructure, Argument)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Argument);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Argument);
  ExpectHasTypeAttributes(ObjectID::Argument);
}

TEST_F(StandardNamespaceStructure, BuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfo);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::BuildInfo);
  ExpectHasTypeAttributes(ObjectID::BuildInfo);
}

TEST_F(StandardNamespaceStructure, DeleteNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DeleteNodesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DeleteNodesItem);
  ExpectHasTypeAttributes(ObjectID::DeleteNodesItem);
}

TEST_F(StandardNamespaceStructure, DeleteReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DeleteReferencesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DeleteReferencesItem);
  ExpectHasTypeAttributes(ObjectID::DeleteReferencesItem);
}

TEST_F(StandardNamespaceStructure, EUInformation)
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

TEST_F(StandardNamespaceStructure, Range)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Range);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::Range);
  ExpectHasTypeAttributes(ObjectID::Range);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SamplingIntervalDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, SemanticChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SemanticChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SemanticChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectID::SemanticChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummaryDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsSummaryDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsSummaryDataType);
  ExpectHasTypeAttributes(ObjectID::ServerDiagnosticsSummaryDataType);
}

TEST_F(StandardNamespaceStructure, ServerStatusDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerStatusDataType);
  ExpectHasTypeAttributes(ObjectID::ServerStatusDataType);
}

TEST_F(StandardNamespaceStructure, ServiceCounterDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServiceCounterDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServiceCounterDataType);
  ExpectHasTypeAttributes(ObjectID::ServiceCounterDataType);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SessionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionSecurityDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionSecurityDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SessionSecurityDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, SignedSoftwareCertificate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SignedSoftwareCertificate);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SignedSoftwareCertificate);
  ExpectHasTypeAttributes(ObjectID::SignedSoftwareCertificate);
}

TEST_F(StandardNamespaceStructure, StatusResult)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StatusResult);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StatusResult);
  ExpectHasTypeAttributes(ObjectID::StatusResult);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::SubscriptionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, UserIdentifyToken)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::UserIdentifyToken);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::AnonymousIdentifyToken));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UserNameIdentifyToken));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::X509IdentifyToken));

  ExpectHasBaseAttributes(ObjectID::UserIdentifyToken);
  ExpectHasTypeAttributes(ObjectID::UserIdentifyToken);
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
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::EventTypes);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));

  ExpectHasBaseAttributes(ObjectID::EventTypes);
}

TEST_F(StandardNamespaceStructure, BaseEventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseEventType);
  EXPECT_EQ(SizeOf(refs), 12);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BaseModelChangeEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SemanticChangeEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SystemEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::EventID));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::EventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::LocalTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Message));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ReceiveTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Severity));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SourceName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SourceNode));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::Time));

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
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SystemEventType);
  ExpectHasTypeAttributes(ObjectID::SystemEventType);
}

TEST_F(StandardNamespaceStructure, EventID)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::EventID);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::EventID);
  ExpectHasVariableAttributes(ObjectID::EventID);
}

TEST_F(StandardNamespaceStructure, EventType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::EventType);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::EventType);
  ExpectHasVariableAttributes(ObjectID::EventType);
}

TEST_F(StandardNamespaceStructure, LocalTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::LocalTime);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::LocalTime);
  ExpectHasVariableAttributes(ObjectID::LocalTime);
}

TEST_F(StandardNamespaceStructure, Message)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Message);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::Message);
  ExpectHasVariableAttributes(ObjectID::Message);
}

TEST_F(StandardNamespaceStructure, ReceiveTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ReceiveTime);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ReceiveTime);
  ExpectHasVariableAttributes(ObjectID::ReceiveTime);
}

TEST_F(StandardNamespaceStructure, Severity)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Severity);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::Severity);
  ExpectHasVariableAttributes(ObjectID::Severity);
}

TEST_F(StandardNamespaceStructure, SourceName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SourceName);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::SourceName);
  ExpectHasVariableAttributes(ObjectID::SourceName);
}

TEST_F(StandardNamespaceStructure, SourceNode)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SourceNode);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::SourceNode);
  ExpectHasVariableAttributes(ObjectID::SourceNode);
}

TEST_F(StandardNamespaceStructure, Time)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Time);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::Time);
  ExpectHasVariableAttributes(ObjectID::Time);
}
