/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Registration of common managers realization
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "common_managers.h"
#include "../error_manager/register.h"
#include "../io_manager/register.h"
#include "../property_tree/manager/register.h"

void Common::RegisterCommonManagers()
{
  PropertyTree::RegisterPropertyTreeAddon();
  ErrorManager::RegisterErrorManagerAddon();
  InputOutputManager::RegisterManagerAddon();
}
