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

#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/nodeid.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include <iostream>

namespace
{

  using namespace OpcUa;
  using namespace OpcUa::Binary;


  #define ApplyToVariantValue(type, var, func, secondParam) \
  { \
    switch (type) \
    { \
      case VariantType::BOOLEAN:          func(var.Boolean, secondParam); break; \
      case VariantType::SBYTE:            func(var.SByte, secondParam); break; \
      case VariantType::BYTE:             func(var.Byte, secondParam); break; \
      case VariantType::INT16:            func(var.Int16, secondParam); break; \
      case VariantType::UINT16:           func(var.UInt16, secondParam); break; \
      case VariantType::INT32:            func(var.Int32, secondParam); break; \
      case VariantType::UINT32:           func(var.UInt32, secondParam); break; \
      case VariantType::INT64:            func(var.Int64, secondParam); break; \
      case VariantType::UINT64:           func(var.UInt64, secondParam); break; \
      case VariantType::FLOAT:            func(var.Float, secondParam); break; \
      case VariantType::DOUBLE:           func(var.Double, secondParam); break; \
      case VariantType::STRING:           func(var.String, secondParam); break; \
      case VariantType::DATE_TIME:        func(var.Time, secondParam); break; \
      case VariantType::GUID:             func(var.Guids, secondParam); break; \
      case VariantType::BYTE_STRING:      func(var.ByteStrings, secondParam); break; \
      case VariantType::XML_ELEMENT:      func(var.String, secondParam); break; \
      case VariantType::NODE_ID:          func(var.Node, secondParam); break; \
      case VariantType::EXPANDED_NODE_ID: func(var.Node, secondParam); break; \
      case VariantType::STATUS_CODE:      func(var.Statuses, secondParam); break; \
      case VariantType::QUALIFIED_NAME:   func(var.Name, secondParam); break; \
      case VariantType::LOCALIZED_TEXT:   func(var.Text, secondParam); break; \
      case VariantType::DIAGNOSTIC_INFO:  func(var.Diagnostic, secondParam); break; \
      case VariantType::VARIANT:          func(var.Variants, secondParam); break; \
      case VariantType::DATA_VALUE:       func(var.Value, secondParam); break; \
      case VariantType::NUL: \
      case VariantType::EXTENSION_OBJECT: \
        break; \
      default: \
        throw std::logic_error("Unknown variant type."); \
    } \
  }

  template <typename T>
  bool IsValueArray(const std::vector<T>& t)
  {
    return t.size() > 1;
  }

  template <typename T>
  void IsValueArray(const std::vector<T>& t, bool& isArray)
  {
    isArray = IsValueArray(t);
  }

  template <typename T>
  bool IsNulValue(const std::vector<T>& t)
  {
    return t.empty();
  }

  template <typename T>
  void IsNulValue(const std::vector<T>& t, bool& isNul)
  {
    isNul = IsNulValue(t);
  }

  template <typename T>
  void RawValueSize(const std::vector<T>& t, std::size_t& size)
  {
    if (IsValueArray(t))
    {
      size = RawSizeContainer(t);
    }
    else if (!IsNulValue(t))
    {
      size = RawSize(t.at(0));
    }
    else
    {
      size = 0;
    }
  }


  template <typename T>
  void SerializeValue(const std::vector<T>& value, OpcUa::Binary::OStream& stream)
  {
    if (IsValueArray(value))
    {
      SerializeContainer(stream, value);
    }
    else if (!IsNulValue(value))
    {
      stream << value.at(0);
    }
  }

  template <typename T>
  void DeserializeContainerValue(std::vector<T>& value, OpcUa::Binary::IStream& stream)
  {
    DeserializeContainer(stream, value);
  }

  template <typename T>
  void DeserializeValue(std::vector<T>& value, OpcUa::Binary::IStream& stream)
  {
    T tmp;
    stream >> tmp;
    value.push_back(tmp);
  }


