// Author: Alexander Rykovanov 2009
//
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

HINSTANCE hInstance = NULL;

HINSTANCE GetModuleHinstance()
{
  return hInstance;  
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
  {
    hInstance = hinstDLL;    
  }
  return TRUE;
}

#endif // _WIN32 || _WIN64
