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
#include <opc/ua/protocol/string_utils.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/variant_visitor.h>

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
  void RawValueSizeArray(const std::vector<T>& t, std::size_t& size)
  {
      size = RawSizeContainer(t);
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
  void SerializeValueArray(const std::vector<T>& value, OpcUa::Binary::DataSerializer& stream)
  {
      SerializeContainer(stream, value);
  }

  template <typename T>
  void SerializeValue(const std::vector<T>& value, OpcUa::Binary::DataSerializer& stream)
  {
    if (IsValueArray(value))
    {
      SerializeContainer(stream, value);
    }
    else if (!IsNulValue(value))
    {
      stream.Serialize(value.at(0));
    }
  }

  template <typename T>
  void DeserializeContainerValue(std::vector<T>& value, OpcUa::Binary::DataDeserializer& stream)
  {
    DeserializeContainer(stream, value);
  }

  template <typename T>
  void DeserializeValue(std::vector<T>& value, OpcUa::Binary::DataDeserializer& stream)
  {
    T tmp;
    stream.Deserialize(tmp);
    value.push_back(tmp);
  }

  struct RawSizeVisitor
  {
    size_t Result = 0;

    template <typename T>
    void OnContainer(const T& val)
    {
      Result = RawSizeContainer(val);
    }

    template <typename T>
    void OnScalar(const T& val)
    {
      Result = RawSize(val);
    }
  };

  struct VariantSerializer
  {
    DataSerializer* Serializer;

    explicit VariantSerializer(DataSerializer* serializer)
      : Serializer(serializer)
    {
    }

    template <typename T>
    void OnContainer(const T& val)
    {
      SerializeContainer(*Serializer, val);
    }

    template <typename T>
    void OnScalar(const T& val)
    {
      Serializer->Serialize(val);
    }
  };

  struct VariantDeserializer
  {
    DataDeserializer* Deserializer;

    explicit VariantDeserializer(DataDeserializer* deserializer)
      : Deserializer(deserializer)
    {

    }

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == true, T>::type get()
    {
      T tmp;
      DeserializeContainer(*Deserializer, tmp);
      return tmp;
    }

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == false, T>::type get()
    {
      T tmp;
      *Deserializer >> tmp;
      return tmp;
    }
  };

  template<typename T>
  bool Compare(const Variant& lhs, const Variant& rhs)
  {
    return lhs.As<T>() == rhs.As<T>();
  }
}

namespace OpcUa
{
  //---------------------------------------------------
  // Variant
  //---------------------------------------------------

