/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua binary Endpoints service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_ENDPOINTS
#define __OPC_UA_BINARY_ENDPOINTS

#include <opc/ua/channel.h>

#include <memory>
#include <string>
#include <vector>

namespace OpcUa
{
  namespace Binary
  {
 
    struct EndpointsFilter
    {
      std::string EndpointUrl;
    };

  } // nmespace Bunary
} // namespace OpcUa

#endif // __OPC_UA_BINARY_ENDPOINTS

