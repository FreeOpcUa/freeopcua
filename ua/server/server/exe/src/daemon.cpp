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


namespace OpcUa
{

  Daemon::Daemon(const char* logFile)
  {
    pid_t pid, sid;

    pid = fork();
    if (pid < 0)
    {
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
      exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0)
    {
      exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);    
  }

  Daemon::~Daemon()
  {
  }

}

