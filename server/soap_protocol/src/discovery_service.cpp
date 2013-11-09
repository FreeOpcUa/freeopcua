/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Class which implements Discovery service over soap.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "discovery_service.h"
#include "serialization/serialize.h"

#include <iostream>

namespace OpcUa
{
  namespace Soap
  {

    BasicHttpBinding_USCOREIDiscoveryEndpointService *DiscoveryService::copy()
    {
      return new DiscoveryService(Computer, Debug);
    }

    int DiscoveryService::FindServers(ns3__FindServersRequest *ns3__FindServersRequest_, ns3__FindServersResponse *ns3__FindServersResponse_)
    {
      return SOAP_OK;
    }

    int DiscoveryService::GetEndpoints(ns3__GetEndpointsRequest* request, ns3__GetEndpointsResponse* response)
    {
      if (Debug) std::clog << "soap: GetEndpoints received." << std::endl;
      OpcUa::EndpointsFilter filter;
      if (request->EndpointUrl)
      {
        filter.EndpointURL = *request->EndpointUrl;
      }

      OpcUa::GetEndpointsResponse resp;
      resp.Endpoints = Computer->Endpoints()->GetEndpoints(filter);
      if (Debug) std::clog << "Found " << resp.Endpoints.size() << " endpoints." << std::endl;

      if (Debug) std::clog << "Serializing response." << std::endl;
      *response = *Soap::Serialize(this, resp);

      if (Debug) std::clog << "soap: GetEndpoints processed." << std::endl;
      return SOAP_OK;
    }

  } // namespace Impl
} // namespace OpcUa

