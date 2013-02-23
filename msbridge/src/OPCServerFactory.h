// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZ_OPCSERVERFACTORY__H__
#define __MOZ_OPCSERVERFACTORY__H__


#include <xpcom/nsIFactory.h>

/**
\brief класс Фабрики OPC серверов
*/

class OPCServerFactory : public nsIFactory
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIFACTORY

  static char* loader_string;//!< короткое имя класса
  static char* class_name;   //!< полное имя класса в формате XPCOM
  static const nsIID kCID;   //!< Идентификатор класса

  OPCServerFactory ();

private:
  ~OPCServerFactory ();

protected:
  /* additional members */
};

#endif