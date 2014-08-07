#include "internal_subscription.h"


namespace OpcUa
{
  namespace Internal
  {

    InternalSubscription::InternalSubscription(SubscriptionData data, boost::asio::io_service& serverio, std::function<void (PublishResult)> callback)
      : Data(data)
        , Callback(callback)
        , io(serverio)
        , timer(io, boost::posix_time::milliseconds(data.RevisedPublishingInterval))
        , LifeTimeCount(data.RevisedLifetimeCount)
    {
      std::cout << "Should now sleep for " << Data.RevisedPublishingInterval << " milliseonds" << std::endl;
      timer.async_wait([&](const boost::system::error_code& error){ this->PublishResults(error); });
    }
    
    InternalSubscription::~InternalSubscription()
    {
      Stop();
    }

    void InternalSubscription::Stop()
    {
      timer.cancel();
    }

    bool InternalSubscription::HasExpired()
    {
      bool expired = KeepAliveCount > LifeTimeCount ;
      if ( expired )
      {
        std::cout << "Subscription has expired " << KeepAliveCount << "  " << LifeTimeCount << std::endl;
      }
      return expired;
    }

    void InternalSubscription::PublishResults(const boost::system::error_code& error)
    {
      if ( error || HasExpired() )
      {
        return; //It is very important to return, instance of InternalSubscription may have been deleted!
      }

      std::vector<PublishResult> results = PopPublishResult();
      if (results.size() > 0 )
      {
        std::cout << "Seems like subscription has a result, calling callback" << std::endl;
        if ( Callback )
        {
          Callback(results[0]);
        }
        else
        {
          std::cout << "No callback defined for this subscription" << std::endl;
        }
      }
      timer.expires_at(timer.expires_at() + boost::posix_time::milliseconds(Data.RevisedPublishingInterval));
      std::cout << "Should now sleep for " << Data.RevisedPublishingInterval << " milliseonds" << std::endl;
      timer.async_wait([&](const boost::system::error_code& error){ this->PublishResults(error); });
    }

    std::vector<PublishResult> InternalSubscription::PopPublishResult()
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      std::cout << "PopPublishresult for subscription: " << Data.ID << " with " << MonitoredItemsTriggered.size() << " triggered items in queue" << std::endl;
      PublishResult result;
      result.SubscriptionID = Data.ID;
      result.Message.PublishTime = CurrentDateTime();

      if ( MonitoredItemsTriggered.size() > 0 )
      {
        NotificationData data = GetNotificationData();
        result.Message.Data.push_back(data);
        result.Statuses.push_back(StatusCode::Good);
      }
          
      // FIXME: parse events and statuschange notification since they can be send in same result
      
      if ( (! Startup ) && result.Statuses.size() == 0 && (  Data.RevizedMaxKeepAliveCount > KeepAliveCount ) ) 
      {
        std::cout << "No event and not need to send keep-alive notification" << std::endl;
        ++KeepAliveCount;
        return  std::vector<PublishResult>(); //No event and we do not need to send keepalive notification yet so return empty list
      }
      KeepAliveCount = 0;
      result.Message.SequenceID = NotificationSequence;
      ++NotificationSequence;
      result.MoreNotifications = false;
      for (const PublishResult& res: NotAcknowledgedResults)
      {
        result.AvailableSequenceNumber.push_back(res.Message.SequenceID);
      }
      NotAcknowledgedResults.push_back(result);
      std::cout << "Sending Notification with " << result.Message.Data.size() << " notifications"  << std::endl;
      std::vector<PublishResult> resultlist;
      resultlist.push_back(result);

      if ( Startup )
      {
        Startup = false;
      }
      return resultlist;
    };

    NotificationData InternalSubscription::GetNotificationData()
    {
      DataChangeNotification notification;
      for ( const MonitoredItems& monitoreditem: MonitoredItemsTriggered)
      {
        notification.Notification.push_back(monitoreditem);
      }
      MonitoredItemsTriggered.clear();
      NotificationData data(notification);
      return data;
    }

