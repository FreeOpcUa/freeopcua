/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Addon for accessing opcua server inside process.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/computer.h>
#include <opc/ua/server/addons/tcp_server_addon.h>

namespace OpcUa
{
  namespace Server
  {

    class BuiltinServerAddon : public TcpServerAddon
    {
    public:
      virtual std::shared_ptr<Remote::Computer> GetComputer() const = 0;
    };

    class BuiltingServerFactory : public Common::AddonFactory
    {
    public:
        DEFINE_CLASS_POINTERS(BuiltingServerFactory);

      public:
        virtual Common::Addon::UniquePtr CreateAddon();
    };


  } // namespace Server
} // namespace OpcUa
