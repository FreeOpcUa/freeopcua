/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief View service set.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/protocol/extension_identifiers.h>
#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/view.h>

#include <algorithm>
#include <stdexcept>

class ViewSerialization : public OpcUaBinarySerialization
{
};

class ViewDeserialization : public OpcUaBinaryDeserialization
{
};


//-------------------------------------------------------
// BrowseDirection
//-------------------------------------------------------

TEST_F(ViewSerialization, BrowseDirection)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  GetStream() << BrowseDirection::Both << flush;

  const std::vector<char> expectedData = {
  2,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(BrowseDirection::Both));
}

TEST_F(ViewDeserialization, BrowseDirection)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  2,0,0,0
  };

  GetChannel().SetData(expectedData);

  BrowseDirection direction;
  GetStream() >> direction;

  ASSERT_EQ(direction, BrowseDirection::Both);
}

//-------------------------------------------------------
// ViewDescription
//-------------------------------------------------------

TEST_F(ViewSerialization, ViewDescription)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ViewDescription desc;

  desc.ID.Encoding = EV_TWO_BYTE;
  desc.ID.TwoByteData.Identifier = 1;
  desc.Timestamp.Value = 2;
  desc.Version = 3;

  GetStream() << desc << flush;

  const std::vector<char> expectedData = {
  0, 1,
  2,0,0,0,0,0,0,0,
  3,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(desc));
}

TEST_F(ViewDeserialization, ViewDescription)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0, 1,
  2,0,0,0,0,0,0,0,
  3,0,0,0
  };

  GetChannel().SetData(expectedData);

  ViewDescription desc;
  GetStream() >> desc;

  ASSERT_EQ(desc.ID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.ID.TwoByteData.Identifier, 1);
  ASSERT_EQ(desc.Timestamp, 2);
  ASSERT_EQ(desc.Version, 3);
}

//-------------------------------------------------------
// BrowseDescription
//-------------------------------------------------------

TEST_F(ViewSerialization, BrowseDescription)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowseDescription desc;

  desc.NodeToBrowse.Encoding = EV_TWO_BYTE;
  desc.NodeToBrowse.TwoByteData.Identifier = 1;
  desc.Direction = BrowseDirection::Inverse;
  desc.ReferenceTypeID.Encoding = EV_TWO_BYTE;
  desc.ReferenceTypeID.TwoByteData.Identifier = 2;
  desc.IncludeSubtypes = true;
  desc.NodeClasses = NODE_CLASS_VARIABLE;
  desc.ResultMask = REFERENCE_NODE_CLASS;

  GetStream() << desc << flush;

  const std::vector<char> expectedData = {
  0, 1,
  1,0,0,0,
  0, 2,
  1,
  2,0,0,0,
  4,0,0,0
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData);
  ASSERT_EQ(expectedData.size(), RawSize(desc));
}

TEST_F(ViewDeserialization, BrowseDescription)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0, 1,
  1,0,0,0,
  0, 2,
  1,
  2,0,0,0,
  4,0,0,0
  };

  GetChannel().SetData(expectedData);

  BrowseDescription desc;
  GetStream() >> desc;

  ASSERT_EQ(desc.NodeToBrowse.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.NodeToBrowse.TwoByteData.Identifier, 1);
  ASSERT_EQ(desc.Direction, BrowseDirection::Inverse);
  ASSERT_EQ(desc.ReferenceTypeID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.ReferenceTypeID.TwoByteData.Identifier, 2);
  ASSERT_EQ(desc.IncludeSubtypes, true);
  ASSERT_EQ(desc.NodeClasses, NODE_CLASS_VARIABLE);
  ASSERT_EQ(desc.ResultMask, REFERENCE_NODE_CLASS);
}

//-------------------------------------------------------
// BrowseRequest
//-------------------------------------------------------

