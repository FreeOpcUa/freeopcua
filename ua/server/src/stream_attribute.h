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
      virtual DataValue Read(const Remote::ReadParameters& params) const
      {
        AttributeValueID value;
        value.Node = params.Node;
        value.Attribute = params.Attribute;

        ReadRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.MaxAge = 0;
        request.TimestampsType = TimestampsToReturn::SERVER;
        request.AttributesToRead.push_back(value);
       
        Stream << request << OpcUa::Binary::flush;

        ReadResponse response;
        Stream >> response;

        return response.Results.empty() ? DataValue() : response.Results.at(0);
      }

      virtual OpcUa::StatusCode Write(const Remote::WriteParameters& params)
      {
        WriteRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;

        WriteValue value;
        value.Node = params.Node;
        value.Attribute = params.Attribute;
        value.Data = params.Value;
        request.NodesToWrite.push_back(value);

        Stream << request << OpcUa::Binary::flush;

        WriteResponse response;
        Stream >> response;

        return response.StatusCodes.at(0);
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H

