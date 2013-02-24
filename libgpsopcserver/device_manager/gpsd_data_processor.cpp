/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Processor of data received from gpsd.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "gpsd_data_processor.h"
#include "gps_tree_updater.h"
#include <common/value.h>
#include <gps.h>
#include <string>

namespace
{
  std::string GetGpsMode(unsigned mode)
  {
    switch(mode)
    {
    case MODE_NOT_SEEN:
      return "not seen";
    case MODE_NO_FIX:
      return "no fix";
    case MODE_2D:
      return "2d";
    case MODE_3D:
      return "3d";
    default:
      return "unknown";
    }
  }
}

void Gps::GpsdDataProcessor::OnGpsData(const gps_data_t& gpsData)
{
  boost::posix_time::ptime t(boost::posix_time::min_date_time);
  t += boost::posix_time::seconds(gpsData.fix.time);
  TreeUpdater->SetIsOnline(Common::Value(gpsData.online, t));
  TreeUpdater->SetGpsMode(Common::Value(GetGpsMode(gpsData.fix.mode), t));
  if (gpsData.fix.mode >= MODE_2D)
  {
    TreeUpdater->SetLatitude(Common::Value(gpsData.fix.latitude, t));
    TreeUpdater->SetLongitude(Common::Value(gpsData.fix.longitude, t));
  }
  if (gpsData.fix.mode == MODE_3D)
  {
    TreeUpdater->SetAltitude(Common::Value(gpsData.fix.altitude, t));
  }
}
