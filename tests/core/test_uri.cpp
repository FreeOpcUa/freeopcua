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

TEST(Uri, CanParseFullURI)
{
  Common::Uri uri("http://host:8080");
  ASSERT_EQ(uri.Scheme(), "http");
  ASSERT_EQ(uri.Host(), "host");
  ASSERT_EQ(uri.Port(), 8080u);
}

TEST(Uri, CanParseFullURIWithUserAndPassword)
{
  Common::Uri uri("http://user:password@host:8080");
  ASSERT_EQ(uri.Scheme(), "http");
  ASSERT_EQ(uri.User(), "user");
  ASSERT_EQ(uri.Password(), "password");
  ASSERT_EQ(uri.Host(), "host");
  ASSERT_EQ(uri.Port(), 8080u);
}

TEST(Uri, CanParseFullWithoutPort)
{
  Common::Uri uri("http://host");
  ASSERT_EQ(uri.Scheme(), "http");
  ASSERT_EQ(uri.Host(), "host");
}

TEST(Uri, ThrowsIfSchemeEmpty)
{
  ASSERT_THROW(Common::Uri("://host"), std::exception);
}

TEST(Uri, ThrowsIfHostEmpty)
{
  ASSERT_THROW(Common::Uri("htp://"), std::exception);
  ASSERT_THROW(Common::Uri("htp://:8080"), std::exception);
}

TEST(Uri, ThrowsIfInvalid)
{
  ASSERT_THROW(Common::Uri("httphost8080"), std::exception);
}

