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

#include <opc/ua/binary/protocol/attribute.h>
#include <opc/ua/binary/stream.h>
#include <opc/ua/binary/types.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>


namespace OpcUa
{
  namespace Binary
  {
   //---------------------------------------------------
    // Variant
    //---------------------------------------------------

    DataValue::DataValue()
      : Encoding(0)
      , Status(0)
      , SourceTimestamp(0)
      , SourcePicoseconds(0)
      , ServerTimestamp(0)
      , ServerPicoseconds(0)
    {
    }

    template<>
    std::size_t RawSize<DataValue>(const DataValue& val)
    {
      std::size_t size = RawSize(val.Encoding);
      if (val.Encoding & DATA_VALUE)
      {
        size += RawSize(val.Value);
      }
      if (val.Encoding & DATA_VALUE_STATUS_CODE)
      {
        size += RawSize(val.Status);
      }
      if (val.Encoding & DATA_VALUE_SOURCE_TIMESTAMP)
      {
        size += RawSize(val.SourceTimestamp);
      }
      if (val.Encoding & DATA_VALUE_SERVER_TIMESTAMP)
      {
        size += RawSize(val.ServerTimestamp);
      }
      if (val.Encoding & DATA_VALUE_SOURCE_PICOSECONDS)
      {
        size += RawSize(val.SourcePicoseconds);
      }
      if (val.Encoding & DATA_VALUE_SERVER_PICOSECONDS)
      {
        size += RawSize(val.ServerPicoseconds);
      }
      return size;
    }

    template<>
    void OStream::Serialize<DataValue>(const DataValue& val)
    {
      *this << val.Encoding;
      if (val.Encoding & DATA_VALUE)
      {
        *this << val.Value;
      }
      if (val.Encoding & DATA_VALUE_STATUS_CODE)
      {
        *this << val.Status;
      }
      if (val.Encoding & DATA_VALUE_SOURCE_TIMESTAMP)
      {
        *this << val.SourceTimestamp;
      }
      if (val.Encoding & DATA_VALUE_SOURCE_PICOSECONDS)
      {
        *this << val.SourcePicoseconds;
      }
      if (val.Encoding & DATA_VALUE_SERVER_TIMESTAMP)
      {
        *this << val.ServerTimestamp;
      }
      if (val.Encoding & DATA_VALUE_SERVER_PICOSECONDS)
      {
        *this << val.ServerPicoseconds;
      }
    }

    template<>
    void IStream::Deserialize<DataValue>(DataValue& val)
    {
      *this >> val.Encoding;
      if (val.Encoding & DATA_VALUE)
      {
        *this >> val.Value;
      }
      if (val.Encoding & DATA_VALUE_STATUS_CODE)
      {
        *this >> val.Status;
      }
      if (val.Encoding & DATA_VALUE_SOURCE_TIMESTAMP)
      {
        *this >> val.SourceTimestamp;
      }
      if (val.Encoding & DATA_VALUE_SOURCE_PICOSECONDS)
      {
        *this >> val.SourcePicoseconds;
      }
      if (val.Encoding & DATA_VALUE_SERVER_TIMESTAMP)
      {
        *this >> val.ServerTimestamp;
      }
      if (val.Encoding & DATA_VALUE_SERVER_PICOSECONDS)
      {
        *this >> val.ServerPicoseconds;
      }
    }

  } // namespace Binary
} // namespace OpcUa

