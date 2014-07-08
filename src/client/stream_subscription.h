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
      virtual SubscriptionData CreateSubscription(const SubscriptionParameters& parameters, std::function<void (PublishResult)> callback)
      {
        std::cout << "Creating Subcsription !!!!!!!!! " << (bool)Callback << std::endl;
        Callback = callback;
        if (Callback) { std::cout << "Callback is defined!!!!!!!!!!!!!!!!!!!!!!!\n";}
        std::cout << "Callback is " << (bool)Callback << std::endl;

        CreateSubscriptionRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = parameters;

        Stream << request << OpcUa::Binary::flush;

        CreateSubscriptionResponse response;
        Stream >> response;
        std::cout << "Callback is " << (bool)Callback << std::endl;
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
        std::cout << "Callback is " << (bool)Callback << std::endl;
        CreateMonitoredItemsRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = parameters;

        Stream << request << OpcUa::Binary::flush;

        ProcessPublishResults();
        std::cout << "Callback is " << (bool)Callback << std::endl;

        MonitoredItemsData data;
        Stream >> data;
        return data;
      }

      virtual std::vector<StatusCode> DeleteMonitoredItems(const DeleteMonitoredItemsParameters params)
      {
        DeleteMonitoredItemsRequest request;
        request.Header.SessionAuthenticationToken = AuthenticationToken;
        request.Parameters = params;

        Stream << request << OpcUa::Binary::flush;

        ProcessPublishResults();

        DeleteMonitoredItemsResponse response;
        Stream >> response;
        return response.Results;
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
          if (typeId == NodeID(829, 0) )
          {
            PublishResult result;
            Stream >> result;
            if (this->Callback)
            {
              std::cout << " Calling callback for one publish result " << std::endl;
              Callback(result);
            }
            else
            {
              std::cout << " PublishResult received but no callback defined" << std::endl;
            }
          }
          else
          {
            std::cout << " got response from server with type: " << typeId << std::endl;
            break;
          }
        }
      }

    private:
      mutable StreamType Stream;
      NodeID AuthenticationToken;
      std::function<void (PublishResult)> Callback;
    };

  }
}

#endif // OPC_UA_REMOTE_STREAM_SUBSCRIPTION_H_