  bool Variant::operator== (const Variant& var) const
  {
    if (Value.empty() ^ var.Value.empty())
    {
      return false;
    }

    if (Value.empty() && var.Value.empty())
    {
      return true;
    }

    if (Value.type() != var.Value.type())
    {
      return false;
    }

    using namespace boost;
    const std::type_info& t = Value.type();
    if (t == typeid(bool))
      return Compare<bool>(*this, var);
    else if (t == typeid(std::vector<bool>))
      return Compare<std::vector<bool>>(*this, var);
    else if (t == typeid(int8_t))
      return Compare<int8_t>(*this, var);
    else if (t == typeid(std::vector<int8_t>))
      return Compare<std::vector<int8_t>>(*this, var);
    else if (t == typeid(uint8_t))
      return Compare<uint8_t>(*this, var);
    else if (t == typeid(std::vector<uint8_t>))
      return Compare<std::vector<uint8_t>>(*this, var);
    else if (t == typeid(int16_t))
      return Compare<int16_t>(*this, var);
    else if (t == typeid(std::vector<int16_t>))
      return Compare<std::vector<int16_t>>(*this, var);
    else if (t == typeid(uint16_t))
      return Compare<uint16_t>(*this, var);
    else if (t == typeid(std::vector<uint16_t>))
      return Compare<std::vector<uint16_t>>(*this, var);

    else if (t == typeid(int32_t))
      return Compare<int32_t>(*this, var);
    else if (t == typeid(std::vector<int32_t>))
      return Compare<std::vector<int32_t>>(*this, var);
    else if (t == typeid(uint32_t))
      return Compare<uint32_t>(*this, var);
    else if (t == typeid(std::vector<uint32_t>))
      return Compare<std::vector<uint32_t>>(*this, var);
    else if (t == typeid(int64_t))
      return Compare<int64_t>(*this, var);
    else if (t == typeid(std::vector<int64_t>))
      return Compare<std::vector<int64_t>>(*this, var);
    else if (t == typeid(uint64_t))
      return Compare<uint64_t>(*this, var);
    else if (t == typeid(std::vector<uint64_t>))
      return Compare<std::vector<uint64_t>>(*this, var);

    else if (t == typeid(float))
      return Compare<float>(*this, var);
    else if (t == typeid(std::vector<float>))
      return Compare<std::vector<float>>(*this, var);

    else if (t == typeid(double))
      return Compare<double>(*this, var);
    else if (t == typeid(std::vector<double>))
      return Compare<std::vector<double>>(*this, var);

    else if (t == typeid(std::string))
      return Compare<std::string>(*this, var);
    else if (t == typeid(std::vector<std::string>))
      return Compare<std::vector<std::string>>(*this, var);

    else if (t == typeid(DateTime))
      return Compare<DateTime>(*this, var);
    else if (t == typeid(std::vector<DateTime>))
      return Compare<std::vector<DateTime>>(*this, var);

    else if (t == typeid(Guid))
      return Compare<Guid>(*this, var);
    else if (t == typeid(std::vector<Guid>))
      return Compare<std::vector<Guid>>(*this, var);

    else if (t == typeid(ByteString))
      return Compare<ByteString>(*this, var);
    else if (t == typeid(std::vector<ByteString>))
      return Compare<std::vector<ByteString>>(*this, var);

    else if (t == typeid(NodeID))
      return Compare<NodeID>(*this, var);
    else if (t == typeid(std::vector<NodeID>))
      return Compare<std::vector<NodeID>>(*this, var);

    else if (t == typeid(StatusCode))
      return Compare<StatusCode>(*this, var);
    else if (t == typeid(std::vector<StatusCode>))
      return Compare<std::vector<StatusCode>>(*this, var);

    else if (t == typeid(LocalizedText))
      return Compare<LocalizedText>(*this, var);
    else if (t == typeid(std::vector<LocalizedText>))
      return Compare<std::vector<LocalizedText>>(*this, var);

    else if (t == typeid(QualifiedName))
      return Compare<QualifiedName>(*this, var);
    else if (t == typeid(std::vector<QualifiedName>))
      return Compare<std::vector<QualifiedName>>(*this, var);
/*
    else if (t == typeid(DataValue))
      return Compare<DataValue>(*this, Value);
    else if (t == typeid(std::vector<DataValue>))
      return Compare<std::vector<DataValue>>(*this, var);
*/
    else if (t == typeid(Variant))
      return Compare<Variant>(*this, var);
    else if (t == typeid(std::vector<Variant>))
      return Compare<std::vector<Variant>>(*this, var);

    else if (t == typeid(DiagnosticInfo))
      return Compare<DiagnosticInfo>(*this, var);
    else if (t == typeid(std::vector<DiagnosticInfo>))
      return Compare<std::vector<DiagnosticInfo>>(*this, var);

    throw std::logic_error(std::string("Unknown variant type '") + t.name() + std::string("'."));
  }

  bool Variant::IsScalar() const
  {
    return !IsArray();
  }

  bool Variant::IsNul() const
  {
    return Value.empty();
  }

