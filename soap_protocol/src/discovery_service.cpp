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

namespace OpcUa
{
  namespace Impl
  {
    BasicHttpBinding_USCOREIDiscoveryEndpointService *SoapDiscoveryService::copy()
    {
      return new SoapDiscoveryService();
    }

    int SoapDiscoveryService::FindServers(ns3__FindServersRequest *ns3__FindServersRequest_, ns3__FindServersResponse *ns3__FindServersResponse_)
    {
      return SOAP_OK;
    }

    int SoapDiscoveryService::GetEndpoints(ns3__GetEndpointsRequest *ns3__GetEndpointsRequest_, ns3__GetEndpointsResponse *ns3__GetEndpointsResponse_)
    {
      return SOAP_OK;
    }

  } // namespace Impl
} // namespace OpcUa
