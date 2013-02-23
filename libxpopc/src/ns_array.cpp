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

#include "ns_array.h"

#include "ns_pruint32.h"
#include "ns_simple_enumerator.h"

#include <algorithm>
#include <nsISupportsPrimitives.h>


namespace XPCOM
{
  NS_IMPL_ISUPPORTS1(nsStlArray, nsIArray);

  nsStlArray::nsStlArray(const std::vector<PRUint32>& vector)
  {
    Array.resize(vector.size());
    std::transform(vector.begin(), vector.end(), Array.begin(), XPCOM::CreateSupportsPRUint32);
  }

  // readonly attribute unsigned long length;
  NS_IMETHODIMP nsStlArray::GetLength(PRUint32 *aLength)
  {
    NS_ENSURE_ARG_POINTER(aLength);
    *aLength = Array.size();
    return NS_OK;
  }

  // void queryElementAt (in unsigned long index, in nsIIDRef uuid, [iid_is (uuid), retval] out nsQIResult result)
  NS_IMETHODIMP nsStlArray::QueryElementAt(PRUint32 index, const nsIID & uuid, void * *result NS_OUTPARAM)
  {
    NS_ENSURE_ARG_MAX(index, Array.size());
    nsCOMPtr<nsISupports> element = Array[index];
    const nsresult res = element->QueryInterface(uuid, result);
    return res;
  }

  // unsigned long indexOf (in unsigned long startIndex, in nsISupports element)
  NS_IMETHODIMP nsStlArray::IndexOf(PRUint32 startIndex, nsISupports *element, PRUint32 *_retval NS_OUTPARAM)
  {
    NS_ENSURE_ARG_MAX(startIndex, Array.size() - 1);
    *_retval = std::distance(std::find(Array.begin() + startIndex, Array.end(), element), Array.begin());
    return NS_OK;
  }

  // nsISimpleEnumerator enumerate (); */
  NS_IMETHODIMP nsStlArray::Enumerate(nsISimpleEnumerator **_retval NS_OUTPARAM)
  {
    NS_ENSURE_ARG_POINTER(_retval);
    *_retval = XPCOM::CreateSimpleEnumerator(Array).forget().get();
    return NS_OK;
  }
}