OpcUa::BrowseDescription CreateBrowseDescription()
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  BrowseDescription desc;
  desc.NodeToBrowse.Encoding = EV_TWO_BYTE;
  desc.NodeToBrowse.TwoByteData.Identifier = 1;
  desc.Direction = BrowseDirection::Inverse;
  desc.ReferenceTypeID.Encoding = EV_TWO_BYTE;
  desc.ReferenceTypeID.TwoByteData.Identifier = 2;
  desc.IncludeSubtypes = true;
  desc.NodeClasses = NODE_CLASS_VARIABLE;
  desc.ResultMask = REFERENCE_NODE_CLASS;
  return desc;
}

bool operator==(const OpcUa::BrowseDescription& lhs, const OpcUa::BrowseDescription& rhs)
{
  return
    rhs.NodeToBrowse.Encoding == lhs.NodeToBrowse.Encoding &&
    rhs.NodeToBrowse.TwoByteData.Identifier == lhs.NodeToBrowse.TwoByteData.Identifier &&
    rhs.Direction == lhs.Direction &&
    rhs.ReferenceTypeID.Encoding == lhs.ReferenceTypeID.Encoding &&
    rhs.ReferenceTypeID.TwoByteData.Identifier == lhs.ReferenceTypeID.TwoByteData.Identifier &&
    rhs.IncludeSubtypes == lhs.IncludeSubtypes &&
    rhs.NodeClasses == lhs.NodeClasses &&
    rhs.ResultMask == lhs.ResultMask;
}

TEST_F(ViewSerialization, BrowseRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowseRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::BROWSE_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.Query.View.ID.Encoding = EV_TWO_BYTE;
  request.Query.View.ID.TwoByteData.Identifier = 1;
  request.Query.View.Timestamp.Value = 2;
  request.Query.View.Version = 3;

  request.Query.MaxReferenciesPerNode = 4;

  request.Query.NodesToBrowse.push_back(CreateBrowseDescription());
  request.Query.NodesToBrowse.push_back(CreateBrowseDescription());

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x0f, 0x2, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  0, 1,  // View.ID
  2,0,0,0,0,0,0,0, // View.TimeStamp
  3,0,0,0, // View.Version

  4,0,0,0, // MaxReferenciesPerNode

  2,0,0,0, // Count of Nodes
  0,1, 1,0,0,0, 0,2, 1, 2,0,0,0, 4,0,0,0, // Node 1
  0,1, 1,0,0,0, 0,2, 1, 2,0,0,0, 4,0,0,0, // Node 2


  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(ViewDeserialization, BrowseRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0x0f, 0x2, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  0, 1,
  2,0,0,0,0,0,0,0,
  3,0,0,0,

  4,0,0,0,

  2,0,0,0,
  0,1, 1,0,0,0, 0,2, 1, 2,0,0,0, 4,0,0,0,
  0,1, 1,0,0,0, 0,2, 1, 2,0,0,0, 4,0,0,0,

  };

  GetChannel().SetData(expectedData);
  BrowseRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::BROWSE_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Query.View.ID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(request.Query.View.ID.TwoByteData.Identifier, 1);
  ASSERT_EQ(request.Query.View.Timestamp, 2);
  ASSERT_EQ(request.Query.View.Version, 3);

  ASSERT_EQ(request.Query.MaxReferenciesPerNode, 4);

  ASSERT_FALSE(request.Query.NodesToBrowse.empty());

  BrowseDescription desc = CreateBrowseDescription();

  ASSERT_TRUE(request.Query.NodesToBrowse[0] == desc);
  ASSERT_TRUE(request.Query.NodesToBrowse[1] == desc);

}

//-------------------------------------------------------
// ReferenceDescription
//-------------------------------------------------------

