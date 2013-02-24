/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Error manager interface
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef manager_h_6a0a0676_991a_46cd_8a4e_3b14595480c7
#define manager_h_6a0a0676_991a_46cd_8a4e_3b14595480c7

#include <opccore/common/class_pointers.h>
#include <opccore/common/exception.h>
#include <opccore/common/noncopyable.h>

namespace ErrorManager
{
  /// @brief Manager of errors for futher processing and logging
  class Manager 
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(Manager);

  public: 
    /// @brief set last error
    /// @param lastError last error.
    /// @throw not throws 
    virtual void SetLastError(const Common::Error& lastError) = 0;

    /// @brief Get last error
    /// @throw not throws
    virtual Common::Error GetLastError() const = 0;
  };
}

#endif // manager_h_6a0a0676_991a_46cd_8a4e_3b14595480c7
