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
    std::vector<std::string> String;
    std::vector<DateTime> Time;
    std::vector<Guid> Guids;
    std::vector<std::vector<uint8_t>> ByteString;
    std::vector<NodeID> Node;
    std::vector<uint32_t> StatusCode;
    std::vector<QualifiedName> Name;
    std::vector<LocalizedText> Text;
    std::vector<DataValue> Value;
    std::vector<Variant> Variants;
    std::vector<DiagnosticInfo> Diagnostic;

    VariantType GetType() const;
  };

  struct Variant
  {
  public:
    VariantType Type;
    VariantValue Value;
    std::vector<uint32_t> Dimensions;

    /// @brief test if holded value is an array.
    bool IsArray() const;

    bool IsNul() const;
    Variant();
    Variant(const Variant& var);

    Variant& operator= (const Variant& variant);
  };
} // namespace OpcUa

#endif //  __OPC_UA_VARIANT_H__
