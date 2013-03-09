/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/client/computer.h>
#include <opc/ua/protocol/binary/stream.h>
#include <opc/ua/protocol/endpoints.h>


namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class EndpointServices : public OpcUa::Remote::EndpointServices
    {
    public:
      explicit EndpointServices(std::shared_ptr<IOChannel> channel)
        : Stream(channel)
      {
      }

      virtual std::vector<Remote::ApplicationDescription> FindServers(const Remote::ApplicationFilter& filter) const
      {
        return std::vector<Remote::ApplicationDescription>();
      }

      virtual std::vector<EndpointDescription> GetEndpoints(const Remote::EndpointFilter& filter) const
      {
        OpcUa::GetEndpointsRequest request;
        request.EndpointURL = filter.EndpointUrl;
        request.LocaleIDs = filter.LocaleIDs;
        request.ProfileUries = filter.ProfileUries;
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
    };

  } // namespace Internal
} // namespace OpcUa

