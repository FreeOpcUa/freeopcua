/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Class connects to gpsd and retrieves data from it.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "gpsd.h"
#include "gpsd_session.h"

#include <boost/thread.hpp>
#include <common/exception.h>

#include <iostream>

namespace
{
  struct Sleeper
  {
    explicit Sleeper(unsigned long t)
      : TimeToSleep(t)
    {
    }

    ~Sleeper()
    {
      sleep(TimeToSleep);
    }

  private:
    unsigned TimeToSleep;
  };

  class GpsdThread
  {
  public:
    GpsdThread(Gps::GpsdCallback::SharedPtr callback, volatile bool& stop)
      : ThreadStopped(stop)
      , Callback(callback)
    {
      std::cout << "Gpsd thread created" << std::endl;
      assert(Callback);
    }

    ~GpsdThread()
    {
      std::cout << "Gpsd thread destructor called." << std::endl;
    }

    void operator() () const
    {
      std::cout << "Gpsd thread started" << std::endl;
      while(!ThreadStopped)
      {
        Sleeper sleeper(1);
        try
        {
          Process();
        }
        catch (const boost::thread_interrupted&)
        {
          return;
        }
        catch (const Common::Error& err)
        {
           // TODO log
        }
        catch (const std::exception& except)
        {
           // TODO log
        }
        catch (...)
        {
          break;
          // TODO log
        }
      }
      std::cout << "Gpsd thread stopped" << std::endl;
    }


    void Process() const
    {
      std::cout << "connecting to gpsd deamon" << std::endl;
      Gpsd::GpsSession gpsdServer;
      while(!ThreadStopped)
      {
        Sleeper sleeper(1);
/*
        std::cout << "waiting gpsd answer..."  << std::endl;
        if (!gpsdServer.IsWaiting())
        {
          continue;
        }
        std::cout << "retriving data from gpsd deamon" << std::endl;
        const gps_data_t& gpsData = gpsdServer.Poll();
*/
        gps_data_t gpsData;
        gpsData.online = 1;
        gpsData.fix.mode = MODE_3D;
        gpsData.fix.longitude = 200;
        gpsData.fix.latitude = 300;
        gpsData.fix.altitude = 100;
        gpsData.fix.time = 20;
        std::cout << "passing gps data for processing" << std::endl;
        Callback->OnGpsData(gpsData);
      }
    }

  private:
    volatile const bool& ThreadStopped;
    const Gps::GpsdCallback::SharedPtr Callback;
  };
}  // namespace

class Gps::Gpsd::GpsdThreadStarter
{
public:
  explicit GpsdThreadStarter(Gps::GpsdCallback::SharedPtr callback)
    : ThreadStopped(false)
    , Thread(callback, ThreadStopped)
    , ThreadStarter(boost::ref(Thread))
  {
  }

  ~GpsdThreadStarter()
  {
    ThreadStopped = true;
    ThreadStarter.join();
  }
  
private:
  volatile bool ThreadStopped;
  GpsdThread Thread;
  boost::thread ThreadStarter;
};

Gps::Gpsd::Gpsd(const std::string& hostname, unsigned port)
{
}

Gps::Gpsd::~Gpsd()
{
}

void Gps::Gpsd::SetCallback(Gps::GpsdCallback::SharedPtr callback)
{
  Callback = callback;
  if (callback)
  {
    ThreadStarter.reset(new GpsdThreadStarter(callback));
    return;
  }
  ThreadStarter = std::unique_ptr<GpsdThreadStarter>();
}

Gps::GpsdCallback::SharedPtr Gps::Gpsd::GetCallback() const
{
  return Callback;
}

