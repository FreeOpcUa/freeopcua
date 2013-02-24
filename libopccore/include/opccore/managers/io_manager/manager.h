/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Interface of Io Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __io_manaher_h_e5915eba_d836_44ed_941b_439c21be598f
#define __io_manaher_h_e5915eba_d836_44ed_941b_439c21be598f

#include <opccore/managers/device_manager/manager.h>

#include <opccore/common/exception.h>
#include <opccore/common/noncopyable.h>
#include <opccore/common/object_id.h>
#include <opccore/common/value.h>
#include <memory>
#include <set>

namespace DeviceManager
{

  class Manager;

}


namespace InputOutputManager
{

  /// @brief Base interface of io request
  /// @note eveery request should be synchronious
  class IoRequest : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(IoRequest);

  public:
    /// @brief Perform request
    virtual void Perform() = 0;
  };

  /// @brief Interface for reading data from device
  class ReadRequest : public IoRequest
  {
  public:
    DEFINE_CLASS_POINTERS(ReadRequest);

  public:
    /// @brief Get data read from device
    /// @param id of object for which data should be returned
    /// @return object data that was read from device
    virtual DeviceManager::ObjectData GetData(Common::ObjectID id) const = 0;
  };

  /// @brief Interface for writing data to device
  class WriteRequest 
    : public IoRequest
  {
  public:
    DEFINE_CLASS_POINTERS(WriteRequest);

  public:
    /// @brief Set data for object that should be written to device
    virtual void SetData(Common::ObjectID id, const Common::Value& value) = 0;
    /// @brief Get error that was happened durin processing object data.
    virtual Common::Error GetResult(Common::ObjectID id) const = 0;
  };

  /// @brief Dispatcher of io requests.
  /// @note Returned requests should be reusable - one request cat be performed multiple trimes
  class RequestManager
    : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(RequestManager);

  public:
    /// @brief Register device manager that will process io operations for objects
    virtual void RegisterDeviceManager(DeviceManager::Manager::SharedPtr manager) = 0;
    /// @brief Unreister device manager
    virtual void UnregisterDeviceManager(DeviceManager::Manager::SharedPtr manager) = 0;
    /// @brief Create request for processing objects reading operations
    virtual ReadRequest::UniquePtr CreateReadRequest(const std::set<Common::ObjectID>& ids) const = 0;
    /// @brief Create request for processing writing operations
    virtual WriteRequest::UniquePtr CreateWriteRequest(const std::set<Common::ObjectID>& ids) const = 0;
  };

} // namespace InputOutputManager

#endif // __io_manaher_h_e5915eba_d836_44ed_941b_439c21be598f

