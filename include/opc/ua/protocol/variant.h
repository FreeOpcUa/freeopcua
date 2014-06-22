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
#include <opc/ua/protocol/status_codes.h>
#include <string>

#include <stdexcept>

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
    explicit VariantValue(const DateTime& val);
    explicit VariantValue(const Guid& val);
    explicit VariantValue(const NodeID& val);
    explicit VariantValue(const QualifiedName& val);
    explicit VariantValue(const LocalizedText& val);
    explicit VariantValue(const DataValue& val);
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
    explicit VariantValue(const std::vector<DateTime>& val);
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
    VariantValue& operator= (const DateTime& val);
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
    VariantValue& operator= (const std::vector<DateTime>& val);
    VariantValue& operator= (const std::vector<ByteString>& val);
    VariantValue& operator= (const std::vector<Guid>& val);
    VariantValue& operator= (const std::vector<NodeID>& val);
    VariantValue& operator= (const std::vector<QualifiedName>& val);
    VariantValue& operator= (const std::vector<LocalizedText>& val);
    VariantValue& operator= (const std::vector<DataValue>& val);
    VariantValue& operator= (const std::vector<Variant>& val);
    VariantValue& operator= (const std::vector<DiagnosticInfo>& val);

    bool operator== (bool val) const;
    bool operator== (int8_t val) const;
    bool operator== (uint8_t val) const;
    bool operator== (int16_t val) const;
    bool operator== (uint16_t val) const;
    bool operator== (int32_t val) const;
    bool operator== (uint32_t val) const;
    bool operator== (int64_t val) const;
    bool operator== (uint64_t val) const;
    bool operator== (float val) const;
    bool operator== (double val) const;
    bool operator== (StatusCode val) const;
    bool operator== (const std::string& val) const;
    bool operator== (const DateTime& val) const;
    bool operator== (const ByteString& val) const;
    bool operator== (const Guid& val) const;
    bool operator== (const NodeID& val) const;
    bool operator== (const QualifiedName& val) const;
    bool operator== (const LocalizedText& val) const;
    bool operator== (const DataValue& val) const;
    bool operator== (const Variant& val) const;
    bool operator== (const DiagnosticInfo& val) const;
    bool operator== (const std::vector<bool>& val) const;
    bool operator== (const std::vector<int8_t>& val) const;
    bool operator== (const std::vector<uint8_t>& val) const;
    bool operator== (const std::vector<int16_t>& val) const;
    bool operator== (const std::vector<uint16_t>& val) const;
    bool operator== (const std::vector<int32_t>& val) const;
    bool operator== (const std::vector<uint32_t>& val) const;
    bool operator== (const std::vector<int64_t>& val) const;
    bool operator== (const std::vector<uint64_t>& val) const;
    bool operator== (const std::vector<float>& val) const;
    bool operator== (const std::vector<double>& val) const;
    bool operator== (const std::vector<StatusCode>& val) const;
    bool operator== (const std::vector<std::string>& val) const;
    bool operator== (const std::vector<ByteString>& val) const;
    bool operator== (const std::vector<DateTime>& val) const;
    bool operator== (const std::vector<Guid>& val) const;
    bool operator== (const std::vector<NodeID>& val) const;
    bool operator== (const std::vector<QualifiedName>& val) const;
    bool operator== (const std::vector<LocalizedText>& val) const;
    bool operator== (const std::vector<DataValue>& val) const;
    bool operator== (const std::vector<Variant>& val) const;
    bool operator== (const std::vector<DiagnosticInfo>& val) const;

    bool operator== (const VariantValue& val) const;

    template<typename T>
    bool operator!= (const T& t) const
    {
      return !(*this == t);
    }
  };

  template<typename T>
  struct has_const_iterator
  {
  private:
    typedef char                      yes;
    typedef struct { char array[2]; } no;

    template<typename C> static yes test(typename C::const_iterator*);
    template<typename C> static no  test(...);
  public:
    static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    typedef T type;
  };

  template <typename T>
  struct has_begin_end
  {
    template<typename C> static char (&f(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&f(...))[2];

    template<typename C> static char (&g(typename std::enable_if<
      std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
      typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

    template<typename C> static char (&g(...))[2];

    static bool const beg_value = sizeof(f<T>(0)) == 1;
    static bool const end_value = sizeof(g<T>(0)) == 1;
  };

  template<typename T> 
    struct is_container_not_string : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value> 
  { };

  template<> 
    struct is_container_not_string<std::string> : std::integral_constant<bool, false> {};

  struct Variant
  {
  public:
    VariantValue Value;
    VariantType Type;
    std::vector<uint32_t> Dimensions;

    Variant();

    void setArray(bool val) {_array = val;};

    bool IsArray() const;

    bool IsNul() const;

    template <typename T>
    Variant& operator=(const T& value)
    {
      Value = VariantValue(value);
      Type = Value.GetType();
      if ( is_container_not_string< T >::value ){ _array = true;}
      return *this;
    }

    template <typename T>
    bool operator==(const T& value) const
    {
      return Value == value;
    }

    template <typename T>
    Variant(const T& value)
    {
      Value = VariantValue(value);
      Type = Value.GetType();
      if ( is_container_not_string< T >::value ) { _array = true;}
    }

    Variant(const Variant& var);
    Variant& operator= (const Variant& variant);

    bool operator== (const Variant& var) const;

    template <typename T>
    bool operator!= (T&& t) const
    {
      return !(*this == t);
    }

  private:
    bool _array = false;
  };


  class VariantVisitor
  {
  public:
    template <typename T>
    void Visit(std::vector<T>& values)
    {
    }

    template <typename T>
    void Visit(const std::vector<T>& values)
    {
    }
  };

  template <typename VisitorType>
  void ApplyVisitor(Variant& var, VisitorType& visitor)
  {
    switch (var.Type)
    {
      case VariantType::BOOLEAN:          visitor.Visit(var.Value.Boolean); break;
      case VariantType::SBYTE:            visitor.Visit(var.Value.SByte); break;
      case VariantType::BYTE:             visitor.Visit(var.Value.Byte); break;
      case VariantType::INT16:            visitor.Visit(var.Value.Int16); break;
      case VariantType::UINT16:           visitor.Visit(var.Value.UInt16); break;
      case VariantType::INT32:            visitor.Visit(var.Value.Int32); break;
      case VariantType::UINT32:           visitor.Visit(var.Value.UInt32); break;
      case VariantType::INT64:            visitor.Visit(var.Value.Int64); break;
      case VariantType::UINT64:           visitor.Visit(var.Value.UInt64); break;
      case VariantType::FLOAT:            visitor.Visit(var.Value.Float); break;
      case VariantType::DOUBLE:           visitor.Visit(var.Value.Double); break;
      case VariantType::STRING:           visitor.Visit(var.Value.String); break;
      case VariantType::DATE_TIME:        visitor.Visit(var.Value.Time); break;
      case VariantType::GUID:             visitor.Visit(var.Value.Guids); break;
      case VariantType::BYTE_STRING:      visitor.Visit(var.Value.ByteStrings); break;
      case VariantType::XML_ELEMENT:      visitor.Visit(var.Value.String); break;
      case VariantType::NODE_ID:          visitor.Visit(var.Value.Node); break;
      case VariantType::EXPANDED_NODE_ID: visitor.Visit(var.Value.Node); break;
      case VariantType::STATUS_CODE:      visitor.Visit(var.Value.Statuses); break;
      case VariantType::QUALIFIED_NAME:   visitor.Visit(var.Value.Name); break;
      case VariantType::LOCALIZED_TEXT:   visitor.Visit(var.Value.Text); break;
      case VariantType::DIAGNOSTIC_INFO:  visitor.Visit(var.Value.Diagnostic); break;
      case VariantType::VARIANT:          visitor.Visit(var.Value.Variants); break;
      case VariantType::DATA_VALUE:       visitor.Visit(var.Value.Value); break;
      case VariantType::NUL:
      case VariantType::EXTENSION_OBJECT:
        break;
      default:
        throw std::logic_error("Unknown variant type.");
    }
  }

  template <typename VisitorType>
  void ApplyVisitor(const Variant& var, VisitorType& visitor)
  {
    switch (var.Type)
    {
      case VariantType::BOOLEAN:          visitor.Visit(var.Value.Boolean); break;
      case VariantType::SBYTE:            visitor.Visit(var.Value.SByte); break;
      case VariantType::BYTE:             visitor.Visit(var.Value.Byte); break;
      case VariantType::INT16:            visitor.Visit(var.Value.Int16); break;
      case VariantType::UINT16:           visitor.Visit(var.Value.UInt16); break;
      case VariantType::INT32:            visitor.Visit(var.Value.Int32); break;
      case VariantType::UINT32:           visitor.Visit(var.Value.UInt32); break;
      case VariantType::INT64:            visitor.Visit(var.Value.Int64); break;
      case VariantType::UINT64:           visitor.Visit(var.Value.UInt64); break;
      case VariantType::FLOAT:            visitor.Visit(var.Value.Float); break;
      case VariantType::DOUBLE:           visitor.Visit(var.Value.Double); break;
      case VariantType::STRING:           visitor.Visit(var.Value.String); break;
      case VariantType::DATE_TIME:        visitor.Visit(var.Value.Time); break;
      case VariantType::GUID:             visitor.Visit(var.Value.Guids); break;
      case VariantType::BYTE_STRING:      visitor.Visit(var.Value.ByteStrings); break;
      case VariantType::XML_ELEMENT:      visitor.Visit(var.Value.String); break;
      case VariantType::NODE_ID:          visitor.Visit(var.Value.Node); break;
      case VariantType::EXPANDED_NODE_ID: visitor.Visit(var.Value.Node); break;
      case VariantType::STATUS_CODE:      visitor.Visit(var.Value.Statuses); break;
      case VariantType::QUALIFIED_NAME:   visitor.Visit(var.Value.Name); break;
      case VariantType::LOCALIZED_TEXT:   visitor.Visit(var.Value.Text); break;
      case VariantType::DIAGNOSTIC_INFO:  visitor.Visit(var.Value.Diagnostic); break;
      case VariantType::VARIANT:          visitor.Visit(var.Value.Variants); break;
      case VariantType::DATA_VALUE:       visitor.Visit(var.Value.Value); break;
      case VariantType::NUL:
      case VariantType::EXTENSION_OBJECT:
        break;
      default:
        throw std::logic_error("Unknown variant type.");
    }
  }

} // namespace OpcUa

#endif //  __OPC_UA_VARIANT_H__
