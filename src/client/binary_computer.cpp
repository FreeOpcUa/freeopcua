/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Remote server implementaion.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "stream_server.h"

#include <opc/common/uri_facade.h>
#include <opc/ua/client/binary_server.h>


std::unique_ptr<OpcUa::Remote::Server> OpcUa::Remote::CreateBinaryServer(std::shared_ptr<IOChannel> connection)
{
  return std::unique_ptr<OpcUa::Remote::Server>(new OpcUa::Internal::BinaryServer(connection));
}

