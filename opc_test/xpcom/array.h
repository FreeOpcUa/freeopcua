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

#ifndef __array_h__8fd24c3a_ef49_42b5_a6c8_2b3c2c0b3dee
#define __array_h__8fd24c3a_ef49_42b5_a6c8_2b3c2c0b3dee

#include <nsCOMPtr.h>
#include <nsIArray.h>
#include <vector>


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


#endif // __array_h__8fd24c3a_ef49_42b5_a6c8_2b3c2c0b3dee
