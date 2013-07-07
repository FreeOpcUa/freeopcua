/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Thread wrapper with coockies.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/thread.h>

#include <iostream>

namespace Common
{

    Thread::Thread(std::function<void()> f, ThreadObserver* observer)
      : Observer(observer)
      , Func(f)
      , Impl(Thread::ThreadProc, this)
    {
    }

    Thread::~Thread()
    {
      try
      {
        Join();
      }
      catch (std::exception&)
      {
      }
    }

    void Thread::Join()
    {
      Impl.join();
    }

    void Thread::Run()
    {
      try
      {
        Func();
        if (Observer)
        {
          Observer->OnSuccess();
        }
      }
      catch (const std::exception& exc)
      {
        if (Observer)
        {
          Observer->OnError(exc);
        }
      }
    }

    void Thread::ThreadProc(Thread* thread)
    {
      thread->Run();
    }

} // namespace Common

