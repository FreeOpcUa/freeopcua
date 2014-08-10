/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc endpoints interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_ENDPOINTS_H
#define OPC_UA_CLIENT_ENDPOINTS_H

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/endpoints.h>
#include <vector>

namespace OpcUa
{
  namespace Remote
  {

    struct ApplicationFilter
    {
    };
    struct ServerParameters
    {
    };

    class EndpointServices : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(EndpointServices);

    public:
      virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const = 0;
      // TODO Here all structuresmust be in one namespace.
      virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const = 0;
      virtual void RegisterServer(const ServerParameters& parameters) = 0;
    };

  } // namespace Remote
} // namespace OpcUa

#endif // OPC_UA_CLIENT_ENDPOINTS_H

