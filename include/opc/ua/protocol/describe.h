/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Descriptions of OPC UA structures.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPCUA_DESCRIPTIONS_H
#define __OPCUA_DESCRIPTIONS_H

#include <stream>
#include <string>

namespace OpcUa
{

  struct Tabs
  {
    explicit Tabs(unsigned num = 0)
      : Num(num)
    { 
    }

    unsigned Num;
  };

  std::ostream& operator <<(std::ostream& os, const Tabs& tabs)
  {
    for (unsigned i = 0; i < tabs.Num; ++i)
    {
      os << " ";
    }
    return os;
  }

  template<typename T>
  std::string Describe(const T& object, const Tabs& tabs);

}

#endif // __OPCUA_DESCRIPTIONS_H

