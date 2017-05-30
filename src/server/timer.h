/******************************************************************************
 *   Copyright (C) 2013-2014 by Alexander Rykovanov                        *
 *   rykovanov.as@gmail.com                                                   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation; version 3 of the License.     *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Lesser General Public License for more details.                      *
 *                                                                            *
 *   You should have received a copy of the GNU Lesser General Public License *
 *   along with this library; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                *
 ******************************************************************************/

#pragma once

#include <atomic>
#include <boost/asio/deadline_timer.hpp>
#include <boost/chrono.hpp>
#include <mutex>
#include <condition_variable>

namespace OpcUa
{
class PeriodicTimer
{
public:
  PeriodicTimer(boost::asio::io_service & io)
    : Timer(io)
    , IsCanceled(true)
    , Stopped(true)
  {
  }

  ~PeriodicTimer()
  {
    Cancel();
  }

  void Start(const boost::asio::deadline_timer::duration_type & t, std::function<void()> handler)
  {
    std::unique_lock<std::mutex> lock(Mutex);

    if (!Stopped)
      { return; }

    Stopped = false;
    IsCanceled = false;
    Timer.expires_from_now(t);
    Timer.async_wait([this, handler, t](const boost::system::error_code & error)
    {
      OnTimer(error, handler, t);
    });
  }

  void Cancel()
  {
    std::unique_lock<std::mutex> lock(Mutex);

    if (Stopped)
      { return; }

    IsCanceled = true;
    Timer.cancel();
    StopEvent.wait(lock, [this]()
    {
      return static_cast<bool>(Stopped);
    });
  }

private:
  void OnTimer(const boost::system::error_code & error, std::function<void()> handler, boost::asio::deadline_timer::duration_type t)
  {
    std::unique_lock<std::mutex> lock(Mutex);

    if (IsCanceled || error)
      {
        Stopped = true;
        IsCanceled = true;
        StopEvent.notify_all();
        return;
      }

    handler();

    Timer.expires_from_now(t);
    Timer.async_wait([this, handler, t](const boost::system::error_code & error)
    {
      OnTimer(error, handler, t);
    });
  }

private:
  std::mutex Mutex;
  std::condition_variable StopEvent;
  boost::asio::deadline_timer Timer;
  std::atomic<bool> IsCanceled;
  std::atomic<bool> Stopped;
};
}
