// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary session services.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/view.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/extension_identifiers.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
  ////////////////////////////////////////////////////////////////////////////
  // NodesDescription
  ////////////////////////////////////////////////////////////////////////////

  NodesQuery::NodesQuery()
    : MaxReferenciesPerNode(0)
  {
  }

  ////////////////////////////////////////////////////////////////////////////
  // ViewDescription
  ////////////////////////////////////////////////////////////////////////////

  ViewDescription::ViewDescription()
    : Timestamp(0)
    , Version(0)
  {
  }

  BrowseDescription::BrowseDescription()
    : Direction(BrowseDirection::Both)
    , IncludeSubtypes(false)
    , NodeClasses(NODE_CLASS_ALL)
    , ResultMask(REFERENCE_ALL)
  {
  }

  BrowseRequest::BrowseRequest()
    : TypeID(BROWSE_REQUEST)
  {
  }

  TranslateBrowsePathsToNodeIDsRequest::TranslateBrowsePathsToNodeIDsRequest()
    : TypeID(TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_REQUEST)
  {
  }

  ReferenceDescription::ReferenceDescription()
    : IsForward(false)
    , TargetNodeClass(NodeClass::All)
  {
  }

  BrowseResult::BrowseResult()
    : Status(StatusCode::Good)
  {
  }

  BrowseResponse::BrowseResponse()
    : TypeID(BROWSE_RESPONSE)
  {
  }

  BrowseNextRequest::BrowseNextRequest()
    : TypeID(BROWSE_NEXT_REQUEST)
    , ReleaseContinuationPoints(false)
  {
  }

  BrowseNextResponse::BrowseNextResponse()
    : TypeID(BROWSE_NEXT_RESPONSE)
  {
  }

  BrowsePathTarget::BrowsePathTarget()
    : RemainingPathIndex(0)
  {
  }

  BrowsePathResult::BrowsePathResult()
    : Status(StatusCode::Good)
  {
  }

  TranslateBrowsePathsToNodeIDsResponse::TranslateBrowsePathsToNodeIDsResponse()
    : TypeID(TRANSLATE_BROWSE_PATHS_TO_NODE_IDS_RESPONSE)
  {
  }

  namespace Binary
  {
    ////////////////////////////////////////////////////////////////
    // NodeClass
    ////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<NodeClass>(const NodeClass&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<NodeClass>(const NodeClass& cls)
    {
      *this << static_cast<uint32_t>(cls);
    }

    template<>
    void DataDeserializer::Deserialize<NodeClass>(NodeClass& direction)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      direction = static_cast<NodeClass>(tmp);
    }


    ////////////////////////////////////////////////////////////////
    // BrowseDirection
    ////////////////////////////////////////////////////////////////


    template<>
    std::size_t RawSize<BrowseDirection>(const BrowseDirection&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<BrowseDirection>(const BrowseDirection& direction)
    {
      *this << static_cast<uint32_t>(direction);
    }

    template<>
    void DataDeserializer::Deserialize<BrowseDirection>(BrowseDirection& direction)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      direction = static_cast<BrowseDirection>(tmp);
    }

    //---------------------------------------------------
    // ViewDescription
    //---------------------------------------------------

    template<>
    std::size_t RawSize<ViewDescription>(const ViewDescription& desc)
    {
      return RawSize(desc.ID) + RawSize(desc.Timestamp) + RawSize(desc.Version);
    }

    template<>
    void DataSerializer::Serialize<ViewDescription>(const ViewDescription& desc)
    {
      *this << desc.ID;
      *this << desc.Timestamp;
      *this << desc.Version;
    }

    template<>
    void DataDeserializer::Deserialize<ViewDescription>(ViewDescription& desc)
    {
      *this >> desc.ID;
      *this >> desc.Timestamp;
      *this >> desc.Version;
    }


    //---------------------------------------------------
    // BrowseDescription
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseDescription>(const BrowseDescription& desc)
    {
      return RawSize(desc.NodeToBrowse) +
             RawSize(desc.Direction) +
             RawSize(desc.ReferenceTypeID) +
             RawSize(desc.IncludeSubtypes) +
             RawSize(desc.NodeClasses) +
             RawSize(desc.ResultMask);
    }

    template<>
    void DataSerializer::Serialize<BrowseDescription>(const BrowseDescription& desc)
    {
      *this << desc.NodeToBrowse;
      *this << desc.Direction;
      *this << desc.ReferenceTypeID;
      *this << desc.IncludeSubtypes;
      *this << desc.NodeClasses;
      *this << desc.ResultMask;
    }

    template<>
    void DataDeserializer::Deserialize<BrowseDescription>(BrowseDescription& desc)
    {
      *this >> desc.NodeToBrowse;
      *this >> desc.Direction;
      *this >> desc.ReferenceTypeID;
      *this >> desc.IncludeSubtypes;
      *this >> desc.NodeClasses;
      *this >> desc.ResultMask;
    }

    ////////////////////////////////////////////////////////////////////////////
    // NodesDescription
    ////////////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<NodesQuery>(const NodesQuery& desc)
    {
      return RawSize(desc.View) + RawSize(desc.MaxReferenciesPerNode) + RawSizeContainer(desc.NodesToBrowse);
    }

    template<>
    void DataSerializer::Serialize<NodesQuery>(const NodesQuery& desc)
    {
      *this << desc.View;
      *this << desc.MaxReferenciesPerNode;
      SerializeContainer(*this, desc.NodesToBrowse);
    }

    template<>
    void DataDeserializer::Deserialize<NodesQuery>(NodesQuery& desc)
    {
      *this >> desc.View;
      *this >> desc.MaxReferenciesPerNode;
      DeserializeContainer(*this, desc.NodesToBrowse);
    }

    //---------------------------------------------------
    // BrowseRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseRequest>(const BrowseRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Query);
    }

    template<>
    void DataSerializer::Serialize<BrowseRequest>(const BrowseRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Query;
    }

    template<>
    void DataDeserializer::Deserialize<BrowseRequest>(BrowseRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Query;
    }

    //---------------------------------------------------
    // ReferenceDescription
    //---------------------------------------------------

    template<>
    std::size_t RawSize<ReferenceDescription>(const ReferenceDescription& desc)
    {
      return RawSize(desc.ReferenceTypeID) +
             RawSize(desc.IsForward) +
             RawSize(desc.TargetNodeID) +
             RawSize(desc.BrowseName) +
             RawSize(desc.DisplayName) +
             RawSize(desc.TargetNodeClass) +
             RawSize(desc.TargetNodeTypeDefinition);
    }

    template<>
    void DataSerializer::Serialize<ReferenceDescription>(const ReferenceDescription& desc)
    {
      *this << desc.ReferenceTypeID;
      *this << desc.IsForward;
      *this << desc.TargetNodeID;
      *this << desc.BrowseName;
      *this << desc.DisplayName;
      *this << desc.TargetNodeClass;
      *this << desc.TargetNodeTypeDefinition;
    }

    template<>
    void DataDeserializer::Deserialize<ReferenceDescription>(ReferenceDescription& desc)
    {
      *this >> desc.ReferenceTypeID;
      *this >> desc.IsForward;
      *this >> desc.TargetNodeID;
      *this >> desc.BrowseName;
      *this >> desc.DisplayName;
      *this >> desc.TargetNodeClass;
      *this >> desc.TargetNodeTypeDefinition;
    }

    //---------------------------------------------------
    // BrowseResult
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseResult>(const BrowseResult& result)
    {
      return RawSize(result.Status) +
             RawSizeContainer(result.ContinuationPoint) +
             RawSizeContainer(result.Referencies);
    }

    template<>
    void DataSerializer::Serialize<BrowseResult>(const BrowseResult& result)
    {
      *this << result.Status;
      SerializeContainer(*this, result.ContinuationPoint);
      SerializeContainer(*this, result.Referencies);
    }

    template<>
    void DataDeserializer::Deserialize<BrowseResult>(BrowseResult& result)
    {
      *this >> result.Status;
      DeserializeContainer(*this, result.ContinuationPoint);
      DeserializeContainer(*this, result.Referencies);
    }

    //---------------------------------------------------
    // BrowseResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseResponse>(const BrowseResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) +
             RawSizeContainer(response.Results) +
             RawSizeContainer(response.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<BrowseResponse>(const BrowseResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;

      SerializeContainer(*this, response.Results, 0);
      SerializeContainer(*this, response.Diagnostics, 0);
    }

    template<>
    void DataDeserializer::Deserialize<BrowseResponse>(BrowseResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;

      DeserializeContainer(*this, response.Results);
      DeserializeContainer(*this, response.Diagnostics);
    }

    //---------------------------------------------------
    // BrowseNextRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseNextRequest>(const BrowseNextRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) +
        RawSize(request.ReleaseContinuationPoints) +
        RawSize(request.ContinuationPoints);
    }

    template<>
    void DataSerializer::Serialize<BrowseNextRequest>(const BrowseNextRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      *this << request.ReleaseContinuationPoints;
      SerializeContainer(*this, request.ContinuationPoints);
    }

    template<>
    void DataDeserializer::Deserialize<BrowseNextRequest>(BrowseNextRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;

      *this >> request.ReleaseContinuationPoints;
      DeserializeContainer(*this, request.ContinuationPoints);
    }

    //---------------------------------------------------
    // BrowseNextResponse
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseNextResponse>(const BrowseNextResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) +
             RawSizeContainer(response.Results) +
             RawSizeContainer(response.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<BrowseNextResponse>(const BrowseNextResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;

      SerializeContainer(*this, response.Results, 0);
      SerializeContainer(*this, response.Diagnostics, 0);
    }

    template<>
    void DataDeserializer::Deserialize<BrowseNextResponse>(BrowseNextResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;

      DeserializeContainer(*this, response.Results);
      DeserializeContainer(*this, response.Diagnostics);
    }

    ////////////////////////////////////////////////////////////////////
    // BrowsePathTarget
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<BrowsePathTarget>(const BrowsePathTarget& target)
    {
      return RawSize(target.Node) + RawSize(target.RemainingPathIndex);
    }

    template<>
    void DataSerializer::Serialize<BrowsePathTarget>(const BrowsePathTarget& target)
    {
      *this << target.Node;
      *this << target.RemainingPathIndex;
    }

    template<>
    void DataDeserializer::Deserialize<BrowsePathTarget>(BrowsePathTarget& target)
    {
      *this >> target.Node;
      *this >> target.RemainingPathIndex;
    }

    template<>
    void DataSerializer::Serialize<std::vector<BrowsePathTarget>>(const std::vector<BrowsePathTarget>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<BrowsePathTarget>>(std::vector<BrowsePathTarget>& targets)
    {
      DeserializeContainer(*this, targets);
    }

    ////////////////////////////////////////////////////////////////////
    // BrowsePathResult
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<BrowsePathResult>(const BrowsePathResult& result)
    {
      return RawSize(result.Status) + RawSizeContainer(result.Targets);
    }

    template<>
    void DataSerializer::Serialize<BrowsePathResult>(const BrowsePathResult& result)
    {
      *this << result.Status;
      *this << result.Targets;
    }

    template<>
    void DataDeserializer::Deserialize<BrowsePathResult>(BrowsePathResult& result)
    {
      *this >> result.Status;
      *this >> result.Targets;
    }

    template<>
    void DataSerializer::Serialize<std::vector<BrowsePathResult>>(const std::vector<BrowsePathResult>& results)
    {
      SerializeContainer(*this, results);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<BrowsePathResult>>(std::vector<BrowsePathResult>& results)
    {
      DeserializeContainer(*this, results);
    }

    ////////////////////////////////////////////////////////////////////
    // TranslateBrowsePathsResult
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<TranslateBrowsePathsResult>(const TranslateBrowsePathsResult& result)
    {
      return RawSizeContainer(result.Paths) + RawSizeContainer(result.Diagnostics);
    }

    template<>
    void DataSerializer::Serialize<TranslateBrowsePathsResult>(const TranslateBrowsePathsResult& result)
    {
      *this << result.Paths;
      *this << result.Diagnostics;
    }

    template<>
    void DataDeserializer::Deserialize<TranslateBrowsePathsResult>(TranslateBrowsePathsResult& result)
    {
      *this >> result.Paths;
      *this >> result.Diagnostics;
    }

    ////////////////////////////////////////////////////////////////////
    // TranslateBrowsePathsToNodeIDsResponse
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<TranslateBrowsePathsToNodeIDsResponse>(const TranslateBrowsePathsToNodeIDsResponse& response)
    {
      return RawSize(response.TypeID) + RawSize(response.Header) + RawSize(response.Result);
    }

    template<>
    void DataSerializer::Serialize<TranslateBrowsePathsToNodeIDsResponse>(const TranslateBrowsePathsToNodeIDsResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;
      *this << response.Result;
    }

    template<>
    void DataDeserializer::Deserialize<TranslateBrowsePathsToNodeIDsResponse>(TranslateBrowsePathsToNodeIDsResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;
      *this >> response.Result;
    }


    ////////////////////////////////////////////////////////////////////
    // RelativePathElement
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<RelativePathElement>(const RelativePathElement& element)
    {
      return RawSize(element.IncludeSubtypes) + RawSize(element.IsInverse) + RawSize(element.ReferenceTypeID) + RawSize(element.TargetName);
    }

    template<>
    void DataSerializer::Serialize<RelativePathElement>(const RelativePathElement& path)
    {
      *this << path.ReferenceTypeID;
      *this << path.IsInverse;
      *this << path.IncludeSubtypes;
      *this << path.TargetName;
    }

    template<>
    void DataDeserializer::Deserialize<RelativePathElement>(RelativePathElement& path)
    {
      *this >> path.ReferenceTypeID;
      *this >> path.IsInverse;
      *this >> path.IncludeSubtypes;
      *this >> path.TargetName;
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<RelativePathElement>>(std::vector<RelativePathElement>& targets)
    {
      DeserializeContainer(*this, targets);
    }

    template<>
    void DataSerializer::Serialize<std::vector<RelativePathElement>>( const std::vector<RelativePathElement>& targets)
    {
      SerializeContainer(*this, targets);
    }

    ////////////////////////////////////////////////////////////////////
    // RelativePath
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<RelativePath>(const RelativePath& rpath)
    {
      return RawSizeContainer(rpath.Elements);
    }

    template<>
    void DataSerializer::Serialize<RelativePath>(const RelativePath& path)
    {
      *this << path.Elements;
    }

    template<>
    void DataDeserializer::Deserialize<RelativePath>(RelativePath& path)
    {
      *this >> path.Elements;
    }

    ////////////////////////////////////////////////////////////////////
    // BrowsePath
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<BrowsePath>(const BrowsePath& path)
    {
      return RawSize(path.StartingNode) + RawSize(path.Path);
    }

    template<>
    void DataDeserializer::Deserialize<BrowsePath>(BrowsePath& path)
    {
      *this >> path.StartingNode;
      *this >> path.Path;
    }

    template<>
    void DataSerializer::Serialize<BrowsePath>(const BrowsePath& path)
    {
      *this << path.StartingNode;
      *this << path.Path;
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<BrowsePath>>(std::vector<BrowsePath>& paths)
    {
      DeserializeContainer(*this, paths);
    }

    template<>
    void DataSerializer::Serialize<std::vector<BrowsePath>>(const std::vector<BrowsePath>& paths)
    {
      SerializeContainer(*this, paths);
    }

    ////////////////////////////////////////////////////////////////////
    // TranslateBrowsePathsParameters
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<TranslateBrowsePathsParameters>(const TranslateBrowsePathsParameters& params)
    {
      return RawSizeContainer(params.BrowsePaths);
    }

    template<>
    void DataSerializer::Serialize<TranslateBrowsePathsParameters>(const TranslateBrowsePathsParameters& params)
    {
      *this << params.BrowsePaths;
    }

    template<>
    void DataDeserializer::Deserialize<TranslateBrowsePathsParameters>(TranslateBrowsePathsParameters& path)
    {
      *this >> path.BrowsePaths;
    }

    ////////////////////////////////////////////////////////////////////
    // TranslateBrowsePathsToNodeIDsRequest
    ////////////////////////////////////////////////////////////////////

    template<>
    std::size_t RawSize<TranslateBrowsePathsToNodeIDsRequest>(const TranslateBrowsePathsToNodeIDsRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Parameters)  ;
    }

    template<>
    void DataSerializer::Serialize<TranslateBrowsePathsToNodeIDsRequest>(const TranslateBrowsePathsToNodeIDsRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<TranslateBrowsePathsToNodeIDsRequest>(TranslateBrowsePathsToNodeIDsRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

  } // namespace Binary
} // namespace OpcUa
