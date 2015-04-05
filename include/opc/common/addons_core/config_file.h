/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon_manager.h>

namespace Common
{

  struct ModuleConfiguration
  {
    AddonId Id;
    std::vector<AddonId> Dependencies;
    std::string Path;
    AddonParameters Parameters;
  };

  typedef std::vector<Common::ModuleConfiguration> ModulesConfiguration;

  struct Configuration
  {
    Common::AddonParameters Parameters;
    std::vector<ModuleConfiguration> Modules;
  };

  Common::AddonInformation GetAddonInfomation(const ModuleConfiguration& config);

  Configuration ParseConfiguration(const std::string& configPath);
  void SaveConfiguration(const ModulesConfiguration& configuration, const std::string& configPath);

  Configuration ParseConfigurationFiles(const std::string& directory);

}

