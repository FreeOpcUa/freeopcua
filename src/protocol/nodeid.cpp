/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Node ID.
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


  NodeID::NodeID(uint32_t integerId, uint16_t index)
  {
    Encoding = EV_NUMERIC;
    NumericData.Identifier = integerId;
    NumericData.NamespaceIndex = index;
  }

  NodeID::NodeID(std::string stringId, uint16_t index)
  {
    Encoding = EV_STRING;
    StringData.Identifier = stringId;
    StringData.NamespaceIndex = index;
  }

  bool NodeID::IsInteger() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_TWO_BYTE || enc == EV_FOUR_BYTE || enc == EV_NUMERIC;
  }

  bool NodeID::IsString() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_STRING;
  }

  bool NodeID::IsBinary() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_BYTE_STRING;
  }

  bool NodeID::IsGuid() const
  {
    const NodeIDEncoding enc = GetEncodingValue();
    return enc == EV_GUID;
  }

  std::string NodeID::GetStringIdentifier() const
  {
    if (IsString())
    {
      return StringData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  std::vector<uint8_t> NodeID::GetBinaryIdentifier() const
  {
    if (IsBinary())
    {
      return BinaryData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  Guid NodeID::GetGuidIdentifier() const
  {
    if (IsGuid())
    {
      return GuidData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  uint32_t NodeID::GetIntegerIdentifier() const
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
        throw std::logic_error("Cannot get integer identifier from NodeID - it is not in numeric format.");
      }
    }
  }

  uint32_t NodeID::GetNamespaceIndex() const
  {
    switch (GetEncodingValue())
    {
      case EV_FOUR_BYTE:
        return FourByteData.NamespaceIndex;
      case EV_NUMERIC:
        return NumericData.NamespaceIndex;
      case EV_STRING:
        return StringData.NamespaceIndex;
      case EV_GUID:
        return GuidData.NamespaceIndex;
      case EV_BYTE_STRING:
        return BinaryData.NamespaceIndex;
      default:
        return 0;
    }
  }

  void NodeID::SetNamespaceIndex(uint32_t ns)
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
      case EV_GUID:
        GuidData.NamespaceIndex = ns;
        return;
      case EV_BYTE_STRING:
        BinaryData.NamespaceIndex = ns;
        return;
      default:
        return;
    }
  }

  NodeID::NodeID()
    : Encoding(EV_TWO_BYTE)
    , ServerIndex(0)
  {
  }

  NodeID::NodeID(const NodeID& node)
  {
    Encoding = node.Encoding;
    const NodeIDEncoding enc = node.GetEncodingValue();
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
      case EV_GUID:
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
        throw std::logic_error("Invalid Node ID encoding value.");
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

  NodeID& NodeID::operator=(const NodeID& node)
  {
    Encoding = node.Encoding;
    const NodeIDEncoding enc = node.GetEncodingValue();
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
      case EV_GUID:
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
        throw std::logic_error("Invalid Node ID encoding value.");
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
    return *this;
  }

  NodeID::NodeID(MessageID messageID)
    : Encoding(EV_FOUR_BYTE)
    , ServerIndex(0)
  {
    FourByteData.Identifier = messageID;
  }

  NodeID::NodeID(ReferenceID referenceID)
    : Encoding(EV_NUMERIC)
    , ServerIndex(0)
  {
    NumericData.Identifier = static_cast<uint32_t>(referenceID);
  }

  NodeID::NodeID(ObjectID objectID)
    : Encoding(EV_NUMERIC)
    , ServerIndex(0)
  {
    NumericData.Identifier = static_cast<uint32_t>(objectID);
  }

  NodeID::NodeID(ExpandedObjectID objectID)
    : Encoding(EV_FOUR_BYTE)
    , ServerIndex(0)
  {
    FourByteData.Identifier = static_cast<uint32_t>(objectID);
  }

  MessageID GetMessageID(const NodeID& id)
  {
    return static_cast<MessageID>(id.GetIntegerIdentifier());
  }


  bool NodeID::operator== (const NodeID& node) const
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

  bool NodeID::operator < (const NodeID& node) const
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

  NodeIDEncoding NodeID::GetEncodingValue() const
  {
    return static_cast<NodeIDEncoding>(Encoding & EV_VALUE_MASK);
  }

  bool NodeID::HasNamespaceURI() const
  {
    return Encoding & EV_NAMESPACE_URI_FLAG;
  }

  bool NodeID::HasServerIndex() const
  {
    return Encoding & EV_SERVER_INDEX_FLAG;
  }

  void NodeID::SetNamespaceURI(const std::string& uri)
  {
    Encoding = static_cast<NodeIDEncoding>(Encoding | EV_NAMESPACE_URI_FLAG);
    NamespaceURI = uri;
  }

  void NodeID::SetServerIndex(uint32_t index)
  {
    Encoding = static_cast<NodeIDEncoding>(Encoding | EV_SERVER_INDEX_FLAG);
    ServerIndex = index;
  }

  bool NodeID::operator!= (const NodeID& node) const
  {
    return !(*this == node);
  }

  bool NodeID::operator!= (MessageID messageID) const
  {
    return !(*this == messageID);
  }

  bool NodeID::operator!= (ReferenceID referenceID) const
  {
    return !(*this == referenceID);
  }

  bool NodeID::operator!= (ObjectID objectID) const
  {
    return !(*this == objectID);
  }

  bool NodeID::operator!= (ExpandedObjectID objectID) const
  {
    return !(*this == objectID);
  }



  bool NodeID::operator== (MessageID messageID) const
  {
    return *this == NodeID(messageID);
  }

  bool NodeID::operator== (ReferenceID referenceID) const
  {
    return *this == NodeID(referenceID);
  }

  bool NodeID::operator== (ObjectID messageID) const
  {
    return *this == NodeID(messageID);
  }

  bool NodeID::operator== (ExpandedObjectID messageID) const
  {
    return *this == NodeID(messageID);
  }

  namespace Binary
  {
    template<>
    std::size_t RawSize<NodeIDEncoding>(const NodeIDEncoding&)
    {
      return 1;
    }

    template<>
    void DataSerializer::Serialize<NodeIDEncoding>(const NodeIDEncoding& value)
    {
      *this << static_cast<uint8_t>(value);
    }

    template<>
    void DataDeserializer::Deserialize<NodeIDEncoding>(NodeIDEncoding& value)
    {
      uint8_t tmp = 0;
      *this >> tmp;
      value = static_cast<NodeIDEncoding>(tmp);
    }

    template<>
    std::size_t RawSize<NodeID>(const NodeID& id)
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
        case EV_GUID:
        {
          const std::size_t sizeofEncoding = 1;
          const std::size_t sizeofNamespace = 2;
          const std::size_t sizeofGuid = 16;
          size = sizeofEncoding + sizeofNamespace + sizeofGuid;
          break;
        }

      default:
        throw std::logic_error("Unable serialize NodeID. Unknown encoding type.");
      };

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
    void DataSerializer::Serialize<OpcUa::NodeID>(const OpcUa::NodeID& id)
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
        case EV_GUID:
        {
          *this << id.GuidData.NamespaceIndex;
          *this << id.GuidData.Identifier;
          break;
        }

      default:
        throw std::logic_error("Unable serialize NodeID. Unknown encoding type.");
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
    void DataDeserializer::Deserialize<OpcUa::NodeID>(OpcUa::NodeID& id)
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
        case EV_GUID:
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

  } // namespace Binary
} // namespace OpcUa

