/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once

#include <soapH.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/view.h>


namespace OpcUa
{
  namespace Soap
  {

    GetEndpointsRequest Deserialize(const ns3__GetEndpointsRequest* request);
    GetEndpointsResponse Deserialize(const ns3__GetEndpointsResponse* response);

    BrowseRequest Deserialize(const ns3__BrowseRequest* request);
  }
}
