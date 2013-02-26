/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua Binary. Secure channel service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_MESSAGES_DISCOVERY_SERVICE_H__
#define __OPC_UA_MESSAGES_DISCOVERY_SERVICE_H__

#include <opc/ua/binary/protocol/common.h>

namespace OpcUa
{

  //------------------------------------------------------
  // Endpoints
  //------------------------------------------------------

  struct GetEndpointsRequest
  {
    NodeID TypeID;
    RequestHeader Header;

    std::string EndpointURL;
    std::vector<std::string> LocaleIDs;
    std::vector<std::string> ProfileUries;

    GetEndpointsRequest();
  };


  struct GetEndpointsResponse
  {
    NodeID TypeID;
    ResponseHeader Header;

    std::vector<EndpointDescription> Endpoints;

    GetEndpointsResponse();
  };

} // namespace OpcUa

#endif // __OPC_UA_MESSAGES_DISCOVERY_SERVICE_H__

