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
#include <opc/ua/server/standard_namespace.h>

#include <functional>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace OpcUa;

class StandardNamespaceStructure : public Test
{
protected:
  virtual void SetUp()
  {
    NameSpace = OpcUa::UaServer::CreateAddressSpace();
    const bool debug = false;
    OpcUa::UaServer::FillStandardNamespace(*NameSpace, false);
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

protected:
  UaServer::AddressSpace::UniquePtr NameSpace;
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
  std::vector<ReferenceDescription> referencies = NameSpace->Browse(query);
  ASSERT_EQ(referencies.size(), 3);
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
  std::vector<ReferenceDescription> referencies = NameSpace->Browse(query);
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
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::Server));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));

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
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureAddNodesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureAddReferencesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureApplicationDescription));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureArgument));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureBuildInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureDeleteNodesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureDeleteReferencesItem));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureEUInformation));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureModelChangeStructureDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureRange));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureSamplingIntervalDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureSemanticChangeStructureDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureServerStatusDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureServiceCounterDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureSessionDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureSessionSecurityDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureSignedSoftwareCertificate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureStatusResult));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureSubscriptionDiagnosticsDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::StructureUserIdentifyToken));

  ExpectHasBaseAttributes(ObjectID::Structure);
  ExpectHasTypeAttributes(ObjectID::Structure);
}

TEST_F(StandardNamespaceStructure, StructureAddNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureAddNodesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureAddNodesItem);
  ExpectHasTypeAttributes(ObjectID::StructureAddNodesItem);
}

TEST_F(StandardNamespaceStructure, StructureAddReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureAddReferencesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureAddReferencesItem);
  ExpectHasTypeAttributes(ObjectID::StructureAddReferencesItem);
}

TEST_F(StandardNamespaceStructure, StructureApplicationDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureApplicationDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureApplicationDescription);
  ExpectHasTypeAttributes(ObjectID::StructureApplicationDescription);
}

TEST_F(StandardNamespaceStructure, StructureArgument)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureArgument);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureArgument);
  ExpectHasTypeAttributes(ObjectID::StructureArgument);
}

TEST_F(StandardNamespaceStructure, StructureBuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureBuildInfo);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureBuildInfo);
  ExpectHasTypeAttributes(ObjectID::StructureBuildInfo);
}

TEST_F(StandardNamespaceStructure, StructureDeleteNodesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureDeleteNodesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureDeleteNodesItem);
  ExpectHasTypeAttributes(ObjectID::StructureDeleteNodesItem);
}

TEST_F(StandardNamespaceStructure, StructureDeleteReferencesItem)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureDeleteReferencesItem);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureDeleteReferencesItem);
  ExpectHasTypeAttributes(ObjectID::StructureDeleteReferencesItem);
}

TEST_F(StandardNamespaceStructure, StructureEUInformation)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureEUInformation);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureEUInformation);
  ExpectHasTypeAttributes(ObjectID::StructureEUInformation);
}

TEST_F(StandardNamespaceStructure, EUModelChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureModelChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureModelChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectID::StructureModelChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, StructureRange)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureRange);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureRange);
  ExpectHasTypeAttributes(ObjectID::StructureRange);
}

TEST_F(StandardNamespaceStructure, StructureSamplingIntervalDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureSamplingIntervalDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureSamplingIntervalDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::StructureSamplingIntervalDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSemanticChangeStructureDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureSemanticChangeStructureDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureSemanticChangeStructureDataType);
  ExpectHasTypeAttributes(ObjectID::StructureSemanticChangeStructureDataType);
}

TEST_F(StandardNamespaceStructure, StructureServerDiagnosticsSummaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureServerDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureServerDiagnosticsSummaryType);
  ExpectHasTypeAttributes(ObjectID::StructureServerDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, StructureServerStatusDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureServerStatusDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureServerStatusDataType);
  ExpectHasTypeAttributes(ObjectID::StructureServerStatusDataType);
}