  bool Variant::IsArray() const
  {
    const std::type_info& t = Value.type();
    return
    (t == typeid(std::vector<bool>))       ||
    (t == typeid(std::vector<int8_t>))     ||
    (t == typeid(std::vector<uint8_t>))    ||
    (t == typeid(std::vector<int16_t>))    ||
    (t == typeid(std::vector<uint16_t>))   ||
    (t == typeid(std::vector<int32_t>))    ||
    (t == typeid(std::vector<uint32_t>))   ||
    (t == typeid(std::vector<int64_t>))    ||
    (t == typeid(std::vector<uint64_t>))   ||
    (t == typeid(std::vector<float>))      ||
    (t == typeid(std::vector<double>))     ||
    (t == typeid(std::vector<std::string>))||
    (t == typeid(std::vector<DateTime>))   ||
    (t == typeid(std::vector<Guid>))       ||
    (t == typeid(std::vector<ByteString>)) ||
    (t == typeid(std::vector<NodeID>))     ||
    (t == typeid(std::vector<StatusCode>)) ||
    (t == typeid(std::vector<LocalizedText>)) ||
    (t == typeid(std::vector<QualifiedName>)) ||
//    (t == typeid(std::vector<DataValue>))  ||
    (t == typeid(std::vector<Variant>))    ||
    (t == typeid(std::vector<DiagnosticInfo>));
  }

  VariantType Variant::Type() const
  {
    if (Value.empty())
      return VariantType::NUL;

    const std::type_info& t = Value.type();
    if (t == typeid(bool) || t == typeid(std::vector<bool>))
      return VariantType::BOOLEAN;
    else if (t == typeid(int8_t) || t == typeid(std::vector<int8_t>))
      return VariantType::SBYTE;
    else if (t == typeid(uint8_t) || t == typeid(std::vector<uint8_t>))
      return VariantType::BYTE;
    else if (t == typeid(int16_t) || t == typeid(std::vector<int16_t>))
      return VariantType::INT16;
    else if (t == typeid(uint16_t) || t == typeid(std::vector<uint16_t>))
      return VariantType::UINT16;
    else if (t == typeid(int32_t) || t == typeid(std::vector<int32_t>))
      return VariantType::INT32;
    else if (t == typeid(uint32_t) || t == typeid(std::vector<uint32_t>))
      return VariantType::UINT32;
    else if (t == typeid(int64_t) || t == typeid(std::vector<int64_t>))
      return VariantType::INT64;
    else if (t == typeid(uint64_t) || t == typeid(std::vector<uint64_t>))
      return VariantType::UINT64;
    else if (t == typeid(float) || t == typeid(std::vector<float>))
      return VariantType::FLOAT;
    else if (t == typeid(double) || t == typeid(std::vector<double>))
      return VariantType::DOUBLE;
    else if (t == typeid(std::string) || t == typeid(std::vector<std::string>))
      return VariantType::STRING;
    else if (t == typeid(DateTime) || t == typeid(std::vector<DateTime>))
      return VariantType::DATE_TIME;
    else if (t == typeid(Guid) || t == typeid(std::vector<Guid>))
      return VariantType::GUID;
    else if (t == typeid(ByteString) || t == typeid(std::vector<ByteString>))
      return VariantType::BYTE_STRING;
    else if (t == typeid(NodeID) || t == typeid(std::vector<NodeID>))
      return VariantType::NODE_ID;
    else if (t == typeid(StatusCode) || t == typeid(std::vector<StatusCode>))
      return VariantType::STATUS_CODE;
    else if (t == typeid(LocalizedText) || t == typeid(std::vector<LocalizedText>))
      return VariantType::LOCALIZED_TEXT;
    else if (t == typeid(QualifiedName) || t == typeid(std::vector<QualifiedName>))
      return VariantType::QUALIFIED_NAME;
/*
    else if (t == typeid(DataValue) || t == typeid(std::vector<DataValue>))
      return VariantType::DATA_VALUE;
*/
    else if (t == typeid(Variant) || t == typeid(std::vector<Variant>))
      return VariantType::VARIANT;
    else if (t == typeid(DiagnosticInfo) || t == typeid(std::vector<DiagnosticInfo>))
      return VariantType::DIAGNOSTIC_INFO;

    throw std::runtime_error(std::string("Unknown variant type '") + t.name() + "'.");
  }


