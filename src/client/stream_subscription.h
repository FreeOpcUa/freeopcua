/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote SubscriptionServices implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_REMOTE_STREAM_SUBSCRIPTION_H_
#define OPC_UA_REMOTE_STREAM_SUBSCRIPTION_H_

#include <opc/ua/view.h>
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/monitored_items.h>
#include <opc/ua/protocol/binary/stream.h>

namespace OpcUa
{
  namespace Internal
  {
    template <typename StreamType>
    class SubscriptionServices : public OpcUa::Remote::SubscriptionServices
    {
    public:
      SubscriptionServices(std::shared_ptr<IOChannel> channel, const NodeID& sessionToken)
        : Stream(channel)
        , AuthenticationToken(sessionToken)
      {
      }

    public:
      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters)
      {
        CreateSubscriptionRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = parameters;

        Stream << request << OpcUa::Binary::flush;

        CreateSubscriptionResponse response;
        Stream >> response;
        return response.Data;
      }
      
      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID> subscriptions)
      {
        //FIXME: implement _:-)
        return std::vector<StatusCode>();
      }
 

      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
      {
        CreateMonitoredItemsRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = parameters;

        Stream << request << OpcUa::Binary::flush;

        CreateMonitoredItemsResponse response;
        Stream >> response;
        return response.Data;
      }

      virtual std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIds)
      {
        return std::vector<PublishResult>();
      }

      virtual void CreatePublishRequest(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
      {
        PublishRequest request;
        Stream << request << OpcUa::Binary::flush;
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  }
}

#endif // OPC_UA_REMOTE_STREAM_SUBSCRIPTION_H_
