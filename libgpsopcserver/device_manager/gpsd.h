/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Class connects to gpsd and retrieves data from it.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef gpsd_h_e243f5bc_6b8e_4f1f_83ef_d86592f7c08f
#define gpsd_h_e243f5bc_6b8e_4f1f_83ef_d86592f7c08f

#include <common/class_pointers.h>
#include <string>

struct gps_data_t;

namespace Gps
{
  class GpsdCallback
  {
  public:
   DEFINE_CLASS_POINTERS(GpsdCallback);

  public:
    virtual void OnGpsData(const gps_data_t& gpsData) = 0;
  
  public:
    virtual ~GpsdCallback()
    {
    }
  };


  /// @class Gpsd 
  /// @brief Accessor to gpsd deamon.
  /// Connects to gpsd deamon, reads data from it in separate thread.
  class Gpsd
  {
  public:
    DEFINE_CLASS_POINTERS(Gpsd);

  public:
    /// @param hostName host which willbe connected to.
    /// @param port port number at host
    Gpsd(const std::string& hostName = std::string(), unsigned port = -1);
    virtual ~Gpsd();
    void SetCallback(GpsdCallback::SharedPtr callback);
    GpsdCallback::SharedPtr GetCallback() const;

  private:
    class GpsdThreadStarter;
    std::unique_ptr<GpsdThreadStarter> ThreadStarter;
    GpsdCallback::SharedPtr Callback;
  };
}

#endif // gpsd_h_e243f5bc_6b8e_4f1f_83ef_d86592f7c08f
