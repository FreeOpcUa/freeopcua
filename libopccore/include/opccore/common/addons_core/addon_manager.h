/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addons manager interface definition
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __COMMON_ADDONS_MANAGER_H__
#define __COMMON_ADDONS_MANAGER_H__

#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/class_pointers.h>
#include <vector>

namespace Common
{
  class Addon;
  class AddonFactory;

  class AddonsManager : private NonCopyable
  {
  public: 
    DEFINE_CLASS_POINTERS(AddonsManager);

  public:
    /// @brief register new addon.
    /// @param id id of registering addon
    /// @param factory addon factory
    /// @param dependencies ids of addons of which this addon depends on.
    /// @throws if addon already redistered. If manager started thows if not all dependencies resolved.
    /// If manager already started addon will be immediately created and initialized.
    virtual void Register(AddonID id, AddonFactory::UniquePtr factory, const std::vector<AddonID>& dependencies = std::vector<AddonID>()) = 0;

    /// @brief unregister addon
    /// @param id id of unregistering addon
    /// @throws if addon not found
    /// For unregistering addon will be called method Addon::Stop() and addon data will be removed.
    virtual void Unregister(AddonID id) = 0;

    /// @brief getting addon by id
    /// @param id id of the required addon
    /// @return addon instance
    /// @throws if addon is not registered or not initialized yet.
    virtual Addon::SharedPtr GetAddon(AddonID id) const = 0;

    /// @brief starting work.
    /// creates all addons and initializes them.
    /// @throws if not all addons dependencies can be resolved.
    virtual void Start() = 0;

    // @brief Stopping all addons;
    virtual void Stop() = 0;
  };

  
  /// @brief Get instance of addons core
  /// @throws in case of error
  /// @note Only one instance of addons manager can be at one time.
  /// When all smart pointers are gone addons manager deletes.
  AddonsManager::SharedPtr GetAddonsManager();

  /// @brief Get instance of addon
  /// @return instance od addon casted to specified type
  /// @throws if unable to cast addon, unable to find addon, or in casr of error
  template <class AddonClass>
  typename std::shared_ptr<AddonClass> GetAddon(AddonID id)
  {
    return std::dynamic_pointer_cast<AddonClass>(GetAddonsManager()->GetAddon(id));
  }

} // namespace Common

#endif // __COMMON_ADDONS_MANAGER_H__

