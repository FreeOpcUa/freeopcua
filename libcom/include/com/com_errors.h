/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Com errors.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __com_errors_h_8f4fe616_f4e5_4d86_abf8_024235794afc
#define __com_errors_h_8f4fe616_f4e5_4d86_abf8_024235794afc

#include <com/com_types.h>

namespace Com
{

  const HRESULT COM_SUCCESS             = 0;
  const HRESULT COM_NOINTERFACE         = 1; // object doesn't support requested interface.
  const HRESULT COM_NOT_IMPLEMENTED     = 3; // there is a dev task for this functionality.
  const HRESULT COM_INTERNAL_ERROR      = 4; // internal error not depend on user actions.
  const HRESULT COM_UNHANDLED_EXCEPTION = 5;
  const HRESULT COM_UNKNOWN_CLASS       = 6;
  const HRESULT COM_ERROR               = 7; // user error.
  
  const HRESULT COM_INVALID_PARAMETERS = 40;
  const HRESULT COM_INVALID_PARAMETER1 = 41;
  const HRESULT COM_INVALID_PARAMETER2 = 42;
  const HRESULT COM_INVALID_PARAMETER3 = 43;
  const HRESULT COM_INVALID_PARAMETER4 = 44;
  const HRESULT COM_INVALID_PARAMETER5 = 45;
  const HRESULT COM_INVALID_PARAMETER6 = 46;
  const HRESULT COM_INVALID_PARAMETER7 = 47;
  const HRESULT COM_INVALID_PARAMETER8 = 48;
  const HRESULT COM_INVALID_PARAMETER9 = 48;
  const HRESULT COM_INVALID_PARAMETER10_PLUS = 49;

  const HRESULT COM_UNABLE_OPEN_REGISTRY = 100;
  const HRESULT COM_CLASS_REGISTERED_MULTIPLE_TIMES = 101;
  const HRESULT COM_CLASS_NOT_REGISTERED = 102;
  const HRESULT COM_CLASS_ALREADY_REGISTERED = 103;

} // nmespace Com

#endif // __com_errors_h_8f4fe616_f4e5_4d86_abf8_024235794afc

