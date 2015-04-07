/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Node Id.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "binary_serialization.h"

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/nodeid.h>

#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace OpcUa
{


  NodeId::NodeId(uint32_t integerId, uint16_t index)
  {
    Encoding = EV_NUMERIC;
    NumericData.Identifier = integerId;
    NumericData.NamespaceIndex = index;
  }

  NodeId::NodeId(std::string stringId, uint16_t index)
  {
    Encoding = EV_STRING;
    StringData.Identifier = stringId;
    StringData.NamespaceIndex = index;
  }

  bool NodeId::IsInteger() const
  {
    const NodeIdEncoding enc = GetEncodingValue();
    return enc == EV_TWO_BYTE || enc == EV_FOUR_BYTE || enc == EV_NUMERIC;
  }

  bool NodeId::IsString() const
  {
    const NodeIdEncoding enc = GetEncodingValue();
    return enc == EV_STRING;
  }

  bool NodeId::IsBinary() const
  {
    const NodeIdEncoding enc = GetEncodingValue();
    return enc == EV_BYTE_STRING;
  }

  bool NodeId::IsGuid() const
  {
    const NodeIdEncoding enc = GetEncodingValue();
    return enc == EV_GUId;
  }

  std::string NodeId::GetStringIdentifier() const
  {
    if (IsString())
    {
      return StringData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  std::vector<uint8_t> NodeId::GetBinaryIdentifier() const
  {
    if (IsBinary())
    {
      return BinaryData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  Guid NodeId::GetGuidIdentifier() const
  {
    if (IsGuid())
    {
      return GuidData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  uint32_t NodeId::GetIntegerIdentifier() const
  {
    switch (GetEncodingValue())
    {
      case EV_TWO_BYTE:
      {
        return TwoByteData.Identifier;
      }
      case EV_FOUR_BYTE:
      {
        return FourByteData.Identifier;
      }
      case EV_NUMERIC:
      {
        return NumericData.Identifier;
      }
      default:
      {
        throw std::logic_error("Cannot get integer identifier from NodeId - it is not in numeric format.");
      }
    }
  }

  uint32_t NodeId::GetNamespaceIndex() const
  {
    switch (GetEncodingValue())
    {
      case EV_FOUR_BYTE:
        return FourByteData.NamespaceIndex;
      case EV_NUMERIC:
        return NumericData.NamespaceIndex;
      case EV_STRING:
        return StringData.NamespaceIndex;
      case EV_GUId:
        return GuidData.NamespaceIndex;
      case EV_BYTE_STRING:
        return BinaryData.NamespaceIndex;
      default:
        return 0;
    }
  }

  void NodeId::SetNamespaceIndex(uint32_t ns)
  {
    switch (GetEncodingValue())
    {
      case EV_FOUR_BYTE:
        FourByteData.NamespaceIndex = ns;
        return;
      case EV_NUMERIC:
        NumericData.NamespaceIndex = ns;
        return;
      case EV_STRING:
        StringData.NamespaceIndex = ns;
        return;
      case EV_GUId:
        GuidData.NamespaceIndex = ns;
        return;
      case EV_BYTE_STRING:
        BinaryData.NamespaceIndex = ns;
        return;
      default:
        return;
    }
  }

  NodeId::NodeId()
    : Encoding(EV_TWO_BYTE)
    , ServerIndex(0)
  {
  }

  void NodeId::CopyNodeId(const NodeId& node)
  {
    Encoding = node.Encoding;
    const NodeIdEncoding enc = node.GetEncodingValue();
    switch (enc)
    {
      case EV_TWO_BYTE:
      {
        TwoByteData.Identifier = node.TwoByteData.Identifier;
        break;
      }
      case EV_FOUR_BYTE:
      {
        FourByteData.NamespaceIndex = node.FourByteData.NamespaceIndex;
        FourByteData.Identifier = node.FourByteData.Identifier;
        break;
      }
      case EV_NUMERIC:
      {
        NumericData.NamespaceIndex = node.NumericData.NamespaceIndex;
        NumericData.Identifier = node.NumericData.Identifier;
        break;
      }
      case EV_STRING:
      {
        StringData.NamespaceIndex = node.StringData.NamespaceIndex;
        StringData.Identifier = node.StringData.Identifier;
        break;
      }
      case EV_GUId:
      {
        GuidData.NamespaceIndex = node.GuidData.NamespaceIndex;
        GuidData.Identifier = node.GuidData.Identifier;
        break;
      }
      case EV_BYTE_STRING:
      {
        BinaryData.NamespaceIndex = node.BinaryData.NamespaceIndex;
        BinaryData.Identifier = node.BinaryData.Identifier;
        break;
      }
      default:
      {
        throw std::logic_error("Invalid Node Id encoding value.");
      }
    }

    if (node.HasServerIndex())
    {
      ServerIndex = node.ServerIndex;
    }
    if (node.HasNamespaceURI())
    {
      NamespaceURI = node.NamespaceURI;
    }

  }

  NodeId::NodeId(const NodeId& node)
  {
    CopyNodeId(node);
  }

  NodeId::NodeId(const ExpandedNodeId& node)
  {
    CopyNodeId(node);
  }

  NodeId::operator ExpandedNodeId()
  {
    ExpandedNodeId node;
    node.CopyNodeId(*this);

    return node;
  } 

  NodeId& NodeId::operator=(const NodeId& node)
  {
    CopyNodeId(node);
    return *this;
  }

  NodeId& NodeId::operator=(const ExpandedNodeId& node)
  {
    CopyNodeId(node);
    return *this;
  }

  NodeId::NodeId(MessageId messageId)
    : Encoding(EV_FOUR_BYTE)
    , ServerIndex(0)
  {
    FourByteData.Identifier = messageId;
  }

  NodeId::NodeId(ReferenceId referenceId)
    : Encoding(EV_NUMERIC)
    , ServerIndex(0)
  {
    NumericData.Identifier = static_cast<uint32_t>(referenceId);
  }

  NodeId::NodeId(ObjectId objectId)
    : Encoding(EV_NUMERIC)
    , ServerIndex(0)
  {
    NumericData.Identifier = static_cast<uint32_t>(objectId);
  }

  NodeId::NodeId(ExpandedObjectId objectId)
    : Encoding(EV_FOUR_BYTE)
    , ServerIndex(0)
  {
    FourByteData.Identifier = static_cast<uint32_t>(objectId);
  }

  MessageId GetMessageId(const NodeId& id)
  {
    return static_cast<MessageId>(id.GetIntegerIdentifier());
  }


  bool NodeId::operator== (const NodeId& node) const
  {
    if (GetNamespaceIndex() != node.GetNamespaceIndex())
    {
      return false;
    }
    if (IsInteger() && node.IsInteger())
    {
      return GetIntegerIdentifier() == node.GetIntegerIdentifier();
    }
    if (IsString() && node.IsString())
    {
      return GetStringIdentifier() == node.GetStringIdentifier();
    }
    if (IsBinary() && node.IsBinary())
    {
      return GetBinaryIdentifier() == node.GetBinaryIdentifier();
    }
    if (IsGuid() && node.IsGuid())
    {
      return GetGuidIdentifier() == node.GetGuidIdentifier();
    }
    return false;
  }

  bool NodeId::operator < (const NodeId& node) const
  {
    if (GetNamespaceIndex() != node.GetNamespaceIndex())
    {
      return GetNamespaceIndex() < node.GetNamespaceIndex();
    }
    if (IsInteger() && node.IsInteger())
    {
      return GetIntegerIdentifier() < node.GetIntegerIdentifier();
    }
    if (IsString() && node.IsString())
    {
      return GetStringIdentifier() < node.GetStringIdentifier();
    }
    if (IsBinary() && node.IsBinary())
    {
      const std::vector<uint8_t>& l = GetBinaryIdentifier();
      const std::vector<uint8_t>& r = node.GetBinaryIdentifier();
      return std::lexicographical_compare(l.cbegin(), l.cend(), r.cbegin(), r.cend());
    }
    if (IsGuid() && node.IsGuid())
    {
      return GetGuidIdentifier() < node.GetGuidIdentifier();
    }
    return Encoding < node.Encoding; //FIXME Can we get there? and should we?

  }

  NodeIdEncoding NodeId::GetEncodingValue() const
  {
    return static_cast<NodeIdEncoding>(Encoding & EV_VALUE_MASK);
  }

  bool NodeId::HasNamespaceURI() const
  {
    return (Encoding & EV_NAMESPACE_URI_FLAG) != 0;
  }

  bool NodeId::HasServerIndex() const
  {
    return (Encoding & EV_Server_INDEX_FLAG) != 0;
  }

  void NodeId::SetNamespaceURI(const std::string& uri)
  {
    Encoding = static_cast<NodeIdEncoding>(Encoding | EV_NAMESPACE_URI_FLAG);
    NamespaceURI = uri;
  }

  void NodeId::SetServerIndex(uint32_t index)
  {
    Encoding = static_cast<NodeIdEncoding>(Encoding | EV_Server_INDEX_FLAG);
    ServerIndex = index;
  }

  bool NodeId::operator!= (const NodeId& node) const
  {
    return !(*this == node);
  }

  bool NodeId::operator!= (MessageId messageId) const
  {
    return !(*this == messageId);
  }

  bool NodeId::operator!= (ReferenceId referenceId) const
  {
    return !(*this == referenceId);
  }

  bool NodeId::operator!= (ObjectId objectId) const
  {
    return !(*this == objectId);
  }

  bool NodeId::operator!= (ExpandedObjectId objectId) const
  {
    return !(*this == objectId);
  }



  bool NodeId::operator== (MessageId messageId) const
  {
    return *this == NodeId(messageId);
  }

  bool NodeId::operator== (ReferenceId referenceId) const
  {
    return *this == NodeId(referenceId);
  }

  bool NodeId::operator== (ObjectId messageId) const
  {
    return *this == NodeId(messageId);
  }

  bool NodeId::operator== (ExpandedObjectId messageId) const
  {
    return *this == NodeId(messageId);
  }


  ///ExpandednNdeId
  ExpandedNodeId::ExpandedNodeId()
  {
    Encoding = EV_TWO_BYTE;
    ServerIndex = 0;
  }

  ExpandedNodeId::ExpandedNodeId(uint32_t integerId, uint16_t index)
  {
    Encoding = EV_NUMERIC;
    NumericData.Identifier = integerId;
    NumericData.NamespaceIndex = index;
  }

  ExpandedNodeId::ExpandedNodeId(std::string stringId, uint16_t index)
  {
    Encoding = EV_STRING;
    StringData.Identifier = stringId;
    StringData.NamespaceIndex = index;
  }

  ExpandedNodeId::ExpandedNodeId(const NodeId& node)
  {
    CopyNodeId(node);
  }

  ExpandedNodeId::ExpandedNodeId(const ExpandedNodeId& node)
  {
    CopyNodeId(node);
  }


  ExpandedNodeId::ExpandedNodeId(MessageId messageId)
  {
    Encoding = EV_FOUR_BYTE;
    ServerIndex = 0;
    FourByteData.Identifier = messageId;
  }

  ExpandedNodeId::ExpandedNodeId(ReferenceId referenceId)
  {
    Encoding = EV_NUMERIC;
    ServerIndex = 0;
    NumericData.Identifier = static_cast<uint32_t>(referenceId);
  }

  ExpandedNodeId::ExpandedNodeId(ObjectId objectId)
  {
    Encoding = EV_NUMERIC;
    ServerIndex = 0;
    NumericData.Identifier = static_cast<uint32_t>(objectId);
  }

  ExpandedNodeId::ExpandedNodeId(ExpandedObjectId objectId)
  {
    Encoding = EV_FOUR_BYTE;
    ServerIndex = 0;
    FourByteData.Identifier = static_cast<uint32_t>(objectId);
  }


  namespace Binary
  {
    template<>
    std::size_t RawSize<NodeIdEncoding>(const NodeIdEncoding&)
    {
      return 1;
    }

    template<>
    void DataSerializer::Serialize<NodeIdEncoding>(const NodeIdEncoding& value)
    {
      *this << static_cast<uint8_t>(value);
    }

    template<>
    void DataDeserializer::Deserialize<NodeIdEncoding>(NodeIdEncoding& value)
    {
      uint8_t tmp = 0;
      *this >> tmp;
      value = static_cast<NodeIdEncoding>(tmp);
    }

    template<>
    std::size_t RawSize<NodeId>(const NodeId& id)
    {
      std::size_t size = 0;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          size = 2;
          break;
        }
        case EV_FOUR_BYTE:
        {
          size = 4;
          break;
        }
        case EV_NUMERIC:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofNamespace = 2;
          const std::size_t sizeofIdentifier = 4;
          size = sizeofEncoding + sizeofNamespace + sizeofIdentifier;
          break;
        }
        case EV_STRING:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofSize = 4;
          const std::size_t sizeofNamespace = 2;
          size = sizeofEncoding + sizeofNamespace + sizeofSize + id.StringData.Identifier.size();
          break;
        }
        case EV_BYTE_STRING:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofSize = 4;
          const std::size_t sizeofNamespace = 2;
          size = sizeofEncoding + sizeofNamespace + sizeofSize + id.BinaryData.Identifier.size();
          break;
        }
        case EV_GUId:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofNamespace = 2;
          const std::size_t sizeofGuid = 16;
          size = sizeofEncoding + sizeofNamespace + sizeofGuid;
          break;
        }

      default:
        throw std::logic_error("Unable serialize NodeId. Unknown encoding type.");
      };

      return size;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::NodeId>(const OpcUa::NodeId& id)
    {
      //unset server and namespace flags in encoding, they should only be used in ExpandedNode Id
      uint8_t nodeid_encoding = id.Encoding;
      nodeid_encoding &= ~EV_Server_INDEX_FLAG;
      nodeid_encoding &= ~EV_NAMESPACE_URI_FLAG;

      *this << nodeid_encoding;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          *this << id.TwoByteData.Identifier;
          break;
        }
        case EV_FOUR_BYTE:
        {
          *this << id.FourByteData.NamespaceIndex;
          *this << id.FourByteData.Identifier;
          break;
        }
        case EV_NUMERIC:
        {
          *this << id.NumericData.NamespaceIndex;
          *this << id.NumericData.Identifier;
          break;
        }
        case EV_STRING:
        {
          *this << id.StringData.NamespaceIndex;
          *this << id.StringData.Identifier;
          break;
        }
        case EV_BYTE_STRING:
        {
          *this << id.BinaryData.NamespaceIndex;
          *this << id.BinaryData.Identifier;
          break;
        }
        case EV_GUId:
        {
          *this << id.GuidData.NamespaceIndex;
          *this << id.GuidData.Identifier;
          break;
        }

      default:
        throw std::logic_error("Unable serialize NodeId. Unknown encoding type.");
      };

    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::NodeId>(OpcUa::NodeId& id)
    {
      *this >> id.Encoding;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          *this >> id.TwoByteData.Identifier;
          break;
        }
        case EV_FOUR_BYTE:
        {
          *this >> id.FourByteData.NamespaceIndex;
          *this >> id.FourByteData.Identifier;
          break;
        }
        case EV_NUMERIC:
        {
          *this >> id.NumericData.NamespaceIndex;
          *this >> id.NumericData.Identifier;
          break;
        }
        case EV_STRING:
        {
          *this >> id.StringData.NamespaceIndex;
          *this >> id.StringData.Identifier;
          break;
        }
        case EV_BYTE_STRING:
        {
          *this >> id.BinaryData.NamespaceIndex;
          *this >> id.BinaryData.Identifier;
          break;
        }
        case EV_GUId:
        {
          *this >> id.GuidData.NamespaceIndex;
          *this >> id.GuidData.Identifier;
          break;
        }

        default:
        {
          throw std::logic_error("Unable to deserialize NodeId. Unknown encoding type received.");
        }
      };

      if (id.HasNamespaceURI())
      {
        *this >> id.NamespaceURI;
      }
      if (id.HasServerIndex())
      {
        *this >> id.ServerIndex;
      }
    };

    template<>
    std::size_t RawSize<ExpandedNodeId>(const ExpandedNodeId& id)
    {
      std::size_t size = RawSize((NodeId)id);

      if (id.HasNamespaceURI())
      {
        const std::size_t sizeofSize = 4;
        size += sizeofSize + id.NamespaceURI.size();
      }
      if (id.HasServerIndex())
      {
        const std::size_t sizeofServerIndex = 4;
        size += sizeofServerIndex;
      }
      return size;
    }

    template<>
    void DataSerializer::Serialize<OpcUa::ExpandedNodeId>(const OpcUa::ExpandedNodeId& id)
    {
      *this << id.Encoding;

      switch (id.GetEncodingValue())
      {
        case EV_TWO_BYTE:
        {
          *this << id.TwoByteData.Identifier;
          break;
        }
        case EV_FOUR_BYTE:
        {
          *this << id.FourByteData.NamespaceIndex;
          *this << id.FourByteData.Identifier;
          break;
        }
        case EV_NUMERIC:
        {
          *this << id.NumericData.NamespaceIndex;
          *this << id.NumericData.Identifier;
          break;
        }
        case EV_STRING:
        {
          *this << id.StringData.NamespaceIndex;
          *this << id.StringData.Identifier;
          break;
        }
        case EV_BYTE_STRING:
        {
          *this << id.BinaryData.NamespaceIndex;
          *this << id.BinaryData.Identifier;
          break;
        }
        case EV_GUId:
        {
          *this << id.GuidData.NamespaceIndex;
          *this << id.GuidData.Identifier;
          break;
        }

      default:
        throw std::logic_error("Unable serialize ExpandedNodeId. Unknown encoding type.");
      };

      if (id.HasNamespaceURI())
      {
        *this << id.NamespaceURI;
      }
      if (id.HasServerIndex())
      {
        *this << id.ServerIndex;
      }
    }

    template<>
    void DataDeserializer::Deserialize<OpcUa::ExpandedNodeId>(OpcUa::ExpandedNodeId& id)
    {
      *this >> *(NodeId*) &id;
    };


  } // namespace Binary
} // namespace OpcUa

