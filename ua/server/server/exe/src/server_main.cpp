/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Server main.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#include "server_options.h"
#include "server.h"

#include <iostream>

int main(int argc, char** argv)
{
  try
  {
    OpcUa::Server::CommandLine options(argc, argv);
    OpcUa::Application::UniquePtr application = OpcUa::CreateApplication();
    application->Start(options.GetModules());
    application->WaitForTerminate();
    application->Stop();
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

