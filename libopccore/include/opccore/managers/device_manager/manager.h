/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Device manager interface
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __device_manager_h__82176679_1c3e_4512_bbf4_5657a390dbf7
#define __device_manager_h__82176679_1c3e_4512_bbf4_5657a390dbf7

#include <opccore/common/exception.h>
#include <opccore/common/class_pointers.h>
#include <opccore/common/noncopyable.h>
#include <opccore/common/object_id.h>
#include <opccore/common/value.h>
#include <map>
#include <set>

namespace DeviceManager
{
  struct ObjectData
  {
    Common::Value Value; //<! Value that read o write to device
    Common::Error Error; //<! Error that was during operation
  };

  typedef std::map<Common::ObjectID, ObjectData> ObjectDataMap;

  /// @brief Interface of device manager
  class Manager : private Common::NonCopyable
  {
  public:
    DEFINE_CLASS_POINTERS(Manager);

  public:
    /// @brief Get ids of objects which are serviced by device manager.
    /// @note Not throws.
    virtual std::set<Common::ObjectID> GetObjectIDs() const = 0;
    /// @brief Read data from device
    /// @param[in,out] data desciptor. Manager will read data for passed objctsIDs
    virtual void Read(ObjectDataMap& data) = 0;

    /// @brief Write data to device
    /// @param[in,out] data desciptor. Manager will write data. For every object will be filled error.
    virtual void Write(ObjectDataMap& data) = 0;
  };

}

#endif // __device_manager_h__82176679_1c3e_4512_bbf4_5657a390dbf7
