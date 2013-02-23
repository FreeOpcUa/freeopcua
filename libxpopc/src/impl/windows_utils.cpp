// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#if defined(_WIN32) || defined(_WIN64)

#include "windows_utils.h"
#include <windows.h>

std::string Windows::GetLastErrorString() 
{ 
  char* lpMsgBuf;
  DWORD dw = GetLastError(); 

  FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR) &lpMsgBuf,
      0, NULL );

  const std::string msg = lpMsgBuf;
  LocalFree(lpMsgBuf);
  return msg;
}

void Windows::ShowLastErrorMessage(const char* caption)
{
  MessageBoxA(GetDesktopWindow(), caption, GetLastErrorString().c_str(), 0);
}

#endif // defined(_WIN32) || defined(_WIN64)
