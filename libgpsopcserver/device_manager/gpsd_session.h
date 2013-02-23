/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Gpsd session wrapper.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef _GPSD_SESSION_H_178931bb_c941_488c_958d_c319bd5a225c
#define _GPSD_SESSION_H_178931bb_c941_488c_958d_c319bd5a225c

#include <gps.h> // gpsd header

namespace Gpsd
{
  class GpsSession
  {
	public:
    /// @brief Open session to the local gpsd deamon
    /// @throws std::exception if failed to connect
    GpsSession();
    /// @brief Open session to the remote gpsd deamon
    /// @param host ip or host name where gpsd is running
    explicit GpsSession(const char* host, const char* port = 0);
    virtual ~GpsSession();

  public:
    /// @brief put a command to gpsd and return the updated struct
    /// @throws std::exception if connection failed
    gps_data_t Send(const char *request); 

    /// @brief set watcher and policy flags
    gps_data_t Stream(int); 

    /// @brief block until gpsd returns new data, then return the updated struct
    gps_data_t Poll(void); 

    /// @brief nonblocking check for data waitin
    /// @throws std::exception if connection failed
    bool IsWaiting(void) const;

    /// @throws not throws
    void ClearFix(void);

    /// @throws Not throws
    void EnableDebug(int, FILE*);

  private:
    GpsSession(const GpsSession&);
    GpsSession& operator=(const GpsSession&);

  private:
    mutable gps_data_t GpsData;
  };
} // namespace Gpsd

#endif // _GPSD_SESSION_H_178931bb_c941_488c_958d_c319bd5a225c
