/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Tests of nsIOPCSyncIO interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __ns_test_iopcsyncio_h__1d5640b7_f301_4521_ac2f_95c2167f7fa1
#define __ns_test_iopcsyncio_h__1d5640b7_f301_4521_ac2f_95c2167f7fa1

#include <nspr.h>
#include <nsStringAPI.h>
#include <vector>

class nsISupports;

void Test_nsIOPCSyncIO(nsISupports& group, const std::vector<nsString>& itemIDs);

#endif // __ns_test_iopcsyncio_h__1d5640b7_f301_4521_ac2f_95c2167f7fa1

