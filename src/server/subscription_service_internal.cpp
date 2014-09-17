/// @author Olivier Roulet-dubonnet 2014
/// @brief  SubcsriptionService implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "subscription_service_internal.h"


namespace OpcUa
{
  namespace Internal
  {

    using namespace OpcUa::Remote;

    SubscriptionServiceInternal::SubscriptionServiceInternal(std::shared_ptr<UaServer::AddressSpace> addressspace, bool debug)
        : AddressSpace(addressspace)
          , Debug(debug)
          , work(new boost::asio::io_service::work(io))
      {
        //Initialize the worker thread for subscriptions
        service_thread = std::thread([&](){ io.run(); });
      }

   SubscriptionServiceInternal::~SubscriptionServiceInternal()
      {
        if (Debug) std::cout << "address_space_in_memory| Stopping boost io service." << std::endl;
        io.stop();
        if (Debug) std::cout << "address_space_in_memory| Joining service thread." << std::endl;
        service_thread.join();
      }

      std::shared_ptr<UaServer::AddressSpace> SubscriptionServiceInternal::GetAddressSpace()
      {
        return AddressSpace;
      }

      boost::asio::io_service& SubscriptionServiceInternal::GetIOService() 
      {
        return io;
      }

      void SubscriptionServiceInternal::DeleteAllSubscriptions()
      {
        if (Debug) std::cout << "Deleting all subscriptions." << std::endl;
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<IntegerID> ids(SubscriptionsMap.size());\
        std::transform(SubscriptionsMap.begin(), SubscriptionsMap.end(), ids.begin(), [](const SubscriptionsIDMap::value_type& i){return i.first;});
        lock.unlock();
        DeleteSubscriptions(ids);
      }

      std::vector<StatusCode> SubscriptionServiceInternal::DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<StatusCode> result;
        for (const IntegerID& subid: subscriptions)
        {
          std::cout << "Deleting Subscription: " << subid << std::endl;
          size_t count = SubscriptionsMap.erase(subid);
          if ( count > 0)
          {
            result.push_back(StatusCode::Good);
          }
          else
          {
            std::cout << "Error, got request to delete non existing Subscription: " << subid << std::endl;
            result.push_back(StatusCode::BadSubscriptionIdInvalid);
          }
        }
        return result;
      }

      SubscriptionData SubscriptionServiceInternal::CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        SubscriptionData data;
        data.ID = ++LastSubscriptionID;
        data.RevisedLifetimeCount = request.Parameters.RequestedLifetimeCount;
        data.RevisedPublishingInterval = request.Parameters.RequestedPublishingInterval;
        data.RevizedMaxKeepAliveCount = request.Parameters.RequestedMaxKeepAliveCount;
        if (Debug) std::cout << "Creating Subscription with ID: " << data.ID << std::endl;

        std::shared_ptr<InternalSubscription> sub(new InternalSubscription(*this, data, request.Header.SessionAuthenticationToken, callback));
        SubscriptionsMap[data.ID] = sub;
        return data;
      }

      MonitoredItemsData SubscriptionServiceInternal::CreateMonitoredItems(const MonitoredItemsParameters& params)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        MonitoredItemsData data;

        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(params.SubscriptionID);
        if ( itsub == SubscriptionsMap.end()) //SubscriptionID does not exist, return errors for all items
        {
          for (int j=0; j<(int)params.ItemsToCreate.size(); j++)
          {
            CreateMonitoredItemsResult res;
            res.Status = StatusCode::BadSubscriptionIdInvalid;
            data.Results.push_back(res);
          }
          return data;
        }

        for (const MonitoredItemRequest& req: params.ItemsToCreate) //FIXME: loop could be in InternalSubscription
        {
          CreateMonitoredItemsResult result = itsub->second->CreateMonitoredItem(req);
          data.Results.push_back(result);
        }
        return data;
     
      }

      std::vector<StatusCode> SubscriptionServiceInternal::DeleteMonitoredItems(const DeleteMonitoredItemsParameters& params)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);

        std::vector<StatusCode> results;

        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(params.SubscriptionId);
        if ( itsub == SubscriptionsMap.end()) //SubscriptionID does not exist, return errors for all items
        {
          for (int j=0; j<(int)params.MonitoredItemsIds.size(); j++)
          {
            results.push_back(StatusCode::BadSubscriptionIdInvalid);
          }
          return results;
        }

        results = itsub->second->DeleteMonitoredItemsIds(params.MonitoredItemsIds);
        return results;
      }

      void SubscriptionServiceInternal::Publish(const PublishRequest& request)
      {
        boost::unique_lock<boost::shared_mutex> lock(DbMutex);
        if ( PublishRequestQueues[request.Header.SessionAuthenticationToken] < 100 )
        {
          std::cout << "Increasing queue for publish request: " <<request.Header.SessionAuthenticationToken << std::endl;
          PublishRequestQueues[request.Header.SessionAuthenticationToken] += 1;
        }
          std::cout << "queue size for publish request is: " <<PublishRequestQueues[request.Header.SessionAuthenticationToken] << std::endl;
        //FIXME: else spec says we should return error to warn client

        for (SubscriptionAcknowledgement ack:  request.Parameters.Acknowledgements)
        {
          SubscriptionsIDMap::iterator sub_it = SubscriptionsMap.find(ack.SubscriptionID);
          if ( sub_it != SubscriptionsMap.end())
          {
            sub_it->second->NewAcknowlegment(ack); 
          }
        }
      }

      bool SubscriptionServiceInternal::PopPublishRequest(NodeID node)
      {
        std::map<NodeID, uint32_t>::iterator queue_it = PublishRequestQueues.find(node); 
        if ( queue_it == PublishRequestQueues.end() )
        {
          std::cout << "Error request for publish queue for unknown session" << node << std::endl;
          return false;
        }
        else
        {
          if ( queue_it->second == 0 )
          {
            std::cout << "Missing publish request, cannot send response for session: " << node << std::endl;
            return false;
          }
          else
          {
            std::cout << "poping publish request for session: " << node << std::endl;
            --queue_it->second;
            return true;
          }
        }
      }

      void SubscriptionServiceInternal::TriggerEvent(NodeID node, Event event)
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);

        for (auto sub : SubscriptionsMap)
        {
          sub.second->TriggerEvent(node, event);
        }

      }


  }


  namespace UaServer
  {
    SubscriptionService::UniquePtr CreateSubscriptionService(std::shared_ptr<UaServer::AddressSpace> addressspace, bool debug)
    {
      return SubscriptionService::UniquePtr(new Internal::SubscriptionServiceInternal(addressspace, debug));
    }
  }



}

