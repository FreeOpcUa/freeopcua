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

#include <opc/ua/protocol/string_utils.h>

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
      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters, std::function<void (PublishResult)> callback=0)
      {
        CreateSubscriptionRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = parameters;

        Stream << request << OpcUa::Binary::flush;

        CreateSubscriptionResponse response;
        Stream >> response;
        return response.Data;
      }
      
      virtual std::vector<StatusCode> DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
      {
        DeleteSubscriptionRequest request;
        request.SubscriptionsIds = subscriptions;

        Stream << request << OpcUa::Binary::flush;

        DeleteSubscriptionResponse response;
        Stream >> response;
        return response.Results;
      }
 

      virtual MonitoredItemsData CreateMonitoredItems(const MonitoredItemsParameters& parameters)
      {
        CreateMonitoredItemsRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = parameters;

        Stream << request << OpcUa::Binary::flush;

        ProcessPublishResults();

        MonitoredItemsData data;
        Stream >> data;
        return data;
      }

      virtual std::vector<PublishResult> PopPublishResults(const std::vector<IntegerID>& subscriptionsIds)
      {
        return std::vector<PublishResult>();
      }

      virtual void Publish(const std::vector<SubscriptionAcknowledgement>& acknowledgements)
      {
        PublishRequest request;
        request.Parameters.Acknowledgements = acknowledgements;
        Stream << request << OpcUa::Binary::flush;
      }

    private:
      void ProcessPublishResults()
      {
        NodeID typeId;
        ResponseHeader header;
        for(;;)
        {
          Stream >> typeId;
          Stream >> header;
          std::cout << " got header with type: " << typeId << std::endl;
          if (typeId == NodeID(829, 0) )
          {
            PublishResult result;
            Stream >> result;
            std::cout << " got one publish result " << typeId << std::endl;
          }
          else
          {
            break;
          }
        }
      }
      mutable StreamType Stream;
      NodeID AuthenticationToken;
    };

  }
}

#endif // OPC_UA_REMOTE_STREAM_SUBSCRIPTION_H_
