/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief modules identifiers. Used for building error codes.
/// Modules are parts of project not addons.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __modules_h__f01eb2ae_a824_4dc3_a0f2_bf586d241024
#define __modules_h__f01eb2ae_a824_4dc3_a0f2_bf586d241024

namespace Common
{
  enum Modules
  {
    MODULE_OS = 0,
    MODULE_COMMON,
    MODULE_ADDONS_MANAGER,
    MODULE_OPCUA_CORE,
    MODULE_OPCUA_CLIENT,
  };
}

#endif // __modules_h__f01eb2ae_a824_4dc3_a0f2_bf586d241024

