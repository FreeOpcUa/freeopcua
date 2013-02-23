/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Rrealization of nsIArray over stl container
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "ns_pruint32.h"

#include <nsISupportsPrimitives.h>

namespace
{
  class nsSupportsPRUint32 : public nsISupportsPRUint32
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISUPPORTSPRIMITIVE
    NS_DECL_NSISUPPORTSPRUINT32

    explicit nsSupportsPRUint32(PRUint32 value)
      : Value(value)
      , Type(TYPE_PRINT32)
    {
    }

    private:
      ~nsSupportsPRUint32()
    {
    }

  private:
    PRUint32 Value;
    PRUint32 Type;
  };

  NS_IMPL_ISUPPORTS2(nsSupportsPRUint32, nsISupportsPRUint32, nsISupportsPrimitive);

  // readonly attribute unsigned short type
  NS_IMETHODIMP nsSupportsPRUint32::GetType(PRUint16 *aType)
  {
    *aType = Type;
    return NS_OK;
  }

  // attribute PRUint32 data
  NS_IMETHODIMP nsSupportsPRUint32::GetData(PRUint32 *aData)
  {
    *aData = Value;
    return NS_OK;
  }

  NS_IMETHODIMP nsSupportsPRUint32::SetData(PRUint32 aData)
  {
    Value = aData;
    return NS_OK;
  }

  // string toString ()
  NS_IMETHODIMP nsSupportsPRUint32::ToString(char **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }
}

namespace XPCOM
{
  nsCOMPtr<nsISupportsPRUint32> CreateSupportsPRUint32(unsigned value)
  {
    return nsCOMPtr<nsISupportsPRUint32>(new nsSupportsPRUint32(value));
  }
}


