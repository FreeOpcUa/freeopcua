/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tests of IUnknown implementation and smart ptr api.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "../src/sqlite_registry.h"

#include <com/impl/error.h>
#include <gtest/gtest.h>

#include <unistd.h>

using namespace testing;

namespace
{
  const char* DatabaseName = "test_com.sqlite";
  class SqliteRegistry : public ::testing::Test
  {
  protected:
    virtual void TearDown()
    {
      unlink(DatabaseName);
    }
  };
}

TEST_F(SqliteRegistry, RegisterWorks)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";
  const std::string libraryPath = "library_path";

  ASSERT_NO_THROW(registry->RegisterClass(clsId.c_str(), libraryPath.c_str()));
  ASSERT_EQ(libraryPath, registry->GetLibraryPath(clsId.c_str()));
}

TEST_F(SqliteRegistry, UnregisterWorks)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId_unregister";
  const std::string libraryPath = "library_path_unregister";

  registry->RegisterClass(clsId.c_str(), libraryPath.c_str());
  registry->UnregisterClass(clsId.c_str());
  ASSERT_THROW(registry->GetLibraryPath(clsId.c_str()), Com::Impl::Error);
}

TEST_F(SqliteRegistry, ClassCannotBeRegisteredMultipleTimes)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";
  const std::string libraryPath = "library_path";

  ASSERT_NO_THROW(registry->RegisterClass(clsId.c_str(), libraryPath.c_str()));
  ASSERT_THROW(registry->RegisterClass(clsId.c_str(), libraryPath.c_str()), Com::Impl::Error);
}

TEST_F(SqliteRegistry, LibraryCanBeRegisteredForSeveralClasses)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";
  const std::string clsId1 = "clsId1";
  const std::string libraryPath = "library_path";

  ASSERT_NO_THROW(registry->RegisterClass(clsId.c_str(), libraryPath.c_str()));
  ASSERT_NO_THROW(registry->RegisterClass(clsId1.c_str(), libraryPath.c_str()));

  ASSERT_EQ(libraryPath, registry->GetLibraryPath(clsId.c_str()));
  ASSERT_EQ(libraryPath, registry->GetLibraryPath(clsId1.c_str()));
}

TEST_F(SqliteRegistry, RegisterThrowIfClsIdEmpty)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string libraryPath = "library_path";

  ASSERT_THROW(registry->RegisterClass(std::string(), libraryPath.c_str()), Com::Impl::Error);
}

TEST_F(SqliteRegistry, RegisterThrowIfLibraryPathEmpty)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";

  ASSERT_THROW(registry->RegisterClass(clsId.c_str(), std::string()), Com::Impl::Error);
}

TEST_F(SqliteRegistry, GetLibraryPathThrowsIfClassIDEmpty)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";
  const std::string libraryPath = "library_path";

  ASSERT_NO_THROW(registry->RegisterClass(clsId.c_str(), libraryPath.c_str()));
  ASSERT_THROW(registry->GetLibraryPath(std::string()), Com::Impl::Error);
}

TEST_F(SqliteRegistry, GetLibraryPathThrowsForUnregisteredClass)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";

  ASSERT_THROW(registry->GetLibraryPath(clsId.c_str()), Com::Impl::Error);
}


TEST_F(SqliteRegistry, UnregisterThrowsIfClassIDEmpty)
{
  const std::unique_ptr<Com::ClassRegistry> registry = Com::CreateSqliteRegistry(DatabaseName);
  const std::string clsId = "clsId";
  const std::string libraryPath = "library_path";

  ASSERT_NO_THROW(registry->RegisterClass(clsId.c_str(), libraryPath.c_str()));
  ASSERT_THROW(registry->UnregisterClass(std::string()), Com::Impl::Error);
}

