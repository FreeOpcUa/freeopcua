/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Class which implements Discovery service over soap.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <soapBasicHttpBinding_USCOREIDiscoveryEndpointService.h>

namespace OpcUa
{
  namespace Impl
  {
    class SoapDiscoveryService : public BasicHttpBinding_USCOREIDiscoveryEndpointService
    {
      typedef BasicHttpBinding_USCOREIDiscoveryEndpointService ParentType;
    public:
      SoapDiscoveryService()
      {
      }

      SoapDiscoveryService(struct soap* s)
        : ParentType(s)
      {
      }

      SoapDiscoveryService(soap_mode iomode)
        : ParentType(iomode)
      {
      }

      SoapDiscoveryService(soap_mode imode, soap_mode omode)
        : ParentType(imode, omode)
      {
      }

      virtual BasicHttpBinding_USCOREIDiscoveryEndpointService *copy();
      virtual int FindServers(ns3__FindServersRequest *ns3__FindServersRequest_, ns3__FindServersResponse *ns3__FindServersResponse_);
      virtual int GetEndpoints(ns3__GetEndpointsRequest *ns3__GetEndpointsRequest_, ns3__GetEndpointsResponse *ns3__GetEndpointsResponse_);
    };
  }
}

