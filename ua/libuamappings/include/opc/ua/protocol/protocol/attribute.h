/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Attributes service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_ATTRIBUTES_H__
#define __OPC_UA_BINARY_ATTRIBUTES_H__

#include <opc/ua/attribute_ids.h>
#include <opc/ua/binary/data_value.h>
#include <opc/ua/binary/variant.h>
#include <opc/ua/binary/protocol/common.h>

namespace OpcUa
{
  namespace Binary
  {

    enum class TimestampsToReturn
    {
      SOURCE = 0,
      SERVER = 1,
      BOTH   = 2,
      NEITHER = 3
    };

    struct AttributeValueID
    {
      NodeID Node;
      AttributeID Attribute;
      std::string IndexRange;
      QualifiedName DataEncoding;

      AttributeValueID();
    };

    struct ReadRequest
    {
      NodeID TypeID;
      RequestHeader Header;
      
      Duration MaxAge;
      TimestampsToReturn TimestampsType;
      std::vector<AttributeValueID> AttributesToRead;

      ReadRequest();
    };

    struct ReadResponse
    {
      NodeID TypeID;
      ResponseHeader Header;

      std::vector<DataValue> Results;
      std::vector<DiagnosticInfo> Diagnostics;

      ReadResponse();
    };


    struct WriteValue
    {
      NodeID Node;
      AttributeID Attribute;
      std::string NumericRange;
      DataValue Data;
    };

    struct WriteRequest
    {
      NodeID TypeID;
      RequestHeader Header;

      std::vector<WriteValue> NodesToWrite;

      WriteRequest();
    };

    struct WriteResponse
    {
      NodeID TypeID;
      ResponseHeader Header;

      std::vector<uint32_t> StatusCodes;
      std::vector<DiagnosticInfo> Diagnostics;

      WriteResponse();
    };

  } // namespace Binary
} // namespace OpcUa

#endif // __OPC_UA_BINARY_ATTRIBUTES_H__
