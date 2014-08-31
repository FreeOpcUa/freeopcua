/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa client command line options parser.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "server_options.h"

#include <opc/common/addons_core/config_file.h>

#include <algorithm>
#include <functional>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#ifndef CONFIG_PATH
#define CONFIG_PATH "/etc/opcua/server"
#endif

namespace
{
  namespace po = boost::program_options;
  using namespace OpcUa;

  const char* DefaultLogFilePath = "/var/log/opcua/server.log";

  const char* OPTION_HELP = "help";
  const char* OPTION_CONFIG = "config-dir";
  const char* OPTION_DAEMON = "daemon";
  const char* OPTION_LOGFILE = "log-file";

  std::string GetConfigOptionValue(const po::variables_map& vm)
  {
    if (vm.count(OPTION_CONFIG))
    {
      return vm[OPTION_CONFIG].as<std::string>();
    }
    return CONFIG_PATH;
  }


  bool GetDaemonMode(const po::variables_map& vm)
  {
    return vm.count(OPTION_DAEMON) != 0;
  }

  std::string GetLogFile(const po::variables_map& vm)
  {
    if (vm.count(OPTION_LOGFILE))
    {
      return vm[OPTION_LOGFILE].as<std::string>();
    }
    return DefaultLogFilePath;
  }

}


namespace OpcUa
{
  namespace UaServer
  {

    CommandLine::CommandLine(int argc, const char** argv)
      : StartPossible(true)
      , IsDaemon(false)
    {
      // Declare the supported options.
      po::options_description desc("Parameters");
      desc.add_options()
        (OPTION_HELP, "Print help message and exit.")
        (OPTION_CONFIG, po::value<std::string>(), "Path to directory with configuration files.")
        (OPTION_LOGFILE, po::value<std::string>(), "Set path to the log file. Default 'var/log/opcua/server.log")
        (OPTION_DAEMON, "Start in daemon mode.")
        ;

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);

      if (vm.count(OPTION_HELP))
      {
        desc.print(std::cout);
        StartPossible = false;
        return;
      }

      IsDaemon = GetDaemonMode(vm);
      const std::string configDir = GetConfigOptionValue(vm);
      const Common::Configuration& configuration = Common::ParseConfigurationFiles(configDir);
      std::transform(configuration.Modules.begin(), configuration.Modules.end(), std::back_inserter(Modules), std::bind(&Common::GetAddonInfomation, std::placeholders::_1));
      Parameters = configuration.Parameters;
      LogFile = ::GetLogFile(vm);
    }

  } // namespace UaServer
} // namespace OpcUa

