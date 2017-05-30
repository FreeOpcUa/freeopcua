/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Daemon class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "daemon.h"


namespace OpcUa
{

Daemon::Daemon()
{
  SetTerminateHandlers();
}

Daemon::~Daemon()
{
}

void Daemon::WaitForTerminate()
{
  std::unique_lock<std::mutex> lock(Mutex);
  ExitEvent.wait(lock, [this]() { return Terminating; });
}

void Daemon::Terminate()
{
  Terminating = true;
  ExitEvent.notify_all();
}

}

