/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Daemon class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/class_pointers.h>
#include <mutex>
#include <condition_variable>

namespace OpcUa
{

class Daemon
{
public:
  DEFINE_CLASS_POINTERS(Daemon)

public:
  Daemon();
  ~Daemon();

  void Daemonize(const std::string & str);
  void WaitForTerminate();

  void Terminate();

private:
  void SetTerminateHandlers();

private:
  std::mutex Mutex;
  std::condition_variable ExitEvent;
  volatile bool Terminating = false;
};

} // namespace OpcUa

