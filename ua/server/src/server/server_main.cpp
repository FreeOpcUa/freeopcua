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

#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/dynamic_addon_factory.h>
#include <opc/ua/server/tcp_server.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <iostream>
#include <signal.h>
#include <stdexcept>


namespace
{

  using namespace OpcUa;

  struct Tabs
  {
    Tabs(unsigned num = 0)
      : Num(num)
    { 
    }
    unsigned Num;
  };

  std::ostream& operator <<(std::ostream& os, const Tabs& tabs)
  {
    for (unsigned i = 0; i < tabs.Num; ++i)
    {
      os << " ";
    }
    return os;
  }

  class Stub : public Server::IncomingConnectionProcessor
  {
   public:
    virtual void Process(std::shared_ptr<IOChannel> clientChannel)
    {
    }

    virtual void StopProcessing(std::shared_ptr<IOChannel> clientChannel)
    {
    }
  };

  boost::mutex ExitMutex;
  boost::condition_variable ExitEvent;

  void TerminateSignal(int signum)
  {
    std::cout << "terminating.." << std::endl;
    ExitEvent.notify_all();
  }

  void Process(int argc, char** argv)
  {
    boost::mutex::scoped_lock lock(ExitMutex);
    if (signal(SIGTERM, TerminateSignal) == SIG_ERR)
    {
      std::cout << "unable to set SIGTERM handler" << std::endl;
    }
    if (signal(SIGINT, TerminateSignal) == SIG_ERR)
    {
      std::cout << "unable to set SIGINT handler" << std::endl;
    }


    Server::CommandLine cmdLine(argc, argv);
    Common::AddonsManager::UniquePtr addons = Common::CreateAddonsManager();
    const Server::ModulesConfiguration& modules = cmdLine.GetModules();
    for (Server::ModulesConfiguration::const_iterator moduleIt = modules.begin(); moduleIt != modules.end(); ++moduleIt)
    {
      Common::AddonConfiguration addonConfig;
      addonConfig.ID = moduleIt->ID;
      addonConfig.Factory = Common::CreateDynamicAddonFactory(moduleIt->Path.c_str());
      addonConfig.Dependencies = moduleIt->DependsOn;
      addonConfig.Parameters = moduleIt->Parameters;
      addons->Register(addonConfig);
    }
    addons->Start();
    ExitEvent.wait(lock);
    addons->Stop();
  }
}

int main(int argc, char** argv)
{
  try
  {
    Process(argc, argv);
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

