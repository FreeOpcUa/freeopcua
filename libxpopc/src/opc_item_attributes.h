/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCItemAttributesEditor interface which is extending nsIOPCItemAttributes
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __opc_item_attributes_h__4f117247_6999_436c_aa3f_f66e19bb67ef
#define __opc_item_attributes_h__4f117247_6999_436c_aa3f_f66e19bb67ef

#include <mozopc.h>
#include <nsCOMPtr.h>
#include <vector>

namespace XPCOM
{
  class nsIOPCItemAttributesEditor : public nsIOPCItemAttributes
  {
  public:
    virtual void SetAccessPath(const nsAString & aAccessPath) = 0;
    virtual void SetItemID(const nsAString& aItemID) = 0;
    virtual void SetIsActive(PRBool aIsActive) = 0;
    virtual void SetClientHandle(PRUint32 aClientHandle) = 0;
    virtual void SetServerHandle(PRUint32 aServerHandle) = 0;
    virtual void SetAccessRights(PRUint32 aAccessRights) = 0;
    virtual void SetRequestedDataType(PRUint32 aRequestedDataType) = 0;
    virtual void SetCanonicalDataType(PRUint32 aCanonicalDataType) = 0;
  };

  nsCOMPtr<nsIOPCItemAttributesEditor> CreateItemAttributes();
}

#endif //  __opc_item_attributes_h__4f117247_6999_436c_aa3f_f66e19bb67ef
