// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZOPC_COM_BASE_H__
#define __MOZOPC_COM_BASE_H__

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <objbase.h>

namespace COM
{
  template<class BaseInterface>
  class IUnknownImpl : public BaseInterface
  {
  public:
    explicit IUnknownImpl(IUnknown* owner = NULL);

  public: // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR   *ppvObj);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);
  
   protected:
     virtual ~IUnknownImpl();
  
  private:
    LONG RefCount;
    IUnknown* Owner;
  };


  template<class BaseInterface>
  IUnknownImpl<BaseInterface>::IUnknownImpl(IUnknown* owner)
    : Owner(owner)
    , RefCount(0)
  {
  }

  template<class BaseInterface>
  IUnknownImpl<BaseInterface>::~IUnknownImpl()
  {
  }

  template<class BaseInterface>
  HRESULT STDMETHODCALLTYPE IUnknownImpl<BaseInterface>::QueryInterface(REFIID riid, LPVOID *result)
  {
    *result = NULL;

    if (IsEqualGUID(riid, __uuidof(IUnknown)))
    {
      if (Owner != NULL)
      {
        return Owner->QueryInterface(riid, result);
      }
      *result = static_cast<IUnknown*>(this);
    }
    
    if (*result != NULL)
    {
      AddRef();
    }
    return *result ? S_OK : E_NOINTERFACE;
  }

  template<class BaseInterface>
  ULONG STDMETHODCALLTYPE IUnknownImpl<BaseInterface>::AddRef(void)
  {
    return InterlockedIncrement(&RefCount);
  }

  template<class BaseInterface>
  ULONG STDMETHODCALLTYPE IUnknownImpl<BaseInterface>::Release(void)
  {
    ULONG refCount = InterlockedDecrement(&RefCount);
    if (refCount == 0)
    {
      delete this;
      return 0;
    }
    return refCount;
  }
}

#endif // _WIN32 || _WIN64

#endif // __MOZOPC_COM_BASE_H__
