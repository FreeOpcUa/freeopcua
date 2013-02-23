// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_OPCSERVERS_FACTORY__H__
#define __MOZ_OPCSERVERS_FACTORY__H__

#include <xpcom/nsID.h>
#include <xpcom/nsIFactory.h>

/**
\brief класс Фабрики перечислителя OPC серверов
*/


class OPCServersFactory: public nsIFactory
{
private:
	nsrefcnt mRefCnt;
public:

  static char* loader_string;//!< короткое имя класса
  static char* class_name;   //!< полное имя класса в формате XPCOM
  static const nsIID kCID;   //!< Идентификатор класса

  OPCServersFactory();
	virtual ~OPCServersFactory();
	NS_IMETHOD QueryInterface(const nsIID &aIID, void **aResult);
	NS_IMETHOD_(nsrefcnt) AddRef(void);
	NS_IMETHOD_(nsrefcnt) Release(void);
	NS_IMETHOD CreateInstance(nsISupports *aOuter, const nsIID & iid, void * *result);
	NS_IMETHOD LockFactory(PRBool lock);
};


#endif // __MOZ_OPCSERVERS_FACTORY__H__