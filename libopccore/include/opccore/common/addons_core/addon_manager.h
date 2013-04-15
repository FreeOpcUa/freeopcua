/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addons manager interface definition
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef __COMMON_ADDONS_MANAGER_H__
#define __COMMON_ADDONS_MANAGER_H__

#include <opccore/common/class_pointers.h>
#include <opccore/common/noncopyable.h>
#include <string>
#include <vector>

namespace Common
{
  typedef std::string AddonID;

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
    virtual void Register(const AddonID& id, std::unique_ptr<AddonFactory> factory, const std::vector<AddonID>& dependencies = std::vector<AddonID>()) = 0;

    /// @brief unregister addon
    /// @param id id of unregistering addon
    /// @throws if addon not found
    /// For unregistering addon will be called method Addon::Stop() and addon data will be removed.
    virtual void Unregister(const AddonID& id) = 0;

    /// @brief getting addon by id
    /// @param id id of the required addon
    /// @return addon instance
    /// @throws if addon is not registered or not initialized yet.
    virtual std::shared_ptr<Addon> GetAddon(const AddonID& id) const = 0;

    /// @brief starting work.
    /// creates all addons and initializes them.
    /// @throws if not all addons dependencies can be resolved.
    virtual void Start(/*const AddonsConfiguration& config*/) = 0;

    // @brief Stopping all addons;
    virtual void Stop() = 0;
  };

  
  /// @brief Get instance of addons core
  /// @throws in case of error
  /// @note Only one instance of addons manager can be at one time.
  /// When all smart pointers are gone addons manager deletes.
  AddonsManager::UniquePtr CreateAddonsManager();

  /// @brief Get instance of addon
  /// @return instance od addon casted to specified type
  /// @throws if unable to cast addon, unable to find addon, or in casr of error
  template <class AddonClass>
  typename std::shared_ptr<AddonClass> GetAddon(AddonsManager& addons, const AddonID& id)
  {
    return std::dynamic_pointer_cast<AddonClass>(addons.GetAddon(id));
  }

  template <class AddonClass>
  typename std::shared_ptr<AddonClass> GetAddon(AddonsManager& addons, const char* id)
  {
    return std::dynamic_pointer_cast<AddonClass>(addons.GetAddon(id));
  }


} // namespace Common

#endif // __COMMON_ADDONS_MANAGER_H__

