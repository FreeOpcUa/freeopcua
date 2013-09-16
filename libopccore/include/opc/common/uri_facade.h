/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Thread wrapper with coockies.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <libxml/uri.h>

#include <opc/common/exception.h>

namespace Common
{

  class Uri
  {
  public:
    explicit Uri(const std::string& uriString)
    {
      Initialize(uriString.c_str());
    }

    explicit Uri(const char* uriString)
    {
      Initialize(uriString);
    }

    std::string Scheme() const
    {
      return SchemeStr;
    }

    std::string Host() const
    {
      return HostStr;
    }

    unsigned Port() const
    {
      return PortNum;
    }

  private:
    void Initialize(const char* uriString)
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

  private:
    std::string SchemeStr;
    std::string HostStr;
    unsigned PortNum;
  };

} // namespace Common

