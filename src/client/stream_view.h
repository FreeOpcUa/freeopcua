/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/view.h>
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

      virtual std::vector<BrowsePathResult> TranslateBrowsePathsToNodeIds(const TranslateBrowsePathsParameters& params) const
      {
        TranslateBrowsePathsToNodeIDsRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = params;

        Stream << request << OpcUa::Binary::flush;
        
        TranslateBrowsePathsToNodeIDsResponse response;
        Stream >> response;

        return response.Result.Paths;
      }


      virtual std::vector<ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
      {
        BrowseRequest browse;
        browse.Header.SessionAuthenticationToken = AuthenticationToken;
        browse.Query = query;

        Stream << browse << OpcUa::Binary::flush;

        BrowseResponse response;
        Stream >> response;

        if (!response.Results.empty())
        {
          const BrowseResult& result = *response.Results.begin();
          ContinuationPoint = result.ContinuationPoint;
          return result.Referencies;
        }

        return  std::vector<ReferenceDescription>();
      }

      virtual std::vector<ReferenceDescription> BrowseNext() const
      {
        if (ContinuationPoint.empty())
        {
          return std::vector<ReferenceDescription>();
        }

        const std::vector<ReferenceDescription>& referencies = Next();
        if (referencies.empty())
        {
          Release();
        }
        return referencies;
      }

    private:
      std::vector<ReferenceDescription> Next() const
      {
        return SendBrowseNext(false);
      }

      void Release() const
      {
        SendBrowseNext(true);
      }

      std::vector<ReferenceDescription> SendBrowseNext(bool releasePoint) const
      {
        BrowseNextRequest browseNext;
        browseNext.Header.SessionAuthenticationToken = AuthenticationToken;
        browseNext.ReleaseContinuationPoints= false;
        browseNext.ContinuationPoints.push_back(ContinuationPoint);

        Stream << browseNext << OpcUa::Binary::flush;

        BrowseNextResponse response;
        Stream >> response;
        return !response.Results.empty() ? response.Results.begin()->Referencies :  std::vector<ReferenceDescription>();
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
      mutable std::vector<uint8_t> ContinuationPoint;
    };

  } // namespace Internal
} // namespace OpcUa

#endif // OPC_UA_CLIENT_INTERNAL_STREAM_VIEW_H

