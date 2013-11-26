/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary DateTime type.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_DATE_TIME__H__
#define __OPC_UA_BINARY_DATE_TIME__H__

#include <stdint.h>
#include <time.h>

namespace OpcUa
{

  typedef uint64_t DateTime;

  DateTime CurrentDateTime();
  DateTime ToDateTime(time_t t, unsigned usec = 0);
  time_t ToTimeT(DateTime dateTime);

  typedef double Duration; // Time duration in miliseconds.

} // OpcUa


#endif // __OPC_UA_BINARY_DATE_TIME__H__

