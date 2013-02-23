/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Dynamic library wrapper.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef _test_opc_server_dll_h_
#define _test_opc_server_dll_h_

#include <memory>
#include <string>

namespace Com
{
  namespace Impl
  {
    class DynamicLibraryManager
    {
    public:
      DynamicLibraryManager(){}
      virtual ~DynamicLibraryManager(){}

      virtual void* FindSymbol(const std::string& funcName, const std::string& library) = 0;

      template <typename FuncType> 
      FuncType Find(const std::string& funcName, const std::string& library)
      {
        return reinterpret_cast<FuncType>(FindSymbol(funcName, library));
      }

    private:
      DynamicLibraryManager(const DynamicLibraryManager&);
      DynamicLibraryManager& operator=(const DynamicLibraryManager&);
    };


    std::unique_ptr<DynamicLibraryManager> CreateDynamicLibraryManager();
  }
}

#endif // _test_opc_server_dll_h_

