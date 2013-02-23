/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tests of IClassRegistry implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "../src/iclass_registry_impl.h"
#include "../src/sqlite_registry.h"

#include <com/impl/error.h>
#include <gtest/gtest.h>

#include <unistd.h>

using namespace testing;

namespace
{
  const char* DatabaseName = "test_com.sqlite";

  class IClassRegistryTest : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
      registry.reset(new Com::Impl::ClassRegistry(Com::CreateSqliteRegistry(DatabaseName))); 
    }
    virtual void TearDown()
    {
      registry.reset();
      unlink(DatabaseName);
    }

  protected:
   std::string GetStdString(Com::IString::SharedPtr iString)
   {
     const char* str = NULL;
     iString->Get(&str);
     EXPECT_EQ(Com::COM_SUCCESS, iString->Get(&str));
     EXPECT_TRUE(str != NULL);
     return std::string(str);
   }

  protected:
    Com::IClassRegistry::UniquePtr registry; 
  };
}

TEST_F(IClassRegistryTest, RegisterWorks)
{
  const std::string clsId = "clsId";
  const std::string libraryPath = "library_path";

  ASSERT_EQ(Com::COM_SUCCESS, registry->Register(clsId.c_str(), libraryPath.c_str()));
  Com::IString::SharedPtr libraryPathString;
  ASSERT_EQ(Com::COM_SUCCESS, registry->GetLibraryPath(clsId.c_str(), Com::ResultPtr(libraryPathString)));
  ASSERT_TRUE(libraryPathString != Com::IString::SharedPtr());
  ASSERT_EQ(libraryPath, GetStdString(libraryPathString));
}

TEST_F(IClassRegistryTest, UnregisterWorks)
{
  const std::string clsId = "clsId_unregister";
  const std::string libraryPath = "library_path_unregister";

  ASSERT_EQ(registry->Register(clsId.c_str(), libraryPath.c_str()), Com::COM_SUCCESS);
  ASSERT_EQ(registry->Unregister(clsId.c_str()), Com::COM_SUCCESS);
  Com::IString::SharedPtr resultStr;
  ASSERT_EQ(registry->GetLibraryPath(clsId.c_str(), Com::ResultPtr(resultStr)), Com::COM_CLASS_NOT_REGISTERED);
}

TEST_F(IClassRegistryTest, ClassCannotBeRegisteredMultipleTimes)
{
  ASSERT_EQ(registry->Register("clsId", "libraryPath"), Com::COM_SUCCESS);
  ASSERT_EQ(registry->Register("clsId", "libraryPath"), Com::COM_CLASS_ALREADY_REGISTERED);
}

TEST_F(IClassRegistryTest, LibraryCanBeRegisteredForSeveralClasses)
{
  ASSERT_EQ(registry->Register("clsId",  "library_path"), Com::COM_SUCCESS);
  ASSERT_EQ(registry->Register("clsId1", "library_path"), Com::COM_SUCCESS);
}

TEST_F(IClassRegistryTest, ClsIdCannotBeEmptyInRegistry)
{
  ASSERT_EQ(registry->Register("", "libraryPath"), Com::COM_INVALID_PARAMETER1);
  ASSERT_EQ(registry->Register(NULL, "libraryPath"), Com::COM_INVALID_PARAMETER1);
}

TEST_F(IClassRegistryTest, LibraryPathCannotBeEmpty)
{
  ASSERT_EQ(registry->Register("clsId", ""), Com::COM_INVALID_PARAMETER2);
  ASSERT_EQ(registry->Register("clsId", NULL), Com::COM_INVALID_PARAMETER2);
}


TEST_F(IClassRegistryTest, ParametersCannotBeEmptyInGetLibraryPath)
{
  ASSERT_EQ(registry->GetLibraryPath(NULL, NULL), Com::COM_INVALID_PARAMETER1);
  ASSERT_EQ(registry->GetLibraryPath("", NULL), Com::COM_INVALID_PARAMETER2);
  ASSERT_EQ(registry->GetLibraryPath("clsId", NULL), Com::COM_INVALID_PARAMETER2);
}


TEST_F(IClassRegistryTest, GetLibraryPathReturnsErrorForUnregisteredClass)
{
  Com::IString::SharedPtr libraryPath;
  ASSERT_EQ(registry->GetLibraryPath("clsId", Com::ResultPtr(libraryPath)), Com::COM_CLASS_NOT_REGISTERED);
}

TEST_F(IClassRegistryTest, UnregisterThrowsIfClassIDEmpty)
{
  ASSERT_EQ(registry->Unregister(""), Com::COM_INVALID_PARAMETER1);
  ASSERT_EQ(registry->Unregister(NULL), Com::COM_INVALID_PARAMETER1);
}

