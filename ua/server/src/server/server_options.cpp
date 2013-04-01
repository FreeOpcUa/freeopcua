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

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


namespace
{
  namespace po = boost::program_options;
  using namespace OpcUa;

  const char* OPTION_HELP = "help";
  const char* OPTION_PORT = "port";

  int GetPortOptionValue(const po::variables_map& vm)
  {
    if (vm.count(OPTION_PORT))
    {
      return vm[OPTION_PORT].as<int>();
    }
    return 4841;
  }

}


namespace OpcUa
{
  namespace Server
  {

    CommandLine::CommandLine(int argc, char** argv)
    {
      // Declare the supported options.
      po::options_description desc("Parameters");
      desc.add_options()
        (OPTION_HELP, "produce help message")
        (OPTION_PORT, po::value<int>(), "Port number which will be listen for client conection.")
        ;

      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);    

      if (vm.count(OPTION_HELP)) 
      {
        desc.print(std::cout);
        return;
      }

      Port = GetPortOptionValue(vm);
    }

  } // namespace Server
} // namespace OpcUa
