/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Server main.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "common_addons.h"
#include "daemon.h"
#include "server_options.h"

#include <thread>
#include <chrono>

#include <iostream>

int main(int argc, char** argv)
{
  try
  {
    const char** arv = (const char**)argv;
    OpcUa::Server::CommandLine options(argc, arv);
    if (!options.NeedStartServer())
    {
      return 0;
    }

    OpcUa::Daemon daemon;
    if (options.IsDaemonMode())
    {
      daemon.Daemonize(options.GetLogFile());
    }

    Common::AddonsManager::UniquePtr manager = Common::CreateAddonsManager();
    OpcUa::Server::LoadConfiguration(options.GetConfigDir(), *manager);

    manager->Start();
    daemon.WaitForTerminate();
    manager->Stop();

    return 0;
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown error." << std::endl;
  }
  return -1;
}