TEST_F(ViewSerialization, ReferenceDescription)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  ReferenceDescription desc;

  desc.ReferenceTypeID.Encoding = EV_TWO_BYTE;
  desc.ReferenceTypeID.TwoByteData.Identifier = 1;

  desc.IsForward = true;

  desc.TargetNodeID.Encoding = EV_TWO_BYTE;
  desc.TargetNodeID.TwoByteData.Identifier = 2;

  desc.BrowseName.NamespaceIndex = 3;
  desc.BrowseName.Name = "name";

  desc.DisplayName.Encoding = HAS_LOCALE | HAS_TEXT;
  desc.DisplayName.Locale = "loc";
  desc.DisplayName.Text = "text";

  desc.TargetNodeClass = NodeClass::Method;

  desc.TargetNodeTypeDefinition.Encoding = EV_TWO_BYTE;
  desc.TargetNodeTypeDefinition.TwoByteData.Identifier = 5;


  GetStream() << desc << flush;

  const std::vector<char> expectedData = {
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(desc));
}

TEST_F(ViewDeserialization, ReferenceDescription)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5
  };

  GetChannel().SetData(expectedData);

  ReferenceDescription desc;

  GetStream() >> desc;

  ASSERT_EQ(desc.ReferenceTypeID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.ReferenceTypeID.TwoByteData.Identifier, 1);

  ASSERT_EQ(desc.IsForward, true);

  ASSERT_EQ(desc.TargetNodeID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.TargetNodeID.TwoByteData.Identifier, 2);

  ASSERT_EQ(desc.BrowseName.NamespaceIndex, 3);
  ASSERT_EQ(desc.BrowseName.Name, "name");

  ASSERT_EQ(desc.DisplayName.Encoding, HAS_LOCALE | HAS_TEXT);
  ASSERT_EQ(desc.DisplayName.Locale, "loc");
  ASSERT_EQ(desc.DisplayName.Text, "text");

  ASSERT_EQ(desc.TargetNodeClass, NodeClass::Method);

  ASSERT_EQ(desc.TargetNodeTypeDefinition.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.TargetNodeTypeDefinition.TwoByteData.Identifier, 5);
}

//-------------------------------------------------------
// BrowseResult
//-------------------------------------------------------

OpcUa::ReferenceDescription CreateReferenceDescription()
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;
  ReferenceDescription desc;

  desc.ReferenceTypeID.Encoding = EV_TWO_BYTE;
  desc.ReferenceTypeID.TwoByteData.Identifier = 1;

  desc.IsForward = true;

  desc.TargetNodeID.Encoding = EV_TWO_BYTE;
  desc.TargetNodeID.TwoByteData.Identifier = 2;

  desc.BrowseName.NamespaceIndex = 3;
  desc.BrowseName.Name = "name";

  desc.DisplayName.Encoding = HAS_LOCALE | HAS_TEXT;
  desc.DisplayName.Locale = "loc";
  desc.DisplayName.Text = "text";

  desc.TargetNodeClass = NodeClass::Method;

  desc.TargetNodeTypeDefinition.Encoding = EV_TWO_BYTE;
  desc.TargetNodeTypeDefinition.TwoByteData.Identifier = 5;
  return desc;
}

TEST_F(ViewSerialization, BrowseResult)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowseResult result;
  result.Status = static_cast<OpcUa::StatusCode>(1);
  result.ContinuationPoint = {2,3,4,5};
  result.Referencies.push_back(CreateReferenceDescription());

  GetStream() << result << flush;

  const std::vector<char> expectedData = {
  1,0,0,0,
  4,0,0,0, 2,3,4,5,

  1,0,0,0,
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5
  };

  ASSERT_EQ(expectedData.size(), RawSize(result));
  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
}

