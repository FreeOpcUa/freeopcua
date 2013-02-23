/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Declaration factory function of nsIOPCBrowse
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __opc_browse_h_f1d0ef1b_35aa_4a44_94f0_d3c6edfa32b2
#define __opc_browse_h_f1d0ef1b_35aa_4a44_94f0_d3c6edfa32b2

#include <mozopc.h>
#include <nsCOMPtr.h>

namespace XPCOM
{
  nsCOMPtr<nsIOPCBrowse> CreateOpcBrowse();
}

#endif // __opc_browse_h_f1d0ef1b_35aa_4a44_94f0_d3c6edfa32b2

