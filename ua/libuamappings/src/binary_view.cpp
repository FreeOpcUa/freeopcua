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
#include <opc/ua/extension_identifiers.h>

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
    : Direction(BrowseDirection::Forward)
    , IncludeSubtypes(false)
    , NodeClasses(NODE_CLASS_ALL)
    , ResultMask(REFERENCE_ALL)
  {
  }

  BrowseRequest::BrowseRequest()
    : TypeID(BROWSE_REQUEST)
  {
  }

  ReferenceDescription::ReferenceDescription()
    : IsForward(false)
    , TargetNodeClass(NodeClass::All)
  {
  }

  BrowseResult::BrowseResult()
    : StatusCode(0)
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
    void OStream::Serialize<NodeClass>(const NodeClass& cls)
    {
      *this << static_cast<uint32_t>(cls);
    }

    template<>
    void IStream::Deserialize<NodeClass>(NodeClass& direction)
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
    void OStream::Serialize<BrowseDirection>(const BrowseDirection& direction)
    {
      *this << static_cast<uint32_t>(direction);
    }

    template<>
    void IStream::Deserialize<BrowseDirection>(BrowseDirection& direction)
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
    void OStream::Serialize<ViewDescription>(const ViewDescription& desc)
    {
      *this << desc.ID;
      *this << desc.Timestamp;
      *this << desc.Version;
    }

    template<>
    void IStream::Deserialize<ViewDescription>(ViewDescription& desc)
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
    void OStream::Serialize<BrowseDescription>(const BrowseDescription& desc)
    {
      *this << desc.NodeToBrowse;
      *this << desc.Direction;
      *this << desc.ReferenceTypeID;
      *this << desc.IncludeSubtypes;
      *this << desc.NodeClasses;
      *this << desc.ResultMask;
    }

    template<>
    void IStream::Deserialize<BrowseDescription>(BrowseDescription& desc)
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
    void OStream::Serialize<NodesQuery>(const NodesQuery& desc)
    {
      *this << desc.View;
      *this << desc.MaxReferenciesPerNode;
      SerializeContainer(*this, desc.NodesToBrowse);
    }

    template<>
    void IStream::Deserialize<NodesQuery>(NodesQuery& desc)
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
    void OStream::Serialize<BrowseRequest>(const BrowseRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Query;
    }

    template<>
    void IStream::Deserialize<BrowseRequest>(BrowseRequest& request)
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
      return RawSize(desc.TypeID) +
             RawSize(desc.IsForward) +
             RawSize(desc.TargetNodeID) +
             RawSize(desc.BrowseName) +
             RawSize(desc.DisplayName) +
             RawSize(desc.TargetNodeClass) +
             RawSize(desc.TypeDefinition);
    }

    template<>
    void OStream::Serialize<ReferenceDescription>(const ReferenceDescription& desc)
    {
      *this << desc.TypeID;
      *this << desc.IsForward;
      *this << desc.TargetNodeID;
      *this << desc.BrowseName;
      *this << desc.DisplayName;
      *this << desc.TargetNodeClass;
      *this << desc.TypeDefinition;
    }

    template<>
    void IStream::Deserialize<ReferenceDescription>(ReferenceDescription& desc)
    {
      *this >> desc.TypeID;
      *this >> desc.IsForward;
      *this >> desc.TargetNodeID;
      *this >> desc.BrowseName;
      *this >> desc.DisplayName;
      *this >> desc.TargetNodeClass;
      *this >> desc.TypeDefinition;
    }

    //---------------------------------------------------
    // BrowseResult
    //---------------------------------------------------

    template<>
    std::size_t RawSize<BrowseResult>(const BrowseResult& result)
    {
      return RawSize(result.StatusCode) +
             RawSizeContainer(result.ContinuationPoint) +
             RawSizeContainer(result.Referencies);
    }

    template<>
    void OStream::Serialize<BrowseResult>(const BrowseResult& result)
    {
      *this << result.StatusCode;
      SerializeContainer(*this, result.ContinuationPoint);
      SerializeContainer(*this, result.Referencies);
    }

    template<>
    void IStream::Deserialize<BrowseResult>(BrowseResult& result)
    {
      *this >> result.StatusCode;
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
    void OStream::Serialize<BrowseResponse>(const BrowseResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;

      SerializeContainer(*this, response.Results);
      SerializeContainer(*this, response.Diagnostics);
    }

    template<>
    void IStream::Deserialize<BrowseResponse>(BrowseResponse& response)
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
    void OStream::Serialize<BrowseNextRequest>(const BrowseNextRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      *this << request.ReleaseContinuationPoints;
      SerializeContainer(*this, request.ContinuationPoints);
    }

    template<>
    void IStream::Deserialize<BrowseNextRequest>(BrowseNextRequest& request)
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
    void OStream::Serialize<BrowseNextResponse>(const BrowseNextResponse& response)
    {
      *this << response.TypeID;
      *this << response.Header;

      SerializeContainer(*this, response.Results);
      SerializeContainer(*this, response.Diagnostics);
    }

    template<>
    void IStream::Deserialize<BrowseNextResponse>(BrowseNextResponse& response)
    {
      *this >> response.TypeID;
      *this >> response.Header;

      DeserializeContainer(*this, response.Results);
      DeserializeContainer(*this, response.Diagnostics);
    }

  } // namespace Binary
} // namespace OpcUa

