/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Server main.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef _OPC_UA_Server_OPTIONS_H_
#define _OPC_UA_Server_OPTIONS_H_


#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/config_file.h>

#include <string>
#include <vector>


namespace OpcUa
{
  namespace Server
  {

    class CommandLine
    {
    public:
      CommandLine(int argc, const char** argv);

      bool NeedStartServer() const
      {
        return StartPossible;
      }

      bool IsDaemonMode() const
      {
        return IsDaemon;
      }

      std::string GetConfigDir() const
      {
    	return ConfigDir;
      }

      std::string GetLogFile() const
      {
        return LogFile;
      }

    private:
      bool StartPossible;
      bool IsDaemon;
      std::string ConfigDir;
      std::string LogFile;
    };

  }
}

#endif // _OPC_UA_Server_OPTIONS_H_

