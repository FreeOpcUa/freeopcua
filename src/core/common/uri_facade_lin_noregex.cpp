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

#include <libxml/uri.h>

namespace Common
{

void Uri::Initialize(const std::string& uriString)
{
  xmlURIPtr uri = xmlParseURI(uriString.c_str());
  if (!uri)
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }

  if (uri->scheme)
    {
      SchemeStr = uri->scheme;
    }

  if (uri->user)
    {
      UserStr = uri->user;
      int ix = UserStr.find(':');

      if (ix > 0)
        {
          PasswordStr = UserStr.substr(ix + 1);
          UserStr = UserStr.substr(0, ix);
        }
    }

  if (uri->server)
    {
      HostStr = uri->server;
    }

  PortNum = uri->port;
  xmlFreeURI(uri);

  if (SchemeStr.empty() || HostStr.empty())
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }
}

} // namespace Common