TEST_F(ViewDeserialization, BrowseResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1,0,0,0,
  4,0,0,0, 2,3,4,5,

  1,0,0,0,
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5
  };

  GetChannel().SetData(expectedData);

  BrowseResult result;
  GetStream() >> result;

  ASSERT_EQ(result.Status, static_cast<OpcUa::StatusCode>(1));
  std::vector<uint8_t> cont = {2,3,4,5};
  ASSERT_EQ(result.ContinuationPoint, cont);
  ASSERT_FALSE(result.Referencies.empty());

  const ReferenceDescription& desc = result.Referencies[0];
  ASSERT_EQ(desc.ReferenceTypeID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.ReferenceTypeID.TwoByteData.Identifier, 1);
  ASSERT_EQ(desc.IsForward, true);
  ASSERT_EQ(desc.TargetNodeID.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.TargetNodeID.TwoByteData.Identifier, 2);
  ASSERT_EQ(desc.BrowseName.NamespaceIndex, 3);
  ASSERT_EQ(desc.BrowseName.Name, "name");
  ASSERT_EQ(desc.DisplayName.Encoding, HAS_LOCALE | HAS_TEXT);
  ASSERT_EQ(desc.DisplayName.Locale, "loc");
  ASSERT_EQ(desc.DisplayName.Text, "text");
  ASSERT_EQ(desc.TargetNodeClass, NodeClass::Method);
  ASSERT_EQ(desc.TargetNodeTypeDefinition.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(desc.TargetNodeTypeDefinition.TwoByteData.Identifier, 5);
}

//-------------------------------------------------------
// BrowseRessponce
//-------------------------------------------------------

OpcUa::BrowseResult CreateBrowseResult()
{
  OpcUa::BrowseResult result;
  result.Status = static_cast<OpcUa::StatusCode>(1);
  result.ContinuationPoint = {2,3,4,5};
  result.Referencies.push_back(CreateReferenceDescription());
  return result;
}

TEST_F(ViewSerialization, BrowseResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowseResponse response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::BROWSE_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.Results.push_back(CreateBrowseResult());

  DiagnosticInfo diag1;
  diag1.EncodingMask = DIM_LOCALIZED_TEXT;
  diag1.LocalizedText = 4;
  DiagnosticInfo diag2;
  diag2.EncodingMask = DIM_ADDITIONAL_INFO;
  diag2.AdditionalInfo = "add";
  response.Diagnostics.push_back(diag1);
  response.Diagnostics.push_back(diag2);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x12, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // BrowseResults
  1,0,0,0,

  1,0,0,0,
  4,0,0,0, 2,3,4,5,
  1,0,0,0,
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5,

  2,0,0,0,
  // Diagnostics
  DIM_LOCALIZED_TEXT, 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(ViewDeserialization, BrowseResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0x12, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // BrowseResults
  1,0,0,0,

  1,0,0,0,
  4,0,0,0, 2,3,4,5,
  1,0,0,0,
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5,

  2,0,0,0,
  // Diagnostics
  DIM_LOCALIZED_TEXT, 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };


  GetChannel().SetData(expectedData);

  BrowseResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::BROWSE_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Results.size(), 1);
  ASSERT_EQ(response.Diagnostics.size(), 2);
}

//-------------------------------------------------------
// BrowseNextRequest
//-------------------------------------------------------


TEST_F(ViewSerialization, BrowseNextRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowseNextRequest request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::BROWSE_NEXT_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  request.ReleaseContinuationPoints = true;
  request.ContinuationPoints.push_back(std::vector<uint8_t>{1});

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x15, 0x2, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1,
  1,0,0,0, 1,0,0,0, 1
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(ViewDeserialization, BrowseNextRequest)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0x15, 0x2, // TypeID
  // RequestHeader
  TEST_REQUEST_HEADER_BINARY_DATA,

  1,
  1,0,0,0, 1,0,0,0, 1
  };

  GetChannel().SetData(expectedData);
  BrowseNextRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::BROWSE_NEXT_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.ReleaseContinuationPoints, 1);
  ASSERT_TRUE(!request.ContinuationPoints.empty());
  ASSERT_EQ(request.ContinuationPoints[0], std::vector<uint8_t>(1,1));
}

//-------------------------------------------------------
// BrowseNextResponse
//-------------------------------------------------------

