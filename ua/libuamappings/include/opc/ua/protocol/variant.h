/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief C++ types of binary protocol.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_VARIANT_H__
#define __OPC_UA_VARIANT_H__

#include <opc/ua/protocol/datetime.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/status_codes.h>
#include <string>

#include <boost/variant.hpp>

namespace OpcUa
{
  enum class VariantType : uint8_t
  {
    NUL              = 0,
    BOOLEAN          = 1,
    SBYTE            = 2,
    BYTE             = 3,
    INT16            = 4,
    UINT16           = 5,
    INT32            = 6,
    UINT32           = 7,
    INT64            = 8,
    UINT64           = 9,
    FLOAT            = 10,
    DOUBLE           = 11,
    STRING           = 12,
    DATE_TIME        = 13,
    GUID             = 14,
    BYTE_STRING      = 15,
    XML_ELEMENT      = 16,
    NODE_ID          = 17,
    EXPANDED_NODE_ID = 18,
    STATUS_CODE      = 19,
    QUALIFIED_NAME   = 20,
    LOCALIZED_TEXT   = 21,
    EXTENSION_OBJECT = 22,
    DATA_VALUE       = 23,
    VARIANT          = 24,
    DIAGNOSTIC_INFO  = 25,
  };

  const uint8_t VALUE_TYPE_MASK = 0x3F;
  const uint8_t HAS_DIMENSIONS_MASK = 0x40;
  const uint8_t HAS_ARRAY_MASK = 0x80;


  struct DataValue;
  struct Variant;

  struct VariantValue
  {
    std::vector<bool> Boolean;
    std::vector<int8_t> SByte;
    std::vector<uint8_t> Byte;
    std::vector<int16_t> Int16;
    std::vector<uint16_t> UInt16;
    std::vector<int32_t> Int32;
    std::vector<uint32_t> UInt32;
    std::vector<int64_t> Int64;
    std::vector<uint64_t> UInt64;
    std::vector<float> Float;
    std::vector<double> Double;
    std::vector<StatusCode> Statuses;
    std::vector<std::string> String;
    std::vector<DateTime> Time;
    std::vector<Guid> Guids;
    std::vector<ByteString> ByteStrings;
    std::vector<NodeID> Node;
    std::vector<QualifiedName> Name;
    std::vector<LocalizedText> Text;
    std::vector<DataValue> Value;
    std::vector<Variant> Variants;
    std::vector<DiagnosticInfo> Diagnostic;

    VariantType GetType() const;

    VariantValue()
    {
    }

    explicit VariantValue(bool val);
    explicit VariantValue(int8_t val);
    explicit VariantValue(uint8_t val);
    explicit VariantValue(int16_t val);
    explicit VariantValue(uint16_t val);
    explicit VariantValue(int32_t val);
    explicit VariantValue(uint32_t val);
    explicit VariantValue(int64_t val);
    explicit VariantValue(uint64_t val);
    explicit VariantValue(float val);
    explicit VariantValue(double val);
    explicit VariantValue(StatusCode val);
    explicit VariantValue(const std::string& val);
    explicit VariantValue(const ByteString& val);
//    VariantValue(DateTime val) : Time{val} {} TODO create separate type instead of typedef to uint64_t
    explicit VariantValue(const Guid& val);
    explicit VariantValue(const NodeID& val);
    explicit VariantValue(const QualifiedName& val);
    explicit VariantValue(const LocalizedText& val);
    explicit VariantValue(const DataValue& val);
 //   explicit VariantValue(const Variant& val);
    explicit VariantValue(const DiagnosticInfo& val);

