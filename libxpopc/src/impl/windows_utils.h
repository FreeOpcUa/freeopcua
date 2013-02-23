// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZOPC_WINDOWS_UTILS_H__
#define __MOZOPC_WINDOWS_UTILS_H__

#if defined(_WIN32) || defined(_WIN64)

#include <string>

namespace Windows
{
  // retrieves message for corresponding GetLastError()
  std::string GetLastErrorString();

  // MessageBox with last error message
  void ShowLastErrorMessage(const char* caption);
}

#endif // defined(_WIN32) || defined(_WIN64)

#endif // __MOZOPC_WINDOWS_UTILS_H__