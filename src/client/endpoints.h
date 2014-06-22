/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/protocol/channel.h>
#include <opc/ua/server.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/endpoints.h>

#ifndef OPC_UA_INTERNAL_SERVICES_H
#define OPC_UA_INTERNAL_SERVICES_H

namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class EndpointServices : public OpcUa::Remote::EndpointServices
    {
    public:
      explicit EndpointServices(IOChannel::SharedPtr channel, const NodeID& sessionToken)
        : Stream(channel)
        , AuthenticationToken(sessionToken)
      {
      }

      virtual std::vector<ApplicationDescription> FindServers(const FindServersParameters& params) const
      {
        OpcUa::FindServersRequest request;
        request.Parameters = params;
        Stream << request << OpcUa::Binary::flush;

        OpcUa::FindServersResponse response;
        Stream >> response;
        return response.Data.Descriptions;
      }

      virtual std::vector<EndpointDescription> GetEndpoints(const EndpointsFilter& filter) const
      {
        OpcUa::GetEndpointsRequest request;
        request.Filter.EndpointURL = filter.EndpointURL;
        request.Filter.LocaleIDs = filter.LocaleIDs;
        request.Filter.ProfileUries = filter.ProfileUries;
        Stream << request << OpcUa::Binary::flush;

        OpcUa::GetEndpointsResponse response;
        Stream >> response;
        return response.Endpoints;
      }

      virtual void RegisterServer(const Remote::ServerParameters& parameters)
      {
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  } // namespace Internal
} // namespace OpcUa

#endif //  OPC_UA_INTERNAL_SERVICES_H

