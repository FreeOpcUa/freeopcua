/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief CoCreateInterface inplementation. Creates class instance with using registry.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include <com/class_ids.h>
#include <com/iclass_registry.h>
#include <com/impl/error.h>
#include <getopt.h>

namespace
{
  enum  OperationType : int
  {
     OP_UNKNOWN = 0,
     OP_REGISTER_CLASS = 1,
     OP_UNREGISTER_CLASS = 2,
  };

  struct Operation
  {
    OperationType Type;
    std::string ClsId;
    std::string LibraryPath;

    Operation()
      : Type(OP_UNKNOWN)
    {
    }

    bool IsValid() const
    {
      return !ClsId.empty() && Type != OP_UNKNOWN;
    }
  };


  Operation GetOperation(int argc, char** argv)
  {
    static option options[] =
    {
      {"register",   no_argument,       0, 'r'},
      {"unregister", no_argument,       0, 'u'},
      {"clsid",      required_argument, 0, 'c'},
      {"library",    required_argument, 0, 'l'},
      {0,0,0,0}
    };
    
    Operation operation;
    while(true)
    {
      const int optNum = getopt_long(argc, argv, "r:u:c:l", options, NULL);
      if (optNum == -1)
      {
        break;
      }

      switch (optNum)
      {
      case 'r':
        operation.Type = OP_REGISTER_CLASS;
        break;

      case 'u':
        operation.Type = OP_UNREGISTER_CLASS;
        break;
       
      case 'c':
        operation.ClsId = optarg ? optarg : "";
        break;
    
      case 'l':
        operation.LibraryPath = optarg ? optarg : "";
        break;

      case '?':
        throw Com::Impl::Error(Com::COM_ERROR, std::string("Unknown command line option ") + (optarg ? optarg : ""));

      default:
        throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, std::string("Function getopt returned invalid result "));
      }
    }
    return operation;
  }


  Com::IClassRegistry::SharedPtr CreateClassRegistry()
  {
    Com::IUnknown::SharedPtr unk;
    if(Com::HRESULT res = Com::CoCreateInstance(CLSID_ClassRegistry, Com::ResultPtr(unk)))
    {
      throw Com::Impl::Error(res, "Unable to create class registry object.");
    }
    Com::IClassRegistry::SharedPtr registry;
    if (Com::HRESULT res = unk->QueryInterface(IID_IClassRegistry, Com::ResultPtr(registry)))
    {
      throw Com::Impl::Error(res, "Class registry object doesn't support IClassRegistry interface.");
    }
    return registry;
  }

  void Perform(Com::IClassRegistry& registry, const Operation& operation)
  {
    switch (operation.Type)
    {
    case OP_REGISTER_CLASS:
      if (const Com::HRESULT res = registry.Register(operation.ClsId.c_str(), operation.LibraryPath.c_str()))
      {
        throw Com::Impl::Error(res, std::string("unable to register class '") + operation.ClsId + std::string("' in the library '") + operation.LibraryPath + std::string("'.") );
      }
      break;

    case OP_UNREGISTER_CLASS:
      if (const Com::HRESULT res = registry.Unregister(operation.ClsId.c_str()))
      {
        throw Com::Impl::Error(res, std::string("unable to register class '") + operation.ClsId + std::string("' in the library '") + operation.LibraryPath + std::string("'.") );
      }
      break;

    default:
      throw Com::Impl::Error(Com::COM_ERROR, "I can't understand whate to do.");
    }

    if (!operation.IsValid())
    {
      throw Com::Impl::Error(Com::COM_ERROR, "Invalid command-line parameters.");
    }
  }

}


int main(int argc, char** argv)
{
  CATCH_BLOCK_START

  const Operation operation = GetOperation(argc, argv);
  Com::IClassRegistry::SharedPtr registry = CreateClassRegistry();
  Perform(*registry, operation);
  CATCH_BLOCK_FINISH
}

