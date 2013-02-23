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

#ifndef __ns_array_h__eca9d0d2_1679_4bac_b80c_6ff0410c33bd
#define __ns_array_h__eca9d0d2_1679_4bac_b80c_6ff0410c33bd

#include <nsCOMPtr.h>
#include <nsIArray.h>
#include <vector>

namespace XPCOM
{
  class nsStlArray : public nsIArray
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIARRAY

    explicit nsStlArray(const std::vector<PRUint32>& vec);
    explicit nsStlArray(const std::vector<nsCOMPtr<nsISupports> >& vec)
      : Array(vec)
    {
    }

  private:
    virtual ~nsStlArray()
    {
    }

  private:
    std::vector<nsCOMPtr<nsISupports> > Array;
  };
}

#endif // __ns_array_h__eca9d0d2_1679_4bac_b80c_6ff0410c33bd
