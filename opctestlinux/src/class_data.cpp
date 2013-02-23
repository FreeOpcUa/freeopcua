// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include "class_data.h"

//#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>

namespace
{
  const char* HELP_DESCRIPTION = "OPC server test tool."
                                 "\nAuthor Rykovanov Alexander (rykovanov.as@gmail.com)"
                                 "\nDestributes under GPL license."
                                 "\nHome page http://mozopc.sourceforge.net."
                                 "\nAlowed options:";
/*
  const char* HELP_OPTION_NAME       = "help";
  const char* FILENAME_OPTION_NAME   = "filename";
  const char* CLASSNAME_OPTION_NAME  = "name";
  const char* GUID_OPTION_NAME       = "guid";
  const char* CONTRACTID_OPTION_NAME = "contract-id";
  const char* PROGID_OPTION_NAME = "prog-id";

  void ThrowMissignParameter(const char* optionName)
  {
    namespace po = boost::program_options;
#if BOOST_VERSION > 104800
    throw po::invalid_command_line_syntax(po::invalid_command_line_syntax::missing_parameter, optionName);
#else
    throw po::invalid_command_line_syntax(optionName, po::invalid_command_line_syntax::missing_parameter);
#endif
  }
*/

}

namespace OPCTest
{
  CoClassData::CoClassData(
                           const std::string& filename, 
//                           const std::string& name, 
                           const std::string& guid
//                           const std::string& contractId,
//                           const std::string& progId
                           )
    : Filename(filename)
//    , ClassName(name)
    , ClassGuid(guid)
//    , ClassContractId(contractId)
//    , ClassProgId(progId)
  {
  }

  CoClassData::~CoClassData()
  {
  }

  std::string CoClassData::GetFilename() const
  {
    return Filename;
  }
/*
  std::string CoClassData::GetContractId() const
  {
    return ClassContractId;
  }

  std::string CoClassData::GetProgId() const
  {
    return ClassProgId;
  }
*/
  std::string CoClassData::GetClsId() const
  {
    return ClassGuid;
  }
/*
  std::string CoClassData::GetName() const
  {                                       
    return ClassName;                     
  }
*/
  class ClassDataBuilder
  {
  public:
    std::unique_ptr<CoClassData> Build(int argc, char** argv);
  };

  std::unique_ptr<CoClassData> ClassDataBuilder::Build(int argc, char** argv)
  {
/*
    namespace po = boost::program_options;
    // Declare the supported options.
    po::options_description desc(HELP_DESCRIPTION);
    desc.add_options()
      (HELP_OPTION_NAME, "produce help message")
      (FILENAME_OPTION_NAME, po::value<std::string>(), "Filename of opc server library that should be tested.")
      (GUID_OPTION_NAME, po::value<std::string>(), "GUID of the class");

//      (CLASSNAME_OPTION_NAME, po::value<std::string>(), "Short Description of the class")
//      (CONTRACTID_OPTION_NAME, po::value<std::string>(), "Contract ID of the class")
//      (PROGID_OPTION_NAME, po::value<std::string>(), "Prog ID of the class")
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count(HELP_OPTION_NAME)) 
    {
      std::stringstream str;
      str << desc;
      throw std::logic_error(str.str().c_str());
    }

    std::string filename;
    if (vm.count(FILENAME_OPTION_NAME))
    {
      filename = vm[FILENAME_OPTION_NAME].as<std::string>();
    }
    else if(getenv("FILENAME"))
    {
      filename = getenv("FILENAME");
    }
    else
    {
      ThrowMissignParameter(FILENAME_OPTION_NAME);
    }

    if (!vm.count(CLASSNAME_OPTION_NAME)) 
    {
      ThrowMissignParameter(CLASSNAME_OPTION_NAME);
    }
    const std::string& clsName = vm[CLASSNAME_OPTION_NAME].as<std::string>();

    std::string clsGuid = vm[GUID_OPTION_NAME].as<std::string>();
    if (!vm.count(GUID_OPTION_NAME)) 
    {
      clsGuid = vm[GUID_OPTION_NAME].as<std::string>();
    }
    else 
*/
    std::string clsGuid;
    if (getenv("CLSID"))
    {
      clsGuid = getenv("CLSID");
    }
    else
    {
      throw std::logic_error("CLSID env variable not set.");
//      ThrowMissignParameter(GUID_OPTION_NAME);
    }
    std::string filename;
    if (getenv("FILENAME"))
    {
      filename = getenv("FILENAME");
    }
    else
    {
      throw std::logic_error("FILENAME env variable not set.");
//      ThrowMissignParameter(GUID_OPTION_NAME);
    }

/*
    if (!vm.count(CONTRACTID_OPTION_NAME)) 
    {
      ThrowMissignParameter(CONTRACTID_OPTION_NAME);
    }
    const std::string& clsContractId = vm[CONTRACTID_OPTION_NAME].as<std::string>();
    
    if (!vm.count(PROGID_OPTION_NAME)) 
    {
      ThrowMissignParameter(PROGID_OPTION_NAME);
    }
    const std::string& clsProgId = vm[PROGID_OPTION_NAME].as<std::string>();
 */
//    std::cout << "Class Name: " << clsName << std::endl;
//    std::cout << "Class GUID: " << clsGuid << std::endl;
//    std::cout << "Class ContractId: " << clsContractId << std::endl;

    return std::unique_ptr<CoClassData>(new CoClassData(filename, /*clsName, */clsGuid/*, clsContractId, clsProgId*/));
  }

  static std::unique_ptr<CoClassData> Data;

  void InitializeClassData(int argc, char** argv)
  {
    Data = ClassDataBuilder().Build(argc, argv);
  }

  const CoClassData& GetClassData()
  {
    return *Data;
  }
}

