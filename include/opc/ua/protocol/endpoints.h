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

#include <opc/ua/protocol/types.h>

namespace OpcUa
{

  //------------------------------------------------------
  // Endpoints
  //------------------------------------------------------

  struct EndpointsFilter
  {
    std::string EndpointURL;
    std::vector<std::string> LocaleIds;
    std::vector<std::string> ProfileUries;
  };

  struct GetEndpointsRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    EndpointsFilter Filter;

    GetEndpointsRequest();
  };


  struct GetEndpointsResponse
  {
    NodeId TypeId;
    ResponseHeader Header;

    std::vector<EndpointDescription> Endpoints;

    GetEndpointsResponse();
  };

  ////////////////////////////////////////////////////////
  // FindServersRequest
  ////////////////////////////////////////////////////////
  struct FindServersParameters
  {
    std::string EndpointURL;
    std::vector<LocaleId> Locales;
    std::vector<std::string> ServersToReturn;
  };

  struct FindServersRequest
  {
    NodeId TypeId;
    RequestHeader Header;
    FindServersParameters Parameters;

    FindServersRequest();
  };

  struct FindServersData
  {
    std::vector<ApplicationDescription> Descriptions;
  };

  struct FindServersResponse
  {
    NodeId TypeId;
    ResponseHeader Header;
    FindServersData Data;

    FindServersResponse();
  };

} // namespace OpcUa

#endif // __OPC_UA_MESSAGES_DISCOVERY_SERVICE_H__
