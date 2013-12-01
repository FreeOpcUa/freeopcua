/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include <opc/ua/protocol/datetime.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace OpcUa;

TEST(DateTime, ToTimeT_And_Back)
{
  const time_t t = ToTimeT(ToDateTime(2));
  ASSERT_EQ(t, 2);
}
