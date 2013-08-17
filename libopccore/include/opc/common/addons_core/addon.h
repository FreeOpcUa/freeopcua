/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addon interface definition
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __COMMON_ADDON_H__
#define __COMMON_ADDON_H__

#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/addon_parameters.h>
#include <opc/common/class_pointers.h>
#include <opc/common/interface.h>

namespace Common
{

  class Addon : private Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(Addon);

  public:
    /// @brief initialize addon.
    /// @note Calls when all addons dependencies resolved.
    /// suppose that addon fully ready for work.
    virtual void Initialize(AddonsManager& manager, const AddonParameters& parameters) = 0;

    /// @brief Stopping addon work.
    /// After calling this method addon should throw exception on any calls.
    virtual void Stop() = 0;
  };

  // @brief factory of addon.
  class AddonFactory : private Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(AddonFactory);

  public:
    /// @brief Create instance of addon
    virtual Addon::UniquePtr CreateAddon() = 0;
  };

} // namespace Common

#endif // __COMMON_ADDON_H__

