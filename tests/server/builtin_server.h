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

#include <opc/ua/services/services.h>

namespace OpcUa
{
namespace Server
{

class BuiltinServer : public Common::Interface
{
public:
  DEFINE_CLASS_POINTERS(BuiltingServerFactory);

public:
  virtual OpcUa::Services::SharedPtr GetServices() const = 0;
};


} // namespace UaServer
} // namespace OpcUa
