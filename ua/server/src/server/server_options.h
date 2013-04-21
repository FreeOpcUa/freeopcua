/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Server main.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef _OPC_UA_SERVER_OPTIONS_H_
#define _OPC_UA_SERVER_OPTIONS_H_


#include <opccore/common/addons_core/addon_parameters.h>
#include <string>
#include <vector>

namespace OpcUa
{
  namespace Server
  {

    struct ModuleConfig
    {
      std::string ID;
      std::string Path;
      std::vector<std::string> DependsOn;
      Common::AddonParameters Parameters;
    };

    typedef std::vector<ModuleConfig> ModulesConfiguration;

    class CommandLine
    {
    public:
      CommandLine(int argc, char** argv);

      int GetPort() const
      {
        return Port;
      }

      ModulesConfiguration GetModules()
      {
        return Modules;
      }

    private:
      int Port;
      ModulesConfiguration Modules;
    };

  }
}

#endif // _OPC_UA_SERVER_OPTIONS_H_