  void Variant::Visit(VariantVisitor& visitor) const
  {
    using namespace boost;
    const std::type_info& t = Value.type();
    if (t == typeid(bool))
      visitor.Visit(any_cast<bool>(Value));
    else if (t == typeid(std::vector<bool>))
      visitor.Visit(any_cast<std::vector<bool>>(Value));
    else if (t == typeid(int8_t))
      visitor.Visit(any_cast<int8_t>(Value));
    else if (t == typeid(std::vector<int8_t>))
      visitor.Visit(any_cast<std::vector<int8_t>>(Value));
    else if (t == typeid(uint8_t))
      visitor.Visit(any_cast<uint8_t>(Value));
    else if (t == typeid(std::vector<uint8_t>))
      visitor.Visit(any_cast<std::vector<uint8_t>>(Value));
    else if (t == typeid(int16_t))
      visitor.Visit(any_cast<int16_t>(Value));
    else if (t == typeid(std::vector<int16_t>))
      visitor.Visit(any_cast<std::vector<int16_t>>(Value));
    else if (t == typeid(uint16_t))
      visitor.Visit(any_cast<uint16_t>(Value));
    else if (t == typeid(std::vector<uint16_t>))
      visitor.Visit(any_cast<std::vector<uint16_t>>(Value));

    else if (t == typeid(int32_t))
      visitor.Visit(any_cast<int32_t>(Value));
    else if (t == typeid(std::vector<int32_t>))
      visitor.Visit(any_cast<std::vector<int32_t>>(Value));
    else if (t == typeid(uint32_t))
      visitor.Visit(any_cast<uint32_t>(Value));
    else if (t == typeid(std::vector<uint32_t>))
      visitor.Visit(any_cast<std::vector<uint32_t>>(Value));
    else if (t == typeid(int64_t))
      visitor.Visit(any_cast<int64_t>(Value));
    else if (t == typeid(std::vector<int64_t>))
      visitor.Visit(any_cast<std::vector<int64_t>>(Value));
    else if (t == typeid(uint64_t))
      visitor.Visit(any_cast<uint64_t>(Value));
    else if (t == typeid(std::vector<uint64_t>))
      visitor.Visit(any_cast<std::vector<uint64_t>>(Value));

    else if (t == typeid(float))
      visitor.Visit(any_cast<float>(Value));
    else if (t == typeid(std::vector<float>))
      visitor.Visit(any_cast<std::vector<float>>(Value));

    else if (t == typeid(double))
      visitor.Visit(any_cast<double>(Value));
    else if (t == typeid(std::vector<double>))
      visitor.Visit(any_cast<std::vector<double>>(Value));

    else if (t == typeid(std::string))
      visitor.Visit(any_cast<std::string>(Value));
    else if (t == typeid(std::vector<std::string>))
      visitor.Visit(any_cast<std::vector<std::string>>(Value));

    else if (t == typeid(DateTime))
      visitor.Visit(any_cast<DateTime>(Value));
    else if (t == typeid(std::vector<DateTime>))
      visitor.Visit(any_cast<std::vector<DateTime>>(Value));

    else if (t == typeid(Guid))
      visitor.Visit(any_cast<Guid>(Value));
    else if (t == typeid(std::vector<Guid>))
      visitor.Visit(any_cast<std::vector<Guid>>(Value));

    else if (t == typeid(ByteString))
      visitor.Visit(any_cast<ByteString>(Value));
    else if (t == typeid(std::vector<ByteString>))
      visitor.Visit(any_cast<std::vector<ByteString>>(Value));

    else if (t == typeid(NodeID))
      visitor.Visit(any_cast<NodeID>(Value));
    else if (t == typeid(std::vector<NodeID>))
      visitor.Visit(any_cast<std::vector<NodeID>>(Value));

    else if (t == typeid(StatusCode))
      visitor.Visit(any_cast<StatusCode>(Value));
    else if (t == typeid(std::vector<StatusCode>))
      visitor.Visit(any_cast<std::vector<StatusCode>>(Value));

    else if (t == typeid(LocalizedText))
      visitor.Visit(any_cast<LocalizedText>(Value));
    else if (t == typeid(std::vector<LocalizedText>))
      visitor.Visit(any_cast<std::vector<LocalizedText>>(Value));

    else if (t == typeid(QualifiedName))
      visitor.Visit(any_cast<QualifiedName>(Value));
    else if (t == typeid(std::vector<QualifiedName>))
      visitor.Visit(any_cast<std::vector<QualifiedName>>(Value));
/*
    else if (t == typeid(DataValue))
      visitor.Visit(any_cast<DataValue>(Value));
    else if (t == typeid(std::vector<DataValue>))
      visitor.Visit(any_cast<std::vector<DataValue>>(Value));
*/
    else if (t == typeid(Variant))
      visitor.Visit(any_cast<Variant>(Value));
    else if (t == typeid(std::vector<Variant>))
      visitor.Visit(any_cast<std::vector<Variant>>(Value));

    else if (t == typeid(DiagnosticInfo))
      visitor.Visit(any_cast<DiagnosticInfo>(Value));
    else if (t == typeid(std::vector<DiagnosticInfo>))
      visitor.Visit(any_cast<std::vector<DiagnosticInfo>>(Value));
    else
      throw std::runtime_error(std::string("Unknown variant type '") + t.name() + "'.");
  }

