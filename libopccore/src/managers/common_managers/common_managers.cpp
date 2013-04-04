/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Registration of common managers realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common_managers.h"
#include "../error_manager/register.h"
#include "../io_manager/register.h"
#include "../property_tree/manager/register.h"

void Common::RegisterCommonManagers()
{
  InputOutputManager::RegisterManagerAddon();
}
