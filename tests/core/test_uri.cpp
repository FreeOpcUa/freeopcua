/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Dynamic addon for tests.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/uri_facade.h>

#include <gtest/gtest.h>

TEST(Uri, CanParseValidURI)
{
  Common::Uri uri("http://host:8080");
  ASSERT_EQ(uri.Scheme(), "http");
  ASSERT_EQ(uri.Host(), "host");
  ASSERT_EQ(uri.Port(), 8080u);
}

TEST(Uri, ThrowsIfInvalid)
{
  ASSERT_THROW(Common::Uri uri("http://host:#8080"), std::exception);
}