  ObjectID VariantTypeToDataType(VariantType vt)
  {
    switch (vt)
    {
      case VariantType::BOOLEAN:          return ObjectID::Boolean;
      case VariantType::SBYTE:            return ObjectID::SByte;
      case VariantType::BYTE:             return ObjectID::Byte;
      case VariantType::INT16:            return ObjectID::Int16;
      case VariantType::UINT16:           return ObjectID::UInt16;
      case VariantType::INT32:            return ObjectID::Int32;
      case VariantType::UINT32:           return ObjectID::UInt32;
      case VariantType::INT64:            return ObjectID::Int64;
      case VariantType::UINT64:           return ObjectID::UInt64;
      case VariantType::FLOAT:            return ObjectID::Float;
      case VariantType::DOUBLE:           return ObjectID::Double;
      case VariantType::STRING:           return ObjectID::String;
      case VariantType::DATE_TIME:        return ObjectID::DateTime;
      case VariantType::GUID:             return ObjectID::Guid;
      case VariantType::BYTE_STRING:      return ObjectID::ByteString;
      case VariantType::XML_ELEMENT:      return ObjectID::XmlElement;
      case VariantType::NODE_ID:          return ObjectID::NodeID;
      case VariantType::EXPANDED_NODE_ID: return ObjectID::ExpandedNodeID;
      case VariantType::STATUS_CODE:      return ObjectID::StatusCode;
      case VariantType::QUALIFIED_NAME:   return ObjectID::QualifiedName;
      case VariantType::LOCALIZED_TEXT:   return ObjectID::LocalizedText;
      case VariantType::DIAGNOSTIC_INFO:  return ObjectID::DiagnosticInfo;
      case VariantType::DATA_VALUE:       return ObjectID::DataValue;
      case VariantType::NUL:              return ObjectID::Null;
      case VariantType::EXTENSION_OBJECT:
      case VariantType::VARIANT:
      default:
      {
        throw std::runtime_error("Unknown variant type.");
      }
    }
  }

