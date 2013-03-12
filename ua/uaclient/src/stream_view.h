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
#include <opc/ua/protocol/binary/stream.h>

#ifndef OPC_UA_CLIENT_INTERNAL_STREAM_VIEW_H
#define OPC_UA_CLIENT_INTERNAL_STREAM_VIEW_H


namespace OpcUa
{
  namespace Internal
  {

    template <typename StreamType>
    class ViewServices : public OpcUa::Remote::ViewServices
    {
    public:
      ViewServices(std::shared_ptr<IOChannel> channel, const NodeID& sessionToken)
        : Stream(channel)
        , AuthenticationToken(sessionToken)
      {
      }

      virtual BrowseResult Browse(const BrowseDescription& desc) const
      {
        return BrowseResult();
      }

      virtual BrowseResult BrowseNext(const std::vector<char>& continuationPoint, bool releaseContinuationPoint) const
      {
        return BrowseResult();
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_CLIENT_INTERNAL_STREAM_VIEW_H

