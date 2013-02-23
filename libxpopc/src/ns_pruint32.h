/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Rrealization of nsISupportsPRUint32 over stl container
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __ns_pruint32_h__d0cb0ce1_9791_4e94_bfb3_c53d87076cd4
#define __ns_pruint32_h__d0cb0ce1_9791_4e94_bfb3_c53d87076cd4

#include <nsCOMPtr.h>

class nsISupportsPRUint32;

namespace XPCOM
{
  nsCOMPtr<nsISupportsPRUint32> CreateSupportsPRUint32(unsigned value);
}

#endif // __ns_pruint32_h__d0cb0ce1_9791_4e94_bfb3_c53d87076cd4

