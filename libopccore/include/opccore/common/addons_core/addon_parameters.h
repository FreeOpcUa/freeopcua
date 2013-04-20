/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Addon parameters
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef OPCCORE_ADDONS_CORE_ADDON_PARAMETERS_H
#define OPCCORE_ADDONS_CORE_ADDON_PARAMETERS_H

#include <string>
#include <vector>

namespace Common
{

  struct Parameter
  {
    std::string Name;
    std::string Value;
  };

  struct ParametersGroup
  {
    std::string Name;
    std::vector<Parameter> Parameters;
  };


  struct AddonParameters
  {
    std::vector<ParametersGroup> Groups;
    std::vector<Parameter> Parameters;
  };

}

#endif // OPCCORE_ADDONS_CORE_ADDON_PARAMETERS_H

