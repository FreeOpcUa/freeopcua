/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Uri.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <string>

namespace Common
{

  class Uri
  {
  public:
    explicit Uri::Uri(const std::string& uriString)
    {
      Initialize(uriString.c_str(), uriString.size());
    }

    explicit Uri::Uri(const char* uriString)
    {
      Initialize(uriString, 0);
    }

    std::string Uri::Scheme() const
    {
      return SchemeStr;
    }

    std::string Uri::Host() const
    {
      return HostStr;
    }

    unsigned Uri::Port() const
    {
      return PortNum;
    }

  private:
    void Initialize(const char* uriString, std::size_t len);

  private:
    std::string SchemeStr;
    std::string HostStr;
    unsigned PortNum;
  };

} // namespace Common

