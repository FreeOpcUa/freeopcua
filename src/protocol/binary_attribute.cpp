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
#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/expanded_object_ids.h>

#include <algorithm>
#include <memory>
#include <string>
#include <iostream>


namespace OpcUa
{
  // TODO move to appropriate file
  AttributeValueId::AttributeValueId()
    : Attribute(AttributeId::Value)
  {
  }

  ReadParameters::ReadParameters()
    : MaxAge(0)
    , TimestampsType(TimestampsToReturn::Neither)
  {

  }

  ReadRequest::ReadRequest()
    : TypeId(READ_REQUEST)
  {
  }


  ReadResponse::ReadResponse()
    : TypeId(READ_RESPONSE)
  {
  }


  WriteRequest::WriteRequest()
    : TypeId(WRITE_REQUEST)
  {
  }


  WriteResponse::WriteResponse()
    : TypeId(WRITE_RESPONSE)
  {
  }

  namespace Binary
  {

    template<> 
    std::size_t RawSize<AttributeId>(const AttributeId&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<AttributeId>(const AttributeId& attr)
    {
      *this << static_cast<uint32_t>(attr);
    }

    template<>
    void DataDeserializer::Deserialize<AttributeId>(AttributeId& t)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      t = static_cast<AttributeId>(tmp);
    }


    //---------------------------------------------------
    // AttributeValueId
    //---------------------------------------------------

    template<>
    std::size_t RawSize<AttributeValueId>(const AttributeValueId& attr)
    {
      return RawSize(attr.Node) + RawSize(attr.Attribute) + RawSize(attr.IndexRange) + RawSize(attr.DataEncoding);
    }

    template<>
    void DataSerializer::Serialize<AttributeValueId>(const AttributeValueId& attr)
    {
      *this << attr.Node;
      *this << attr.Attribute;
      *this << attr.IndexRange;
      *this << attr.DataEncoding;
    }

    template<>
    void DataDeserializer::Deserialize<AttributeValueId>(AttributeValueId& attr)
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
    void DataSerializer::Serialize<ReadParameters>(const ReadParameters& params)
    {
      *this << params.MaxAge;
      *this << params.TimestampsType;
      SerializeContainer(*this, params.AttributesToRead);
    }

    template<>
    void DataDeserializer::Deserialize<ReadParameters>(ReadParameters& params)
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
      return RawSize(request.TypeId) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void DataSerializer::Serialize<ReadRequest>(const ReadRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<ReadRequest>(ReadRequest& request)
    {
      *this >> request.TypeId;
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
    void DataSerializer::Serialize<ReadResult>(const ReadResult& result)
    {
      SerializeContainer(*this, result.Results, 0);
      SerializeContainer(*this, result.Diagnostics, 0);
    }

    template<>
    void DataDeserializer::Deserialize<ReadResult>(ReadResult& result)
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
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Result);
    }

    template<>
    void DataSerializer::Serialize<ReadResponse>(const ReadResponse& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.Result;
    }

    template<>
    void DataDeserializer::Deserialize<ReadResponse>(ReadResponse& resp)
    {
      *this >> resp.TypeId;
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
    void DataSerializer::Serialize<WriteValue>(const WriteValue& val)
    {
      *this << val.Node;
      *this << val.Attribute;
      *this << val.NumericRange;
      *this << val.Data;
    }

    template<>
    void DataDeserializer::Deserialize<WriteValue>(WriteValue& val)
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
    void DataSerializer::Serialize<WriteParameters>(const WriteParameters& parameters)
    {
      SerializeContainer(*this, parameters.NodesToWrite);
    }

    template<>
    void DataDeserializer::Deserialize<WriteParameters>(WriteParameters& parameters)
    {
      DeserializeContainer(*this, parameters.NodesToWrite);
    }


    //---------------------------------------------------
    // WriteRequest
    //---------------------------------------------------
    template<>
    std::size_t RawSize<WriteRequest>(const WriteRequest& request)
    {
      return RawSize(request.TypeId) + RawSize(request.Header) + RawSize(request.Parameters);
    }

    template<>
    void DataSerializer::Serialize<WriteRequest>(const WriteRequest& request)
    {
      *this << request.TypeId;
      *this << request.Header;
      *this << request.Parameters;
    }

    template<>
    void DataDeserializer::Deserialize<WriteRequest>(WriteRequest& request)
    {
      *this >> request.TypeId;
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
    void DataSerializer::Serialize<WriteResult>(const WriteResult& result)
    {
      SerializeContainer(*this, result.StatusCodes, 0);
      SerializeContainer(*this, result.Diagnostics, 0);
    }

    template<>
    void DataDeserializer::Deserialize<WriteResult>(WriteResult& result)
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
      return RawSize(resp.TypeId) + RawSize(resp.Header) + RawSize(resp.Result);
    }

    template<>
    void DataSerializer::Serialize<WriteResponse>(const WriteResponse& resp)
    {
      *this << resp.TypeId;
      *this << resp.Header;
      *this << resp.Result;
    }

    template<>
    void DataDeserializer::Deserialize<WriteResponse>(WriteResponse& resp)
    {
      *this >> resp.TypeId;
      *this >> resp.Header;
      *this >> resp.Result;
    }

  } // namespace Binary
} // namespace OpcUa

