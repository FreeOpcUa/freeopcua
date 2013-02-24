/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tests of CoCreateInstance.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <com/class_ids.h>
#include <com/iclass_factory.h>
#include <com/iclass_registry.h>
#include <com/iunknown.h>

#include <gtest/gtest.h>
#include <unistd.h>

#include "test_com.h"

using namespace testing;


namespace
{
  const char* DatabaseName = "test_com.sqlite";

  class CoCreateInstance : public ::testing::Test
  {
  protected:
    virtual void TearDown()
    {
      unlink(DatabaseName);
    }
  };
}


TEST_F(CoCreateInstance, NullParameters)
{
  Com::IUnknown::SharedPtr unk;
  ASSERT_EQ(Com::CoCreateInstance(NULL, Com::ResultPtr(unk)), Com::COM_INVALID_PARAMETER1);
  ASSERT_EQ(Com::CoCreateInstance("classID", NULL), Com::COM_INVALID_PARAMETER2);
  ASSERT_EQ(Com::CoCreateInstance(NULL, NULL), Com::COM_INVALID_PARAMETER1);
}

TEST_F(CoCreateInstance, SupportsIClassRegistry)
{
  Com::IUnknown::SharedPtr unk;
  ASSERT_EQ(Com::CoCreateInstance(CLSID_ClassRegistry, Com::ResultPtr(unk)), Com::COM_SUCCESS);
  ASSERT_NE(Com::IUnknown::SharedPtr(), unk);
  Com::IClassRegistry::SharedPtr registry;
  ASSERT_EQ(unk->QueryInterface(IID_IClassRegistry, Com::ResultPtr(registry)), Com::COM_SUCCESS);
  ASSERT_NE(Com::IClassRegistry::SharedPtr(), registry);
}

TEST_F(CoCreateInstance, SupportsIClassFactory)
{
  Com::IUnknown::SharedPtr unk;
  ASSERT_EQ(Com::CoCreateInstance(CLSID_ClassFactory, Com::ResultPtr(unk)), Com::COM_SUCCESS);
  ASSERT_NE(Com::IUnknown::SharedPtr(), unk);
  Com::IClassFactory::SharedPtr factory;
  ASSERT_EQ(unk->QueryInterface(IID_IClassFactory, Com::ResultPtr(factory)), Com::COM_SUCCESS);
  ASSERT_NE(Com::IClassFactory::SharedPtr(), factory);
}

namespace
{
  void RegisterTestClass()
  {
    Com::IUnknown::SharedPtr unk;
    ASSERT_EQ(Com::CoCreateInstance(CLSID_ClassRegistry, Com::ResultPtr(unk)), Com::COM_SUCCESS);
    ASSERT_NE(Com::IUnknown::SharedPtr(), unk);
    Com::IClassRegistry::SharedPtr registry;
    ASSERT_EQ(unk->QueryInterface(IID_IClassRegistry, Com::ResultPtr(registry)), Com::COM_SUCCESS);
    ASSERT_NE(Com::IClassRegistry::SharedPtr(), registry);
    ASSERT_EQ(registry->Register(CLSID_Test, "./.libs/libcom_test.so"), Com::COM_SUCCESS);
    //ASSERT_EQ(registry->Register(CLSID_Test, "/media/600gb/treww/mozopc_tmp/libcom/.libs/libcom_test.so"), Com::COM_SUCCESS);
  }
}


TEST_F(CoCreateInstance, CreatingClassFromSharedLibraryWorks)
{
  RegisterTestClass();
  Com::IUnknown::SharedPtr unk;
  ASSERT_EQ(Com::CoCreateInstance(CLSID_Test, Com::ResultPtr(unk)), Com::COM_SUCCESS);
  ASSERT_NE(Com::IUnknown::SharedPtr(), unk);
}

