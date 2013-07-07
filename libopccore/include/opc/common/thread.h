/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Thread wrapper with coockies.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/interface.h>

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

  class Thread
  {
  public:
    /// @brief Starts f in a separate thread.
    Thread(std::function<void()> f, ThreadObserver* observer = 0);

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
    std::function<void()> Func;
    std::thread Impl;
  };

} // namespace Common

