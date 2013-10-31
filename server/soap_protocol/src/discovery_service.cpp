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
  namespace Impl
  {

    BasicHttpBinding_USCOREIDiscoveryEndpointService *SoapDiscoveryService::copy()
    {
      return new SoapDiscoveryService(Computer, Debug);
    }

    int SoapDiscoveryService::FindServers(ns3__FindServersRequest *ns3__FindServersRequest_, ns3__FindServersResponse *ns3__FindServersResponse_)
    {
      return SOAP_OK;
    }

    int SoapDiscoveryService::GetEndpoints(ns3__GetEndpointsRequest* request, ns3__GetEndpointsResponse* response)
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
      ns3__GetEndpointsResponse* r = Soap::Serialize(this, resp);
      response->Endpoints = r->Endpoints;
      response->ResponseHeader = r->ResponseHeader;

      if (Debug) std::clog << "soap: GetEndpoints processed." << std::endl;
      return SOAP_OK;
    }

  } // namespace Impl
} // namespace OpcUa

