/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief API for creating com classes. This api exported from dlls.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/impl/module.h>
#include "opc_server.h"

#include <com/com.h>
#include <com/com_errors.h>
#include <opc/opcda.h>

#include <string.h>

#include <iostream>

Com::IUnknown* Opc::CreateInstance()
{
  OpcClient::Server::UniquePtr server(OpcClient::CreateServer());
  return new Opc::Server(std::move(server));
}

