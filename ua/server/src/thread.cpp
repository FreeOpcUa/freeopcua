/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Thread wrapper with coockies.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <internal/thread.h>

#include <iostream>

namespace OpcUa
{
  namespace Internal
  {

    Thread::Thread(std::function<void()> f, ThreadObserver& observer)
      : Observer(observer)
      , Func(f)
      , Impl(Thread::ThreadProc, this)
    {
    }

    void Thread::Join()
    {
      Impl.join();
    }

    void Thread::Run() const
    {
      try
      {
        Func();
        Observer.OnSuccess();
      }
      catch (const std::exception& exc)
      {
        Observer.OnError(exc);
      }
    }

    void Thread::ReportSuccess() const
    {
      try
      {
      }
      catch (const std::logic_error& exc)
      {
        std::cerr << "FATAL ERROR! Oserver threw an exception during call of OnSuccess. " << exc.what() << std::endl;
        exit(-1);
      }
    }

    void Thread::ThreadProc(Thread* thread)
    {
      thread->Run();
    }

  } // bnamespace Server
} // namespace OpcUa

