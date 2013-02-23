/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Tests of nsIOPCServer interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $
#ifndef __test_ns_iopcbrowse_h_ccd8774f_3e39_41b5_acfb_8b4de03dcf6a
#define __test_ns_iopcbrowse_h_ccd8774f_3e39_41b5_acfb_8b4de03dcf6a

#include <nsStringAPI.h>
#include <vector>

class nsIOPCServer;

void Test_nsIOPCBrowse(nsIOPCServer& server);
std::vector<nsString> GetServerItemIDs(nsIOPCServer& server);

#endif // __test_ns_iopcbrowse_h_ccd8774f_3e39_41b5_acfb_8b4de03dcf6a
