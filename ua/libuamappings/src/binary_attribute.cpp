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

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/attribute_ids.h>

#include <algorithm>
#include <memory>
#include <string>

namespace OpcUa
{
  // TODO move to appropriate file
  AttributeValueID::AttributeValueID()
    : Attribute(AttributeID::VALUE)
  {
  }

  ReadParameters::ReadParameters()
    : MaxAge(0)
    , TimestampsType(TimestampsToReturn::NEITHER)
  {

  }

  ReadRequest::ReadRequest()
    : TypeID(READ_REQUEST)
  {
  }


  ReadResponse::ReadResponse()
    : TypeID(READ_RESPONSE)
  {
  }


  WriteRequest::WriteRequest()
    : TypeID(WRITE_REQUEST)
  {
  }


  WriteResponse::WriteResponse()
    : TypeID(WRITE_RESPONSE)
  {
  }



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
    // ReadParameters
    //---------------------------------------------------

    template<>
    std::size_t RawSize<ReadParameters>(const ReadParameters& params)
    {
      return RawSize(params.MaxAge) +
        RawSize(params.TimestampsType) +
        RawSizeContainer(params.AttributesToRead);
    }

    template<>
    void OStream::Serialize<ReadParameters>(const ReadParameters& params)
    {
      *this << params.MaxAge;
      *this << params.TimestampsType;
      SerializeContainer(*this, params.AttributesToRead);
    }

    template<>
    void IStream::Deserialize<ReadParameters>(ReadParameters& params)
    {
      *this >> params.MaxAge;
      *this >> params.TimestampsType;
      DeserializeContainer(*this, params.AttributesToRead);
    }

    //---------------------------------------------------
    // ReadRequest
    //---------------------------------------------------

    template<>
    std::size_t RawSize<ReadRequest>(const ReadRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void OStream::Serialize<ReadRequest>(const ReadRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void IStream::Deserialize<ReadRequest>(ReadRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    //---------------------------------------------------
    // ReadResult
    //---------------------------------------------------
    template<>
    std::size_t RawSize<ReadResult>(const ReadResult& result)
    {
      return RawSizeContainer(result.Results) +
        RawSizeContainer(result.Diagnostics);
    }

    template<>
    void OStream::Serialize<ReadResult>(const ReadResult& result)
    {
      SerializeContainer(*this, result.Results, 0);
      SerializeContainer(*this, result.Diagnostics, 0);
    }

    template<>
    void IStream::Deserialize<ReadResult>(ReadResult& result)
    {
      DeserializeContainer(*this, result.Results);
      DeserializeContainer(*this, result.Diagnostics);
    }


    //---------------------------------------------------
    // ReadResponse
    //---------------------------------------------------
    template<>
    std::size_t RawSize<ReadResponse>(const ReadResponse& resp)
    {
      return RawSize(resp.TypeID) + RawSize(resp.Header) + RawSize(resp.Result);
    }

    template<>
    void OStream::Serialize<ReadResponse>(const ReadResponse& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;
      *this << resp.Result;
    }

    template<>
    void IStream::Deserialize<ReadResponse>(ReadResponse& resp)
    {
      *this >> resp.TypeID;
      *this >> resp.Header;
      *this >> resp.Result;
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
    // WriteParameters
    //---------------------------------------------------
    template<>
    std::size_t RawSize<WriteParameters>(const WriteParameters& parameters)
    {
      return RawSizeContainer(parameters.NodesToWrite);
    }

    template<>
    void OStream::Serialize<WriteParameters>(const WriteParameters& parameters)
    {
      SerializeContainer(*this, parameters.NodesToWrite);
    }

    template<>
    void IStream::Deserialize<WriteParameters>(WriteParameters& parameters)
    {
      DeserializeContainer(*this, parameters.NodesToWrite);
    }


    //---------------------------------------------------
    // WriteRequest
    //---------------------------------------------------
    template<>
    std::size_t RawSize<WriteRequest>(const WriteRequest& request)
    {
      return RawSize(request.TypeID) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void OStream::Serialize<WriteRequest>(const WriteRequest& request)
    {
      *this << request.TypeID;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void IStream::Deserialize<WriteRequest>(WriteRequest& request)
    {
      *this >> request.TypeID;
      *this >> request.Header;
      *this >> request.Parameters;
    }

    //---------------------------------------------------
    // WriteResult
    //---------------------------------------------------
    template<>
    std::size_t RawSize<WriteResult>(const WriteResult& result)
    {
      return RawSizeContainer(result.StatusCodes) + RawSizeContainer(result.Diagnostics);
    }

    template<>
    void OStream::Serialize<WriteResult>(const WriteResult& result)
    {
      SerializeContainer(*this, result.StatusCodes, 0);
      SerializeContainer(*this, result.Diagnostics, 0);
    }

    template<>
    void IStream::Deserialize<WriteResult>(WriteResult& result)
    {
      DeserializeContainer(*this, result.StatusCodes);
      DeserializeContainer(*this, result.Diagnostics);
    }

    //---------------------------------------------------
    // WriteResponse
    //---------------------------------------------------
    template<>
    std::size_t RawSize<WriteResponse>(const WriteResponse& resp)
    {
      return RawSize(resp.TypeID) + RawSize(resp.Header) + RawSize(resp.Result);
    }

    template<>
    void OStream::Serialize<WriteResponse>(const WriteResponse& resp)
    {
      *this << resp.TypeID;
      *this << resp.Header;
      *this << resp.Result;
    }

    template<>
    void IStream::Deserialize<WriteResponse>(WriteResponse& resp)
    {
      *this >> resp.TypeID;
      *this >> resp.Header;
      *this >> resp.Result;
    }

  } // namespace Binary
} // namespace OpcUa

