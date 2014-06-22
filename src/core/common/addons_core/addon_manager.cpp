/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Addons manager realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/addons_core/errors.h>
#include <opc/common/exception.h>

#include <boost/thread/mutex.hpp>
#include <map>


namespace
{
  struct AddonData
  {
    Common::AddonID ID;
    Common::AddonFactory::SharedPtr Factory;
    std::vector<Common::AddonID> Dependencies;
    Common::AddonParameters Parameters;
    Common::Addon::SharedPtr Addon;

    AddonData(const Common::AddonInformation& configuration)
      : ID(configuration.ID)
      , Factory(configuration.Factory)
      , Dependencies(configuration.Dependencies)
      , Parameters(configuration.Parameters)
    {
    }
  };

  bool IsAddonNotStarted(const std::pair<Common::AddonID, AddonData>& addonData)
  {
    return addonData.second.Addon == std::shared_ptr<Common::Addon>();
  }

  void StopAddon(const std::pair<Common::AddonID, AddonData>& addonPair)
  {
    if (!addonPair.second.Addon)
    {
      return;
    }
    try
    {
      std::clog << "Stopping addon '" << addonPair.second.ID << "'" <<  std::endl;
      addonPair.second.Addon->Stop();
      std::clog << "Addon '" << addonPair.second.ID << "' successfuly stopped." <<  std::endl;
    }
    catch (const std::exception& exc)
    {
      std::cerr << "Failed to stop addon '" << addonPair.second.ID << "': " << exc.what() <<  std::endl;
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

    virtual void Register(const Common::AddonInformation& addonConfiguration)
    {
      // TODO lock manager
      if (ManagerStarted && !addonConfiguration.Dependencies.empty())
      {
        THROW_ERROR1(UnableToRegisterAddonWhenStarted, addonConfiguration.ID);
      }

      EnsureAddonNotRegistered(addonConfiguration.ID);
      Addons.insert(std::make_pair(addonConfiguration.ID, AddonData(addonConfiguration)));
      if (ManagerStarted)
      {
        DoStart();
      }
    }

    virtual void Unregister(const Common::AddonID& id)
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

    virtual Common::Addon::SharedPtr GetAddon(const Common::AddonID& id) const
    {
      // TODO lock manager
      EnsureAddonRegistered(id);
      EnsureAddonInitialized(id);
      return Addons.find(id)->second.Addon;
    }

    virtual void Start()
    {
      if (ManagerStarted)
      {
        THROW_ERROR(AddonsManagerAlreadyStarted);
      }
      // TODO lock manager
      if (!DoStart())
      {
        StopAddons();
        THROW_ERROR(FailedToStartAddons);
      }
      ManagerStarted = true;
    }

    virtual void Stop()
    {
      if (!ManagerStarted)
      {
        THROW_ERROR(AddonsManagerAlreadyStopped);
      }

      StopAddons();
      ManagerStarted = false;
    }
  private:
    void StopAddons()
    {
      // TODO lock manager
      if (!Addons.empty())
      {
        std::for_each(Addons.begin(), Addons.end(), StopAddon);
        Addons.clear();
      }
    }

    bool DoStart()
    {
      while (AddonData* addonData = GetNextAddonDataForStart())
      {
        std::cout << "Creating addon '" << addonData->ID << "'" <<  std::endl;
        Common::Addon::SharedPtr addon = addonData->Factory->CreateAddon();
        std::cout << "Initializing addon '" << addonData->ID << "'" <<  std::endl;
        try
        {
          addon->Initialize(*this, addonData->Parameters);
          std::cout << "Addon '" << addonData->ID << "' successfully initialized." <<  std::endl;
        }
        catch (const std::exception& exc)
        {
          std::cerr << "Failed to initialize addon '" << addonData->ID << "': "<< exc.what() <<  std::endl;
          return false;
        }
        addonData->Addon = addon;
      }
      EnsureAllAddonsStarted();
      return true;
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

Common::AddonsManager::UniquePtr Common::CreateAddonsManager()
{
  return AddonsManager::UniquePtr(new AddonsManagerImpl());
}