TEST_F(ViewSerialization, BrowseNextResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowseNextResponse response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::BROWSE_NEXT_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  response.Results.push_back(CreateBrowseResult());
  DiagnosticInfo diag;
  diag.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO);
  diag.LocalizedText = 4;
  diag.InnerDiagnostics.reset(new DiagnosticInfo());
  diag.InnerDiagnostics->EncodingMask = DIM_ADDITIONAL_INFO;
  diag.InnerDiagnostics->AdditionalInfo = "add";
  response.Diagnostics.push_back(diag);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x18, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // BrowseResults
  1,0,0,0,

  1,0,0,0,
  4,0,0,0, 2,3,4,5,
  1,0,0,0,
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5,

  1,0,0,0,
  // Diagnostics
  static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(ViewDeserialization, BrowseNextResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0x18, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // BrowseResults
  1,0,0,0,

  1,0,0,0,
  4,0,0,0, 2,3,4,5,
  1,0,0,0,
  0, 1,
  1,
  0, 2,
  3,0, 4,0,0,0, 'n','a','m','e',
  3,
  3,0,0,0, 'l','o','c',
  4,0,0,0, 't','e','x','t',
  4,0,0,0,
  0, 5,

  1,0,0,0,
  // Diagnostics
  static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };


  GetChannel().SetData(expectedData);

  BrowseNextResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::BROWSE_NEXT_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Results.size(), 1);
  ASSERT_EQ(response.Diagnostics.size(), 1);
}

//-------------------------------------------------------
// BrowsePathTarget
//-------------------------------------------------------

TEST_F(ViewSerialization, BrowsePathTarget)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowsePathTarget target;

  target.Node.Encoding = EV_TWO_BYTE;
  target.Node.TwoByteData.Identifier = 1;
  target.RemainingPathIndex = 2;

  GetStream() << target << flush;

  const std::vector<char> expectedData = {
  0, 1,
  2,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(target));
}

TEST_F(ViewDeserialization, BrowsePathTarget)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  0, 1,
  2,0,0,0,
  };

  GetChannel().SetData(expectedData);

  BrowsePathTarget target;

  GetStream() >> target;

  ASSERT_EQ(target.Node.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(target.Node.TwoByteData.Identifier, 1);
  ASSERT_EQ(target.RemainingPathIndex, 2);
}

//-------------------------------------------------------
// BrowsePathTarget
//-------------------------------------------------------

TEST_F(ViewSerialization, BrowsePathResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;


  BrowsePathTarget target;
  target.Node.Encoding = EV_TWO_BYTE;
  target.Node.TwoByteData.Identifier = 1;
  target.RemainingPathIndex = 2;

  BrowsePathResult result;
  result.Status = static_cast<StatusCode>(3);
  result.Targets.push_back(target);

  GetStream() << result << flush;

  const std::vector<char> expectedData = {
  3,0,0,0,
  1,0,0,0,
  0, 1,
  2,0,0,0,
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(result));
}

TEST_F(ViewDeserialization, BrowsePathResult)
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  3,0,0,0,
  1,0,0,0,
  0, 1,
  2,0,0,0,
  };

  GetChannel().SetData(expectedData);

  BrowsePathResult result;

  GetStream() >> result;

  ASSERT_EQ(result.Status, static_cast<StatusCode>(3));
  ASSERT_EQ(result.Targets.size(), 1);
  ASSERT_EQ(result.Targets[0].Node.Encoding, EV_TWO_BYTE);
  ASSERT_EQ(result.Targets[0].Node.TwoByteData.Identifier, 1);
  ASSERT_EQ(result.Targets[0].RemainingPathIndex, 2);
}

//-------------------------------------------------------
// TranslateBrowsePathsResult
//-------------------------------------------------------

