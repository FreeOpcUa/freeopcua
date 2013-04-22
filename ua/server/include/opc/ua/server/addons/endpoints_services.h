/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_endpoints_service_h
#define opc_ua_endpoints_service_h

#include <opc/ua/protocol/types.h>
#include <opccore/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    class EndpointsServicesAddon : public Common::Addon
    {
    public:
      virtual void AddEndpoints(const std::vector<OpcUa::EndpointDescription>& endpoints) = 0;
    };

    const char EndpointsServicesAddonID[] = "endpoints_services";

  } // namespace Server
} // nmespace OpcUa

#endif // opc_ua_get_endpoints_service_h

