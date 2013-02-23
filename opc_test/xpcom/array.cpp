/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIArray over stl container
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "array.h"

#include <algorithm>
#include <nsISupportsPrimitives.h>

class nsSupportsPRUint32 : public nsISupportsPRUint32
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISUPPORTSPRIMITIVE
  NS_DECL_NSISUPPORTSPRUINT32

  nsSupportsPRUint32(PRUint32 value)
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

/* Implementation file */
NS_IMPL_ISUPPORTS2(nsSupportsPRUint32, nsISupportsPRUint32, nsISupportsPrimitive);

/* readonly attribute unsigned short type; */
NS_IMETHODIMP nsSupportsPRUint32::GetType(PRUint16 *aType)
{
  *aType = Type;
  return NS_OK;
}

/* attribute PRUint32 data; */
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

/* string toString (); */
NS_IMETHODIMP nsSupportsPRUint32::ToString(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

nsCOMPtr<nsISupports> CreateNsPRUint32(PRUint32 value)
{
  return nsCOMPtr<nsISupports>(new nsSupportsPRUint32(value));
}

NS_IMPL_ISUPPORTS1(nsStlArray, nsIArray);

nsStlArray::nsStlArray(const std::vector<PRUint32>& vector)
{
  Array.resize(vector.size());
  std::transform(vector.begin(), vector.end(), Array.begin(), CreateNsPRUint32);
}

/* readonly attribute unsigned long length; */
NS_IMETHODIMP nsStlArray::GetLength(PRUint32 *aLength)
{
  NS_ENSURE_ARG(aLength);
  *aLength = Array.size();
  return NS_OK;
}

/* void queryElementAt (in unsigned long index, in nsIIDRef uuid, [iid_is (uuid), retval] out nsQIResult result); */
NS_IMETHODIMP nsStlArray::QueryElementAt(PRUint32 index, const nsIID & uuid, void * *result NS_OUTPARAM)
{
  NS_ENSURE_ARG_MAX(index, Array.size());
  nsresult res = NS_OK;
  nsCOMPtr<nsISupports> element = Array[index];
  res = element->QueryInterface(uuid, result);
  return res;
}

/* unsigned long indexOf (in unsigned long startIndex, in nsISupports element); */
NS_IMETHODIMP nsStlArray::IndexOf(PRUint32 startIndex, nsISupports *element, PRUint32 *_retval NS_OUTPARAM)
{
  NS_ENSURE_ARG_MAX(startIndex, Array.size() - 1);
  *_retval = std::distance(std::find(Array.begin() + startIndex, Array.end(), element), Array.begin());
  return NS_OK;
}

/* nsISimpleEnumerator enumerate (); */
NS_IMETHODIMP nsStlArray::Enumerate(nsISimpleEnumerator **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
