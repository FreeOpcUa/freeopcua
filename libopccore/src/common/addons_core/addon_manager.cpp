/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addons manager realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/common/addons_core/addon.h>
#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/addons_core/errors.h>
#include <opccore/common/exception.h>

#include <map>
#include <boost/thread/mutex.hpp>


namespace
{
  struct AddonData
  {
    Common::AddonFactory::SharedPtr Factory;
    std::vector<Common::AddonID> Dependencies;
    Common::Addon::SharedPtr Addon;

    AddonData(Common::AddonFactory::UniquePtr factory, const std::vector<Common::AddonID> dependencies)
      : Factory(std::move(factory))
      , Dependencies(dependencies)
    {
    }
  };

  bool IsAddonNotStarted(const std::pair<Common::AddonID, AddonData>& addonData)
  {
    return addonData.second.Addon == std::shared_ptr<Common::Addon>();
  }

  void StopAddon(const std::pair<Common::AddonID, AddonData>& addonPair)
  {
    if (addonPair.second.Addon)
    {
      addonPair.second.Addon->Stop();
    }
  }


  class AddonsManagerImpl : public Common::AddonsManager
  {
    typedef std::map<Common::AddonID, AddonData> AddonList;

  public:
    AddonsManagerImpl()
      : ManagerStarted(false)
    {
    }

    virtual ~AddonsManagerImpl()
    {
      try
      {
        Stop();
      }
      catch(const Common::Error& err)
      {
        // TODO: log error
      }
      catch (...)
      {
        // TODO: log error
      }
    }

    virtual void Register(Common::AddonID id, Common::AddonFactory::UniquePtr factory, const std::vector<Common::AddonID>& dependencies)
    {
      // TODO lock manager
      if (ManagerStarted && !dependencies.empty())
      {
        THROW_ERROR1(UnableToRegisterAddonWhenStarted, id);
      }

      EnsureAddonNotRegistered(id);
      Addons.insert(std::make_pair(id, AddonData(std::move(factory), dependencies)));
      if (ManagerStarted)
      {
        DoStart(Common::AddonsConfiguration());
      }
    }

    virtual void Unregister(Common::AddonID id)
    {
      // TODO lock manager
      EnsureAddonRegistered(id);
      AddonData& addonData = Addons.find(id)->second;
      if (addonData.Addon)
      {
        addonData.Addon->Stop();
      }
      Addons.erase(id);
    }

    virtual Common::Addon::SharedPtr GetAddon(Common::AddonID id) const
    {
      // TODO lock manager
      EnsureAddonRegistered(id);
      EnsureAddonInitialized(id);
      return Addons.find(id)->second.Addon;
    }

    virtual void Start(const Common::AddonsConfiguration& configuration)
    {
      if (ManagerStarted)
      {
        THROW_ERROR(AddonsManagerAlreadyStarted);
      }
      // TODO lock manager
      DoStart(configuration);
      ManagerStarted = true;
    }

    virtual void Stop()
    {
      if (!ManagerStarted)
      {
        THROW_ERROR(AddonsManagerAlreadyStopped);
      }

      // TODO lock manager
      if (!Addons.empty())
      {
        std::for_each(Addons.begin(), Addons.end(), StopAddon);
        Addons.clear();
      }

      ManagerStarted = false;
    }
  private:
    void DoStart(const Common::AddonsConfiguration& configuration)
    {
      for (Common::AddonInitilizersList::const_iterator addonIt = configuration.StaticAddonsInitializers.begin(); addonIt != configuration.StaticAddonsInitializers.end(); ++addonIt)
      {
        Common::AddonInitializerFunc initializeAddon = *addonIt;
        initializeAddon(*this);
      }

      while (AddonData* addonData = GetNextAddonDataForStart())
      {
        Common::Addon::SharedPtr addon = addonData->Factory->CreateAddon();
        addonData->Addon = addon;
        addon->Initialize();
      }
      EnsureAllAddonsStarted();
   }

   AddonData* GetNextAddonDataForStart()
   {
     for (AddonList::iterator it = Addons.begin(); it != Addons.end(); ++it)
     {
       if (!IsAddonStarted(it->second) && IsAllAddonsStarted(it->second.Dependencies))
       {
         return &it->second;
       }
     }
     return 0;
   }

   bool IsAddonStarted(const AddonData& addonData) const
   {
     return static_cast<bool>(addonData.Addon);
   }

   bool IsAllAddonsStarted(const std::vector<Common::AddonID> ids) const
   {
     for (std::vector<Common::AddonID>::const_iterator it = ids.begin(); it != ids.end(); ++it)
     {
       const AddonList::const_iterator addonIt = Addons.find(*it);
       if (addonIt == Addons.end())
       {
         THROW_ERROR1(AddonNotFound, *it);
       }

       if (!IsAddonStarted(addonIt->second))
       {
         return false;
       }
     }
     return true;
   }

   void EnsureAddonInitialized(Common::AddonID id) const
   {
     if (!Addons.find(id)->second.Addon)
     {
       THROW_ERROR1(AddonNotInitializedYet, id);
     }
   }

   void EnsureAddonRegistered(Common::AddonID id) const
   {
     if (!IsAddonRegistered(id))
     {
       THROW_ERROR1(AddonNotRegistered, id);
     }
   }
  
   void EnsureAddonNotRegistered(Common::AddonID id) const
   {
     if (IsAddonRegistered(id))
     {
       THROW_ERROR1(AddonRegisteredButShouldnt, id);
     }
   }

   bool IsAddonRegistered(Common::AddonID id) const
   {
     return Addons.find(id) != Addons.end();
   }

   void EnsureAllAddonsStarted() const
   {
     AddonList::const_iterator addonIt = std::find_if(Addons.begin(), Addons.end(), IsAddonNotStarted);
     if (!Addons.empty() && addonIt != Addons.end())
     {
       THROW_ERROR1(AddonIsNotStarted, addonIt->first);
     }
   }

  private:
    AddonList Addons;
    bool ManagerStarted;
  };
}

Common::AddonsManager::SharedPtr Common::GetAddonsManager()
{
  static boost::mutex mutex;
  boost::mutex::scoped_lock lock(mutex);
  static AddonsManager::WeakPtr manager;
  AddonsManager::SharedPtr managerInstance = manager.lock();
  if (managerInstance)
  {
    return managerInstance;
  }
  
  managerInstance = AddonsManager::SharedPtr(new AddonsManagerImpl());
  manager = managerInstance;
  return managerInstance;
}

Common::AddonsManager::UniquePtr Common::CreateAddonsManager()
{
  return AddonsManager::UniquePtr(new AddonsManagerImpl());
}

