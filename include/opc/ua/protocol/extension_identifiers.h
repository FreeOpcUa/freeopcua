/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc binary cnnection channel.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_EXTENSION_IdENTIFIERS
#define __OPC_UA_BINARY_EXTENSION_IdENTIFIERS

#include <stdint.h>

namespace OpcUa
{
  enum ExtensionObjectId : uint16_t
  {
    USER_IdENTIFY_TOKEN_ANONYMOUS = 0x141, // 321
    USER_IdENTIFY_TOKEN_USERNAME = 0x144, // 324
  };
}

#endif // __OPC_UA_BINARY_EXTENSION_IdENTIFIERS
