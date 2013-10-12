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
#include "factory.h"

namespace OpcUa
{
  namespace Impl
  {

    BasicHttpBinding_USCOREIDiscoveryEndpointService *SoapDiscoveryService::copy()
    {
      return new SoapDiscoveryService(Computer);
    }

    int SoapDiscoveryService::FindServers(ns3__FindServersRequest *ns3__FindServersRequest_, ns3__FindServersResponse *ns3__FindServersResponse_)
    {
      return SOAP_OK;
    }

    int SoapDiscoveryService::GetEndpoints(ns3__GetEndpointsRequest* request, ns3__GetEndpointsResponse* response)
    {
      OpcUa::EndpointsFilter filter;
      if (request->EndpointUrl)
      {
        filter.EndpointURL = *request->EndpointUrl;
      }

      OpcUa::GetEndpointsResponse resp;
      resp.Endpoints = Computer->Endpoints()->GetEndpoints(filter);

      return SOAP_ERR;
    }

  } // namespace Impl
} // namespace OpcUa
