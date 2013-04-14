/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Common test utls.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#include "common.h"

#include <opccore/common/addons_core/dynamic_addon_factory.h>
#include <src/server/server_options.h>

std::unique_ptr<Common::AddonsManager> OpcUa::LoadAddons(const std::string& configPath)
{
  std::string config = "--config=";
  config += configPath;

  char* argv[2];
  argv[0] =  "test.exe";
  argv[1] =   const_cast<char*>(config.c_str());
  OpcUa::Server::CommandLine cmdline(2, argv);
  OpcUa::Server::ModulesConfiguration modules = cmdline.GetModules();
  Common::AddonsManager::UniquePtr addons = Common::CreateAddonsManager();
  for (auto module : modules)
  {
    addons->Register(module.ID, Common::CreateDynamicAddonFactory(module.Path.c_str()), module.DependsOn);
  }
  addons->Start();
  return addons;
}


