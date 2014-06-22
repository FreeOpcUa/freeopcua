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

#include <opc/ua/protocol/message_identifiers.h>
#include <opc/ua/protocol/object_ids.h>
#include <opc/ua/protocol/expanded_object_ids.h>
#include <opc/ua/protocol/guid.h>
#include <opc/ua/protocol/reference_ids.h>

#include <sstream>
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
    NodeID(ExpandedObjectID objectID);
    NodeID(uint32_t integerId, uint16_t index);
    NodeID(std::string stringId, uint16_t index);

    NodeID& operator= (const NodeID& node);

    NodeID& operator= (MessageID messageID)
    {
      *this = NodeID(messageID);
      return *this;
    }

    NodeID& operator= (ReferenceID referenceID)
    {
      *this = NodeID(referenceID);
      return *this;
    }

    NodeID& operator= (ObjectID objectID)
    {
      *this = NodeID(objectID);
      return *this;
    }

    NodeID& operator= (ExpandedObjectID objectID)
    {
      *this = NodeID(objectID);
      return *this;
    }

    bool operator== (const NodeID& node) const;
    bool operator== (MessageID messageID) const;
    bool operator== (ReferenceID referenceID) const;
    bool operator== (ObjectID objectID) const;
    bool operator== (ExpandedObjectID objectID) const;

    bool operator!= (const NodeID& node) const;
    bool operator!= (MessageID messageID) const;
    bool operator!= (ReferenceID referenceID) const;
    bool operator!= (ObjectID objectID) const;
    bool operator!= (ExpandedObjectID objectID) const;

    bool operator< (const NodeID& node) const;

    NodeIDEncoding GetEncodingValue() const;
    bool HasNamespaceURI() const;
    bool HasServerIndex() const;

    void SetNamespaceURI(const std::string& uri);
    void SetServerIndex(uint32_t index);
    void SetNamespaceIndex(uint32_t ns);

    bool IsInteger() const;
    bool IsString() const;
    bool IsBinary() const;
    bool IsGuid() const;

    uint32_t GetNamespaceIndex() const;

    uint32_t GetIntegerIdentifier() const;
    std::string GetStringIdentifier() const;
    std::vector<uint8_t> GetBinaryIdentifier() const;
    Guid GetGuidIdentifier() const;
  };

  inline NodeID TwoByteNodeID(uint8_t value)
  {
    NodeID id;
    id.Encoding = EV_TWO_BYTE;
    id.TwoByteData.Identifier = value;
    return id;
  }

  inline NodeID FourByteNodeID(uint16_t value, uint8_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_FOUR_BYTE;
    id.FourByteData.Identifier = value;
    id.FourByteData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID NumericNodeID(uint32_t value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_NUMERIC;
    id.NumericData.Identifier = value;
    id.NumericData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID StringNodeID(std::string value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_STRING;
    id.StringData.Identifier = value;
    id.StringData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID BinaryNodeID(std::vector<uint8_t> value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_BYTE_STRING;
    id.BinaryData.Identifier = value;
    id.BinaryData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeID GuidNodeID(Guid value, uint16_t namespaceIndex = 0)
  {
    NodeID id;
    id.Encoding = EV_GUID;
    id.GuidData.Identifier = value;
    id.GuidData.NamespaceIndex = namespaceIndex;
    return id;
  }

   //std::ostream& operator<<(std::ostream& os, const NodeID& nodeid);

} // namespace OpcUa

#endif // __OPC_UA_MAPPING_NODE_ID_H__

