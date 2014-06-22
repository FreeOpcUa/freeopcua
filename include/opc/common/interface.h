/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Interface base class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_INTERFACE_BASE_H__
#define __OPC_UA_INTERFACE_BASE_H__

namespace Common
{

  class Interface
  {
  protected:
    Interface(){}
    virtual ~Interface(){}

    Interface(const Interface&) = delete;
    const Interface& operator=(const Interface&) = delete;
  };

}

#endif // __OPC_UA_INTERFACE_BASE_H__

