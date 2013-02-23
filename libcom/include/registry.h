/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Com registry implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __registry_h_f79ce3aa_5783_4a21_a749_356e05b3b0f3
#define __registry_h_f79ce3aa_5783_4a21_a749_356e05b3b0f3

#include <string>

namespace Com
{

  class ClassRegistry
  {
  public:
    ClassRegistry()
    {
    }

    virtual std::string GetLibraryPath(const std::string& clsid) const = 0;
    virtual void RegisterClass(const std::string& clsid, const std::string& libraryPath) = 0;
    virtual void UnregisterClass(const std::string& clsid) = 0;
   
  private:
    ClassRegistry(const ClassRegistry&);
    ClassRegistry& operator=(const ClassRegistry&);
  };

} // namespace Com

#endif // __registry_h_f79ce3aa_5783_4a21_a749_356e05b3b0f3
