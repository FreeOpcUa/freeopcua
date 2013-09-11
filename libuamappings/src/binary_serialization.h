/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_SERIALIZATION_TOOLS_H__
#define __OPC_UA_BINARY_SERIALIZATION_TOOLS_H__

#include <stdint.h>


namespace OpcUa
{
  template<class Stream, class Container>
  inline void SerializeContainer(Stream& out, const Container& c, uint32_t emptySizeValue = ~uint32_t())
  {
    if (c.size() == 0)
    {
      out.Serialize(emptySizeValue);
      return;
    }
    out.Serialize((uint32_t)c.size());
    for (auto it = c.begin(); it != c.end(); ++ it)
    {
      out.Serialize(*it);
    }
  }

  template<class Stream, class Container>
  inline void DeserializeContainer(Stream& in, Container& c)
  {
    uint32_t size = 0;
    in.Deserialize(size);

    c.clear();
    if (!size || size == ~uint32_t())
    {
      return;
    }

    for (uint32_t i = 0; i < size; ++i)
    {
      typename Container::value_type val;
      in.Deserialize(val);
      c.push_back(val);
    }
  }
}

#endif // __OPC_UA_BINARY_SERIALIZATION_TOOLS_H__

