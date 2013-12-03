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
  time_t timet = time(0);
  const time_t t = ToTimeT(ToDateTime(timet));
  ASSERT_EQ(t, timet);
}

TEST(DateTime, FromTimeT)
{
  time_t t = 1;
  const time_t converted = OpcUa::ToTimeT(OpcUa::ToDateTime(t));
  ASSERT_EQ(t, converted);
}

TEST(DateTime, FromDateTime)
{
  OpcUa::DateTime t = OpcUa::CurrentDateTime();
  const OpcUa::DateTime converted = OpcUa::ToDateTime(OpcUa::ToTimeT(t));
  ASSERT_EQ(t/10000000LL*10000000LL, converted);
}

TEST(DateTime, ZeroToTimeT)
{
  DateTime t(0);
  const time_t converted = OpcUa::ToTimeT(t);
  ASSERT_EQ(converted, -11676096000);
}

TEST(DateTime, ZeroTimeT)
{
  time_t t = 0;
  const DateTime converted = OpcUa::ToDateTime(t);
  ASSERT_EQ(converted/10000000, 11676096000);
}
