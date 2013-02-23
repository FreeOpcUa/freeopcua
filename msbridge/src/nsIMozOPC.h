// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_NS_IMOZOPC_H__
#define __MOZ_NS_IMOZOPC_H__

#include "msopcbridge.h"

namespace MsOpcBridge
{
  class nsMsOpc : public nsIMsOpc
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIMSOPC

    nsMsOpc(nsISupports* outer);

  private:
    ~nsMsOpc();

  private:
    nsISupports* Outer;
  };
} // namespace MsBridge

#endif // __MOZ_NS_IMOZOPC_H__
