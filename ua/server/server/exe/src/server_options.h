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


#include <opc/common/addons_core/addon.h>

#include <string>
#include <vector>

namespace OpcUa
{
  namespace Server
  {

    typedef std::vector<Common::AddonConfiguration> ModulesConfiguration;

    class CommandLine
    {
    public:
      CommandLine(int argc, char** argv);

      ModulesConfiguration GetModules()
      {
        return Modules;
      }

    private:
      ModulesConfiguration Modules;
    };

  }
}

#endif // _OPC_UA_SERVER_OPTIONS_H_