  VariantType DataTypeToVariantType(const NodeID& dataType)
  {
    if (dataType.GetNamespaceIndex())
    {
      std::string msg("Cannot convert to variant type: invalid namespace of node ");
      throw std::runtime_error(msg + ToString(dataType));
    }

    switch (static_cast<OpcUa::ObjectID>(dataType.GetIntegerIdentifier()))
    {
      case ObjectID::Boolean:        return VariantType::BOOLEAN;
      case ObjectID::SByte:          return VariantType::SBYTE;
      case ObjectID::Byte:           return VariantType::BYTE;
      case ObjectID::Int16:          return VariantType::INT16;
      case ObjectID::UInt16:         return VariantType::UINT16;
      case ObjectID::Int32:          return VariantType::INT32;
      case ObjectID::UInt32:         return VariantType::UINT32;
      case ObjectID::Int64:          return VariantType::INT64;
      case ObjectID::UInt64:         return VariantType::UINT64;
      case ObjectID::Float:          return VariantType::FLOAT;
      case ObjectID::Double:         return VariantType::DOUBLE;
      case ObjectID::String:         return VariantType::STRING;
      case ObjectID::DateTime:       return VariantType::DATE_TIME;
      case ObjectID::Guid:           return VariantType::GUID;
      case ObjectID::ByteString:     return VariantType::BYTE_STRING;
      case ObjectID::XmlElement:     return VariantType::XML_ELEMENT;
      case ObjectID::NodeID:         return VariantType::NODE_ID;
      case ObjectID::ExpandedNodeID: return VariantType::EXPANDED_NODE_ID;
      case ObjectID::StatusCode:     return VariantType::STATUS_CODE;
      case ObjectID::QualifiedName:  return VariantType::QUALIFIED_NAME;
      case ObjectID::LocalizedText:  return VariantType::LOCALIZED_TEXT;
      case ObjectID::DiagnosticInfo: return VariantType::DIAGNOSTIC_INFO;
      case ObjectID::DataValue:      return VariantType::DATA_VALUE;
      case ObjectID::Null:           return VariantType::NUL;
      default:
        return VariantType::NODE_ID;
/*
      {
        std::string msg("Unknown type id ");
        throw std::runtime_error(msg  + ToString(dataType));
      }
*/
    }
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

      RawSizeVisitor rawSizeCalc;
      TypedVisitor<RawSizeVisitor> visitor(rawSizeCalc);
      var.Visit(visitor);
      size += rawSizeCalc.Result;
      if (!var.Dimensions.empty())
      {
        size += RawSizeContainer(var.Dimensions);
      }

      return size;
    }

    template<>
    void DataSerializer::Serialize<Variant>(const Variant& var)
    {
      uint8_t encodingMask = static_cast<uint8_t>(var.Type());
      if (var.IsArray())
      {
        encodingMask |= HAS_ARRAY_MASK;
      }
      if (!var.Dimensions.empty())
      {
        encodingMask |= HAS_DIMENSIONS_MASK;
      }
      Serialize(encodingMask);
      if (var.IsNul())
      {
        return;
      }

      VariantSerializer variantSerializer(this);
      TypedVisitor<VariantSerializer> visitor(variantSerializer);
      var.Visit(visitor);

      if (!var.Dimensions.empty())
      {
        SerializeContainer(*this, var.Dimensions);
      }
    }

