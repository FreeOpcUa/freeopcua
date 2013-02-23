/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Class builder of gps property tree and supplier data for it.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef gps_data_accessor_9d7d814c_1ae2_4830_9eae_ce41ffd15ee9
#define gps_data_accessor_9d7d814c_1ae2_4830_9eae_ce41ffd15ee9

#include <common/object_id.h>
#include <common/value.h>
#include <managers/device_manager/manager.h>
#include <managers/property_tree/group.h>
#include <string>
#include <set>


namespace Gps
{
  /// @class PropertyTreeUpdater
  /// @brief builder of Property subtree where will be placed gps data
  class PropertyTreeUpdater
  {
  public:
    DEFINE_CLASS_POINTERS(PropertyTreeUpdater);

  public:
    /// @param rootGroup group where gps data subtree will be builded and where new gps data
    /// will be updated.
    explicit PropertyTreeUpdater(Gefest::Group::SharedPtr rootGroup);
    virtual ~PropertyTreeUpdater();

  public:
    void SetLatitude(const Common::Value& value);
    void SetLongitude(const Common::Value& value);
    void SetAltitude(const Common::Value& value);
    void SetGpsMode(const Common::Value& value);
    void SetIsOnline(const Common::Value& value);
    // TODO: Add more fields
    std::set<Common::ObjectID> GetObjectIDs() const;
    void GetData(DeviceManager::ObjectDataMap& data) const;

  private:
    void BuildSubTree();
    void DestroySubTree() throw();

  private:
    Gefest::Group::SharedPtr RootGroup;
    struct PropertiesHolder;
    const std::unique_ptr<PropertiesHolder> Properties;
  };

} // namespace Gps

#endif // gps_data_accessor_9d7d814c_1ae2_4830_9eae_ce41ffd15ee9
