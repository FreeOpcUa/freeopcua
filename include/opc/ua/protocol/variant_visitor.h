/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Different implementations of varant visitors.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#pragma once

#include <opc/ua/protocol/variant.h>


namespace OpcUa
{

  template <typename Delegate>
  class TypedVisitor : public VariantVisitor
  {
  public:
    typedef void result_type;

  public:
    TypedVisitor(Delegate& impl)
      : Impl(impl)
    {
    }

  public:
    virtual void Visit(bool val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<bool>& val) { Impl.OnContainer(val); }
    virtual void Visit(int8_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<int8_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(uint8_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<uint8_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(int16_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<int16_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(uint16_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<uint16_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(int32_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<int32_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(uint32_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<uint32_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(int64_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<int64_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(uint64_t val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<uint64_t>& val) { Impl.OnContainer(val); }
    virtual void Visit(float val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<float>& val) { Impl.OnContainer(val); }
    virtual void Visit(double val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<double>& val) { Impl.OnContainer(val); }
    virtual void Visit(const std::string& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<std::string>& val) { Impl.OnContainer(val); }
    virtual void Visit(const DateTime& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<DateTime>& val) { Impl.OnContainer(val); }
    virtual void Visit(const Guid& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<Guid>& val) { Impl.OnContainer(val); }
    virtual void Visit(const ByteString& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<ByteString>& val) { Impl.OnContainer(val); }
    virtual void Visit(const NodeId& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<NodeId>& val) { Impl.OnContainer(val); }
    virtual void Visit(const StatusCode& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<StatusCode>& val) { Impl.OnContainer(val); }
    virtual void Visit(const LocalizedText& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<LocalizedText>& val) { Impl.OnContainer(val); }
    virtual void Visit(const QualifiedName& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<QualifiedName>& val) { Impl.OnContainer(val); }
    virtual void Visit(const Variant& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<Variant>& val) { Impl.OnContainer(val); }
    virtual void Visit(const DiagnosticInfo& val) { Impl.OnScalar(val); }
    virtual void Visit(const std::vector<DiagnosticInfo>& val) { Impl.OnContainer(val); }

  private:
    Delegate& Impl;
  };


}
