/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Declaration of external functions that should be implemented for every opc server. It supplies registration information of the CoClass.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __aggregate_h_34f5b5f1_d19b_4c39_b825_8be357c26302
#define __aggregate_h_34f5b5f1_d19b_4c39_b825_8be357c26302

#include <nsError.h>
#include <nsISupports.h>
#include <vector>

namespace XPCOM
{
  template <typename InterfaceType>
  class nsAggregator : public InterfaceType
  {
  public:
    NS_DECL_ISUPPORTS

    nsAggregator()
    {
    }

    virtual ~nsAggregator()
    {
    }

    void AddChild(nsISupports& aggregable)
    {
      Aggregated.push_back(&aggregable);
    }

  private:
    std::vector<nsISupports*> Aggregated;
  };

  template <class InterfaceType>
  NS_IMPL_ADDREF(nsAggregator<InterfaceType>);

  template <class InterfaceType>
  NS_IMPL_RELEASE(nsAggregator<InterfaceType>);

  template <class InterfaceType>
  nsresult nsAggregator<InterfaceType>::QueryInterface(const nsIID& id, void** result)
  {
    *result = NULL;
    if (id.Equals(NS_GET_IID(nsISupports)))
    {
      *result = static_cast<void*>(this);
      NS_ADDREF(this);
      return NS_OK;
    }

    for (std::vector<nsISupports*>::iterator it = Aggregated.begin(); it != Aggregated.end(); ++it)
    {
      nsISupports* aggregate = *it;
      nsresult err = aggregate->QueryInterface(id, result);
      switch(err)
      {
      case NS_ERROR_NO_INTERFACE:
        continue;

      default:
        return err;
      }
    }
    return NS_ERROR_NO_INTERFACE;
  }

  template<typename InterfaceType>
  class nsAggregable : public InterfaceType
  {
  public:
    NS_DECL_ISUPPORTS

  public:
    template <typename AggregatorType>
    explicit nsAggregable(AggregatorType& aggregator);

    virtual ~nsAggregable()
    {
    }

  private:
    nsISupports* Aggregator;
  };

  template <class Interface>
  NS_IMPL_ADDREF_USING_AGGREGATOR(XPCOM::nsAggregable<Interface>, Aggregator);

  template <class Interface>
  NS_IMPL_RELEASE_USING_AGGREGATOR(XPCOM::nsAggregable<Interface>, Aggregator);

  template <typename Interface>
  template <typename AggregatorType>
  XPCOM::nsAggregable<Interface>::nsAggregable(AggregatorType& aggregator)
    : Aggregator(&aggregator)
  {
    aggregator.AddChild(*this);
  }

  template <class Interface>
  nsresult XPCOM::nsAggregable<Interface>::QueryInterface(const nsIID& iid, void** result)
  {
    *result = NULL;
    if (iid.Equals(NS_GET_IID(nsISupports)))
    {
      return Aggregator->QueryInterface(iid, result);
    }
    else if (iid.Equals(NS_GET_TEMPLATE_IID(Interface)))
    {
      Interface* interface = static_cast<Interface*>(this);
      *result = static_cast<void*>(interface);
      NS_ADDREF(this);
      return NS_OK;
    }
    return NS_ERROR_NO_INTERFACE;
  }

} // namespace XPCOM

#endif // __aggregate_h_34f5b5f1_d19b_4c39_b825_8be357c26302
