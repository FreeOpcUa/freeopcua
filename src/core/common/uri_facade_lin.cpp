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

void Uri::Initialize(const std::string &uriString)
{
  /*
   acording to wiki https://en.wikipedia.org/wiki/Uniform_Resource_Identifier#URI_resolution

   The scheme, consisting of a sequence of characters beginning with a letter and followed by any combination of letters, digits, plus (+), period (.), or hyphen (-)
   ([a-zA-Z][a-zA-Z0-9.+-]*)

   An optional authentication section of a user name and password, separated by a colon, followed by an at symbol (@)
   (([^@:]*)(:([^@]+))?@)?

   A host, consisting of either a registered name (including but not limited to a hostname), or an IP address.
   IPv4 addresses must be in dot-decimal notation, and IPv6 addresses must be enclosed in brackets ([ ])
   ([^/:?]+)

   An optional port number, separated from the hostname by a colon
   (:([0-9]+))?

   A path, which contains data, usually organized in hierarchical form, that appears as a sequence of segments separated by slashes.
   Such a sequence may resemble or map exactly to a file system path, but does not always imply a relation to one.
   The path must begin with a single slash (/) if an authority part was present
   (/[^?#]*)?

   rest of URI is query + fragment, currently not used in freeopcua
   (.*)
   */
  std::regex uri_regex("([a-zA-Z][a-zA-Z0-9.+-]*)://(([^@:]*)(:([^@]+))?@)?([^/:?]+)(:([0-9]+))?(/[^?#]*)?(.*)");
  std::smatch uri_match;
  if (!std::regex_match(uriString, uri_match, uri_regex))
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }


  enum {Scheme = 1, User = 3, Password = 5, Host = 6, Port = 8, Path = 9, QueryAndFragment = 10};

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