TEST_F(StandardNamespaceStructure, StructureServiceCounterDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureServiceCounterDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureServiceCounterDataType);
  ExpectHasTypeAttributes(ObjectID::StructureServiceCounterDataType);
}

TEST_F(StandardNamespaceStructure, StructureSessionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureSessionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureSessionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::StructureSessionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSessionSecurityDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureSessionSecurityDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureSessionSecurityDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::StructureSessionSecurityDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureSignedSoftwareCertificate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureSignedSoftwareCertificate);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureSignedSoftwareCertificate);
  ExpectHasTypeAttributes(ObjectID::StructureSignedSoftwareCertificate);
}

TEST_F(StandardNamespaceStructure, StructureStatusResult)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureStatusResult);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureStatusResult);
  ExpectHasTypeAttributes(ObjectID::StructureStatusResult);
}

TEST_F(StandardNamespaceStructure, StructureSubscriptionDiagnosticsDataType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureSubscriptionDiagnosticsDataType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::StructureSubscriptionDiagnosticsDataType);
  ExpectHasTypeAttributes(ObjectID::StructureSubscriptionDiagnosticsDataType);
}

TEST_F(StandardNamespaceStructure, StructureUserIdentifyToken)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StructureUserIdentifyToken);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::AnonymousIdentifyToken));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::UserNameIdentifyToken));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::X509IdentifyToken));

  ExpectHasBaseAttributes(ObjectID::StructureUserIdentifyToken);
  ExpectHasTypeAttributes(ObjectID::StructureUserIdentifyToken);
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

TEST_F(StandardNamespaceStructure, ObjectTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ObjectTypes);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseObjectType));

  ExpectHasBaseAttributes(ObjectID::ObjectTypes);
}

TEST_F(StandardNamespaceStructure, BaseObjectType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseObjectType);
  EXPECT_EQ(SizeOf(refs), 15);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BaseEventType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeEncodingType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeSystemType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HistoricalEventConfigurationType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ModellingRuleType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerCapabilitiesType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerRedundancyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsObjectType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionDiagnosticsSummaryType));
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

TEST_F(StandardNamespaceStructure, HistoricalEventConfigurationType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HistoricalEventConfigurationType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::HistoricalEventConfigurationType);
  ExpectHasTypeAttributes(ObjectID::HistoricalEventConfigurationType);
}

TEST_F(StandardNamespaceStructure, ModellingRuleType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRuleType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::NamingRule));

  ExpectHasBaseAttributes(ObjectID::ModellingRuleType);
  ExpectHasTypeAttributes(ObjectID::ModellingRuleType);
}

TEST_F(StandardNamespaceStructure, NamingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NamingRule);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::NamingRule);
  ExpectHasVariableAttributes(ObjectID::NamingRule);
}

TEST_F(StandardNamespaceStructure, ServerCapabilitiesType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilitiesType);
  EXPECT_EQ(SizeOf(refs), 8);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::AggregateFunctions));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::LocaleIDArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::MaxBrowseContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::MaxHistoryContinuationPoints));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::MinSupportedSampleRate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ModellingRules));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ServerProfileArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SoftwareCertificates));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilitiesType);
  ExpectHasTypeAttributes(ObjectID::ServerCapabilitiesType);
}

TEST_F(StandardNamespaceStructure, AggregateFunctions)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::AggregateFunctions);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::AggregateFunctions);
}

TEST_F(StandardNamespaceStructure, ModellingRules)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ModellingRules);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ModellingRules);
}

TEST_F(StandardNamespaceStructure, LocaleIDArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::LocaleIDArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::LocaleIDArray);
  ExpectHasVariableAttributes(ObjectID::LocaleIDArray);
}

TEST_F(StandardNamespaceStructure, MaxBrowseContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MaxBrowseContinuationPoints);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::MaxBrowseContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::MaxBrowseContinuationPoints);
}

TEST_F(StandardNamespaceStructure, MaxHistoryContinuationPoints)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MaxHistoryContinuationPoints);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::MaxHistoryContinuationPoints);
  ExpectHasVariableAttributes(ObjectID::MaxHistoryContinuationPoints);
}

