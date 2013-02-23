/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __OPC_UA_BINARY_SERIALIZATION_TOOLS_H__
#define __OPC_UA_BINARY_SERIALIZATION_TOOLS_H__

#include <opc/ua/binary/stream.h>
#include <stdint.h>


namespace OpcUa
{
  namespace Binary
  {

    template<class Container>
    inline void SerializeContainer(OpcUa::Binary::OStream& out, const Container& c)
    {
      if (c.size() == 0)
      {
        out << ~uint32_t();
        return;
      }
      out << (uint32_t)c.size();
      for (auto it = c.begin(); it != c.end(); ++ it)
      {
        out << *it;
      }
    }

    template<class Container>
    inline void DeserializeContainer(OpcUa::Binary::IStream& in, Container& c)
    {
      uint32_t size = 0;
      in >> size;

      c.clear();
      if (!size || size == ~uint32_t())
      {
        return;
      }

      for (uint32_t i = 0; i < size; ++i)
      {
        typename Container::value_type val;
        in >> val;
        c.push_back(val);
      }
    }

  }
}

#endif // __OPC_UA_BINARY_SERIALIZATION_TOOLS_H__

