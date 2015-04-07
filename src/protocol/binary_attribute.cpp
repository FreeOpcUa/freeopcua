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

#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/protocol.h>
#include <opc/ua/protocol/attribute_ids.h>

#include <algorithm>
#include <memory>
#include <string>
#include <iostream>


namespace OpcUa
{

  namespace Binary
  {

    template<> 
    std::size_t RawSize<AttributeId>(const AttributeId&)
    {
      return 4;
    }

    template<>
    void DataSerializer::Serialize<AttributeId>(const AttributeId& attr)
    {
      *this << static_cast<uint32_t>(attr);
    }

    template<>
    void DataDeserializer::Deserialize<AttributeId>(AttributeId& t)
    {
      uint32_t tmp = 0;
      *this >> tmp;
      t = static_cast<AttributeId>(tmp);
    }



  } // namespace Binary
} // namespace OpcUa

