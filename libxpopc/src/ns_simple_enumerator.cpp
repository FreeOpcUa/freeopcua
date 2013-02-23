#include "ns_simple_enumerator.h"

namespace
{
  class nsSimpleEnumerator : public nsISimpleEnumerator
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSISIMPLEENUMERATOR

    explicit nsSimpleEnumerator(const std::vector<nsCOMPtr<nsISupports> >& elements);

  private:
    ~nsSimpleEnumerator();

  private:
   const std::vector<nsCOMPtr<nsISupports> > Elements;
   std::vector<nsCOMPtr<nsISupports> >::const_iterator Iterator;
  };

  NS_IMPL_ISUPPORTS1(nsSimpleEnumerator, nsISimpleEnumerator)

  nsSimpleEnumerator::nsSimpleEnumerator(const std::vector<nsCOMPtr<nsISupports> >& elements)
    : Elements(elements)
  {
    Iterator = Elements.begin();
  }

  nsSimpleEnumerator::~nsSimpleEnumerator()
  {
  }

  // boolean hasMoreElements ();
  NS_IMETHODIMP nsSimpleEnumerator::HasMoreElements(PRBool *_retval NS_OUTPARAM)
  {
    NS_ENSURE_ARG(_retval);
    *_retval = Iterator != Elements.end() ? PR_TRUE : PR_FALSE;
    return NS_OK;
  }

  // nsISupports getNext ();
  NS_IMETHODIMP nsSimpleEnumerator::GetNext(nsISupports **_retval NS_OUTPARAM)
  {
    NS_ENSURE_ARG_POINTER(_retval);
    NS_ENSURE_TRUE(Iterator != Elements.end(), NS_ERROR_FAILURE);
    NS_ENSURE_STATE((*Iterator).get());
    if (Elements.empty())
    {
      *_retval = NULL;
      return NS_ERROR_FAILURE;
    }

    *_retval = Iterator->get();
    ++Iterator;
    (*_retval)->AddRef();
    return NS_OK;
  }
} // namespace

namespace XPCOM
{
  nsCOMPtr<nsISimpleEnumerator> CreateSimpleEnumerator(const std::vector<nsCOMPtr<nsISupports> >& elements)
  {
    return nsCOMPtr<nsISimpleEnumerator>(new nsSimpleEnumerator(elements));
  }
}