TEST_F(StandardNamespaceStructure, MinSupportedSampleRate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::MinSupportedSampleRate);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::MinSupportedSampleRate);
  ExpectHasVariableAttributes(ObjectID::MinSupportedSampleRate);
}

TEST_F(StandardNamespaceStructure, ServerProfileArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerProfileArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ServerProfileArray);
  ExpectHasVariableAttributes(ObjectID::ServerProfileArray);
}

TEST_F(StandardNamespaceStructure, SoftwareCertificates)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SoftwareCertificates);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::SoftwareCertificates);
  ExpectHasVariableAttributes(ObjectID::SoftwareCertificates);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 5);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::EnableFlag));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::ServerDiagnosticsSummary));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SubscriptionDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::SamplingIntervalDiagnosticsArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::SessionsDiagnosticsSummary));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsType);
  ExpectHasTypeAttributes(ObjectID::ServerDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, EnableFlag)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::EnableFlag);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::EnableFlag);
  ExpectHasVariableAttributes(ObjectID::EnableFlag);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SamplingIntervalDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsArray);
  ExpectHasVariableAttributes(ObjectID::SamplingIntervalDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsSummary);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::StructureServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsSummary);
  ExpectHasVariableAttributes(ObjectID::ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, SessionDiagnosticsSummary)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionsDiagnosticsSummary);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SessionDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsSummary);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsArray);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::SubscriptionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsArray);
}

TEST_F(StandardNamespaceStructure, ServerRedundancyType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerRedundancyType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::RedundancySupport));

  ExpectHasBaseAttributes(ObjectID::ServerRedundancyType);
  ExpectHasTypeAttributes(ObjectID::ServerRedundancyType);
}

TEST_F(StandardNamespaceStructure, RedundancySupportTypeRedundancySupport)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::RedundancySupportTypeRedundancySupport);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::RedundancySupportTypeRedundancySupport);
  ExpectHasVariableAttributes(ObjectID::RedundancySupportTypeRedundancySupport);
}

TEST_F(StandardNamespaceStructure, ServerType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerType);
  EXPECT_EQ(SizeOf(refs), 10);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::Auditing));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::NamespaceArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerProfileArray));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerCapabilities));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerDiagnostics));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::ServerRedundancy));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerStatus));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServiceLevel));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent, ObjectID::VendorServerInfo));

  ExpectHasBaseAttributes(ObjectID::ServerType);
  ExpectHasTypeAttributes(ObjectID::ServerType);
}

TEST_F(StandardNamespaceStructure, Auditing)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Auditing);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition,  ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,   ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::Auditing);
  ExpectHasVariableAttributes(ObjectID::Auditing);
}

TEST_F(StandardNamespaceStructure, NamespaceArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NamespaceArray);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition,  ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::NamespaceArray);
  ExpectHasVariableAttributes(ObjectID::NamespaceArray);
}

TEST_F(StandardNamespaceStructure, ServerArray)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerArray);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition,  ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::ServerArray);
  ExpectHasVariableAttributes(ObjectID::ServerArray);
}

TEST_F(StandardNamespaceStructure, ServerCapabilities)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerCapabilities);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerCapabilitiesType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRuleMandatory));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty,  ObjectID::ServerProfileArray));

  ExpectHasBaseAttributes(ObjectID::ServerCapabilities);
  ExpectHasTypeAttributes(ObjectID::ServerCapabilities);
}

TEST_F(StandardNamespaceStructure, ServerDiagnostics)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnostics);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ServerDiagnostics);
  ExpectHasTypeAttributes(ObjectID::ServerDiagnostics);
}

TEST_F(StandardNamespaceStructure, ServerRedundancy)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerRedundancy);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::ServerRedundancyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ServerRedundancy);
  ExpectHasTypeAttributes(ObjectID::ServerRedundancy);
}

TEST_F(StandardNamespaceStructure, ServerStatus)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatus);
  EXPECT_EQ(SizeOf(refs), 8);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::StructureServerStatusDataType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule,  ObjectID::ModellingRuleMandatory));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::StructureBuildInfo));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::CurrentTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::SecondsTillShutdown));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::ShutdownReason));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::StartTime));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::State));

  ExpectHasBaseAttributes(ObjectID::ServerStatus);
  ExpectHasVariableAttributes(ObjectID::ServerStatus);
}

