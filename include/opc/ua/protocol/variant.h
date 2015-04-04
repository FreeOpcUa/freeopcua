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
    GUId             = 14,
    BYTE_STRING      = 15,
    XML_ELEMENT      = 16,
    NODE_Id          = 17,
    EXPANDED_NODE_Id = 18,
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


  class Variant;


  // Such monster due to msvs.
  class VariantVisitor
  {
  public:
    typedef void result_type;

  public:
    virtual void Visit(bool val) = 0;
    virtual void Visit(const std::vector<bool>& val) = 0;
    virtual void Visit(int8_t val) = 0;
    virtual void Visit(const std::vector<int8_t>& val) = 0;
    virtual void Visit(uint8_t val) = 0;
    virtual void Visit(const std::vector<uint8_t>& val) = 0;
    virtual void Visit(int16_t val) = 0;
    virtual void Visit(const std::vector<int16_t>& val) = 0;
    virtual void Visit(uint16_t val) = 0;
    virtual void Visit(const std::vector<uint16_t>& val) = 0;
    virtual void Visit(int32_t val) = 0;
    virtual void Visit(const std::vector<int32_t>& val) = 0;
    virtual void Visit(uint32_t val) = 0;
    virtual void Visit(const std::vector<uint32_t>& val) = 0;
    virtual void Visit(int64_t val) = 0;
    virtual void Visit(const std::vector<int64_t>& val) = 0;
    virtual void Visit(uint64_t val) = 0;
    virtual void Visit(const std::vector<uint64_t>& val) = 0;
    virtual void Visit(float val) = 0;
    virtual void Visit(const std::vector<float>& val) = 0;
    virtual void Visit(double val) = 0;
    virtual void Visit(const std::vector<double>& val) = 0;
    virtual void Visit(const std::string& val) = 0;
    virtual void Visit(const std::vector<std::string>& val) = 0;
    virtual void Visit(const DateTime& val) = 0;
    virtual void Visit(const std::vector<DateTime>& val) = 0;
    virtual void Visit(const Guid& val) = 0;
    virtual void Visit(const std::vector<Guid>& val) = 0;
    virtual void Visit(const ByteString& val) = 0;
    virtual void Visit(const std::vector<ByteString>& val) = 0;
    virtual void Visit(const NodeId& val) = 0;
    virtual void Visit(const std::vector<NodeId>& val) = 0;
    virtual void Visit(const StatusCode& val) = 0;
    virtual void Visit(const std::vector<StatusCode>& val) = 0;
    virtual void Visit(const LocalizedText& val) = 0;
    virtual void Visit(const std::vector<LocalizedText>& val) = 0;
    virtual void Visit(const QualifiedName& val) = 0;
    virtual void Visit(const std::vector<QualifiedName>& val) = 0;
    virtual void Visit(const Variant& val) = 0;
    virtual void Visit(const std::vector<Variant>& val) = 0;
    virtual void Visit(const DiagnosticInfo& val) = 0;
    virtual void Visit(const std::vector<DiagnosticInfo>& val) = 0;
  };


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
    Variant(MessageId id) : Variant(NodeId(id)){}
    Variant(ReferenceId id) : Variant(NodeId(id)){}
    Variant(ObjectId id) : Variant(NodeId(id)){}
    Variant(ExpandedObjectId id) : Variant(NodeId(id)){}
    explicit Variant(VariantType);

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

    Variant& operator=(MessageId value)
    {
      Value = NodeId(value);
      return *this;
    }

    Variant& operator=(ReferenceId value)
    {
      Value = NodeId(value);
      return *this;
    }

    Variant& operator=(ObjectId value)
    {
      Value = NodeId(value);
      return *this;
    }

    Variant& operator=(ExpandedObjectId value)
    {
      Value = NodeId(value);
      return *this;
    }


    bool operator== (const Variant& var) const;

    template <typename T>
    bool operator==(const T& value) const
    {
      return boost::any_cast<T>(Value) == value;
    }

    bool operator==(MessageId id) const
    {
      return *this == NodeId(id);
    }

    bool operator==(ReferenceId id) const
    {
      return *this == NodeId(id);
    }

    bool operator==(ObjectId id) const
    {
      return *this == NodeId(id);
    }

    bool operator==(ExpandedObjectId id) const
    {
      return *this == NodeId(id);
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
    void Visit(VariantVisitor& visitor) const;
  };

  ObjectId VariantTypeToDataType(VariantType vt);
  VariantType DataTypeToVariantType(const NodeId& dataType);

} // namespace OpcUa

#endif //  __OPC_UA_VARIANT_H__
