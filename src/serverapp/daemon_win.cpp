/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Daemon class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <windows.h>

#include "daemon.h"
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <signal.h>

namespace
{

OpcUa::Daemon * DaemonInstance = 0;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
    {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:

      if (DaemonInstance)
        {
          std::cout << "terminating.." << std::endl;
          DaemonInstance->Terminate();
          DaemonInstance = nullptr;
        }

      return TRUE;

    default:
      return FALSE;
    }
}

}

namespace OpcUa
{

void Daemon::Daemonize(const std::string & logFile)
{
  // TODO Implement windows srvice behavior.
}

void Daemon::SetTerminateHandlers()
{
  if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE))
    {
      std::cerr << "Cannot set terminate handler. Application may not response on exit event." << std::endl;
    }

  DaemonInstance = this;
}

}

