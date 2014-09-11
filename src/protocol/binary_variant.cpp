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
    typedef size_t result_type;

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == true, size_t>::type operator()(const T& val)
    {
      return RawSizeContainer(val);
    }

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == false, size_t>::type operator()(const T& val)
    {
      return RawSize(val);
    }
  };

  struct VariantSerializer
  {
    DataSerializer* Serializer;

    explicit VariantSerializer(DataSerializer* serializer)
      : Serializer(serializer)
    {

    }

    typedef void result_type;

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == true>::type operator()(const T& val)
    {
      SerializeContainer(*Serializer, val);
    }

    template <typename T>
    typename std::enable_if<is_container_not_string<T>::value == false>::type operator()(const T& val)
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
    if (t == typeid(std::vector<bool>))
      return Compare<std::vector<bool>>(*this, var);
    if (t == typeid(int8_t))
      return Compare<int8_t>(*this, var);
    if (t == typeid(std::vector<int8_t>))
      return Compare<std::vector<int8_t>>(*this, var);
    if (t == typeid(uint8_t))
      return Compare<uint8_t>(*this, var);
    if (t == typeid(std::vector<uint8_t>))
      return Compare<std::vector<uint8_t>>(*this, var);
    if (t == typeid(int16_t))
      return Compare<int16_t>(*this, var);
    if (t == typeid(std::vector<int16_t>))
      return Compare<std::vector<int16_t>>(*this, var);
    if (t == typeid(uint16_t))
      return Compare<uint16_t>(*this, var);
    if (t == typeid(std::vector<uint16_t>))
      return Compare<std::vector<uint16_t>>(*this, var);

    if (t == typeid(int32_t))
      return Compare<int32_t>(*this, var);
    if (t == typeid(std::vector<int32_t>))
      return Compare<std::vector<int32_t>>(*this, var);
    if (t == typeid(uint32_t))
      return Compare<uint32_t>(*this, var);
    if (t == typeid(std::vector<uint32_t>))
      return Compare<std::vector<uint32_t>>(*this, var);
    if (t == typeid(int64_t))
      return Compare<int64_t>(*this, var);
    if (t == typeid(std::vector<int64_t>))
      return Compare<std::vector<int64_t>>(*this, var);
    if (t == typeid(uint64_t))
      return Compare<uint64_t>(*this, var);
    if (t == typeid(std::vector<uint64_t>))
      return Compare<std::vector<uint64_t>>(*this, var);

    if (t == typeid(float))
      return Compare<float>(*this, var);
    if (t == typeid(std::vector<float>))
      return Compare<std::vector<float>>(*this, var);

    if (t == typeid(double))
      return Compare<double>(*this, var);
    if (t == typeid(std::vector<double>))
      return Compare<std::vector<double>>(*this, var);

    if (t == typeid(std::string))
      return Compare<std::string>(*this, var);
    if (t == typeid(std::vector<std::string>))
      return Compare<std::vector<std::string>>(*this, var);

    if (t == typeid(DateTime))
      return Compare<DateTime>(*this, var);
    if (t == typeid(std::vector<DateTime>))
      return Compare<std::vector<DateTime>>(*this, var);

    if (t == typeid(Guid))
      return Compare<Guid>(*this, var);
    if (t == typeid(std::vector<Guid>))
      return Compare<std::vector<Guid>>(*this, var);

    if (t == typeid(ByteString))
      return Compare<ByteString>(*this, var);
    if (t == typeid(std::vector<ByteString>))
      return Compare<std::vector<ByteString>>(*this, var);

    if (t == typeid(NodeID))
      return Compare<NodeID>(*this, var);
    if (t == typeid(std::vector<NodeID>))
      return Compare<std::vector<NodeID>>(*this, var);

    if (t == typeid(StatusCode))
      return Compare<StatusCode>(*this, var);
    if (t == typeid(std::vector<StatusCode>))
      return Compare<std::vector<StatusCode>>(*this, var);

    if (t == typeid(LocalizedText))
      return Compare<LocalizedText>(*this, var);
    if (t == typeid(std::vector<LocalizedText>))
      return Compare<std::vector<LocalizedText>>(*this, var);

    if (t == typeid(QualifiedName))
      return Compare<QualifiedName>(*this, var);
    if (t == typeid(std::vector<QualifiedName>))
      return Compare<std::vector<QualifiedName>>(*this, var);

    if (t == typeid(DataValue))
      return Compare<DataValue>(*this, Value);
    if (t == typeid(std::vector<DataValue>))
      return Compare<std::vector<DataValue>>(*this, var);

    if (t == typeid(Variant))
      return Compare<Variant>(*this, var);
    if (t == typeid(std::vector<Variant>))
      return Compare<std::vector<Variant>>(*this, var);

    if (t == typeid(DiagnosticInfo))
      return Compare<DiagnosticInfo>(*this, var);
    if (t == typeid(std::vector<DiagnosticInfo>))
      return Compare<std::vector<DiagnosticInfo>>(*this, var);

    throw std::logic_error("Unknown variant type.");
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
      size += var.Visit(rawSizeCalc);
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
      var.Visit(variantSerializer);

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
      if(encodingMask == ((uint8_t)VariantType::DATA_VALUE))
        var = deserializer.get<DataValue>();
      if(encodingMask == ((uint8_t)VariantType::DATA_VALUE | HAS_ARRAY_MASK))
        var = deserializer.get<std::vector<DataValue>>();
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

