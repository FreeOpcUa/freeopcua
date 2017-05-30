/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Uri.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <windows.h>
#include <wininet.h>

#include <opc/common/uri_facade.h>
#include <opc/common/exception.h>


namespace Common
{

void Uri::Initialize(const char * uriString, std::size_t size)
{
  URL_COMPONENTS url = {0};
  url.dwStructSize = sizeof(url);
  url.dwSchemeLength = 1;
  url.dwUserNameLength = 1;
  url.dwPasswordLength = 1;
  url.dwHostNameLength = 1;
  DWORD options = 0;

  // TODO msdn says do not use this function in services and in server patforms. :(
  // TODO http://msdn.microsoft.com/en-us/library/windows/desktop/aa384376(v=vs.85).aspx
  if (!InternetCrackUrl(uriString, size, options, &url))
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }


  SchemeStr = std::string(url.lpszScheme, url.lpszScheme + url.dwSchemeLength);
  UserStr = std::string(url.lpszUserName, url.lpszUserName + url.dwUserNameLength);
  PasswordStr = std::string(url.lpszPassword, url.lpszPassword + url.dwPasswordLength);
  HostStr = std::string(url.lpszHostName, url.lpszHostName + url.dwHostNameLength);
  PortNum = url.nPort;

  if (SchemeStr.empty() || HostStr.empty())
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }
}

} // namespace Common


