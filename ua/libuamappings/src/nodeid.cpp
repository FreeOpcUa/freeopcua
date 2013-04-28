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

namespace
{
  using namespace OpcUa;

  inline bool IsInteger(const NodeID& id)
  {
    const NodeIDEncoding enc = id.GetEncodingValue();
    return enc == EV_TWO_BYTE || enc == EV_FOUR_BYTE || enc == EV_NUMERIC;
  }
  inline bool IsString(const NodeID& id)
  {
    const NodeIDEncoding enc = id.GetEncodingValue();
    return enc == EV_STRING;
  }
  inline bool IsBinary(const NodeID& id)
  {
    const NodeIDEncoding enc = id.GetEncodingValue();
    return enc == EV_BYTE_STRING;
  }
  inline bool IsGuid(const NodeID& id)
  {
    const NodeIDEncoding enc = id.GetEncodingValue();
    return enc == EV_GUID;
  }


  inline std::string GetStringIdentifier(const NodeID& id)
  {
    if (IsString(id))
    {
      return id.StringData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  inline std::vector<uint8_t> GetBinaryIdentifier(const NodeID& id)
  {
    if (IsBinary(id))
    {
      return id.BinaryData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  inline Guid GetGuidIdentifier(const NodeID& id)
  {
    if (IsGuid(id))
    {
      return id.GuidData.Identifier;
    }
    throw std::logic_error("Node id is not in String format.");
  }

  inline uint32_t GetIntegerIdentifier(const NodeID& id)
  {
    switch (id.GetEncodingValue())
    {
      case EV_TWO_BYTE:
      {
        return id.TwoByteData.Identifier;
      } 
      case EV_FOUR_BYTE:
      {
        return id.FourByteData.Identifier;
      }
      case EV_NUMERIC:
      {
        return id.NumericData.Identifier;
      }
      default:
      {
        throw std::logic_error("Cannot get integer identifier from NodeID - it is not in numeric format.");
      }
    }
  }

  inline uint32_t GetNamespaceIndex(const NodeID& id)
  {
    switch (id.GetEncodingValue())
    {
      case EV_FOUR_BYTE:
        return id.FourByteData.NamespaceIndex;
      case EV_NUMERIC:
        return id.NumericData.NamespaceIndex;
      case EV_STRING:
        return id.StringData.NamespaceIndex;
      case EV_GUID:
        return id.GuidData.NamespaceIndex;
      case EV_BYTE_STRING:
        return id.BinaryData.NamespaceIndex;
      default:
        return 0;
    }
  }
}

namespace OpcUa
{

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

  MessageID GetMessageID(const NodeID& id)
  {
    const uint32_t identifier = GetIntegerIdentifier(id);
    switch (identifier)
    {
      case ACTIVATE_SESSION_REQUEST: //  = 0x1d3, //467;
      case ACTIVATE_SESSION_RESPONSE: // = 0x1d6, //470;
      case BROWSE_REQUEST: //  = 0x20f // 527;
      case BROWSE_RESPONSE: // = 0x212 //530;
      case BROWSE_NEXT_REQUEST: //  = 0x215 // 533
      case BROWSE_NEXT_RESPONSE: // = 0x218 // 536
      case CLOSE_SECURE_CHANNEL_REQUEST: // = 0x1c4 // 452
      case CLOSE_SESSION_REQUEST: //  = 0x1d9 // 473;
      case CLOSE_SESSION_RESPONSE: // = 0x1dc // 476;
      case CREATE_SESSION_REQUEST: //  = 0x1cd // 461;
      case CREATE_SESSION_RESPONSE: // = 0x1d0 // 464;
      case GET_ENDPOINTS_REQUEST: //  = 0x1ac // 428
      case GET_ENDPOINTS_RESPONSE: // = 0x1af // 431
      case OPEN_SECURE_CHANNEL_REQUEST: //  = 0x1be // 446
      case OPEN_SECURE_CHANNEL_RESPONSE: // = 0x1c1 // 449
      case READ_REQUEST: //  = 0x277 // 631
      case READ_RESPONSE: // = 0x27A // 634
      case WRITE_REQUEST: //  = 0x2A1 //673
      case WRITE_RESPONSE: // = 0x2A4 // 676
        return static_cast<MessageID>(identifier);

      default:
        return INVALID;
    }
  }


  bool NodeID::operator== (const NodeID& node) const
  {
    if (GetNamespaceIndex(*this) != GetNamespaceIndex(node))
    {
      return false;
    }
    if (IsInteger(*this) && IsInteger(node) && GetIntegerIdentifier(*this) == GetIntegerIdentifier(node))
    {
      return true;
    }
    if (IsString(*this) && IsString(node) && GetStringIdentifier(*this) == GetStringIdentifier(node))
    {
      return true;
    }
    if (IsBinary(*this) && IsBinary(node) && GetBinaryIdentifier(*this) == GetBinaryIdentifier(node))
    {
      return true;
    }
    if (IsGuid(*this) && IsGuid(node) && GetGuidIdentifier(*this) == GetGuidIdentifier(node))
    {
      return true;
    }
    return false;    
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

  namespace Binary
  {
    template<>
    std::size_t RawSize<NodeIDEncoding>(const NodeIDEncoding&)
    {
      return 1;
    }

    template<>
    void OStream::Serialize<NodeIDEncoding>(const NodeIDEncoding& value)
    {
      *this << static_cast<uint8_t>(value);
    }

    template<>
    void IStream::Deserialize<NodeIDEncoding>(NodeIDEncoding& value)
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
    void OStream::Serialize<OpcUa::NodeID>(const OpcUa::NodeID& id)
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
    void IStream::Deserialize<OpcUa::NodeID>(OpcUa::NodeID& id)
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

