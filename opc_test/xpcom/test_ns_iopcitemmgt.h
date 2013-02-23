/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of nsIOPCItemMgt interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __test_ns_iopcitemmgt_h__b6ec8c1a_7dc5_4a50_9a1a_118d51703c38
#define __test_ns_iopcitemmgt_h__b6ec8c1a_7dc5_4a50_9a1a_118d51703c38

#include <nsStringAPI.h>
#include <string>
#include <vector>


class nsISupports;
PRUint32 Test_nsIOPCItemMgt(nsISupports& mgt, const std::vector<nsString>& itemIDs);
void Test_nsIOPCItemMgt_RemoveItems(nsISupports& supports, PRUint32 itemHandle);

#endif // __test_ns_iopcitemmgt_h__b6ec8c1a_7dc5_4a50_9a1a_118d51703c38
