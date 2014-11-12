/// @author Olivier Roulet-dubonnet 2014
/// @brief  SubcsriptionService implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "subscription_service_internal.h"

#include <boost/thread/locks.hpp>


namespace OpcUa
{
  namespace Internal
  {

    SubscriptionServiceInternal::SubscriptionServiceInternal(Server::AddressSpace::SharedPtr addressspace, boost::asio::io_service& ioService, bool debug)
      : io(ioService)
      , AddressSpace(addressspace)
      , Debug(debug)
    {
    }

    SubscriptionServiceInternal::~SubscriptionServiceInternal()
    {
    }

    Server::AddressSpace& SubscriptionServiceInternal::GetAddressSpace()
    {
      return *AddressSpace;
    }

    boost::asio::io_service& SubscriptionServiceInternal::GetIOService()
    {
      return io;
    }

    void SubscriptionServiceInternal::DeleteAllSubscriptions()
    {
      if (Debug) std::cout << "SubscriptionService | Deleting all subscriptions." << std::endl;

      std::vector<IntegerID> ids(SubscriptionsMap.size());
      {
        boost::shared_lock<boost::shared_mutex> lock(DbMutex);
        std::transform(SubscriptionsMap.begin(), SubscriptionsMap.end(), ids.begin(), [](const SubscriptionsIDMap::value_type& i){return i.first;});
      }

      DeleteSubscriptions(ids);
    }

    std::vector<StatusCode> SubscriptionServiceInternal::DeleteSubscriptions(const std::vector<IntegerID>& subscriptions)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::vector<StatusCode> result;
      for (const IntegerID& subid: subscriptions)
      {
        SubscriptionsIDMap::iterator itsub = SubscriptionsMap.find(subid);
        if ( itsub == SubscriptionsMap.end())
        {
          std::cout << "SubscriptionService | Error, got request to delete non existing Subscription: " << subid << std::endl;
          result.push_back(StatusCode::BadSubscriptionIdInvalid);
        }
        else
        {
          if (Debug) std::cout << "SubscriptionService | Deleting Subscription: " << subid << std::endl;
          //stop subscription and post its deletion to asio, so it does not get destroyed before its timer
          itsub->second->Stop();
          std::shared_ptr<InternalSubscription> isub = itsub->second;
          io.post([isub, this](){ this->DeleteInternalSubscriptionObject(isub); });
          SubscriptionsMap.erase(subid); 
          result.push_back(StatusCode::Good);
        }
      }
      return result;
    }

    void SubscriptionServiceInternal::DeleteInternalSubscriptionObject(std::shared_ptr<InternalSubscription> subscription)
    {
      if (Debug) std::cout << "SubscriptionService | Deleting Subscription phase 2" << std::endl;
      subscription.reset();//maybe not necessary but let the compilers knows why we call that method
    }

    SubscriptionData SubscriptionServiceInternal::CreateSubscription(const CreateSubscriptionRequest& request, std::function<void (PublishResult)> callback)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      SubscriptionData data;
      data.ID = ++LastSubscriptionID;
      data.RevisedLifetimeCount = request.Parameters.RequestedLifetimeCount;
      data.RevisedPublishingInterval = request.Parameters.RequestedPublishingInterval;
      data.RevizedMaxKeepAliveCount = request.Parameters.RequestedMaxKeepAliveCount;
      if (Debug) std::cout << "SubscriptionService | Creating Subscription with ID: " << data.ID << std::endl;

      std::shared_ptr<InternalSubscription> sub(new InternalSubscription(*this, data, request.Header.SessionAuthenticationToken, callback, Debug));
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
        PublishRequestQueues[request.Header.SessionAuthenticationToken] += 1;
      }
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
        std::cout << "SubscriptionService | Error request for publish queue for unknown session: " << node << " queue are available for: ";
        for ( auto i: PublishRequestQueues ){
          std::cout << "    " << i.first ;
        }
        std::cout << std::endl;
        return false;
      }
      else
      {
        if ( queue_it->second == 0 )
        {
          std::cout << "SubscriptionService | Missing publish request, cannot send response for session: " << node << std::endl;
          return false;
        }
        else
        {
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

  } // namespace Internal

  namespace Server
  {

    SubscriptionService::UniquePtr CreateSubscriptionService(std::shared_ptr<Server::AddressSpace> addressspace, boost::asio::io_service& io, bool debug)
    {
      return SubscriptionService::UniquePtr(new Internal::SubscriptionServiceInternal(addressspace, io, debug));
    }

  }
}

