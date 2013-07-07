/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H
#define OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H

#include <opc/ua/view.h>
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/binary/stream.h>


namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class AttributeServices : public OpcUa::Remote::AttributeServices
    {
    public:
      AttributeServices(std::shared_ptr<IOChannel> channel, const NodeID& sessionToken)
        : Stream(channel)
        , AuthenticationToken(sessionToken)
      {
      }

    public:
      virtual std::vector<DataValue> Read(const ReadParameters& params) const
      {
        ReadRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = params;
       
        Stream << request << OpcUa::Binary::flush;

        ReadResponse response;
        Stream >> response;

        return response.Result.Results;
      }

      virtual std::vector<OpcUa::StatusCode> Write(const std::vector<WriteValue>& values)
      {
        WriteRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters.NodesToWrite = values;

        Stream << request << OpcUa::Binary::flush;

        WriteResponse response;
        Stream >> response;

        return response.Result.StatusCodes;
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H

