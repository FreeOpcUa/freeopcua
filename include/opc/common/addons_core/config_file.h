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
    AddonID ID;
    std::vector<AddonID> Dependencies;
    std::string Path;
    AddonParameters Parameters;
  };

  Common::AddonInformation GetAddonInfomation(const ModuleConfiguration& config);

  typedef std::vector<Common::ModuleConfiguration> ModulesConfiguration;

  ModulesConfiguration ParseConfiguration(const std::string& configPath);
  void SaveConfiguration(const ModulesConfiguration& configuration, const std::string& configPath);

  ModulesConfiguration ParseConfigurationFiles(const std::string& directory);

}

