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

#include <boost/any.hpp>
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


  class DataValue;


  class Variant
  {
    boost::any Value;

  public:
    std::vector<uint32_t> Dimensions;

    Variant(){}
    Variant(const Variant& var)
      : Value(var.Value)
    {
    }

    template <typename T>
    Variant(const T& value) : Value(value){}
    Variant(MessageID id) : Variant(NodeID(id)){}
    Variant(ReferenceID id) : Variant(NodeID(id)){}
    Variant(ObjectID id) : Variant(NodeID(id)){}
    Variant(ExpandedObjectID id) : Variant(NodeID(id)){}

    Variant& operator= (const Variant& variant)
    {
      this->Value = variant.Value;
      return *this;
    }

    template <typename T>
    Variant& operator=(const T& value)
    {
      Value = value;
      return *this;
    }

    Variant& operator=(MessageID value)
    {
      Value = NodeID(value);
      return *this;
    }

    Variant& operator=(ReferenceID value)
    {
      Value = NodeID(value);
      return *this;
    }

    Variant& operator=(ObjectID value)
    {
      Value = NodeID(value);
      return *this;
    }

    Variant& operator=(ExpandedObjectID value)
    {
      Value = NodeID(value);
      return *this;
    }


    bool operator== (const Variant& var) const;

    template <typename T>
    bool operator==(const T& value) const
    {
      return boost::any_cast<T>(Value) == value;
    }

    bool operator==(MessageID id) const
    {
      return *this == NodeID(id);
    }

    bool operator==(ReferenceID id) const
    {
      return *this == NodeID(id);
    }

    bool operator==(ObjectID id) const
    {
      return *this == NodeID(id);
    }

    bool operator==(ExpandedObjectID id) const
    {
      return *this == NodeID(id);
    }


    template <typename T>
    bool operator!= (const T& t) const
    {
      return !(*this == t);
    }

    bool IsArray() const;
    bool IsScalar() const;
    bool IsNul() const;

    template <typename T>
    T As() const
    {
      return boost::any_cast<T>(Value);
    }

    template <typename T>
    explicit operator T() const
    {
      return As<T>();
    }

    VariantType Type() const;

    template <typename VisitorType>
    typename VisitorType::result_type Visit(VisitorType visitor) const
    {
      using namespace boost;
      const std::type_info& t = Value.type();
      if (t == typeid(bool))
        return visitor(any_cast<bool>(Value));
      if (t == typeid(std::vector<bool>))
        return visitor(any_cast<std::vector<bool>>(Value));
      if (t == typeid(int8_t))
        return visitor(any_cast<int8_t>(Value));
      if (t == typeid(std::vector<int8_t>))
        return visitor(any_cast<std::vector<int8_t>>(Value));
      if (t == typeid(uint8_t))
        return visitor(any_cast<uint8_t>(Value));
      if (t == typeid(std::vector<uint8_t>))
        return visitor(any_cast<std::vector<uint8_t>>(Value));
      if (t == typeid(int16_t))
        return visitor(any_cast<int16_t>(Value));
      if (t == typeid(std::vector<int16_t>))
        return visitor(any_cast<std::vector<int16_t>>(Value));
      if (t == typeid(uint16_t))
        return visitor(any_cast<uint16_t>(Value));
      if (t == typeid(std::vector<uint16_t>))
        return visitor(any_cast<std::vector<uint16_t>>(Value));

      if (t == typeid(int32_t))
        return visitor(any_cast<int32_t>(Value));
      if (t == typeid(std::vector<int32_t>))
        return visitor(any_cast<std::vector<int32_t>>(Value));
      if (t == typeid(uint32_t))
        return visitor(any_cast<uint32_t>(Value));
      if (t == typeid(std::vector<uint32_t>))
        return visitor(any_cast<std::vector<uint32_t>>(Value));
      if (t == typeid(int64_t))
        return visitor(any_cast<int64_t>(Value));
      if (t == typeid(std::vector<int64_t>))
        return visitor(any_cast<std::vector<int64_t>>(Value));
      if (t == typeid(uint64_t))
        return visitor(any_cast<uint64_t>(Value));
      if (t == typeid(std::vector<uint64_t>))
        return visitor(any_cast<std::vector<uint64_t>>(Value));

      if (t == typeid(float))
        return visitor(any_cast<float>(Value));
      if (t == typeid(std::vector<float>))
        return visitor(any_cast<std::vector<float>>(Value));

      if (t == typeid(double))
        return visitor(any_cast<double>(Value));
      if (t == typeid(std::vector<double>))
        return visitor(any_cast<std::vector<double>>(Value));

      if (t == typeid(std::string))
        return visitor(any_cast<std::string>(Value));
      if (t == typeid(std::vector<std::string>))
        return visitor(any_cast<std::vector<std::string>>(Value));

      if (t == typeid(DateTime))
        return visitor(any_cast<DateTime>(Value));
      if (t == typeid(std::vector<DateTime>))
        return visitor(any_cast<std::vector<DateTime>>(Value));

      if (t == typeid(Guid))
        return visitor(any_cast<Guid>(Value));
      if (t == typeid(std::vector<Guid>))
        return visitor(any_cast<std::vector<Guid>>(Value));

      if (t == typeid(ByteString))
        return visitor(any_cast<ByteString>(Value));
      if (t == typeid(std::vector<ByteString>))
        return visitor(any_cast<std::vector<ByteString>>(Value));

      if (t == typeid(NodeID))
        return visitor(any_cast<NodeID>(Value));
      if (t == typeid(std::vector<NodeID>))
        return visitor(any_cast<std::vector<NodeID>>(Value));

      if (t == typeid(StatusCode))
        return visitor(any_cast<StatusCode>(Value));
      if (t == typeid(std::vector<StatusCode>))
        return visitor(any_cast<std::vector<StatusCode>>(Value));

      if (t == typeid(LocalizedText))
        return visitor(any_cast<LocalizedText>(Value));
      if (t == typeid(std::vector<LocalizedText>))
        return visitor(any_cast<std::vector<LocalizedText>>(Value));

      if (t == typeid(QualifiedName))
        return visitor(any_cast<QualifiedName>(Value));
      if (t == typeid(std::vector<QualifiedName>))
        return visitor(any_cast<std::vector<QualifiedName>>(Value));

      if (t == typeid(DataValue))
        return visitor(any_cast<DataValue>(Value));
      if (t == typeid(std::vector<DataValue>))
        return visitor(any_cast<std::vector<DataValue>>(Value));

      if (t == typeid(Variant))
        return visitor(any_cast<Variant>(Value));
      if (t == typeid(std::vector<Variant>))
        return visitor(any_cast<std::vector<Variant>>(Value));

      if (t == typeid(DiagnosticInfo))
        return visitor(any_cast<DiagnosticInfo>(Value));
      if (t == typeid(std::vector<DiagnosticInfo>))
        return visitor(any_cast<std::vector<DiagnosticInfo>>(Value));

      throw std::runtime_error(std::string("Unknown variant type '") + t.name() + "'.");
    }
  };

} // namespace OpcUa

#endif //  __OPC_UA_VARIANT_H__
