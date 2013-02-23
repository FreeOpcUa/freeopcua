// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __VARIANT_UTILS__H__
#define __VARIANT_UTILS__H__

#include <nsIVariant.h>

#include <windows.h>
#include <string>

bool type2str(VARTYPE type,std::wstring&);
bool str2type(const std::wstring& str,VARTYPE& type);

bool ms2moz(const VARIANT* ms,nsIVariant** moz);

bool moz2ms(nsIVariant* ns,VARIANT* moz);

#endif // __VARIANT_UTILS__H__