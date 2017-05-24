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

    // do _not_ allow destructors to throw exceptions as it
    // creates a compiler error on OSX and may break shared_ptr
    // cleanup and cause memory leaks
    /*
    // c++11 destructors default to noexcept
    // allow implementations to do so.
    virtual ~Interface() noexcept (false) {}
    */

    Interface(const Interface&) = delete;
    const Interface& operator=(const Interface&) = delete;
  };

}

#endif // __OPC_UA_INTERFACE_BASE_H__