    void InternalSubscription::NewAcknowlegment(const SubscriptionAcknowledgement& ack)
    {
      NotAcknowledgedResults.remove_if([&](PublishResult res){ return ack.SequenceNumber == res.Message.SequenceID; });
    }
    
    std::vector<StatusCode> InternalSubscription::DeleteMonitoredItemsIds(const std::vector<IntegerID>& monitoreditemsids)
    {
      std::vector<StatusCode> results;
      for (const IntegerID handle: monitoreditemsids)
      {
        MonitoredItemsMap.erase(handle);
        std::map<IntegerID, DataMonitoredItems>::iterator it_monitoreditem = MonitoredItemsMap.find(handle);
        if ( it_monitoreditem == MonitoredItemsMap.end()) 
        {
          results.push_back(StatusCode::BadMonitoredItemIdInvalid);
        }
        else
        {
          MonitoredItemsMap.erase(it_monitoreditem); //FIXME: check it is correct syntax!!!!!!!!!!
          results.push_back(StatusCode::Good);
        }
      }
      return results;
    }


    bool InternalSubscription::EnqueueEvent(IntegerID monitoreditemid, const Event& event)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      //Find monitoredItem 
      std::map<IntegerID, DataMonitoredItems>::iterator mii_it =  MonitoredItemsMap.find( monitoreditemid );
      if  (mii_it == MonitoredItemsMap.end() ) 
      {
        std::cout << "monitoreditem is deleted" << std::endl;
        return false;
      }
          
      //Check filter against event data and create EventFieldList to send
      //FIXME: Here we should also check event agains WhereClause of filter
      EventFieldList fieldlist;
      fieldlist.ClientHandle = mii_it->second.ClientHandle; 
      fieldlist.EventFields = GetEventFields(mii_it->second.Parameters. Filter.Event, event);
      EventTriggered.push_back(fieldlist);
      return true;
    }

    std::vector<Variant> InternalSubscription::GetEventFields(const EventFilter& filter, const Event& event)
    {
      //Go through filter and add value og matches as in spec
      std::vector<Variant> fields;
      for (SimpleAttributeOperand sattr : filter.SelectClauses)
      {
        if ( sattr.BrowsePath.size() == 0 )
        {
          fields.push_back(event.GetValue(sattr.Attribute));
        }
        else
        {
          fields.push_back(event.GetValue(sattr.BrowsePath));
        }
      }
      return fields;
    }

    CreateMonitoredItemsResult InternalSubscription::AddMonitoredItem(const MonitoredItemRequest& request)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      CreateMonitoredItemsResult res;
      res.Status = OpcUa::StatusCode::Good;
      res.MonitoredItemID = ++LastMonitoredItemID ;
      res.RevisedSamplingInterval = Data.RevisedPublishingInterval; //Force our own rate
      res.RevizedQueueSize = request.Parameters.QueueSize; // We should check that value, maybe set to a default...
      //res.FilterResult = //We can omit that one if we do not change anything in filter
      DataMonitoredItems mdata;
      mdata.SubscriptionID = Data.ID;
      mdata.Parameters = res;
      mdata.Mode = request.Mode;
      mdata.ClientHandle = request.Parameters.ClientHandle;
      MonitoredItemsMap[res.MonitoredItemID] = mdata;

      return res;
    }


    bool InternalSubscription::EnqueueDataChange(IntegerID monitoreditemid, const DataValue& value)
    {
      boost::unique_lock<boost::shared_mutex> lock(DbMutex);

      MonitoredItems event;
      std::map<IntegerID, DataMonitoredItems>::iterator it_monitoreditem = MonitoredItemsMap.find(monitoreditemid);
      if ( it_monitoreditem == MonitoredItemsMap.end()) 
      {
        return false;
      }

      event.ClientHandle = it_monitoreditem->second.ClientHandle; 
      event.Value = value;
      std::cout << "Enqueued DataChange triggered item for sub: " << Data.ID << " and clienthandle: " << event.ClientHandle << std::endl;
      MonitoredItemsTriggered.push_back(event);
      return true;
    }


  }
}


