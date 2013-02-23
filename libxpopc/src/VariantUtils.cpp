// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <nsIComponentManager.h>
#include "VariantUtils.h"


/**
*/

bool type2str(VARTYPE type,std::wstring& type_str)
{
  std::wstring str;

  bool separator=false;

  if(type&VT_ARRAY)
  {
    str=L"VT_ARRAY";
    type=~VT_ARRAY;
    separator=true;
  }

  if(type&VT_BYREF)
  {
    if(separator)
      str+=L"|";
    str+=L"VT_BYREF";
    type=~VT_BYREF;
    separator=true;
  }

  switch(type)
  {
    case VT_EMPTY:    str=L"VT_EMPTY";    break;

    case VT_INT:      str=L"VT_INT";      break;
    case VT_I1:       str=L"VT_I1";       break;
    case VT_I2:       str=L"VT_I2";       break;
    case VT_I4:       str=L"VT_I4";       break;
    case VT_I8:       str=L"VT_I8";       break;
    case VT_INT_PTR:  str=L"VT_INT_PTR";  break; 

    case VT_UINT:     str=L"VT_UINT";     break;
    case VT_UI1:      str=L"VT_UI1";      break;
    case VT_UI2:      str=L"VT_UI2";      break;
    case VT_UI4:      str=L"VT_UI4";      break;
    case VT_UI8:      str=L"VT_UI8";      break;
    case VT_UINT_PTR: str=L"VT_UINT_PTR"; break;

    case VT_DECIMAL:  str=L"VT_DECIMAL";  break;

    case VT_R4:       str=L"VT_R4";       break;
    case VT_R8:       str=L"VT_R8";       break;

    case VT_BOOL:     str=L"VT_BOOL";     break;


    case VT_LPSTR:    str=L"VT_LPSTR";    break;
    case VT_LPWSTR:   str=L"VT_LPWSTR";   break;
    case VT_BSTR:     str=L"VT_BSTR";     break;

    case VT_CY:       str=L"VT_CY";       break;

    case VT_DATE:     str=L"VT_DATE";     break;
    case VT_FILETIME: str=L"VT_FILETIME"; break;
    
    case VT_VARIANT:  str=L"VT_VARIANT";  break;

    // неизвестный тип
    default:          str=L"";            break;
  }
  type_str=str;
  return true;
}


bool str2type(const std::wstring& str,VARTYPE& type)
{
  VARTYPE type1=0;

  if(wcsstr(str.c_str(),L"VT_ARRAY")!=NULL)
    type1|=VT_ARRAY;
  if(wcsstr(str.c_str(),L"VT_BYREF"))
    type1|=VT_BYREF;

  if(wcsstr(str.c_str(),L"VT_EMPTY"))
    type1|=VT_EMPTY;

  else if(wcsstr(str.c_str(),L"VT_INT"))
    type1|=VT_INT;
  else if(wcsstr(str.c_str(),L"VT_INT_PTR"))
    type1|=VT_INT_PTR;
  else if(wcsstr(str.c_str(),L"VT_I1"))
    type1|=VT_I1;
  else if(wcsstr(str.c_str(),L"VT_I2"))
    type1|=VT_I2;
  else if(wcsstr(str.c_str(),L"VT_I4"))
    type1|=VT_I4;
  else if(wcsstr(str.c_str(),L"VT_I8"))
    type1|=VT_I8;

  else if(wcsstr(str.c_str(),L"VT_UINT"))
    type1|=VT_UINT;
  else if(wcsstr(str.c_str(),L"VT_UINT_PTR"))
    type1|=VT_UINT_PTR;
  else if(wcsstr(str.c_str(),L"VT_UI1"))
    type1|=VT_UI1;
  else if(wcsstr(str.c_str(),L"VT_UI2"))
    type1|=VT_UI2;
  else if(wcsstr(str.c_str(),L"VT_UI4"))
    type1|=VT_UI4;
  else if(wcsstr(str.c_str(),L"VT_UI8"))
    type1|=VT_UI8;

  else if(wcsstr(str.c_str(),L"VT_DECIMAL"))
    type1|=VT_DECIMAL;

  else if(wcsstr(str.c_str(),L"VT_R4"))
    type1|=VT_R4;
  else if(wcsstr(str.c_str(),L"VT_R8"))
    type1|=VT_R8;

  else if(wcsstr(str.c_str(),L"VT_BOOL"))
    type1|=VT_BOOL;

  else if(wcsstr(str.c_str(),L"VT_LPSTR"))
    type1|=VT_LPSTR;
  else if(wcsstr(str.c_str(),L"VT_LPWSTR"))
    type1|=VT_LPWSTR;
  else if(wcsstr(str.c_str(),L"VT_BSTR"))
    type1|=VT_BSTR;

  else if(wcsstr(str.c_str(),L"VT_CY"))
    type1|=VT_CY;

  else if(wcsstr(str.c_str(),L"VT_DATE"))
    type1|=VT_DATE;
  else if(wcsstr(str.c_str(),L"VT_FILETIME"))
    type1|=VT_FILETIME;

  else if(wcsstr(str.c_str(),L"VT_VARIANT"))
    type1|=VT_VARIANT;

  else
    return false;

  type=type1;
  return true;
}

bool ms2moz(const VARIANT* ms,nsIVariant** moz)
{
  if(moz==NULL || ms==NULL)
    return false;

  nsresult res=NS_OK;

  nsIComponentManager* mgr=NULL;
  res=NS_GetComponentManager(&mgr);

  if(NS_FAILED(res))
    return false;
  nsIWritableVariant* var=NULL;

  res=mgr->CreateInstanceByContractID(NS_VARIANT_CONTRACTID,
                                  NULL,
                                  NS_GET_IID(nsIWritableVariant),
                                  (void**)&var);
  
  mgr->Release();
  mgr=NULL;

  if(NS_FAILED(res))
    return false;

  var->Release();
  var=NULL;
}

bool moz2ms(nsIVariant* moz,VARIANT* ms)
{
  if(moz==NULL || ms==NULL)
    return false;
}
