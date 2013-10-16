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


namespace OpcUa
{
  namespace Soap
  {

    OpcUa::GetEndpointsRequest Deserialize(const ns3__GetEndpointsRequest* request);
    OpcUa::GetEndpointsResponse Deserialize(const ns3__GetEndpointsResponse* response);

  }
}
