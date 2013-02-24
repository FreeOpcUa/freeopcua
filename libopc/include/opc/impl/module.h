/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef __libopc_module_h_
#define  __libopc_module_h_

#include <com/com_types.h>
#include <com/iunknown.h>

namespace Opc
{
  Com::IUnknown* CreateInstance();
}

#endif // __libopc_module_h_

