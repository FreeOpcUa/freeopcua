/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCBrowseElement
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $
#ifndef __opc_browse_element_h__67c8427d_5c6a_4a16_ad2f_267dbc708472
#define __opc_browse_element_h__67c8427d_5c6a_4a16_ad2f_267dbc708472

#include <mozopc.h>
#include <opc_client/server_browser.h>

namespace XPCOM
{
  class nsOPCBrowseElement : public nsIOPCBrowseElement
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCBROWSEELEMENT

    explicit nsOPCBrowseElement(const OpcClient::BrowseElement& element);

  private:
    virtual ~nsOPCBrowseElement();

  private:
    const OpcClient::BrowseElement Element;
  };
}

#endif // __opc_browse_element_h__67c8427d_5c6a_4a16_ad2f_267dbc708472
