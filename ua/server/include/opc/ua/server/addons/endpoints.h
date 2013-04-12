/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Endpoints addon.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_endpoints_addon_h
#define opc_ua_endpoints_addon_h

#include <opccore/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    struct Endpoint
    {
      std::string EndpointURL;
    };

    class EndpointsAddon : public Common::Addon
    {
    public:
      virtual std::vector<Endpoint> GetEndpoints() const = 0;
    };

  } // namespace Server
} // nmespace OpcUa

#endif // opc_ua_endpoints_addon_h

