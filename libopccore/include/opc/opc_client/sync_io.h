/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Class for performing synchronious io operations.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __sync_io_h__f07084cd_e74d_4b8d_bea7_66895edce9f9
#define __sync_io_h__f07084cd_e74d_4b8d_bea7_66895edce9f9

#include <opc/opc_client/group.h>

#include <opc/common/exception.h>
#include <opc/common/value.h>
#include <opc/common/object_id.h>
#include <vector>

namespace OpcClient
{
  enum DataSource
  {
    SOURCE_DEVICE,
    SOURCE_CACHE,
  };

  struct ReadItemDef
  {
    /// @brief server id of required item
    Common::ObjectID ServerID;
    /// @brief Data retrieved from device or should be written to.
    Common::Value Data;
    /// @brief Result of io operation for an item.
    Common::Error ResultError;
    /// @brief Max age of data in miliseconds
    /// @todo find standart type of time.
    unsigned MaxAge;

  public:
    ReadItemDef()
      : ServerID(0)
      , MaxAge(0)
    {
    }
  };

  struct WriteItemDef
  {
    /// @brief server id of required item
    Common::ObjectID ServerID;
    /// @brief Data retrieved from device or should be written to.
    Common::Value Data;
    /// @brief Result of io operation for an item.
    Common::Error ResultError;

  public:
    WriteItemDef()
      : ServerID(0)
    {
    }
  };

  /// @brief Class that performs synchronous io operation for some group.
  /// It is create new request to IoManager when called any method.
  /// Any changes in group are not affected called method.
  class SyncIO
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(SyncIO);

  public:
    /// @brief Read data of items from cache
    virtual void ReadFromCache(std::vector<ReadItemDef>& items) = 0;
    
    /// @brief Read data of items from device
    virtual void ReadFromDevice(std::vector<ReadItemDef>& items) = 0;
    
    /// @brief Read data of items with taking in data max age.
    /// Items that contain data that younger than max age will be read from cache, other data will be read from device.
    /// @param items items data descriptor.
    /// @throws Common::Error in case of Common error. If all passed item are wrong.
    /// @note Result of reading of concrete item stores in ResulError.
    virtual void ReadMaxAge(std::vector<ReadItemDef>& items) = 0;
    
    /// @brief write data to device.
    /// @throws Common:Error in case of error. If passed items are wrong.
    virtual void Write(std::vector<WriteItemDef>& data) = 0;
  };

  SyncIO::UniquePtr CreateSyncIO(Group::SharedPtr groupInstance);
}

#endif // __sync_io_h__f07084cd_e74d_4b8d_bea7_66895edce9f9
