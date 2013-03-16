/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/client/view.h>
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/binary/stream.h>

#ifndef OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H
#define OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H


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
        request.TimestampsType = TimestampsToReturn::NEITHER;
        request.AttributesToRead.push_back(value);
       
        Stream << request << OpcUa::Binary::flush;

        ReadResponse response;
        Stream >> response;

        return response.Results.empty() ? DataValue() : response.Results.at(0);
      }

      virtual std::vector<Remote::WriteResult> Write(const Remote::WriteParameters& filter)
      {
        return std::vector<Remote::WriteResult>();
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_CLIENT_INTERNAL_STREAM_ATTRIBUTE_H

