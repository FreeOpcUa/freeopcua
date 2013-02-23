// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary session services.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "binary_serialization.h"

#include <opc/ua/binary/protocol/attribute.h>
#include <opc/ua/binary/stream.h>
#include <opc/ua/binary/types.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
  namespace Binary
  {

    template<> 
    std::size_t RawSize<AttributeID>(const AttributeID&)
    {
      return 4;
    }

    template<>
    void OStream::Serialize<AttributeID>(const AttributeID& attr)
    {
      *this << static_cast<uint32_t>(attr);
    }

    template<>
    void IStream::Deserialize<AttributeID>(AttributeID& t)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      t = static_cast<AttributeID>(tmp);
    }


    //---------------------------------------------------
    // TimestampsToReturn
    //---------------------------------------------------


    template<> 
    std::size_t RawSize<TimestampsToReturn>(const TimestampsToReturn&)
    {
      return 4;
    }

    template<>
    void OStream::Serialize<TimestampsToReturn>(const TimestampsToReturn& t)
    {
      *this << static_cast<uint32_t>(t);
    }

    template<>
    void IStream::Deserialize<TimestampsToReturn>(TimestampsToReturn& t)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      t = static_cast<TimestampsToReturn>(tmp);
    }
    //---------------------------------------------------
    // AttributeValueID
    //---------------------------------------------------

    AttributeValueID::AttributeValueID()
      : Attribute(AttributeID::VALUE)
    {
    }

    template<>
    std::size_t RawSize<AttributeValueID>(const AttributeValueID& attr)
    {
      return RawSize(attr.Node) + RawSize(attr.Attribute) + RawSize(attr.IndexRange) + RawSize(attr.DataEncoding);
    }

    template<>
    void OStream::Serialize<AttributeValueID>(const AttributeValueID& attr)
    {
      *this << attr.Node;
      *this << attr.Attribute;
      *this << attr.IndexRange;
      *this << attr.DataEncoding;
    }

    template<>
    void IStream::Deserialize<AttributeValueID>(AttributeValueID& attr)
    {
      *this >> attr.Node;
      *this >> attr.Attribute;
      *this >> attr.IndexRange;
      *this >> attr.DataEncoding;
    }

    //---------------------------------------------------
    // ReadRequest
    //---------------------------------------------------

    ReadRequest::ReadRequest()
      : TypeID(READ_REQUEST)
      , MaxAge(0)
      , TimestampsType(TimestampsToReturn::NEITHER)
    {
    }

    template<>
    std::size_t RawSize<ReadRequest>(const ReadRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) +
        RawSize(request.MaxAge) +
        RawSize(request.TimestampsType) +
        RawSizeContainer(request.AttributesToRead);
    }

    template<>
    void OStream::Serialize<ReadRequest>(const ReadRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      *this << request.MaxAge;
      *this << request.TimestampsType;
      SerializeContainer(*this, request.AttributesToRead);
    }

    template<>
    void IStream::Deserialize<ReadRequest>(ReadRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;

      *this >> request.MaxAge;
      *this >> request.TimestampsType;
      DeserializeContainer(*this, request.AttributesToRead);
    }

    //---------------------------------------------------
    // ReadResponse
    //---------------------------------------------------

    ReadResponse::ReadResponse()
      : TypeID(READ_RESPONSE)
    {
    }

    template<>
    std::size_t RawSize<ReadResponse>(const ReadResponse& resp)
    {
      return RawSize(resp.TypeID) + RawSize(resp.Header) +
        RawSizeContainer(resp.Results) +
        RawSizeContainer(resp.Diagnostics);
    }

    template<>
    void OStream::Serialize<ReadResponse>(const ReadResponse& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;

      SerializeContainer(*this, resp.Results);
      SerializeContainer(*this, resp.Diagnostics);
    }

    template<>
    void IStream::Deserialize<ReadResponse>(ReadResponse& resp)
    {
      *this >> resp.TypeID;
      *this >> resp.Header;

      DeserializeContainer(*this, resp.Results);
      DeserializeContainer(*this, resp.Diagnostics);
    }

    //---------------------------------------------------
    // WriteValue
    //---------------------------------------------------

    template<>
    std::size_t RawSize<WriteValue>(const WriteValue& val)
    {
        return RawSize(val.Node) +
        RawSize(val.Attribute) +
        RawSize(val.NumericRange) +
        RawSize(val.Data);
    }

    template<>
    void OStream::Serialize<WriteValue>(const WriteValue& val)
    {
      *this << val.Node;
      *this << val.Attribute;
      *this << val.NumericRange;
      *this << val.Data;
    }

    template<>
    void IStream::Deserialize<WriteValue>(WriteValue& val)
    {
      *this >> val.Node;
      *this >> val.Attribute;
      *this >> val.NumericRange;
      *this >> val.Data;
    }


    //---------------------------------------------------
    // WriteRequest
    //---------------------------------------------------

    WriteRequest::WriteRequest()
      : TypeID(WRITE_REQUEST)
    {
    }

    template<>
    std::size_t RawSize<WriteRequest>(const WriteRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) +
        RawSizeContainer(request.NodesToWrite);
    }

    template<>
    void OStream::Serialize<WriteRequest>(const WriteRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;

      SerializeContainer(*this, request.NodesToWrite);
    }

    template<>
    void IStream::Deserialize<WriteRequest>(WriteRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;

      DeserializeContainer(*this, request.NodesToWrite);
    }

    //---------------------------------------------------
    // WriteResponse
    //---------------------------------------------------

    WriteResponse::WriteResponse()
      : TypeID(WRITE_RESPONSE)
    {
    }

    template<>
    std::size_t RawSize<WriteResponse>(const WriteResponse& resp)
    {
      return RawSize(resp.TypeID) + RawSize(resp.Header) +
        RawSizeContainer(resp.StatusCodes) +
        RawSizeContainer(resp.Diagnostics);
    }

    template<>
    void OStream::Serialize<WriteResponse>(const WriteResponse& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;

      SerializeContainer(*this, resp.StatusCodes);
      SerializeContainer(*this, resp.Diagnostics);
    }

    template<>
    void IStream::Deserialize<WriteResponse>(WriteResponse& resp)
    {
      *this >> resp.TypeID;
      *this >> resp.Header;

      DeserializeContainer(*this, resp.StatusCodes);
      DeserializeContainer(*this, resp.Diagnostics);
    }

  } // namespace Binary
} // namespace OpcUa

