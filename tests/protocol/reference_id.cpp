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

TEST(ReferenceIdFromNodeId, CanBeConvertedFromValidNodeId)
{
  NodeId id(ReferenceId::HasChild);
  ASSERT_EQ(id.Encoding, EV_NUMERIC);
  ASSERT_EQ(id.NumericData.NamespaceIndex, 0);
  ASSERT_EQ(static_cast<ReferenceId>(id.NumericData.Identifier), ReferenceId::HasChild);
}

