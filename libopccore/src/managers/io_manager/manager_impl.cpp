/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Io Manager implementation
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "manager_impl.h"

#include <opc/managers/io_manager/errors.h>
#include <opc/managers/io_manager/manager.h>



namespace
{
  using namespace InputOutputManager;

  typedef std::map<DeviceManager::Manager::SharedPtr, std::set<Common::ObjectID> > ObjectIdsByDeviceManagerMap;
  typedef std::map<DeviceManager::Manager::SharedPtr, DeviceManager::ObjectDataMap> DataByDeviceManagerMap;

  DeviceManager::Manager::SharedPtr FindManagerByID(const ObjectIdsByDeviceManagerMap& managers, const Common::ObjectID& id)
  {
    for (ObjectIdsByDeviceManagerMap::const_iterator managerIt = managers.begin(); managerIt != managers.end(); ++managerIt)
    {
      if (std::find(managerIt->second.begin(), managerIt->second.end(), id) != managerIt->second.end())
      {
        return managerIt->first;
      }
    }
    return DeviceManager::Manager::SharedPtr();
  }

  DataByDeviceManagerMap GetDataForRequest(const ObjectIdsByDeviceManagerMap& managersMap, const std::set<Common::ObjectID>& ids)
  {
    DataByDeviceManagerMap dataMap;
    for (std::set<Common::ObjectID>::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt)
    {
      DeviceManager::Manager::SharedPtr manager = FindManagerByID(managersMap, *idIt);
      if (!manager)
      {
        THROW_ERROR1(DeviceManagerNotRegisteredForObjectId, *idIt);
      }
      dataMap[manager][*idIt];
    }
    return dataMap;
  }
  
  template <typename RequestType>
  class IoRequestDelegate : public RequestType
  {
  public:
    explicit IoRequestDelegate(const DataByDeviceManagerMap& dataMap)
      : ManagersDataMap(dataMap)
    {
    }

  protected:
    DataByDeviceManagerMap& GetRequestData()
    {
      return ManagersDataMap;
    }

    const DataByDeviceManagerMap& GetRequestData() const
    {
      return ManagersDataMap;
    }

    DeviceManager::ObjectData& GetObjectData(Common::ObjectID id)
    {
      for (DataByDeviceManagerMap::iterator it = ManagersDataMap.begin(); it != ManagersDataMap.end(); ++it)
      {
        DeviceManager::ObjectDataMap& dataMap = it->second;
        const DeviceManager::ObjectDataMap::iterator dataIt = dataMap.find(id);
        if (dataIt != dataMap.end())
        {
          return dataIt->second;
        }
      }
      THROW_ERROR1(ObjectNotRegisteredInRequest, id);
    }

    const DeviceManager::ObjectData& GetObjectData(Common::ObjectID id) const
    {
      for (DataByDeviceManagerMap::const_iterator it = ManagersDataMap.begin(); it != ManagersDataMap.end(); ++it)
      {
        const DeviceManager::ObjectDataMap& dataMap = it->second;
        const DeviceManager::ObjectDataMap::const_iterator dataIt = dataMap.find(id);
        if (dataIt != dataMap.end())
        {
          return dataIt->second;
        }
      }
      THROW_ERROR1(ObjectNotRegisteredInRequest, id);
    }

  private:
    DataByDeviceManagerMap ManagersDataMap;
  };

  /// @brief Interface for reading data from device
  class ReadRequestImpl : public IoRequestDelegate<ReadRequest>
  {
  public:
    explicit ReadRequestImpl(const DataByDeviceManagerMap& dataMap)
      : IoRequestDelegate<ReadRequest>(dataMap)
    {
    }

    /// @brief Get data read from device
    /// @param id of object for which data should be returned
    /// @return object data that was read from device
    virtual DeviceManager::ObjectData GetData(Common::ObjectID id) const
    {
      return GetObjectData(id);
    }
    
    /// @brief Perform request
    virtual void Perform()
    {
      DataByDeviceManagerMap& dataMap = GetRequestData();
      std::for_each(dataMap.begin(), dataMap.end(), ReadData);
    }

  private:
    static void ReadData(DataByDeviceManagerMap::value_type& managerData)
    {
      managerData.first->Read(managerData.second);
    }
  };

  /// @brief Interface for writing data to device
  class WriteRequestImpl : public IoRequestDelegate<WriteRequest>
  {
  public:
    explicit WriteRequestImpl(const DataByDeviceManagerMap& dataMap)
      : IoRequestDelegate<WriteRequest>(dataMap)
    {
    }

    /// @brief Set data for object that should be written to device
    virtual void SetData(Common::ObjectID id, const Common::Value& value)
    {
      DeviceManager::ObjectData& data = GetObjectData(id);
      data.Value = value;
    }

    /// @brief Get error that was happened durin processing object data.
    virtual Common::Error GetResult(Common::ObjectID id) const
    {
      return GetObjectData(id).Error;
    }

    /// @brief Perform request
    virtual void Perform()
    {
      DataByDeviceManagerMap& dataMap = GetRequestData();
      std::for_each(dataMap.begin(), dataMap.end(), WriteData);
    }

  private:
    static void WriteData(DataByDeviceManagerMap::value_type& managerData)
    {
      managerData.first->Write(managerData.second);
    }
  };

  // Request manager realization
  class RequestManagerImpl : public RequestManager
  {
  public:
    RequestManagerImpl();
  
    virtual void RegisterDeviceManager(DeviceManager::Manager::SharedPtr manager);
    virtual void UnregisterDeviceManager(DeviceManager::Manager::SharedPtr manager);
    virtual ReadRequest::UniquePtr CreateReadRequest(const std::set<Common::ObjectID>& ids) const;
    virtual WriteRequest::UniquePtr CreateWriteRequest(const std::set<Common::ObjectID>& ids) const;

  private:
    ObjectIdsByDeviceManagerMap &GetManagers() const
    {
      return *ObjectIdsByManager;
    }

  private:
    std::shared_ptr<ObjectIdsByDeviceManagerMap> ObjectIdsByManager;
  };

  RequestManagerImpl::RequestManagerImpl()
    : ObjectIdsByManager(new ObjectIdsByDeviceManagerMap)
  {
  }

  void RequestManagerImpl::RegisterDeviceManager(DeviceManager::Manager::SharedPtr manager)
  {
    const std::set<Common::ObjectID>& ids = manager->GetObjectIDs();
    if (ids.empty())
    {
      THROW_ERROR(DeviceManagerDoesntProcessData);
    }
    GetManagers()[manager].insert(ids.begin(), ids.end());
  }

  void RequestManagerImpl::UnregisterDeviceManager(DeviceManager::Manager::SharedPtr manager)
  {
    GetManagers().erase(manager);
  }

  ReadRequest::UniquePtr RequestManagerImpl::CreateReadRequest(const std::set<Common::ObjectID>& ids) const
  {
    return ReadRequest::UniquePtr(new ReadRequestImpl(GetDataForRequest(GetManagers(), ids)));
  }

  WriteRequest::UniquePtr RequestManagerImpl::CreateWriteRequest(const std::set<Common::ObjectID>& ids) const
  {
    return WriteRequest::UniquePtr(new WriteRequestImpl(GetDataForRequest(GetManagers(), ids)));
  }
}

RequestManager::UniquePtr InputOutputManager::CreateRequestManager()
{
  return RequestManager::UniquePtr(new RequestManagerImpl());
}

