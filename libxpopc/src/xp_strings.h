/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Convertation strings from XPCOM to std::string and vica vesa
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __xp_strings_h__447cc643_60b2_4674_b0ac_28a27d520dab
#define __xp_strings_h__447cc643_60b2_4674_b0ac_28a27d520dab

#include <nsStringAPI.h>
#include <string>

namespace XPCOM
{
  inline PRUnichar* ToPRUnichar(const std::string& src)
  {
    return ToNewUnicode(NS_ConvertASCIItoUTF16(src.c_str()));
  }

  inline nsString ToNsString(const std::string& src)
  {
    nsString str(NS_ConvertASCIItoUTF16(src.c_str()));
    return str;
  }

  inline std::string ToStdString(const PRUnichar* src)
  {
    const nsCString& srcString = NS_ConvertUTF16toUTF8(src);
    return srcString.get();
  }
}

#endif // __xp_strings_h__447cc643_60b2_4674_b0ac_28a27d520dab