    template<>
    void DataDeserializer::Deserialize<Variant>(Variant& var)
    {

      uint8_t encoding = 0;
      Deserialize(encoding);

      VariantDeserializer deserializer(this);

      const uint8_t encodingMask = encoding & (~HAS_DIMENSIONS_MASK);
      // TODO check validity of type value after decoding.
      if(encodingMask == (uint8_t)VariantType::BOOLEAN)
        var = deserializer.get<bool>();
      if(encodingMask == ((uint8_t)VariantType::BOOLEAN | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<bool>>();
      if(encodingMask == (uint8_t)VariantType::SBYTE)
        var = deserializer.get<int8_t>();
      if(encodingMask == ((uint8_t)VariantType::SBYTE | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<int8_t>>();
      if(encodingMask == (uint8_t)VariantType::BYTE)
        var = deserializer.get<uint8_t>();
      if(encodingMask == ((uint8_t)VariantType::BYTE | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<uint8_t>>();
      if(encodingMask == ((uint8_t)VariantType::INT16))
        var = deserializer.get<int16_t>();
      if(encodingMask == ((uint8_t)VariantType::INT16 | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<int16_t>>();
      if(encodingMask == ((uint8_t)VariantType::UINT16))
        var = deserializer.get<uint16_t>();
      if(encodingMask == ((uint8_t)VariantType::UINT16 | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<uint16_t>>();
      if(encodingMask == ((uint8_t)VariantType::INT32))
        var = deserializer.get<int32_t>();
      if(encodingMask == ((uint8_t)VariantType::INT32 | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<int32_t>>();
      if(encodingMask == ((uint8_t)VariantType::UINT32))
        var = deserializer.get<uint32_t>();
      if(encodingMask == ((uint8_t)VariantType::UINT32 | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<uint32_t>>();
      if(encodingMask == ((uint8_t)VariantType::INT64))
        var = deserializer.get<int64_t>();
      if(encodingMask == ((uint8_t)VariantType::INT64 | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<int64_t>>();
      if(encodingMask == ((uint8_t)VariantType::UINT64))
        var = deserializer.get<uint64_t>();
      if(encodingMask == ((uint8_t)VariantType::UINT64 | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<uint64_t>>();
      if(encodingMask == ((uint8_t)VariantType::FLOAT))
        var = deserializer.get<float>();
      if(encodingMask == ((uint8_t)VariantType::FLOAT | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<float>>();
      if(encodingMask == ((uint8_t)VariantType::DOUBLE))
        var = deserializer.get<double>();
      if(encodingMask == ((uint8_t)VariantType::DOUBLE | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<double>>();
      if(encodingMask == ((uint8_t)VariantType::STRING))
        var = deserializer.get<std::string>();
      if(encodingMask == ((uint8_t)VariantType::STRING | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<std::string>>();
      if(encodingMask == ((uint8_t)VariantType::DATE_TIME))
        var = deserializer.get<DateTime>();
      if(encodingMask == ((uint8_t)VariantType::DATE_TIME | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<DateTime>>();
      if(encodingMask == ((uint8_t)VariantType::GUID))
        var = deserializer.get<Guid>();
      if(encodingMask == ((uint8_t)VariantType::GUID | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<Guid>>();
      if(encodingMask == ((uint8_t)VariantType::BYTE_STRING))
        var = deserializer.get<ByteString>();
      if(encodingMask == ((uint8_t)VariantType::BYTE_STRING | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<ByteString>>();
      if(encodingMask == ((uint8_t)VariantType::NODE_ID))
        var = deserializer.get<NodeID>();
      if(encodingMask == ((uint8_t)VariantType::NODE_ID | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<NodeID>>();
      if(encodingMask == ((uint8_t)VariantType::STATUS_CODE))
        var = deserializer.get<StatusCode>();
      if(encodingMask == ((uint8_t)VariantType::STATUS_CODE | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<StatusCode>>();

      if(encodingMask == ((uint8_t)VariantType::LOCALIZED_TEXT))
        var = deserializer.get<LocalizedText>();
      if(encodingMask == ((uint8_t)VariantType::LOCALIZED_TEXT | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<LocalizedText>>();

      if(encodingMask == ((uint8_t)VariantType::QUALIFIED_NAME))
        var = deserializer.get<QualifiedName>();
      if(encodingMask == ((uint8_t)VariantType::QUALIFIED_NAME | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<QualifiedName>>();
/*
      if(encodingMask == ((uint8_t)VariantType::DATA_VALUE))
        var = deserializer.get<DataValue>();
      if(encodingMask == ((uint8_t)VariantType::DATA_VALUE | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<DataValue>>();
*/
      if(encodingMask == ((uint8_t)VariantType::VARIANT))
        var = deserializer.get<Variant>();
      if(encodingMask == ((uint8_t)VariantType::VARIANT | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<Variant>>();
      if(encodingMask == ((uint8_t)VariantType::DIAGNOSTIC_INFO))
        var = deserializer.get<DiagnosticInfo>();
      if(encodingMask == ((uint8_t)VariantType::DIAGNOSTIC_INFO | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<DiagnosticInfo>>();

      if (encoding & HAS_DIMENSIONS_MASK)
      {
        DeserializeContainer(*this, var.Dimensions);
      }
    }

    template<>
    void DataSerializer::Serialize<std::vector<Variant>>(const std::vector<Variant>& targets)
    {
      SerializeContainer(*this, targets);
    }

    template<>
    void DataDeserializer::Deserialize<std::vector<Variant>>(std::vector<Variant>& targets)
    {
      DeserializeContainer(*this, targets);
    }

  } // namespace Binary
} // namespace OpcUa
