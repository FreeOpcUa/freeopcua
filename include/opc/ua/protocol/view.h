/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. View service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_VIEW_H__
#define __OPC_UA_BINARY_VIEW_H__

#include <opc/ua/protocol/node_classes.h>
#include <opc/ua/protocol/types.h>

namespace OpcUa
{

  //---------------------------------------------------
  // Browse
  //---------------------------------------------------

  enum class BrowseDirection : uint32_t
  {
    Forward = 0,
    Inverse = 1,
    Both    = 2,
  };

  const uint32_t NODE_CLASS_ALL            = 0;
  const uint32_t NODE_CLASS_OBJECT         = 1;
  const uint32_t NODE_CLASS_VARIABLE       = 2;
  const uint32_t NODE_CLASS_METHOD         = 4;
  const uint32_t NODE_CLASS_OBJECT_TYPE    = 8;
  const uint32_t NODE_CLASS_VARIABLE_TYPE  = 16;
  const uint32_t NODE_CLASS_REFERENCE_TYPE = 32;
  const uint32_t NODE_CLASS_DATA_TYPE      = 64;
  const uint32_t NODE_CLASS_VIEW           = 128;


  const uint32_t REFERENCE_NONE            = 0;
  const uint32_t REFERENCE_TYPE            = 1;
  const uint32_t REFERENCE_IS_FORWARD      = 2;
  const uint32_t REFERENCE_NODE_CLASS      = 4;
  const uint32_t REFERENCE_BROWSE_NAME     = 8;
  const uint32_t REFERENCE_DISPLAY_NAME    = 16;
  const uint32_t REFERENCE_TYPE_DEFINITION = 32;
  const uint32_t REFERENCE_ALL             = 63;

  struct ViewDescription
  {
    NodeId Id;
    DateTime Timestamp;
    uint32_t Version;

    ViewDescription();
  };

  struct BrowseDescription
  {
    NodeId NodeToBrowse;
    BrowseDirection Direction;
    NodeId ReferenceTypeId;
    bool IncludeSubtypes; // If false will follow strctly ReferenceTypeId. if true will follow subtypes of ReferenceTypeId.
    uint32_t NodeClasses;
    uint32_t ResultMask;

    BrowseDescription();
  };

  struct NodesQuery
  {
    ViewDescription View;
    uint32_t MaxReferenciesPerNode;
    std::vector<BrowseDescription> NodesToBrowse;

    NodesQuery();
  };

  struct BrowseRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    NodesQuery Query;

    BrowseRequest();
  };


  struct ReferenceDescription
  {
    NodeId ReferenceTypeId;
    bool IsForward = true;
    NodeId TargetNodeId;
    QualifiedName BrowseName;
    LocalizedText DisplayName;
    NodeClass TargetNodeClass;
    NodeId TargetNodeTypeDefinition;

    ReferenceDescription();
  };

  struct BrowseResult
  {
    StatusCode Status;
    std::vector<uint8_t> ContinuationPoint;
    std::vector<ReferenceDescription> Referencies;

    BrowseResult();
  };

  struct BrowseResponse
  {
    NodeId TypeId;
    ResponseHeader Header;

    std::vector<BrowseResult> Results;
    DiagnosticInfoList Diagnostics;

    BrowseResponse();
  };

  //---------------------------------------------------
  // BrowseNext
  //---------------------------------------------------

  struct BrowseNextRequest
  {
    NodeId TypeId;
    RequestHeader Header;

    bool ReleaseContinuationPoints;
    std::vector<std::vector<uint8_t>> ContinuationPoints;

    BrowseNextRequest();
  };

  struct BrowseNextResponse
  {
    NodeId TypeId;
    ResponseHeader Header;

    std::vector<BrowseResult> Results;
    DiagnosticInfoList Diagnostics;

    BrowseNextResponse();
  };

  //---------------------------------------------------
  // TranslateBrowsePathsToNodeIds
  //---------------------------------------------------

  struct BrowsePath
  {
    NodeId StartingNode;
    RelativePath Path;
  };

  struct TranslateBrowsePathsParameters
  {
    std::vector<BrowsePath> BrowsePaths;
  };

  struct TranslateBrowsePathsToNodeIdsRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    TranslateBrowsePathsParameters Parameters;

    TranslateBrowsePathsToNodeIdsRequest();
  };

  //---------------------------------------------------
  // TranslateBrowsePathsToNodeIds
  //---------------------------------------------------

  struct BrowsePathTarget
  {
    NodeId Node;
    uint32_t RemainingPathIndex;

    BrowsePathTarget();
  };

  struct BrowsePathResult
  {
    StatusCode Status;
    std::vector<BrowsePathTarget> Targets;

    BrowsePathResult();
  };

  struct TranslateBrowsePathsResult
  {
    std::vector<BrowsePathResult> Paths;
    DiagnosticInfoList Diagnostics;
  };

  struct TranslateBrowsePathsToNodeIdsResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    TranslateBrowsePathsResult Result;

    TranslateBrowsePathsToNodeIdsResponse();
  };

} // namespace OpcUa

#endif // __OPC_UA_BINARY_VIEW_H__
