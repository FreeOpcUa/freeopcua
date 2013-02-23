/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Rrealization for each for multiple arrays of equal length.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __ns_for_each_h__b67386b5_8c6f_4572_820c_d532e15cc807
#define __ns_for_each_h__b67386b5_8c6f_4572_820c_d532e15cc807

#include "errors.h"

#include <nsIArray.h>


namespace XPCOM
{

  inline void  NsThrowIfFailed(nsresult res)
  {
    if ((res) != NS_OK) 
    {
      THROW_ERROR1(XPCOMError,res);
    }
  }
 
  inline void  NsThrowIfFailed(nsresult res, Common::Error error)
  {
    if ((res) != NS_OK) 
    {
      error.AddError(CREATE_ERROR1(XPCOMError, res));
      throw error;
    }
  }
  
  inline void NsThrowIfFalse(bool cond, nsresult resError)
  {
    if (!cond)
    {
      THROW_ERROR1(XPCOMError, resError);
    }
  }

  class ArrayWrapper
  {
  public:
    ArrayWrapper(nsIArray& array, const nsID& iid)
      : Array(array)
      , IID(iid)
    {
    }

    PRUint32 GetLength()
    {
      PRUint32 length = 0;
      NsThrowIfFailed(Array.GetLength(&length), CREATE_ERROR(UnableToGetArrayLength));
      return length;
    }

    void* GetElement(PRUint32 pos)
    {
      void* element = NULL;
      NsThrowIfFailed(Array.QueryElementAt(pos, IID, &element));
      NsThrowIfFalse(element != NULL, NS_ERROR_INVALID_ARG);
      return element;
    }

  private:
    nsIArray& Array;
    const nsID& IID;
  };


  template <typename Functor> 
  void ForEach(nsIArray& array, const nsID& iid, const Functor& functor)
  {
    ArrayWrapper elements(array, iid);
    const PRUint32 length = elements.GetLength();
    for (PRUint32 pos = 0; pos < length; ++pos)
    {
      try
      {
        void* element = elements.GetElement(pos);
        functor(element);
      }
      catch (const Common::Error& err)
      {
        THROW_ERROR2(FailedToProcessElementWithNumber, pos, err);
      }
    }
  }

  template <typename Functor> 
  void ForEach(nsIArray& array, const nsID& iid, nsIArray& array1, const nsID& iid1, const Functor& functor)
  {
    ArrayWrapper elements(array, iid);
    ArrayWrapper elements1(array1, iid1);
    const PRUint32 length = elements.GetLength();
    if (length != elements1.GetLength())
    {
      THROW_ERROR(ArraysLengthNotEquals);
    }

    for (PRUint32 pos = 0; pos < length; ++pos)
    {
      try
      {
        void* element = elements.GetElement(pos);
        void* element1 = elements1.GetElement(pos);
        functor(element, element1);
      }
      catch (const Common::Error& err)
      {
        THROW_ERROR2(FailedToProcessElementWithNumber, pos, err);
      }
    }
  }
} // namespace XPCOM

#endif // __ns_for_each_h__b67386b5_8c6f_4572_820c_d532e15cc807