    explicit VariantValue(const std::vector<bool>& val);
    explicit VariantValue(const std::vector<int8_t>& val);
    explicit VariantValue(const std::vector<uint8_t>& val);
    explicit VariantValue(const std::vector<int16_t>& val);
    explicit VariantValue(const std::vector<uint16_t>& val);
    explicit VariantValue(const std::vector<int32_t>& val);
    explicit VariantValue(const std::vector<uint32_t>& val);
    explicit VariantValue(const std::vector<int64_t>& val);
    explicit VariantValue(const std::vector<uint64_t>& val);
    explicit VariantValue(const std::vector<float>& val);
    explicit VariantValue(const std::vector<double>& val);
    explicit VariantValue(const std::vector<StatusCode>& val);
    explicit VariantValue(const std::vector<std::string>& val);
    explicit VariantValue(const std::vector<ByteString>& val);
//    VariantValue(DateTime val); TODO create separate type instead of typedef to uint64_t
    explicit VariantValue(const std::vector<Guid>& val);
    explicit VariantValue(const std::vector<NodeID>& val);
    explicit VariantValue(const std::vector<QualifiedName>& val);
    explicit VariantValue(const std::vector<LocalizedText>& val);
    explicit VariantValue(const std::vector<DataValue>& val);
    explicit VariantValue(const std::vector<Variant>& val);
    explicit VariantValue(const std::vector<DiagnosticInfo>& val);

    VariantValue& operator= (bool val);
    VariantValue& operator= (int8_t val);
    VariantValue& operator= (uint8_t val);
    VariantValue& operator= (int16_t val);
    VariantValue& operator= (uint16_t val);
    VariantValue& operator= (int32_t val);
    VariantValue& operator= (uint32_t val);
    VariantValue& operator= (int64_t val);
    VariantValue& operator= (uint64_t val);
    VariantValue& operator= (float val);
    VariantValue& operator= (double val);
    VariantValue& operator= (StatusCode val);
    VariantValue& operator= (const std::string& val);
    VariantValue& operator= (const ByteString& val);
//  Value(DateTime val) : Time{val} {} TODO create separate type instead of typedef to uint64_t
    VariantValue& operator= (const Guid& val);
    VariantValue& operator= (const NodeID& val);
    VariantValue& operator= (const QualifiedName& val);
    VariantValue& operator= (const LocalizedText& val);
    VariantValue& operator= (const DataValue& val);
    VariantValue& operator= (const Variant& val);
    VariantValue& operator= (const DiagnosticInfo& val);

    VariantValue& operator= (const std::vector<bool>& val);
    VariantValue& operator= (const std::vector<int8_t>& val);
    VariantValue& operator= (const std::vector<uint8_t>& val);
    VariantValue& operator= (const std::vector<int16_t>& val);
    VariantValue& operator= (const std::vector<uint16_t>& val);
    VariantValue& operator= (const std::vector<int32_t>& val);
    VariantValue& operator= (const std::vector<uint32_t>& val);
    VariantValue& operator= (const std::vector<int64_t>& val);
    VariantValue& operator= (const std::vector<uint64_t>& val);
    VariantValue& operator= (const std::vector<float>& val);
    VariantValue& operator= (const std::vector<double>& val);
    VariantValue& operator= (const std::vector<StatusCode>& val);
    VariantValue& operator= (const std::vector<std::string>& val);
    VariantValue& operator= (const std::vector<ByteString>& val);
    VariantValue& operator= (const std::vector<Guid>& val);
    VariantValue& operator= (const std::vector<NodeID>& val);
    VariantValue& operator= (const std::vector<QualifiedName>& val);
    VariantValue& operator= (const std::vector<LocalizedText>& val);
    VariantValue& operator= (const std::vector<DataValue>& val);
    VariantValue& operator= (const std::vector<Variant>& val);
    VariantValue& operator= (const std::vector<DiagnosticInfo>& val);
};

  struct Variant
  {
  public:
    VariantValue Value;
    VariantType Type;
    std::vector<uint32_t> Dimensions;

    /// @brief test if holded value is an array.
    bool IsArray() const;

    bool IsNul() const;
    Variant();

    template <typename T>
    Variant& operator=(const T& value)
    {
      Value = value;
      Type = Value.GetType();
      return *this;
    }

    template <typename T>
    Variant(const T& value)
    {
      Value = value;
      Type = Value.GetType();
    }

    Variant(const Variant& var);
    Variant& operator= (const Variant& variant);
  };
} // namespace OpcUa

#endif //  __OPC_UA_VARIANT_H__
