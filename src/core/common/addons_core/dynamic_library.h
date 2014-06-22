/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic library class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef __opc_common_dynamic_library_h
#define __opc_common_dynamic_library_h

#include <string>

namespace Common
{

  class DynamicLibrary
  {
  public:
    DynamicLibrary(const std::string& libraryPath);
    ~DynamicLibrary();

    template <typename FuncType> 
    FuncType Find(const std::string& funcName)
    {
      return reinterpret_cast<FuncType>(FindSymbol(funcName));
    }

  private:
    void* FindSymbol(const std::string& funcName);

  private:
     const std::string Path;
     void* Library;
  };

}

#endif // __opc_common_dynamic_library_h

