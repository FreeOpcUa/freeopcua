/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Class for performing synchronious io operations.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opccore/opc_client/sync_io.h>

#include <opccore/common/addons_core/addon_manager.h>
#include <opccore/common/addons_core/addon.h>
#include <opccore/managers/property_tree/item.h>
#include <opccore/managers/io_manager/manager.h>
#include <opccore/opc_client/errors.h>

namespace
{
  class SyncIOImpl : public OpcClient::SyncIO
  {
  public:
    explicit SyncIOImpl(OpcClient::Group::SharedPtr group)
      : Group(group)
    {
    }

    virtual void ReadFromCache(std::vector<OpcClient::ReadItemDef>& items)
    {
      for (std::vector<OpcClient::ReadItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        try
        {
          const OpcClient::ItemAttributes& attr = Group->GetItemAttributes(itemIt->ServerID);
          itemIt->Data = attr.Item->GetValue();
        }
        catch (const Common::Error& error)
        {
          itemIt->ResultError = error;
        }
      }
    }

    virtual void ReadFromDevice(std::vector<OpcClient::ReadItemDef>& items)
    {
      if (items.empty())
      {
        THROW_ERROR(NothingToRead);
      }

      std::set<Common::ObjectID> ids;
      for (typename std::vector<OpcClient::ReadItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        try
        {
          const OpcClient::ItemAttributes& attr = Group->GetItemAttributes(itemIt->ServerID);
          ids.insert(attr.Item->GetID());
        }
        catch (const Common::Error& error)
        {
          itemIt->ResultError = error;
        }
      }

      if (ids.empty())
      {
        return;
      }

      InputOutputManager::RequestManager::SharedPtr ioManager = Common::GetAddon<InputOutputManager::RequestManager>(Common::ADDON_ID_IO_MANAGER);
      InputOutputManager::ReadRequest::UniquePtr request = ioManager->CreateReadRequest(ids);
      request->Perform();
      for (typename std::vector<OpcClient::ReadItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        if (ids.find(itemIt->ServerID) == ids.end())
        {
          continue;
        }
        const DeviceManager::ObjectData& itemData = request->GetData(itemIt->ServerID);
        itemIt->ResultError = itemData.Error;
        itemIt->Data = itemData.Value;
      }
    }

    virtual void ReadMaxAge(std::vector<OpcClient::ReadItemDef>& items)
    {
      if (items.empty())
      {
        THROW_ERROR(NothingToRead);
      }

      boost::posix_time::ptime currentTime = boost::posix_time::microsec_clock::local_time();
      std::set<Common::ObjectID> ids;
      for (typename std::vector<OpcClient::ReadItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        try
        {
          const OpcClient::ItemAttributes& attr = Group->GetItemAttributes(itemIt->ServerID);
          const Common::Value& value = attr.Item->GetValue();
          const boost::posix_time::time_duration currentAge = currentTime - value.Time;
          if (itemIt->MaxAge > currentAge.total_milliseconds())
          {
            itemIt->Data = value;
            itemIt->ResultError = Common::Error();
            continue;
          }
          ids.insert(attr.Item->GetID());
        }
        catch (const Common::Error& error)
        {
          itemIt->ResultError = error;
        }
      }

      if (ids.empty())
      {
        // if ids empty - all items in error or read from cache
        return;
      }

      InputOutputManager::RequestManager::SharedPtr ioManager = Common::GetAddon<InputOutputManager::RequestManager>(Common::ADDON_ID_IO_MANAGER);
      InputOutputManager::ReadRequest::UniquePtr request = ioManager->CreateReadRequest(ids);
      request->Perform();
      for (typename std::vector<OpcClient::ReadItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        if (ids.find(itemIt->ServerID) == ids.end())
        {
          continue;
        }
        const DeviceManager::ObjectData& itemData = request->GetData(itemIt->ServerID);
        itemIt->ResultError = itemData.Error;
        itemIt->Data = itemData.Value;
      }
    }

    virtual void Write(std::vector<OpcClient::WriteItemDef>& items)
    {
      if (items.empty())
      {
        THROW_ERROR(NothingToWrite);
      }

      std::set<Common::ObjectID> ids;
      std::map<Common::ObjectID, Common::Value> requestData;
      for (typename std::vector<OpcClient::WriteItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        try
        {
          Group->GetItemAttributes(itemIt->ServerID);
          ids.insert(itemIt->ServerID);
          requestData.insert(std::make_pair(itemIt->ServerID, itemIt->Data));
        }
        catch (const Common::Error& error)
        {
          itemIt->ResultError = error;
        }
      }

      if (ids.empty())
      {
        return;
      }

      InputOutputManager::RequestManager::SharedPtr ioManager = Common::GetAddon<InputOutputManager::RequestManager>(Common::ADDON_ID_IO_MANAGER);
      InputOutputManager::WriteRequest::UniquePtr request = ioManager->CreateWriteRequest(ids);
      for (std::map<Common::ObjectID, Common::Value>::const_iterator it = requestData.begin(); it != requestData.end(); ++it)
      {
        request->SetData(it->first, it->second);
      }

      request->Perform();      

      for (typename std::vector<OpcClient::WriteItemDef>::iterator itemIt = items.begin(); itemIt != items.end(); ++itemIt)
      {
        if (ids.find(itemIt->ServerID) == ids.end())
        {
          continue;
        }
        itemIt->ResultError = request->GetResult(itemIt->ServerID);
      }
    }

  private:
    OpcClient::Group::SharedPtr Group;
  };
}

OpcClient::SyncIO::UniquePtr OpcClient::CreateSyncIO(OpcClient::Group::SharedPtr groupInstance)
{
  return SyncIO::UniquePtr(new SyncIOImpl(groupInstance));
}

