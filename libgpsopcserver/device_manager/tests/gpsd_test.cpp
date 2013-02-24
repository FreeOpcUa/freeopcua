/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief test of Gps::Gpsd class
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>
#include <gps.h>
#include "../gpsd.h"


class GpsdTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(GpsdTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION( GpsdTestCase );

class GpsdCallbackTest : public Gps::GpsdCallback
{
public:
  GpsdCallbackTest()
    : WasNotify(false)
  {
  }

  virtual void OnGpsData(const gps_data_t& gpsData)
  {
    WasNotify = true;
    switch (gpsData.fix.mode)
    {
    case MODE_NOT_SEEN:
      std::cout << "gps mode: not seen" << std::endl;
      break;
      
    case MODE_NO_FIX:
      std::cout << "gps mode: no fix" << std::endl;
      break;
      
    case MODE_2D:
      std::cout << "gps mode: 2D" << std::endl;
      std::cout << "latitude  = " << gpsData.fix.latitude << std::endl;
      std::cout << "longitude = " << gpsData.fix.latitude << std::endl;
      break;
      
    case MODE_3D:
      std::cout << "gps mode: 3D" << std::endl;
      std::cout << "latitude  = " << gpsData.fix.latitude << std::endl;
      std::cout << "longitude = " << gpsData.fix.latitude << std::endl;
      std::cout << "altitude  = " << gpsData.fix.altitude << std::endl;
      break;

    default:
      CPPUNIT_ASSERT_MESSAGE("Invalid gps_data_t::fix_data_t::mode value", false);
    }
  }
  
  bool WaitNotify(unsigned seconds)
  {
    if (WasNotify)
    {
      WasNotify = false;
      return true;
    }
    sleep(seconds);
    return WasNotify;
  }
  
private:
  volatile bool WasNotify;
};

void GpsdTestCase::Test()
{
  Gps::Gpsd gpsd;
  const std::shared_ptr<GpsdCallbackTest> callback(new GpsdCallbackTest);
  gpsd.SetCallback(callback);
  CPPUNIT_ASSERT_MESSAGE("Callback from gpsd wasn't received in 60 seconds", callback->WaitNotify(5));
}