TEST_F(StandardNamespaceStructure, BuildInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildInfo);
  EXPECT_EQ(SizeOf(refs), 7);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildDate));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::BuildNumber));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::ManufacturerName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::ProductName));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::ProductURI));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasComponent,      ObjectID::SoftwareVersion));

  ExpectHasBaseAttributes(ObjectID::BuildInfo);
  ExpectHasVariableAttributes(ObjectID::BuildInfo);
}

TEST_F(StandardNamespaceStructure, BuildDate)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildDate);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildDate);
  ExpectHasVariableAttributes(ObjectID::BuildDate);
}

TEST_F(StandardNamespaceStructure, BuildNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BuildNumber);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::BuildNumber);
  ExpectHasVariableAttributes(ObjectID::BuildNumber);
}

TEST_F(StandardNamespaceStructure, ManufacturerName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ManufacturerName);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ManufacturerName);
  ExpectHasVariableAttributes(ObjectID::ManufacturerName);
}

TEST_F(StandardNamespaceStructure, ProductName)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ProductName);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ProductName);
  ExpectHasVariableAttributes(ObjectID::ProductName);
}

TEST_F(StandardNamespaceStructure, ProductURI)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ProductURI);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ProductURI);
  ExpectHasVariableAttributes(ObjectID::ProductURI);
}

TEST_F(StandardNamespaceStructure, SoftwareVersion)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SoftwareVersion);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::SoftwareVersion);
  ExpectHasVariableAttributes(ObjectID::SoftwareVersion);
}

TEST_F(StandardNamespaceStructure, CurrentTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::CurrentTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::CurrentTime);
  ExpectHasVariableAttributes(ObjectID::CurrentTime);
}

TEST_F(StandardNamespaceStructure, SecondsTillShutdown)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SecondsTillShutdown);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::SecondsTillShutdown);
  ExpectHasVariableAttributes(ObjectID::SecondsTillShutdown);
}

TEST_F(StandardNamespaceStructure, ShutdownReason)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ShutdownReason);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::ShutdownReason);
  ExpectHasVariableAttributes(ObjectID::ShutdownReason);
}

TEST_F(StandardNamespaceStructure, StartTime)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StartTime);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::StartTime);
  ExpectHasVariableAttributes(ObjectID::StartTime);
}

TEST_F(StandardNamespaceStructure, State)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::State);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::BaseDataVariableType));

  ExpectHasBaseAttributes(ObjectID::State);
  ExpectHasVariableAttributes(ObjectID::State);
}

TEST_F(StandardNamespaceStructure, ServiceLevel)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServiceLevel);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::ServiceLevel);
  ExpectHasVariableAttributes(ObjectID::ServiceLevel);
}

TEST_F(StandardNamespaceStructure, VendorServerInfo)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::VendorServerInfo);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::VendorServerInfoType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasModellingRule, ObjectID::ModellingRuleMandatory));

  ExpectHasBaseAttributes(ObjectID::VendorServerInfo);
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
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionDiagnosticsSummaryType);
  ExpectHasTypeAttributes(ObjectID::SessionDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, StateType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StateType);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasProperty, ObjectID::StateNumber));

  ExpectHasBaseAttributes(ObjectID::StateType);
  ExpectHasTypeAttributes(ObjectID::StateType);
}

TEST_F(StandardNamespaceStructure, StateNumber)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::StateNumber);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::StateNumber);
  ExpectHasVariableAttributes(ObjectID::StateNumber);
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
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ReferenceTypes);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::References));

  ExpectHasBaseAttributes(ObjectID::ReferenceTypes);
}

