/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon.h>

#include <opc/ua/server.h>

namespace OpcUa
{
  namespace Client
  {
    class Addon : public Common::Addon
    {
    public:
      DEFINE_CLASS_POINTERS(Addon);

    public:
      virtual std::string GetProtocol() const = 0;
      virtual OpcUa::Remote::Server::SharedPtr Connect(const std::string& url) = 0;
    };
  }
}
