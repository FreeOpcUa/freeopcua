// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZOPC_NSAGGREGABLE_SUPPORTS__IMPL_H__
#define __MOZOPC_NSAGGREGABLE_SUPPORTS__IMPL_H__


#if (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#include <nsISupports.h>

namespace XPCOM
{
  // Owner should delete aggregable object directly via destructor
  template <class BaseInterface>
  class nsAggregableSupportsImpl : public BaseInterface
  {
    NS_DECL_ISUPPORTS

  protected:
    typedef BaseInterface BaseInterfaceType;
    typedef nsAggregableSupportsImpl<BaseInterfaceType> BaseType;

  private:
    typedef BaseType SelfType;

  public:
    explicit nsAggregableSupportsImpl(nsISupports* owner)
      : Owner(owner)
      , mRefCnt(1)
    {
    }

    virtual ~nsAggregableSupportsImpl()
    {
    }

  private:
    explicit nsAggregableSupportsImpl(const SelfType&);
    SelfType operator=(const SelfType&);

  private:
    nsISupports* Owner;
  };

  template <class BaseInterface>
  NS_IMETHODIMP_(nsrefcnt) nsAggregableSupportsImpl<BaseInterface>::AddRef()
  {
    NS_PRECONDITION(PRInt32(mRefCnt) >= 0, "Illegal RefCnt");
    NS_ASSERT_OWNINGTHREAD(SelfType);
    const nsrefcnt refcnt = ++mRefCnt; //TODO Make it threadsafe?
    NS_LOG_ADDREF(this, refcnt, __FUNCTION__, sizeof(*this));
    return refcnt;
  }

  template <class BaseInterface>
  NS_IMETHODIMP_(nsrefcnt) nsAggregableSupportsImpl<BaseInterface>::Release()
  {
    NS_PRECONDITION(PRInt32(mRefCnt) != 0, "dup release");
    NS_ASSERT_OWNINGTHREAD(SelfType);
    const nsrefcnt refcnt = --mRefCnt; //TODO Make it threadsafe?
    NS_LOG_RELEASE(this, refcnt, __FUNCTION__);
    if (refcnt == 0)
    {
      mRefCnt = 1;
      delete this;
    }
    return refcnt;
  }

  template <class BaseInterface>
  NS_IMETHODIMP nsAggregableSupportsImpl<BaseInterface>::QueryInterface(const nsIID& uuid, void** result)
  {
    *result = NULL;
    if (uuid.Equals(NS_GET_IID(nsISupports)) == PR_TRUE)
    {
      return Owner->QueryInterface(uuid, result);
    }
    else if (uuid.Equals(NS_GET_TEMPLATE_IID(BaseInterface)) == PR_TRUE)
    {
      *result = reinterpret_cast<BaseInterface*>(this);
    }

    if (*result != NULL)
    {
      Owner->AddRef();
      return NS_OK;
    }
    return NS_NOINTERFACE;
  }
}

#endif // (defined(_WIN32) && !defined(_WIN64)) || defined(__linux__)

#endif // __MOZOPC_NSAGGREGABLE_SUPPORTS__IMPL_H__
