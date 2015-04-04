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

#include <opc/ua/protocol/attribute_ids.h>
#include <opc/ua/protocol/data_value.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/node_classes.h>

namespace OpcUa
{

  struct AttributeValueId
  {
    NodeId Node;
    AttributeId Attribute;
    std::string IndexRange;
    QualifiedName DataEncoding;

    AttributeValueId();

    AttributeValueId(const NodeId& id, AttributeId attr)
      : Node(id)
      , Attribute(attr)
    {
    }
  };

  struct ReadParameters
  {
    Duration MaxAge;
    TimestampsToReturn TimestampsType;
    std::vector<AttributeValueId> AttributesToRead;

    ReadParameters();
  };

  struct ReadRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    ReadParameters Parameters;

    ReadRequest();
  };

  struct ReadResult
  {
    std::vector<DataValue> Results;
    DiagnosticInfoList Diagnostics;
  };


  struct ReadResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    ReadResult Result;

    ReadResponse();
  };


  struct WriteValue
  {
    NodeId Node;
    AttributeId Attribute;
    std::string NumericRange;
    DataValue Data;
  };

  struct WriteParameters
  {
    std::vector<WriteValue> NodesToWrite;
  };

  struct WriteRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    WriteParameters Parameters;

    WriteRequest();
  };

  struct WriteResult
  {
    std::vector<StatusCode> StatusCodes;
    DiagnosticInfoList Diagnostics;
  };

  struct WriteResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    WriteResult Result;

    WriteResponse();
  };

} // namespace OpcUa

#endif // __OPC_UA_BINARY_ATTRIBUTES_H__