TEST_F(StandardNamespaceStructure, References)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::References);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HierarchicalReferences));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::NonHierarchicalReferences));

  ExpectHasReferenceTypeAttributes(ObjectID::References);
  EXPECT_FALSE(HasAttribute(ObjectID::HierarchicalReferences, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HierarchicalReferences)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HierarchicalReferences);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasChild));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasEventSource));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Organizes));

  ExpectHasReferenceTypeAttributes(ObjectID::HierarchicalReferences);
  EXPECT_FALSE(HasAttribute(ObjectID::HierarchicalReferences, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasChild)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasChild);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasSubtype));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::Aggregates));

  ExpectHasReferenceTypeAttributes(ObjectID::HasChild);
  EXPECT_FALSE(HasAttribute(ObjectID::HasChild, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, Aggregates)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Aggregates);
  EXPECT_EQ(SizeOf(refs), 3);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasHistoricalConfiguration));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasComponent));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasProperty));

  ExpectHasReferenceTypeAttributes(ObjectID::Aggregates);
  EXPECT_FALSE(HasAttribute(ObjectID::Aggregates, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasComponent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasComponent);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasOrderedComponent));

  ExpectHasReferenceTypeAttributes(ObjectID::HasComponent);
  EXPECT_TRUE(HasAttribute(ObjectID::HasComponent, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasOrderedComponent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasOrderedComponent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasOrderedComponent);
  EXPECT_TRUE(HasAttribute(ObjectID::HasOrderedComponent, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasHistoricalConfiguration)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasHistoricalConfiguration);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasHistoricalConfiguration);
  EXPECT_TRUE(HasAttribute(ObjectID::HasHistoricalConfiguration, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasProperty)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasProperty);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasProperty);
  EXPECT_TRUE(HasAttribute(ObjectID::HasProperty, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasSubtype)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasSubtype);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasSubtype);
  EXPECT_TRUE(HasAttribute(ObjectID::HasSubtype, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasEventSource)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasEventSource);
  EXPECT_EQ(SizeOf(refs), 1);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasNotifier));

  ExpectHasReferenceTypeAttributes(ObjectID::HasEventSource);
  EXPECT_TRUE(HasAttribute(ObjectID::HasEventSource, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasNotifier)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasNotifier);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasNotifier);
  EXPECT_TRUE(HasAttribute(ObjectID::HasNotifier, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, Organizes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::Organizes);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::Organizes);
  EXPECT_TRUE(HasAttribute(ObjectID::Organizes, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, NonHierarchicalReferences)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::NonHierarchicalReferences);
  EXPECT_EQ(SizeOf(refs), 11);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::FromState));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::GeneratesEvent));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasCause));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasCondition));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasDescription));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasEffect));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasEncoding));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasModelParent));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasModellingRule));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::HasTypeDefinition));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ToState));

  ExpectHasReferenceTypeAttributes(ObjectID::NonHierarchicalReferences);
  EXPECT_FALSE(HasAttribute(ObjectID::NonHierarchicalReferences, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, FromState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::FromState);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::FromState);
  EXPECT_TRUE(HasAttribute(ObjectID::FromState, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, GeneratesEvent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::GeneratesEvent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::GeneratesEvent);
  EXPECT_TRUE(HasAttribute(ObjectID::GeneratesEvent, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasCause)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasCause);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasCause);
  EXPECT_TRUE(HasAttribute(ObjectID::HasCause, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasDescription)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasDescription);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasDescription);
  EXPECT_TRUE(HasAttribute(ObjectID::HasDescription, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasEffect)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasEffect);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasEffect);
  EXPECT_TRUE(HasAttribute(ObjectID::HasEffect, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasEncoding)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasEncoding);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasEncoding);
  EXPECT_TRUE(HasAttribute(ObjectID::HasEncoding, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasModelParent)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasModelParent);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasModelParent);
  EXPECT_TRUE(HasAttribute(ObjectID::HasModelParent, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasModellingRule)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasModellingRule);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasModellingRule);
  EXPECT_TRUE(HasAttribute(ObjectID::HasModellingRule, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, HasTypeDefinition)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::HasTypeDefinition);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::HasTypeDefinition);
  EXPECT_TRUE(HasAttribute(ObjectID::HasTypeDefinition, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, ToState)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ToState);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasReferenceTypeAttributes(ObjectID::ToState);
  EXPECT_TRUE(HasAttribute(ObjectID::ToState, AttributeID::INVERSE_NAME));
}

