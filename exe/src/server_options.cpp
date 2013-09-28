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

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace
{
  namespace po = boost::program_options;
  using namespace OpcUa;

  const char* OPTION_HELP = "help";
  const char* OPTION_CONFIG = "config";
  const char* OPTION_DAEMON = "daemon";

  std::string GetConfigOptionValue(const po::variables_map& vm)
  {
    if (vm.count(OPTION_CONFIG))
    {
      return vm[OPTION_CONFIG].as<std::string>();
    }
    return "/etc/opcua/server.config";
  }


  bool GetDaemonMode(const po::variables_map& vm)
  {
    return vm.count(OPTION_DAEMON) != 0;
  }
}


namespace OpcUa
{
  namespace Server
  {

    CommandLine::CommandLine(int argc, char** argv)
      : StartPossible(true)
      , IsDaemon(false)
    {
      // Declare the supported options.
      po::options_description desc("Parameters");
      desc.add_options()
        (OPTION_HELP, "Print help message and exit.")
        (OPTION_CONFIG, po::value<std::string>(), "Path to config file.")
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
      std::string configFile = GetConfigOptionValue(vm);
      const Common::ModulesConfiguration modules = Common::ParseConfiguration(configFile);
      std::transform(modules.begin(), modules.end(), std::back_inserter(Modules), std::bind(&Common::GetAddonInfomation, std::placeholders::_1));
    }

  } // namespace Server
} // namespace OpcUa

