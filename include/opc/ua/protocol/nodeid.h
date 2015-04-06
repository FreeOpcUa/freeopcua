/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Node Id.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

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

  enum NodeIdEncoding : uint8_t
  {
    EV_TWO_BYTE = 0,
    EV_FOUR_BYTE = 1,
    EV_NUMERIC = 2,
    EV_STRING = 3,
    EV_GUId = 4,
    EV_BYTE_STRING = 5,

    EV_Server_INDEX_FLAG = 0x40,
    EV_NAMESPACE_URI_FLAG = 0x80,

    EV_VALUE_MASK = 0x3f,
  };

  struct ExpandedNodeId;

  struct NodeId
  {
    NodeIdEncoding Encoding;
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

    NodeId();
    NodeId(const NodeId& node);
    NodeId(const ExpandedNodeId& node);
    NodeId(MessageId messageId);
    NodeId(ReferenceId referenceId);
    NodeId(ObjectId objectId);
    NodeId(ExpandedObjectId objectId);
    NodeId(uint32_t integerId, uint16_t index);
    NodeId(std::string stringId, uint16_t index);

    NodeId& operator= (const NodeId& node);
    NodeId& operator= (const ExpandedNodeId& node);
    
    explicit operator ExpandedNodeId(); 

    NodeId& operator= (MessageId messageId)
    {
      *this = NodeId(messageId);
      return *this;
    }

    NodeId& operator= (ReferenceId referenceId)
    {
      *this = NodeId(referenceId);
      return *this;
    }

    NodeId& operator= (ObjectId objectId)
    {
      *this = NodeId(objectId);
      return *this;
    }

    NodeId& operator= (ExpandedObjectId objectId)
    {
      *this = NodeId(objectId);
      return *this;
    }

    bool operator== (const NodeId& node) const;
    bool operator== (MessageId messageId) const;
    bool operator== (ReferenceId referenceId) const;
    bool operator== (ObjectId objectId) const;
    bool operator== (ExpandedObjectId objectId) const;

    bool operator!= (const NodeId& node) const;
    bool operator!= (MessageId messageId) const;
    bool operator!= (ReferenceId referenceId) const;
    bool operator!= (ObjectId objectId) const;
    bool operator!= (ExpandedObjectId objectId) const;

    bool operator< (const NodeId& node) const;

    NodeIdEncoding GetEncodingValue() const;
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

    protected:
    void CopyNodeId(const NodeId& node);
  };

  inline NodeId TwoByteNodeId(uint8_t value)
  {
    NodeId id;
    id.Encoding = EV_TWO_BYTE;
    id.TwoByteData.Identifier = value;
    return id;
  }

  inline NodeId FourByteNodeId(uint16_t value, uint8_t namespaceIndex = 0)
  {
    NodeId id;
    id.Encoding = EV_FOUR_BYTE;
    id.FourByteData.Identifier = value;
    id.FourByteData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeId NumericNodeId(uint32_t value, uint16_t namespaceIndex = 0)
  {
    NodeId id;
    id.Encoding = EV_NUMERIC;
    id.NumericData.Identifier = value;
    id.NumericData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeId StringNodeId(std::string value, uint16_t namespaceIndex = 0)
  {
    NodeId id;
    id.Encoding = EV_STRING;
    id.StringData.Identifier = value;
    id.StringData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeId BinaryNodeId(std::vector<uint8_t> value, uint16_t namespaceIndex = 0)
  {
    NodeId id;
    id.Encoding = EV_BYTE_STRING;
    id.BinaryData.Identifier = value;
    id.BinaryData.NamespaceIndex = namespaceIndex;
    return id;
  }

  inline NodeId GuidNodeId(Guid value, uint16_t namespaceIndex = 0)
  {
    NodeId id;
    id.Encoding = EV_GUId;
    id.GuidData.Identifier = value;
    id.GuidData.NamespaceIndex = namespaceIndex;
    return id;
  }

  struct ExpandedNodeId : public NodeId
  {
    ExpandedNodeId();
    ExpandedNodeId(const NodeId& node);
    ExpandedNodeId(const ExpandedNodeId& node);
    ExpandedNodeId(MessageId messageId);
    ExpandedNodeId(ReferenceId referenceId);
    ExpandedNodeId(ObjectId objectId);
    ExpandedNodeId(ExpandedObjectId objectId);
    ExpandedNodeId(uint32_t integerId, uint16_t index);
    ExpandedNodeId(std::string stringId, uint16_t index);

   //using NodeId::NodeId;
   //using base::base;
  };

} // namespace OpcUa