  void CopyValue(VariantType type, const VariantValue& value, VariantValue& var)
  {
    switch (type)
    {
      case VariantType::BOOLEAN:          var.Boolean = value.Boolean; break;
      case VariantType::SBYTE:            var.SByte = value.SByte; break;
      case VariantType::BYTE:             var.Byte = value.Byte; break;
      case VariantType::INT16:            var.Int16 = value.Int16; break;
      case VariantType::UINT16:           var.UInt16 = value.UInt16; break;
      case VariantType::INT32:            var.Int32 = value.Int32; break;
      case VariantType::UINT32:           var.UInt32 = value.UInt32; break;
      case VariantType::INT64:            var.Int64 = value.Int64; break;
      case VariantType::UINT64:           var.UInt64 = value.UInt64; break;
      case VariantType::FLOAT:            var.Float = value.Float; break;
      case VariantType::DOUBLE:           var.Double = value.Double; break;
      case VariantType::STRING:           var.String = value.String; break;
      case VariantType::DATE_TIME:        var.Time = value.Time; break;
      case VariantType::GUID:             var.Guids = value.Guids; break;
      case VariantType::BYTE_STRING:      var.ByteStrings = value.ByteStrings; break;
      case VariantType::XML_ELEMENT:      var.String = value.String; break;
      case VariantType::NODE_ID:          var.Node = value.Node; break;
      case VariantType::EXPANDED_NODE_ID: var.Node = value.Node; break;
      case VariantType::STATUS_CODE:      var.Statuses = value.Statuses; break;
      case VariantType::QUALIFIED_NAME:   var.Name = value.Name; break;
      case VariantType::LOCALIZED_TEXT:   var.Text = value.Text; break;
      case VariantType::DIAGNOSTIC_INFO:  var.Diagnostic = value.Diagnostic; break;
      case VariantType::VARIANT:          var.Variants = value.Variants; break;
      case VariantType::DATA_VALUE:       var.Value = value.Value; break;
      case VariantType::NUL:
      case VariantType::EXTENSION_OBJECT:
        break;
      default:
        throw std::logic_error("Unknown variant type.");
    }
  }

}

namespace OpcUa
{

    VariantValue::VariantValue(bool val) : Boolean(1, val) {}
    VariantValue::VariantValue(int8_t val) : SByte{val} {}
    VariantValue::VariantValue(uint8_t val) :  Byte{val} {}
    VariantValue::VariantValue(int16_t val) : Int16{val} {}
    VariantValue::VariantValue(uint16_t val) : UInt16{val} {}
    VariantValue::VariantValue(int32_t val) : Int32{val} {}
    VariantValue::VariantValue(uint32_t val) : UInt32{val} {}
    VariantValue::VariantValue(int64_t val) : Int64{val} {}
    VariantValue::VariantValue(uint64_t val) : UInt64{val} {}
    VariantValue::VariantValue(float val) : Float{val} {}
    VariantValue::VariantValue(double val) : Double{val} {}
    VariantValue::VariantValue(StatusCode val) : Statuses{val} {}
    VariantValue::VariantValue(const std::string& val) : String{val} {}
    VariantValue::VariantValue(const ByteString& val) : ByteStrings{val} {}
//    VariantValue::VariantValue(DateTime val) : Time{val} {} TODO create separate type instead of typedef to uint64_t
    VariantValue::VariantValue(const Guid& val) : Guids{val} {}
    VariantValue::VariantValue(const NodeID& val) : Node{val} {}
    VariantValue::VariantValue(const QualifiedName& val) : Name{val} {}
    VariantValue::VariantValue(const LocalizedText& val) : Text{val} {}
    VariantValue::VariantValue(const DataValue& val) : Value{val} {}
//    VariantValue::VariantValue(const Variant& val) : Variants{val} {}
    VariantValue::VariantValue(const DiagnosticInfo& val) : Diagnostic{val} {}

    VariantValue::VariantValue(const std::vector<bool>& val) : Boolean(val) {}
    VariantValue::VariantValue(const std::vector<int8_t>& val) : SByte(val) {}
    VariantValue::VariantValue(const std::vector<uint8_t>& val) :  Byte(val) {}
    VariantValue::VariantValue(const std::vector<int16_t>& val) : Int16(val) {}
    VariantValue::VariantValue(const std::vector<uint16_t>& val) : UInt16(val) {}
    VariantValue::VariantValue(const std::vector<int32_t>& val) : Int32(val) {}
    VariantValue::VariantValue(const std::vector<uint32_t>& val) : UInt32(val) {}
    VariantValue::VariantValue(const std::vector<int64_t>& val) : Int64(val) {}
    VariantValue::VariantValue(const std::vector<uint64_t>& val) : UInt64(val) {}
    VariantValue::VariantValue(const std::vector<float>& val) : Float(val) {}
    VariantValue::VariantValue(const std::vector<double>& val) : Double(val) {}
    VariantValue::VariantValue(const std::vector<StatusCode>& val) : Statuses(val) {} // TODO create separate type for Status code
    VariantValue::VariantValue(const std::vector<std::string>& val) : String(val) {}
    VariantValue::VariantValue(const std::vector<ByteString>& val) : ByteStrings(val) {}
//    VariantValue::VariantValuconst e(DateTime val) : Time{val} {} TODO create separate type instead of typedef to uint64_t
    VariantValue::VariantValue(const std::vector<Guid>& val) : Guids(val) {}
    VariantValue::VariantValue(const std::vector<NodeID>& val) : Node(val) {}
    VariantValue::VariantValue(const std::vector<QualifiedName>& val) : Name(val) {}
    VariantValue::VariantValue(const std::vector<LocalizedText>& val) : Text(val) {}
    VariantValue::VariantValue(const std::vector<DataValue>& val) : Value(val) {}
    VariantValue::VariantValue(const std::vector<Variant>& val) : Variants(val) {}
    VariantValue::VariantValue(const std::vector<DiagnosticInfo>& val) : Diagnostic(val) {}


