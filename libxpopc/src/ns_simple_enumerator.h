/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsISimpleEnumerator
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __ns_simple_enumerator_h__a610991f_4562_416b_ac83_0df792648b67
#define __ns_simple_enumerator_h__a610991f_4562_416b_ac83_0df792648b67

#include <nsCOMPtr.h>
#include <nsISimpleEnumerator.h>
#include <vector>

namespace XPCOM
{
  nsCOMPtr<nsISimpleEnumerator> CreateSimpleEnumerator(const std::vector<nsCOMPtr<nsISupports> >& elements);
} // namespace XPCOM

#endif // __ns_simple_enumerator_h__a610991f_4562_416b_ac83_0df792648b67
