/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief C++ types of binary protocol.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_DATA_VALUE_H__
#define __OPC_UA_DATA_VALUE_H__

#include <opc/ua/protocol/variant.h>

namespace OpcUa
{
   const uint8_t DATA_VALUE = 1;
   const uint8_t DATA_VALUE_STATUS_CODE = 2;
   const uint8_t DATA_VALUE_SOURCE_TIMESTAMP = 4;
   const uint8_t DATA_VALUE_Server_TIMESTAMP = 8;
   const uint8_t DATA_VALUE_SOURCE_PICOSECONDS = 16;
   const uint8_t DATA_VALUE_Server_PICOSECONDS = 32;
   const uint8_t DATA_VALUE_ALL = ~uint8_t();

  struct DataValue
  {
    uint8_t Encoding;
    Variant Value;
    StatusCode Status;
    DateTime SourceTimestamp;
    uint16_t SourcePicoseconds;
    DateTime ServerTimestamp;
    uint16_t ServerPicoseconds;

    DataValue()
      : Encoding(0)
      , Status(StatusCode::Good)
      , SourceTimestamp(0)
      , SourcePicoseconds(0)
      , ServerTimestamp(0)
      , ServerPicoseconds(0)
    {
    }

    DataValue(const DataValue& data) = default;

    explicit DataValue(const Variant& value)
      : DataValue()
    {
      Value = value;
      Encoding |= DATA_VALUE;
    }

    template <typename T>
    explicit DataValue(const T val)
      : DataValue()
    {
      Value = Variant(val);
      Encoding |= DATA_VALUE;
    }

    DataValue& operator= (const Variant& value)
    {
      Value = value;
      Encoding |= DATA_VALUE;
      return *this;
    }

    template <typename T>
    DataValue& operator= (const T& value)
    {
      Value = Variant(value);
      Encoding |= DATA_VALUE;
      return *this;
    }

    template <typename T>
    bool operator== (const T& t) const
    {
      return (Encoding & DATA_VALUE) && Value == t;
    }

    bool operator== (const DataValue& data) const
    {
      if (Encoding != data.Encoding)
        return false;
      if ((Encoding & DATA_VALUE) && Value != data.Value)
        return false;
      if ((Encoding & DATA_VALUE_STATUS_CODE) && Status != data.Status)
        return false;
      if ((Encoding & DATA_VALUE_SOURCE_TIMESTAMP) && SourceTimestamp != data.SourceTimestamp)
        return false;
      if ((Encoding & DATA_VALUE_SOURCE_PICOSECONDS) && SourcePicoseconds != data.SourcePicoseconds)
        return false;
      if ((Encoding & DATA_VALUE_Server_TIMESTAMP) && ServerTimestamp != data.ServerTimestamp)
        return false;
      if ((Encoding & DATA_VALUE_Server_PICOSECONDS) && ServerPicoseconds != data.ServerPicoseconds)
        return false;

      return true;
    }

    void SetSourceTimestamp(const DateTime& t)
    {
      SourceTimestamp = t;
      Encoding |= DATA_VALUE_SOURCE_TIMESTAMP;
    }

    void SetServerTimestamp(const DateTime& t)
    {
      ServerTimestamp = t;
      Encoding |= DATA_VALUE_Server_TIMESTAMP;
    }
  };

} // namespace OpcUa

#endif // __OPC_UA_DATA_VALUE_H__

