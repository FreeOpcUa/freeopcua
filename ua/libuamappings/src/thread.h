/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Thread wrapper with coockies.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opc/ua/interface.h>

#include <thread>
#include <stdexcept>

namespace OpcUa
{

  class ThreadObserver : private Interface
  {
  public:
    /// @brief thread exited with Success.
    virtual void OnSuccess() = 0;
    /// @brief Thread exited with error.
    virtual void OnError(const std::exception& exc) = 0;
  };

  class Thread
  {
  public:
    /// @brief Starts f in a separate thread. 
    Thread(std::function<void()> f, ThreadObserver& observer);
    /// @brief Wait until started thread stop.
    void Join();

  public:
    /// @brief Calls in the separate thread from ThreadProc.
    void Run() const;
    /// @brief static member required for std::thread
    static void ThreadProc(Thread* thread);

    void ReportSuccess() const;

  private:
    ThreadObserver& Observer;
    std::function<void()> Func;
    std::thread Impl;
  };
}

