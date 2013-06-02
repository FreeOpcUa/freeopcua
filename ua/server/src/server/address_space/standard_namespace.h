/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef OPC_UA_SERVER_STANDARD_NAME_SPACE_H
#define OPC_UA_SERVER_STANDARD_NAME_SPACE_H

#include "address_space_internal.h"

#include <memory>


namespace OpcUa
{
  namespace Internal
  {

    std::unique_ptr<Internal::AddressSpace> CreateStandardNamespace();

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_SERVER_STANDARD_NAME_SPACE_H

