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

  void Uri::Initialize(const char* uriString, std::size_t)
  {
    xmlURIPtr uri = xmlParseURI(uriString);
    if (!uri)
    {
      THROW_ERROR1(CannotParseUri, uriString);
    }
    if (uri->scheme)
    {
      SchemeStr = uri->scheme;
    }
    if (uri->server)
    {
      HostStr = uri->server;
    }
    PortNum = uri->port;
    xmlFreeURI(uri);
  }

} // namespace Common