    VariantValue& VariantValue::operator= (bool val)
    {
      Boolean = std::vector<bool>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (int8_t val)
    {
      SByte = std::vector<int8_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (uint8_t val)
    {
      Byte = std::vector<uint8_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (int16_t val)
    {
      Int16 = std::vector<int16_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (uint16_t val)
    {
      UInt16 = std::vector<uint16_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (int32_t val)
    {
      Int32 = std::vector<int32_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (uint32_t val)
    {
      UInt32 = std::vector<uint32_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (int64_t val)
    {
      Int64 = std::vector<int64_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (uint64_t val)
    {
      UInt64 = std::vector<uint64_t>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (float val)
    {
      Float = std::vector<float>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (double val)
    {
      Double = std::vector<double>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (StatusCode val)
    {
      Statuses = std::vector<StatusCode>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::string& val)
    {
      String = std::vector<std::string>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const ByteString& val)
    {
      ByteStrings = std::vector<ByteString>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const Guid& val)
    {
      Guids = std::vector<Guid>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const NodeID& val)
    {
      Node = std::vector<NodeID>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const QualifiedName& val)
    {
      Name = std::vector<QualifiedName>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const LocalizedText& val)
    {
      Text = std::vector<LocalizedText>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const DataValue& val)
    {
      Value = std::vector<DataValue>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const Variant& val)
    {
      Variants = std::vector<Variant>{val};
      return *this;
    }

    VariantValue& VariantValue::operator= (const DiagnosticInfo& val)
    {
      Diagnostic = std::vector<DiagnosticInfo>{val};
      return *this;
    }


    VariantValue& VariantValue::operator= (const std::vector<bool>& val)
    {
      Boolean = std::vector<bool>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<int8_t>& val)
    {
      SByte = std::vector<int8_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<uint8_t>& val)
    {
      Byte = std::vector<uint8_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<int16_t>& val)
    {
      Int16 = std::vector<int16_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<uint16_t>& val)
    {
      UInt16 = std::vector<uint16_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<int32_t>& val)
    {
      Int32 = std::vector<int32_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<uint32_t>& val)
    {
      UInt32 = std::vector<uint32_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<int64_t>& val)
    {
      Int64 = std::vector<int64_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<uint64_t>& val)
    {
      UInt64 = std::vector<uint64_t>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<float>& val)
    {
      Float = std::vector<float>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<double>& val)
    {
      Double = std::vector<double>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<StatusCode>& val)
    {
      Statuses = std::vector<StatusCode>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<std::string>& val)
    {
      String = std::vector<std::string>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<ByteString>& val)
    {
      ByteStrings = std::vector<ByteString>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<Guid>& val)
    {
      Guids = std::vector<Guid>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<NodeID>& val)
    {
      Node = std::vector<NodeID>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<QualifiedName>& val)
    {
      Name = std::vector<QualifiedName>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<LocalizedText>& val)
    {
      Text = std::vector<LocalizedText>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<DataValue>& val)
    {
      Value = std::vector<DataValue>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<Variant>& val)
    {
      Variants = std::vector<Variant>(val);
      return *this;
    }

    VariantValue& VariantValue::operator= (const std::vector<DiagnosticInfo>& val)
    {
      Diagnostic = std::vector<DiagnosticInfo>(val);
      return *this;
    }


  //---------------------------------------------------
  // Variant
  //---------------------------------------------------


  Variant::Variant()
    : Type(VariantType::NUL)
  {
  }
/*
  Variant::Variant(const Variant& var)
  {
    *this = var;
  }
*/

  Variant::Variant(const Variant& var)
  {
    Type = var.Type;
    Dimensions = var.Dimensions;
    CopyValue(Type, var.Value, this->Value);
  }

  Variant& Variant::operator= (const Variant& var)
  {
    Type = var.Type;
    Dimensions = var.Dimensions;
    CopyValue(Type, var.Value, this->Value);
    return *this;
  }

  bool Variant::IsArray() const
  {
    bool isArray = false;
    ApplyToVariantValue(Type, Value, IsValueArray, isArray);
    return isArray;
  }

  bool Variant::IsNul() const
  {
    if (Type == VariantType::NUL)
    {
      return true;
    }
    bool isNul = false;
    ApplyToVariantValue(Type, Value, IsNulValue, isNul);
    return isNul;
  }

  VariantType VariantValue::GetType() const
  {
    if (!Boolean.empty())
    {
      return VariantType::BOOLEAN;
    }
    if (!SByte.empty())
    {
      return VariantType::SBYTE;
    }

    if (!Byte.empty())
    {
      return VariantType::BYTE;
    }

    if (!UInt16.empty())
    {
      return VariantType::UINT16;
    }

    if (!Int16.empty())
    {
      return VariantType::INT16;
    }

    if (!UInt32.empty())
    {
      return VariantType::UINT32;
    }

    if (!Int32.empty())
    {
      return VariantType::INT32;
    }

    if (!UInt64.empty())
    {
      return VariantType::UINT64;
    }

    if (!Int64.empty())
    {
      return VariantType::INT64;
    }

    if (!Float.empty())
    {
      return VariantType::FLOAT;
    }

    if (!Double.empty())
    {
      return VariantType::DOUBLE;
    }
    if (!String.empty())
    {
      return VariantType::STRING;
    }
    if (!Time.empty())
    {
      return VariantType::DATE_TIME;
    }
    if (!Guids.empty())
    {
      return VariantType::GUID;
    }
    if (!ByteStrings.empty())
    {
      return VariantType::BYTE_STRING;
    }
    if (!Node.empty())
    {
      return VariantType::EXPANDED_NODE_ID;
    }
    if (!Statuses.empty())
    {
      return VariantType::STATUS_CODE;
    }
    if (!Name.empty())
    {
      return VariantType::QUALIFIED_NAME;
    }
    if (!Text.empty())
    {
      return VariantType::LOCALIZED_TEXT;
    }
    if (!Value.empty())
    {
      return VariantType::DATA_VALUE;
    }
    if (!Variants.empty())
    {
      return VariantType::VARIANT;
    }
    if (!Diagnostic.empty())
    {
      return VariantType::DIAGNOSTIC_INFO;
    }
    return VariantType::NUL;
  }


  namespace Binary
  {

    template<>
    std::size_t RawSize<Variant>(const Variant& var)
    {
      const uint8_t encodingMask = 0;
      std::size_t size = RawSize(encodingMask);
      if (var.IsNul())
      {
        return size;
      }

      std::size_t valueSize = 0;
      ApplyToVariantValue(var.Type, var.Value, RawValueSize, valueSize);
      size += valueSize;
      if (!var.Dimensions.empty())
      {
        size += RawSizeContainer(var.Dimensions);
      }

      return size;
    }

    template<>
    void OStream::Serialize<Variant>(const Variant& var)
    {
      uint8_t encodingMask = static_cast<uint8_t>(var.Type);
      if (!var.IsNul())
      {
        if (var.IsArray())
        {
          encodingMask |= HAS_ARRAY_MASK;
        }
        if (!var.Dimensions.empty())
        {
          encodingMask |= HAS_DIMENSIONS_MASK;
        }
      }

      *this << encodingMask;
      if (var.IsNul())
      {
        return;
      }

      ApplyToVariantValue(var.Type, var.Value, SerializeValue, *this);

      if (!var.Dimensions.empty())
      {
        SerializeContainer(*this, var.Dimensions);
      }
    }

    template<>
    void IStream::Deserialize<Variant>(Variant& var)
    {

      uint8_t encodingMask = 0;
      *this >> encodingMask;

      // TODO check validity of type value after decoding.
      var.Type = static_cast<VariantType>(encodingMask & VALUE_TYPE_MASK);
      if (encodingMask & HAS_ARRAY_MASK)
      {
        ApplyToVariantValue(var.Type, var.Value, DeserializeContainerValue, *this);
      }
      else
      {
        ApplyToVariantValue(var.Type, var.Value, DeserializeValue, *this);
      }

      if (encodingMask & HAS_DIMENSIONS_MASK)
      {
        DeserializeContainer(*this, var.Dimensions);
      }
    }
  } // namespace Binary
} // namespace OpcUa

