/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Uri.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/common/uri_facade.h>

#include <opc/common/exception.h>

#include <regex>

namespace Common
{

void Uri::Initialize(const char * uriString, std::size_t)
{
  std::regex uri_regex("([a-zA-Z][a-zA-Z0-9.+-]*)://(([^@:]*)(:([^@]+))?@)?([^/:]+)(:([0-9]+))?([^?]*)(.*)?");
  std::string uri_str(uriString);
  std::smatch uri_match;
  if (!std::regex_match(uri_str, uri_match, uri_regex))
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }

  enum {Scheme = 1, User = 3, Password = 5, Host = 6, Port = 8, Path = 9, Query = 10, Fragment = 11};

  SchemeStr = uri_match[Scheme].str();
  PasswordStr = uri_match[Password].str();
  UserStr = uri_match[User].str();
  HostStr = uri_match[Host].str();
  try {
    std::string n = uri_match[Port].str();
    PortNum = n.empty()? 0: stoul(n);
  }
  catch (...) {
    THROW_ERROR1(CannotParseUri, uriString);
  }

  if (SchemeStr.empty() || HostStr.empty())
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }
}

} // namespace Common


