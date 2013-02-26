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

#include <opc/ua/binary/protocol/common.h>

namespace OpcUa
{

  //---------------------------------------------------
  // Browse
  //---------------------------------------------------

  enum class BrowseDirection : uint32_t
  {
    FORWARD = 0,
    INVERSE = 1,
    BOTH    = 2,
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
    NodeID ID;
    DateTime Timestamp;
    uint32_t Version;

    ViewDescription();
  };

  struct BrowseDescription
  {
    NodeID NodeToBrowse;
    BrowseDirection Direction;
    NodeID ReferenceTypeID;
    bool IncludeSubtypes;
    uint32_t NodeClasses;
    uint32_t ResultMask;

    BrowseDescription();
  };

  struct BrowseRequest
  {
    NodeID TypeID;
    RequestHeader Header;

    ViewDescription View;
    uint32_t MaxReferenciesPerNode;
    std::vector<BrowseDescription> NodesToBrowse;

    BrowseRequest();
  };


  struct ReferenceDescription
  {
    NodeID TypeID;
    bool IsForward;
    NodeID TargetNodeID;
    QualifiedName BrowseName;
    LocalizedText DisplayName;
    uint32_t TargetNodeClass; 
    NodeID TypeDefinition;

    ReferenceDescription();
  };

  struct BrowseResult
  {
    uint32_t StatusCode;
    std::vector<uint8_t> ContinuationPoint;
    std::vector<ReferenceDescription> Referencies;

    BrowseResult();
  };

  struct BrowseResponse
  {
    NodeID TypeID;
    ResponseHeader Header;

    std::vector<BrowseResult> Results;
    std::vector<DiagnosticInfo> Diagnostics;

    BrowseResponse();
  };


  struct BrowseNextRequest
  {
    NodeID TypeID;
    RequestHeader Header;

    bool ReleaseContinuationPoints;
    std::vector<std::vector<uint8_t>> ContinuationPoints;

    BrowseNextRequest();
  };

  struct BrowseNextResponse
  {
    NodeID TypeID;
    ResponseHeader Header;

    std::vector<BrowseResult> Results;
    std::vector<DiagnosticInfo> Diagnostics;

    BrowseNextResponse();
  };

} // namespace OpcUa

#endif // __OPC_UA_BINARY_VIEW_H__
