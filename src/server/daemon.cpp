/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Daemon class.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "daemon.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <mutex>
#include <condition_variable>
#include <iostream>
#include <signal.h>

namespace
{

  std::condition_variable* ExitEvent = 0;

  void TerminateSignal(int signum)
  {
    std::cout << "terminating.." << std::endl;
    if (ExitEvent)
    {
      ExitEvent->notify_all();
      ExitEvent = nullptr;
    }
  }

}

namespace OpcUa
{
  Daemon::Daemon()
  {
  }

  Daemon::~Daemon()
  {
  }

  void Daemon::Daemonize(const std::string& logFile)
  {
    pid_t pid, sid;

    pid = fork();
    if (pid < 0)
    {
      std::cerr << "Failed to fork: " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
      exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0)
    {
      std::cerr << "setsid() failed: " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0)
    {
      std::cerr << "Cannot change dir. " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }

    if (!logFile.empty())
    {
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);

      FILE* tmp = fopen(logFile.c_str(), "w");
      if (!tmp)
      {
        std::cerr << "Cannot open log file " << logFile << ". " << strerror(errno) << std::endl;
      }
      dup2(fileno(tmp), STDOUT_FILENO);
      dup2(fileno(tmp), STDERR_FILENO);
    }
  }

  void Daemon::WaitForTerminate()
  {
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    if (signal(SIGTERM, TerminateSignal) == SIG_ERR)
    {
      std::cout << "unable to set SIGTERM handler" << std::endl;
    }
    if (signal(SIGINT, TerminateSignal) == SIG_ERR)
    {
      std::cout << "unable to set SIGINT handler" << std::endl;
    }
    if (signal(SIGSTOP, TerminateSignal) == SIG_ERR)
    {
      std::cout << "unable to set SIGSTOP handler" << std::endl;
    }
    std::condition_variable event;
    ExitEvent = & event;
    event.wait(lock);
  }
}

