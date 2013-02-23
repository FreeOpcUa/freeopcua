/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Processor of data received from gpsd.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __GPSD_DATA_PROCESSOR_H__7a0f3676_1a5f_46f6_9705_8c4d6147226a
#define __GPSD_DATA_PROCESSOR_H__7a0f3676_1a5f_46f6_9705_8c4d6147226a

#include "gpsd.h"

#include <servers/gps/device_manager/gps_tree_updater.h>

struct gps_data_t;

namespace Gps
{
  /// @class GpsdDataProcessor.
  /// @brief Class for processing data received from gpsd deamon.
  /// Updates property tree data with received new gpsd data.
  class GpsdDataProcessor : public GpsdCallback
  {
  public:
    DEFINE_CLASS_POINTERS(GpsdDataProcessor);
  public:
    explicit GpsdDataProcessor(PropertyTreeUpdater::SharedPtr updater)
      : TreeUpdater(updater)
    {
    }

    virtual void OnGpsData(const gps_data_t& gpsData);

  public:
    virtual ~GpsdDataProcessor()
    {
    }
    
  private:
    const PropertyTreeUpdater::SharedPtr TreeUpdater;
  };
} // namespace Gps

#endif //  __GPSD_DATA_PROCESSOR_H__7a0f3676_1a5f_46f6_9705_8c4d6147226a