TEST_F(ViewSerialization, TranslateBrowsePathsResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;


  BrowsePathTarget target;
  target.Node.Encoding = EV_TWO_BYTE;
  target.Node.TwoByteData.Identifier = 1;
  target.RemainingPathIndex = 2;

  BrowsePathResult result;
  result.Status = static_cast<StatusCode>(3);
  result.Targets.push_back(target);

  TranslateBrowsePathsResult translateResult;
  translateResult.Paths.push_back(result);

  GetStream() << translateResult << flush;

  const std::vector<char> expectedData = {
  1,0,0,0, // Count of results
  3,0,0,0, // StatusCode
  1,0,0,0, // TargetsCount
  0, 1,    // TargetNode
  2,0,0,0, // Index

  0,0,0,0, // Count of Diagnostics
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(translateResult));
}

TEST_F(ViewDeserialization, TranslateBrowsePathsResult)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1,0,0,0, // Count of results
  3,0,0,0, // StatusCode
  1,0,0,0, // TargetsCount
  0, 1,    // TargetNode
  2,0,0,0, // Index

  0,0,0,0, // Count of Diagnostics
  };

  GetChannel().SetData(expectedData);

  TranslateBrowsePathsResult translateResult;

  GetStream() >> translateResult;

  ASSERT_EQ(translateResult.Paths.size(), 1);
  ASSERT_EQ(translateResult.Diagnostics.size(), 0);
}

//-------------------------------------------------------
// TranslateBrowsePathsToNodeIDsResponse
//-------------------------------------------------------

TEST_F(ViewSerialization, TranslateBrowsePathsToNodeIDsResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  BrowsePathTarget target;
  target.Node.Encoding = EV_TWO_BYTE;
  target.Node.TwoByteData.Identifier = 1;
  target.RemainingPathIndex = 2;

  BrowsePathResult result;
  result.Status = static_cast<StatusCode>(3);
  result.Targets.push_back(target);

  TranslateBrowsePathsToNodeIDsResponse response;
  response.Result.Paths.push_back(result);

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_RESPONSE);

  FILL_TEST_RESPONSE_HEADER(response.Header);

  DiagnosticInfo diag;
  diag.EncodingMask = static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO);
  diag.LocalizedText = 4;
  diag.InnerDiagnostics.reset(new DiagnosticInfo());
  diag.InnerDiagnostics->EncodingMask = DIM_ADDITIONAL_INFO;
  diag.InnerDiagnostics->AdditionalInfo = "add";
  response.Result.Diagnostics.push_back(diag);

  GetStream() << response << flush;

  const std::vector<char> expectedData = {
  1, 0, (char)0x2D, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // BrowseResults
  1,0,0,0, // Count of results
  3,0,0,0, // StatusCode
  1,0,0,0, // TargetsCount
  0, 1,    // TargetNode
  2,0,0,0, // Index

  // Diagnostics
  1,0,0,0, // Count
  static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd', \
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) << PrintData(GetChannel().SerializedData) << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(response));
}

