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
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/view.h>


namespace OpcUa
{
  namespace Soap
  {

    GetEndpointsRequest Deserialize(const ns3__GetEndpointsRequest* request);
    GetEndpointsResponse Deserialize(const ns3__GetEndpointsResponse* response);

    BrowseRequest Deserialize(const ns3__BrowseRequest* request);
    BrowseResponse Deserialize(const ns3__BrowseResponse* response);

    ReadRequest Deserialize(const ns3__ReadRequest* request);
    ReadResponse Deserialize(const ns3__ReadResponse* response);

    WriteRequest Deserialize(const ns3__WriteRequest* request);
    WriteResponse Deserialize(const ns3__WriteResponse* response);

    Variant Deserialize(const ns3__Variant* var);
  }
}
