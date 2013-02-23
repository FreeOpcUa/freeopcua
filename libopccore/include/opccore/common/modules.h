/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief modules identifiers. Used for building error codes.
/// Modules are parts of project not addons.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __modules_h__f01eb2ae_a824_4dc3_a0f2_bf586d241024
#define __modules_h__f01eb2ae_a824_4dc3_a0f2_bf586d241024

namespace Common
{
  enum Modules
  {
    MODULE_COMMON,
    MODULE_PROPERTY_TREE,
    MODULE_XPCOM,
    MODULE_IO_MANAGER,
    MODULE_GPS_DEVICE_IO_MANAGER,
    MODULE_ADDONS_MANAGER,
    MODULE_TEST_DEVICE_IO_MANAGER,
    MODULE_LINUX_COM,
  };
}

#endif // __modules_h__f01eb2ae_a824_4dc3_a0f2_bf586d241024