TEST_F(StandardNamespaceStructure, VariableTypes)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::VariableTypes);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasTypeDefinition, ObjectID::FolderType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::Organizes, ObjectID::BaseVariableType));

  ExpectHasBaseAttributes(ObjectID::VariableTypes);
}

TEST_F(StandardNamespaceStructure, BaseVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseVariableType);
  EXPECT_EQ(SizeOf(refs), 2);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::BaseDataVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::PropertyType));

  ExpectHasBaseAttributes(ObjectID::BaseVariableType);
  ExpectHasVariableAttributes(ObjectID::BaseVariableType);
}

TEST_F(StandardNamespaceStructure, BaseDataVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::BaseDataVariableType);
  EXPECT_EQ(SizeOf(refs), 13);
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeDescriptionType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::DataTypeDictionaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SamplingIntervalDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsSummaryType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerStatusType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerVendorCapabilityType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionsDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::ServerDiagnosticsVariableType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SessionSecurityDiagnosticsType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsArrayType));
  EXPECT_TRUE(HasReference(refs, ReferenceID::HasSubtype, ObjectID::SubscriptionDiagnosticsType));

  ExpectHasBaseAttributes(ObjectID::BaseDataVariableType);
  ExpectHasVariableAttributes(ObjectID::BaseDataVariableType);
}

TEST_F(StandardNamespaceStructure, DataTypeDescriptionType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypeDescriptionType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DataTypeDescriptionType);
  ExpectHasVariableAttributes(ObjectID::DataTypeDescriptionType);
}

TEST_F(StandardNamespaceStructure, DataTypeDictionaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::DataTypeDictionaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::DataTypeDictionaryType);
  ExpectHasVariableAttributes(ObjectID::DataTypeDictionaryType);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsArrayType);
  ExpectHasVariableAttributes(ObjectID::SamplingIntervalDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SamplingIntervalDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SamplingIntervalDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SamplingIntervalDiagnosticsType);
  ExpectHasVariableAttributes(ObjectID::SamplingIntervalDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsSummaryType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsSummaryType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsSummaryType);
  ExpectHasVariableAttributes(ObjectID::ServerDiagnosticsSummaryType);
}

TEST_F(StandardNamespaceStructure, ServerStatusType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerStatusType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerStatusType);
  ExpectHasVariableAttributes(ObjectID::ServerStatusType);
}

TEST_F(StandardNamespaceStructure, ServerVendorCapabilityType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerVendorCapabilityType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerVendorCapabilityType);
  ExpectHasVariableAttributes(ObjectID::ServerVendorCapabilityType);
}

TEST_F(StandardNamespaceStructure, SessionsDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionsDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionsDiagnosticsArrayType);
  ExpectHasVariableAttributes(ObjectID::SessionsDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, ServerDiagnosticsVariableType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::ServerDiagnosticsVariableType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::ServerDiagnosticsVariableType);
  ExpectHasVariableAttributes(ObjectID::ServerDiagnosticsVariableType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionSecurityDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionSecurityDiagnosticsArrayType);
  ExpectHasVariableAttributes(ObjectID::SessionSecurityDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SessionSecurityDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SessionSecurityDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SessionSecurityDiagnosticsType);
  ExpectHasVariableAttributes(ObjectID::SessionSecurityDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsArrayType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsArrayType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsArrayType);
  ExpectHasVariableAttributes(ObjectID::SubscriptionDiagnosticsArrayType);
}

TEST_F(StandardNamespaceStructure, SubscriptionDiagnosticsType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::SubscriptionDiagnosticsType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::SubscriptionDiagnosticsType);
  ExpectHasVariableAttributes(ObjectID::SubscriptionDiagnosticsType);
}

TEST_F(StandardNamespaceStructure, PropertyType)
{
  const std::vector<ReferenceDescription> refs = Browse(ObjectID::PropertyType);
  EXPECT_EQ(SizeOf(refs), 0);

  ExpectHasBaseAttributes(ObjectID::PropertyType);
  ExpectHasVariableAttributes(ObjectID::PropertyType);
}
