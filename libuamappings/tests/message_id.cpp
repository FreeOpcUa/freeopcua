/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of opc ua binary handshake.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "common.h"

using namespace testing;
using namespace OpcUa;

TEST(MessageIDFromNodeID, CanBeConvertedFromValidNodeID)
{
  NodeID id(OPEN_SECURE_CHANNEL_REQUEST);
  ASSERT_EQ(GetMessageID(id), OPEN_SECURE_CHANNEL_REQUEST);
}


