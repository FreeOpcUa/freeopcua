/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test Remote computer connection.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

#include <opc/ua/client/computer.h>

#include <stdexcept>

TEST(Computer, CanCreateSession)
{
  std::unique_ptr<OpcUa::Remote::Computer> computer = OpcUa::Remote::Connect(GetEndpoint());
}

