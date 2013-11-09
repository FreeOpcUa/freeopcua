/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Thread wrapper with coockies.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>

#include <thread>
#include <stdexcept>

namespace Common
{

  class ThreadObserver : private Common::Interface
  {
  public:
    /// @brief thread exited with Success.
    virtual void OnSuccess() = 0;
    /// @brief Thread exited with error.
    virtual void OnError(const std::exception& exc) = 0;
  };

  typedef std::function<void()> ThreadProc;

  class Thread
  {
  public:
    DEFINE_CLASS_POINTERS(Thread);

  public:
    /// @brief Starts f in a separate thread.
    Thread(std::function<void()> f, ThreadObserver* observer = 0);

    static Thread::UniquePtr Create(ThreadProc f, ThreadObserver* observer = 0)
    {
      return Thread::UniquePtr(new Thread(f, observer));
    }

    static Thread::UniquePtr Create(void (*f)(), ThreadObserver* observer = 0)
    {
      Common::ThreadProc proc(f);
      return Thread::UniquePtr(new Common::Thread(proc, observer));
    }

    ~Thread();
    /// @brief Wait until started thread stop.
    void Join();

  public:
    /// @brief Calls in the separate thread from ThreadProc.
    void Run();
    /// @brief static member required for std::thread
    static void ThreadProc(Thread* thread);

  private:
    ThreadObserver* Observer;
    Common::ThreadProc Func;
    std::thread Impl;
  };

} // namespace Common

