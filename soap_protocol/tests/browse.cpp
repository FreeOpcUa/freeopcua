/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "common.h"

TEST(Browse, Request)
{
  OpcUa::BrowseRequest opcua;
  opcua.Header = OpcUa::Test::CreateRequestHeader();
  opcua.Query.MaxReferenciesPerNode = 1;
  opcua.Query.View.ID = OpcUa::NumericNodeID(2, 2);
  opcua.Query.View.Timestamp = 3;
  opcua.Query.View.Version = 4;

  OpcUa::BrowseDescription desc;
  desc.Direction = OpcUa::BrowseDirection::Both;
  desc.IncludeSubtypes = true;
  desc.NodeClasses = OpcUa::NODE_CLASS_OBJECT;
  desc.NodeToBrowse = OpcUa::NumericNodeID(5, 5);
  desc.ReferenceTypeID = OpcUa::NumericNodeID(6, 6);
  desc.ResultMask = 3;

  opcua.Query.NodesToBrowse.push_back(desc);

  soap service;
  ns3__BrowseRequest* req = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(req, nullptr);
  OpcUa::Test::AssertRequestHeaderValid(req->RequestHeader);

  ASSERT_EQ(req->RequestedMaxReferencesPerNode, opcua.Query.MaxReferenciesPerNode);
  ASSERT_NE(req->View, nullptr);
  ASSERT_EQ(req->View->Timestamp, opcua.Query.View.Timestamp);
  ASSERT_EQ(req->View->ViewVersion, opcua.Query.View.Version);

  ASSERT_TRUE(OpcUa::Test::IsEqualNodeID(req->View->ViewId, "ns=2;i=2;"));

  ASSERT_NE(req->NodesToBrowse, nullptr);
  ASSERT_EQ(req->NodesToBrowse->BrowseDescription.size(), opcua.Query.NodesToBrowse.size());
  ns3__BrowseDescription* deserializedDesc = req->NodesToBrowse->BrowseDescription[0];
  ASSERT_NE(deserializedDesc, nullptr);
  ASSERT_EQ(static_cast<OpcUa::BrowseDirection>(deserializedDesc->BrowseDirection), desc.Direction);
  ASSERT_EQ(deserializedDesc->IncludeSubtypes, desc.IncludeSubtypes);
  ASSERT_EQ(deserializedDesc->NodeClassMask, desc.NodeClasses);
  ASSERT_TRUE(OpcUa::Test::IsEqualNodeID(deserializedDesc->NodeId, "ns=5;i=5;"));
  ASSERT_TRUE(OpcUa::Test::IsEqualNodeID(deserializedDesc->ReferenceTypeId, "ns=6;i=6;"));

  OpcUa::BrowseRequest serialized = OpcUa::Soap::Deserialize(req);
  OpcUa::Test::AssertRequestHeaderEq(serialized.Header, opcua.Header);
  ASSERT_EQ(serialized.TypeID, opcua.TypeID);
  ASSERT_EQ(serialized.Query.MaxReferenciesPerNode, opcua.Query.MaxReferenciesPerNode);
  ASSERT_EQ(serialized.Query.NodesToBrowse.size(), 1);
  const OpcUa::BrowseDescription& serializedDesc = serialized.Query.NodesToBrowse[0];
  const OpcUa::BrowseDescription& sourceDesc = opcua.Query.NodesToBrowse[0];
  ASSERT_EQ(serializedDesc.Direction, sourceDesc.Direction);
  ASSERT_EQ(serializedDesc.IncludeSubtypes, sourceDesc.IncludeSubtypes);
  ASSERT_EQ(serializedDesc.NodeClasses, sourceDesc.NodeClasses);
  ASSERT_EQ(serializedDesc.NodeToBrowse, sourceDesc.NodeToBrowse);
  ASSERT_EQ(serializedDesc.ReferenceTypeID, sourceDesc.ReferenceTypeID);
  ASSERT_EQ(serializedDesc.ResultMask, sourceDesc.ResultMask);
}
