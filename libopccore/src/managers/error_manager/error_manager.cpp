/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Error Manager realization
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "register.h"

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/managers/error_manager/manager.h>
#include <opccore/managers/property_tree/property_tree.h>

namespace
{
  class ErrorManagerImpl 
    : public ErrorManager::Manager
    , public Common::Addon
  {
  public:
    ErrorManagerImpl()
    {
    }

  public: // Addon interface
    virtual void Initialize()
    {
    }

    virtual void Stop()
    {
    }

  public: // ErrorManager::Manager
    virtual void SetLastError(const Common::Error& lastError)
    {
      LastError = lastError;
    }

    virtual Common::Error GetLastError() const
    {
      return LastError;
    }

  private:
    Common::Error LastError;
  };

  class ErrorManagerFactory : public Common::AddonFactory
  {
  public:
    virtual Common::Addon::UniquePtr CreateAddon()
    {
      return Common::Addon::UniquePtr(new ErrorManagerImpl());
    }
  };

} // unnamed namespace



void ErrorManager::RegisterErrorManagerAddon()
{
  const Common::AddonsManager::SharedPtr manager = Common::GetAddonsManager();
  manager->Register(Common::ADDON_ID_ERROR_MANAGER, Common::AddonFactory::UniquePtr(new ErrorManagerFactory()));
}
