/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Register Error Manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef register_h_79d7e2ad_2cb4_4ed4_97e4_5fdc1d25e87e
#define register_h_79d7e2ad_2cb4_4ed4_97e4_5fdc1d25e87e

#include <opc/common/addons_core/addon.h>

namespace InputOutputManager
{

  /// @brief create factory of io manager
  Common::AddonFactory::UniquePtr CreateAddonFactory();

}

#endif // register_h_79d7e2ad_2cb4_4ed4_97e4_5fdc1d25e87e

