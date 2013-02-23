#ifndef __libopc_module_h_
#define  __libopc_module_h_

#include <com/com_types.h>
#include <com/iunknown.h>

namespace Opc
{
  Com::IUnknown* CreateInstance();
}

#endif // __libopc_module_h_

