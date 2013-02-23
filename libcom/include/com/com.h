/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief API for creating com classes. This api exported from dlls.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef _com_h_c728b228_07a2_4d5b_acc2_5669104f2ac7
#define _com_h_c728b228_07a2_4d5b_acc2_5669104f2ac7

#include <com/com_errors.h>
#include <com/com_smart_ptr.h>
#include <com/com_types.h>

namespace Com
{

  class IUnknown;

  extern "C" HRESULT CoCreateInstance(const char* clsid, IUnknown** result);

  typedef HRESULT (*CoCreateInstanceFunc)(const char* clsid, IUnknown** result);

} // namespace Com

#endif // _com_h_c728b228_07a2_4d5b_acc2_5669104f2ac7