TEST_F(ViewDeserialization, TranslateBrowsePathsToNodeIDsResponse)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
  1, 0, (char)0x2D, 0x2, // TypeID
  // RequestHeader
  TEST_RESPONSE_HEADER_BINARY_DATA,

  // BrowseResults
  1,0,0,0, // Count of results
  3,0,0,0, // StatusCode
  1,0,0,0, // TargetsCount
  0, 1,    // TargetNode
  2,0,0,0, // Index

  // Diagnostics
  1,0,0,0, // Count
  static_cast<DiagnosticInfoMask>(DIM_LOCALIZED_TEXT | DIM_INNER_DIAGNOSTIC_INFO), 4,0,0,0, \
  DIM_ADDITIONAL_INFO, 3, 0, 0, 0, 'a', 'd', 'd',
  };

  GetChannel().SetData(expectedData);

  TranslateBrowsePathsToNodeIDsResponse response;
  GetStream() >> response;

  ASSERT_EQ(response.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(response.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(response.TypeID.FourByteData.Identifier, OpcUa::TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_RESPONSE);

  ASSERT_RESPONSE_HEADER_EQ(response.Header);

  ASSERT_EQ(response.Result.Paths.size(), 1);
  ASSERT_EQ(response.Result.Diagnostics.size(), 1);
}

//-------------------------------------------------------
// TranslateBrowsePathsToNodeIDsRequest
//-------------------------------------------------------

TEST_F(ViewSerialization, TranslateBrowsePathsToNodeIDsRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  RelativePathElement element;
  element.ReferenceTypeID = OpcUa::TwoByteNodeID(1);
  element.IsInverse = true;
  element.IncludeSubtypes = true;
  element.TargetName.NamespaceIndex = 2;
  element.TargetName.Name = "name";

  BrowsePath browse;
  browse.StartingNode = OpcUa::TwoByteNodeID(2);
  browse.Path.Elements.push_back(element);

  TranslateBrowsePathsToNodeIDsRequest request;
  request.Parameters.BrowsePaths.push_back(browse);

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_REQUEST);

  FILL_TEST_REQUEST_HEADER(request.Header);

  GetStream() << request << flush;

  const std::vector<char> expectedData = {
    1, 0, (char)0x2A, 0x2, // TypeID
    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    1,0,0,0, // Number of BrowsePaths
    0,2,
    1,0,0,0, // Number of Elements
    0,1,     // Reference Type (Two Byte Node ID)
    1,       // IsInverse
    1,       // IncludeSubTypes
    2,0, 4,0,0,0, 'n','a','m','e', // TargetName
  };

  ASSERT_EQ(expectedData, GetChannel().SerializedData) <<
      "Serialized: " << std::endl << PrintData(GetChannel().SerializedData) << std::endl <<
      "Expected" << std::endl << PrintData(expectedData);
  ASSERT_EQ(expectedData.size(), RawSize(request));
}

TEST_F(ViewDeserialization, TranslateBrowsePathsToNodeIDsRequest)
{
  using namespace OpcUa;
  using namespace OpcUa::Binary;

  const std::vector<char> expectedData = {
    1, 0, (char)0x2A, 0x2, // TypeID
    // RequestHeader
    TEST_REQUEST_HEADER_BINARY_DATA,

    1,0,0,0, // Number of BrowsePaths
    0,2,     // Starting node
    1,0,0,0, // Number of Elements
    0,1,     // Reference Type (Two Byte Node ID)
    1,       // IsInverse
    1,       // IncludeSubTypes
    2,0, 4,0,0,0, 'n','a','m','e', // TargetName
  };

  GetChannel().SetData(expectedData);

  TranslateBrowsePathsToNodeIDsRequest request;
  GetStream() >> request;

  ASSERT_EQ(request.TypeID.Encoding, EV_FOUR_BYTE);
  ASSERT_EQ(request.TypeID.FourByteData.NamespaceIndex, 0);
  ASSERT_EQ(request.TypeID.FourByteData.Identifier, OpcUa::TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_REQUEST);

  ASSERT_REQUEST_HEADER_EQ(request.Header);

  ASSERT_EQ(request.Parameters.BrowsePaths.size(), 1);
  const BrowsePath& browsePath = request.Parameters.BrowsePaths[0];
  ASSERT_EQ(browsePath.StartingNode, OpcUa::TwoByteNodeID(2));
  ASSERT_EQ(browsePath.Path.Elements.size(), 1);

  const RelativePathElement& element = browsePath.Path.Elements[0];
  ASSERT_TRUE(element.IncludeSubtypes);
  ASSERT_TRUE(element.IsInverse);
  ASSERT_EQ(element.ReferenceTypeID, OpcUa::TwoByteNodeID(1));
  ASSERT_EQ(element.TargetName.NamespaceIndex, 2);
  ASSERT_EQ(element.TargetName.Name, "name");
}
