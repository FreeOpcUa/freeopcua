/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Node ID.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_MAPPING_NODE_ID_H__
#define __OPC_UA_MAPPING_NODE_ID_H__

#include <opc/ua/message_identifiers.h>
#include <opc/ua/object_ids.h>
#include <opc/ua/protocol/guid.h>
#include <opc/ua/reference_ids.h>

#include <stdint.h>
#include <string>
#include <vector>

namespace OpcUa
{

  enum NodeIDEncoding : uint8_t
  {
    EV_TWO_BYTE = 0,
    EV_FOUR_BYTE = 1,
    EV_NUMERIC = 2,
    EV_STRING = 3,
    EV_GUID = 4,
    EV_BYTE_STRING = 5,

    EV_SERVER_INDEX_FLAG = 0x40,
    EV_NAMESPACE_URI_FLAG = 0x80,

    EV_VALUE_MASK = 0x3f,
  };

  struct NodeID
  {
    NodeIDEncoding Encoding;
    std::string NamespaceURI;
    uint32_t ServerIndex;

    struct TwoByteDataType
    {
      uint8_t Identifier;

      TwoByteDataType()
        : Identifier(0)
      {
      }

    } TwoByteData;

    struct FourByteDataType
    {
      uint8_t NamespaceIndex;
      uint16_t Identifier;

      FourByteDataType()
        : NamespaceIndex(0)
        , Identifier(0)
      {
      }
    }FourByteData;

    struct NumericDataType
    {
      uint16_t NamespaceIndex;
      uint32_t Identifier;

      NumericDataType()
        : NamespaceIndex(0)
        , Identifier(0)
      {
      }
    }NumericData;


    struct StringDataType
    {
      uint16_t NamespaceIndex;
      std::string Identifier;

      StringDataType()
        : NamespaceIndex(0)
      {
      }

    }StringData;

    struct BinaryDataType
    {
      uint16_t NamespaceIndex;
      std::vector<uint8_t> Identifier;

      BinaryDataType()
        : NamespaceIndex(0)
      {
      }

    }BinaryData;

    struct GuidDataType
    {
      uint16_t NamespaceIndex;
      Guid Identifier;

      GuidDataType()
        : NamespaceIndex(0)
      {
      }

    }GuidData;

    NodeID();
    NodeID(const NodeID& node);
    NodeID(MessageID messageID);
    NodeID(ReferenceID referenceID);
    NodeID(ObjectID objectID);

    bool operator== (const NodeID& node) const;
    bool operator== (MessageID messageID) const;
    bool operator== (ReferenceID messageID) const;
    bool operator== (ObjectID messageID) const;

    bool operator!= (const NodeID& node) const;
    bool operator!= (MessageID messageID) const;
    bool operator!= (ReferenceID messageID) const;
    bool operator!= (ObjectID messageID) const;


    NodeIDEncoding GetEncodingValue() const;
    bool HasNamespaceURI() const;
    bool HasServerIndex() const;
  };

} // namespace OpcUa

#endif // __OPC_UA_MAPPING_NODE_ID_H__

