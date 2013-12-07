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
  opcua.Query.View.Timestamp = OpcUa::ToDateTime(3);
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

  ASSERT_NODE_ID_EQ(req->View->ViewId, "ns=2;i=2;");

  ASSERT_NE(req->NodesToBrowse, nullptr);
  ASSERT_EQ(req->NodesToBrowse->BrowseDescription.size(), opcua.Query.NodesToBrowse.size());
  ns3__BrowseDescription* deserializedDesc = req->NodesToBrowse->BrowseDescription[0];
  ASSERT_NE(deserializedDesc, nullptr);
  ASSERT_EQ(static_cast<OpcUa::BrowseDirection>(deserializedDesc->BrowseDirection), desc.Direction);
  ASSERT_EQ(deserializedDesc->IncludeSubtypes, desc.IncludeSubtypes);
  ASSERT_EQ(deserializedDesc->NodeClassMask, desc.NodeClasses);
  ASSERT_NODE_ID_EQ(deserializedDesc->NodeId, "ns=5;i=5;");
  ASSERT_NODE_ID_EQ(deserializedDesc->ReferenceTypeId, "ns=6;i=6;");

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

//void
TEST(Browse, Response)
{
  // Fill response
  OpcUa::BrowseResponse opcua;
  opcua.Header = OpcUa::Test::CreateResponseHeader();
  opcua.Diagnostics = OpcUa::Test::CreateDiagnosticInfoList();
  OpcUa::BrowseResult browseResult;
  browseResult.ContinuationPoint.push_back(1);
  browseResult.Status = OpcUa::StatusCode::BadAttributeIdInvalid;
  OpcUa::ReferenceDescription ref;
  ref.BrowseName = OpcUa::QualifiedName(1, "name");
  ref.DisplayName = OpcUa::LocalizedText("text", "ru");
  ref.IsForward = true;
  ref.ReferenceTypeID = OpcUa::NumericNodeID(1, 1);
  ref.TargetNodeClass = OpcUa::NodeClass::Object;
  ref.TargetNodeID = OpcUa::NumericNodeID(2, 2);
  ref.TargetNodeTypeDefinition = OpcUa::NumericNodeID(3, 3);
  browseResult.Referencies.push_back(ref);
  opcua.Results.push_back(browseResult);

  // serialize response
  soap service;
  ns3__BrowseResponse* soapResponse = OpcUa::Soap::Serialize(&service, opcua);
  ASSERT_NE(soapResponse, nullptr);
  OpcUa::Test::AssertResponseHeaderValid(soapResponse->ResponseHeader);
  OpcUa::Test::AssertDiagnosticInfoListValid(soapResponse->DiagnosticInfos);
  ASSERT_NE(soapResponse->Results, nullptr);
  ASSERT_EQ(soapResponse->Results->BrowseResult.size(), 1);
  const ns3__BrowseResult* serializedResult = soapResponse->Results->BrowseResult[0];
  ASSERT_NE(serializedResult, nullptr);
  ASSERT_NE(serializedResult->StatusCode, nullptr);
  ASSERT_NE(serializedResult->StatusCode->Code, nullptr);
  ASSERT_EQ(*serializedResult->StatusCode->Code, "0x80390000");

  ASSERT_NE(serializedResult->ContinuationPoint, nullptr);
  ASSERT_NE(serializedResult->ContinuationPoint->__ptr, nullptr);
  ASSERT_EQ(serializedResult->ContinuationPoint->__size, 1);
  ASSERT_EQ(((char*)serializedResult->ContinuationPoint->__ptr)[0], 1);

  ASSERT_NE(serializedResult->References, nullptr);
  ASSERT_EQ(serializedResult->References->ReferenceDescription.size(), 1);
  ns3__ReferenceDescription* serializedRef = serializedResult->References->ReferenceDescription[0];
  ASSERT_NE(serializedRef, nullptr);
  ASSERT_EQ(serializedRef->IsForward, true);
  ASSERT_EQ(serializedRef->NodeClass, static_cast<unsigned>(OpcUa::NodeClass::Object));

  ASSERT_NE(serializedRef->BrowseName, nullptr);
  ASSERT_NE(serializedRef->BrowseName->Name, nullptr);
  ASSERT_NE(serializedRef->BrowseName->NamespaceIndex, nullptr);
  ASSERT_EQ(*serializedRef->BrowseName->Name, "name");
  ASSERT_EQ(*serializedRef->BrowseName->NamespaceIndex, 1);

  ASSERT_NE(serializedRef->DisplayName, nullptr);
  ASSERT_NE(serializedRef->DisplayName->Locale, nullptr);
  ASSERT_NE(serializedRef->DisplayName->Text, nullptr);
  ASSERT_EQ(*serializedRef->DisplayName->Locale, "ru");
  ASSERT_EQ(*serializedRef->DisplayName->Text, "text");

  ASSERT_NODE_ID_EQ(serializedRef->NodeId, "ns=2;i=2;");
  ASSERT_NODE_ID_EQ(serializedRef->ReferenceTypeId, "ns=1;i=1;");
  ASSERT_NODE_ID_EQ(serializedRef->TypeDefinition, "ns=3;i=3;");

  // deserialize response
  const OpcUa::BrowseResponse deserialized = OpcUa::Soap::Deserialize(soapResponse);
  ASSERT_EQ(deserialized.TypeID, opcua.TypeID);
  ASSERT_RESPONSE_HEADER_EQ(deserialized.Header, opcua.Header);

  OpcUa::Test::AssertDiagnosticInfoListValid(deserialized.Diagnostics);

  ASSERT_EQ(deserialized.Results.size(), 1);
  const OpcUa::BrowseResult& deserializedResult = deserialized.Results[0];
  ASSERT_EQ(deserializedResult.ContinuationPoint.size(), 1);
  ASSERT_EQ(deserializedResult.ContinuationPoint[0], 1);
  ASSERT_EQ(deserializedResult.Status, OpcUa::StatusCode::BadAttributeIdInvalid);
  ASSERT_EQ(deserializedResult.Referencies.size(), 1);

  const OpcUa::ReferenceDescription& deserializedRef = deserializedResult.Referencies[0];
  ASSERT_EQ(deserializedRef.BrowseName.Name, "name");
  ASSERT_EQ(deserializedRef.BrowseName.NamespaceIndex, 1);

  ASSERT_EQ(deserializedRef.DisplayName.Encoding, OpcUa::HAS_TEXT | OpcUa::HAS_LOCALE);
  ASSERT_EQ(deserializedRef.DisplayName.Locale, "ru");
  ASSERT_EQ(deserializedRef.DisplayName.Text, "text");

  ASSERT_EQ(deserializedRef.IsForward, true);
  ASSERT_EQ(deserializedRef.ReferenceTypeID, OpcUa::NumericNodeID(1, 1));
  ASSERT_EQ(deserializedRef.TargetNodeClass, OpcUa::NodeClass::Object);
  ASSERT_EQ(deserializedRef.TargetNodeID, OpcUa::NumericNodeID(2, 2));
  ASSERT_EQ(deserializedRef.TargetNodeTypeDefinition, OpcUa::NumericNodeID(3, 3));
}
