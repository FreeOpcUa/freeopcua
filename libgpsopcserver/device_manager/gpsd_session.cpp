/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Gpsd session wrapper.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "gpsd_session.h"

namespace Gpsd
{
  GpsSession::GpsSession()
  {
/*
    if (gps_open_r("localhost",DEFAULT_GPSD_PORT, &GpsData) == -1)
    {
      throw; // todo
    }
*/
    ClearFix();
  }

  GpsSession::GpsSession(const char* host, const char* port)
  {
/*
    if (gps_open_r(host, port, &GpsData) == -1)
    {
      throw; // todo
    }
*/
  }

  GpsSession::~GpsSession()
  {
/*
    if (gps_close(&GpsData) < 0)
    {
      // todo trace error
    }
*/
  }

  gps_data_t GpsSession::Stream(int flags)
  {
/*
    if (gps_stream(&GpsData, flags, NULL) == -1) 
    {
      throw; // todo
    }
*/
    return GpsData;
  }

  gps_data_t GpsSession::Send(const char *request)
  {
/*
    if (gps_send(&GpsData, request) == -1)
    {
      throw; //todo
    }
*/
    return GpsData;
  }

  gps_data_t GpsSession::Poll()
  {
/*
    if (gps_poll(&GpsData) < 0)
    {
      // we return null if there was a read() error or connection is cloed by gpsd
      throw; // todo;
    }
*/
    return GpsData;
  }

  bool GpsSession::IsWaiting() const
  {
    //return gps_waiting(&GpsData);
    return true;
  }

  void GpsSession::ClearFix()
  {
    //gps_clear_fix(&GpsData.fix);
  }

  void GpsSession::EnableDebug(int level, FILE *fp)
  {
    //gps_enable_debug(level, fp);
  }

} // namespace Gpsd
