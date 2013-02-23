/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tests for com server interfaces.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "class_data.h"
#include "dll.h"

#include <com/com.h>
#include <com/iunknown.h>
#include <gtest/gtest.h>
#include <memory>

namespace
{
  class ComDll : public testing::Test
  {
  public:
    ComDll()
    {
    }

  protected:
    virtual void SetUp()
    {
      Dll = Com::Impl::CreateDynamicLibraryManager();
      CreateInstance = Dll->Find<Com::CoCreateInstanceFunc>("CoCreateInstance", OPCTest::GetClassData().GetFilename());
    }

    virtual void TearDown()
    {
      Dll.reset();
    } 
    
  protected:
    std::unique_ptr<Com::Impl::DynamicLibraryManager> Dll;
    Com::CoCreateInstanceFunc CreateInstance;
  };


}

TEST_F(ComDll, ExportsCoCreateInstance)
{
  ASSERT_TRUE(CreateInstance != nullptr);
}

TEST_F(ComDll, ReturnsErrorIfClassIdNull)
{
  Com::IUnknown* unknown = nullptr;
  ASSERT_TRUE(CreateInstance(nullptr, &unknown) == Com::COM_INVALID_PARAMETER1);
  ASSERT_TRUE(unknown == nullptr);
}

TEST_F(ComDll, ReturnsErrorIfResultNull)
{
  ASSERT_TRUE(CreateInstance(OPCTest::GetClassData().GetClsId().c_str(), nullptr) == Com::COM_INVALID_PARAMETER2);
}

TEST_F(ComDll, ReturnsErrorIfInvalidClassID)
{
  Com::IUnknown* unknown = nullptr;
  ASSERT_TRUE(CreateInstance("unknown-iid", &unknown) == Com::COM_UNKNOWN_CLASS);
  ASSERT_TRUE(unknown == nullptr);
}

TEST_F(ComDll, CreatesValidObjectByClassID)
{
  Com::IUnknown* unknown = nullptr;
  ASSERT_TRUE(CreateInstance(OPCTest::GetClassData().GetClsId().c_str(), &unknown) == Com::COM_SUCCESS);
  ASSERT_TRUE(unknown != nullptr);
  unknown->Release();
}
/*
TEST_F(ComDll, CreatesValidObjectByContractID)
{
  Com::IUnknown* unknown = nullptr;
  ASSERT_TRUE(CreateInstance(OPCTest::GetClassData().GetContractId().c_str(), &unknown) == Com::COM_SUCCESS);
  ASSERT_TRUE(unknown != nullptr);
  unknown->Release();
}

TEST_F(ComDll, CreatesValidObjectByProgID)
{
  Com::IUnknown* unknown = nullptr;
  ASSERT_TRUE(CreateInstance(OPCTest::GetClassData().GetProgId().c_str(), &unknown) == Com::COM_SUCCESS);
  ASSERT_TRUE(unknown != nullptr);
  unknown->Release();
}
*/
