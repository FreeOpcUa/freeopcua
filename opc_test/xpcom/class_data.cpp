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

#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>

namespace
{
  const char* HELP_DESCRIPTION = "OPC server test tool."
                                 "\nAuthor Rykovanov Alexander (rykovanov.as@gmail.com)"
                                 "\nDestributes under GPL license."
                                 "\nHome page http://mozopc.sourceforge.net."
                                 "\nAlowed options:";

  const char* HELP_OPTION_NAME       = "help";
  const char* FILENAME_OPTION_NAME   = "filename";
  const char* CLASSNAME_OPTION_NAME  = "name";
  const char* GUID_OPTION_NAME       = "guid";
#ifdef _WIN32
  const char* PROGID_OPTION_NAME     = "prog-id";
  const char* DISABLE_COM_TESTS_OPTION_NAME = "disable-com-tests";
#endif // _WIN32

  const char* CONTRACTID_OPTION_NAME = "contract-id";
  const char* DISABLE_MOZILLA_TESTS_OPTION_NAME = "disable-mozilla-tests";
}

namespace OPCTest
{
  CoClassData::CoClassData(
                           const std::string& filename, 
                           const std::string& name, 
                           const std::string& guid, 
#ifdef _WIN32
                           const std::string& progid,
                           bool disableCom, 
#endif // _WIN32     
                           const std::string& contractId,
                           bool disableMozilla)
    : Filename(filename)
    , ClassName(name)
    , ClassGuid(guid)
#ifdef _WIN32
    , ClassProgId(progid)
    , RegistryKeys(name, guid, progid)
    , DisableComTests(disableCom)
#endif // _WIN32
    , ClassContractId(contractId)
    , DisableMozillaTests(disableMozilla)
  {
  }

  inline CoClassData::~CoClassData()
  {
  }

  std::string CoClassData::GetFilename() const
  {
    return Filename;
  }

  std::string CoClassData::GetContractId() const
  {
    return ClassContractId;
  }

  bool CoClassData::IsMozillaTestsDisabled() const
  {
    return DisableMozillaTests;
  }

  std::string CoClassData::GetClsId() const
  {
    return ClassGuid;
  }

  std::string CoClassData::GetName() const
  {                                       
    return ClassName;                     
  }

#ifdef _WIN32
  std::string CoClassData::GetProgId() const
  {
    return ClassProgId;
  }

  bool CoClassData::IsWindowsTestsDisabled() const
  {
    return DisableComTests;
  }

  OpcRegistry::OpcRegistry(const std::string& name, const std::string& guid, const std::string& progid)
    : ClsId("CLSID")
    , ClsIdKeyName("{" + guid + "}")
    , ClsIdFullPathKey("CLSID\\{" + guid + "}")
    , InProcServer32Key("InProcServer32")
    , ProgId_ClsIdKey(progid + "\\CLSID")
  {
  }
#endif // _WIN32

  class ClassDataBuilder
  {
  public:
    std::unique_ptr<CoClassData> Build(int argc, char** argv);
  };

  std::unique_ptr<CoClassData> ClassDataBuilder::Build(int argc, char** argv)
  {
    namespace po = boost::program_options;
    // Declare the supported options.
    po::options_description desc(HELP_DESCRIPTION);
    desc.add_options()
      (HELP_OPTION_NAME, "produce help message")
      (FILENAME_OPTION_NAME, po::value<std::string>(), "Filename of opc server library that should be tested.")
      (CLASSNAME_OPTION_NAME, po::value<std::string>(), "Short Description of the class")
      (GUID_OPTION_NAME, po::value<std::string>(), "GUID of the class")
#ifdef _WIN32
      (PROGID_OPTION_NAME, po::value<std::string>(), "ProgId of the class")
      (DISABLE_COM_TESTS_OPTION_NAME, "Disable COM server tests.")
#endif // _WIN32
      (CONTRACTID_OPTION_NAME, po::value<std::string>(), "Contract ID of the class")
      (DISABLE_MOZILLA_TESTS_OPTION_NAME, "Disable Mozilla OPC server tests.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count(HELP_OPTION_NAME)) 
    {
      std::stringstream str;
      str << desc;
      throw std::logic_error(str.str().c_str());
    }

    if (!vm.count(FILENAME_OPTION_NAME))
    {
      throw po::invalid_command_line_syntax(FILENAME_OPTION_NAME, po::invalid_command_line_syntax::missing_parameter);
    }
    const std::string filename = vm[FILENAME_OPTION_NAME].as<std::string>();

    if (!vm.count(CLASSNAME_OPTION_NAME)) 
    {
      throw po::invalid_command_line_syntax(CLASSNAME_OPTION_NAME, po::invalid_command_line_syntax::missing_parameter);
    }
    const std::string clsName = vm[CLASSNAME_OPTION_NAME].as<std::string>();

    if (!vm.count(GUID_OPTION_NAME)) 
    {
      throw po::invalid_command_line_syntax(GUID_OPTION_NAME, po::invalid_command_line_syntax::missing_parameter);
    }
    const std::string clsGuid = vm[GUID_OPTION_NAME].as<std::string>();

    bool disableMozilla = false;
    if (vm.count(DISABLE_MOZILLA_TESTS_OPTION_NAME)) 
    {
      disableMozilla = true;
    }

    std::string clsContractId;
    if (!disableMozilla)
    {
      if (!vm.count(CONTRACTID_OPTION_NAME)) 
      {
        throw po::invalid_command_line_syntax(CONTRACTID_OPTION_NAME, po::invalid_command_line_syntax::missing_parameter);
      }
      clsContractId = vm[CONTRACTID_OPTION_NAME].as<std::string>();
    }

#ifdef _WIN32
    bool disableCom = false;
    if (vm.count(DISABLE_COM_TESTS_OPTION_NAME)) 
    {
      disableCom = true;
    }

    std::string clsProgId;
    if (!disableCom)
    {
      if (!vm.count(PROGID_OPTION_NAME)) 
      {
        throw po::invalid_command_line_syntax(PROGID_OPTION_NAME, po::invalid_command_line_syntax::missing_parameter);
      }
      clsProgId = vm[PROGID_OPTION_NAME].as<std::string>();
    }

#endif // _WIN32

    std::cout << "Class Name: " << clsName << std::endl;
    std::cout << "Class GUID: " << clsGuid << std::endl;
#ifdef _WIN32
    std::cout << "COM server tests " << (disableCom ? "disabled" : "enabled") << std::endl;
    std::cout << "Class ProgId: " << clsProgId << std::endl;
#endif // _WIN32
    std::cout << "Mozilla OPC server tests " << (disableMozilla ? "disabled" : "enabled") << std::endl;
    std::cout << "Class ContractId: " << clsContractId << std::endl;

    return std::unique_ptr<CoClassData>(new CoClassData(
     filename,
     clsName, 
     clsGuid, 
#ifdef _WIN32
     clsProgId, 
     disableCom,
#endif //  _WIN32
     clsContractId,
     disableMozilla));
  }

  static std::unique_ptr<CoClassData> Data;

  void InitializeClassData(int argc, char** argv)
  {
    Data = ClassDataBuilder().Build(argc, argv);
  }

  const CoClassData* GetClassData()
  {
    return Data.get();
  }

}
