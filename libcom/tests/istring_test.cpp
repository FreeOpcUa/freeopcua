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

#include <com/com_errors.h>
#include <com/impl/istring_impl.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;


TEST(IString, AddRefRelease)
{
  Com::IString* unk = new Com::Impl::String();
  EXPECT_EQ(2u, unk->AddRef());
  EXPECT_EQ(1u, unk->Release());
  EXPECT_EQ(0u, unk->Release());
}

TEST(IString, StringReturnsUnknown)
{
  Com::IString::SharedPtr str = Com::SharedPtr<Com::IString>(new Com::Impl::String());
  Com::IUnknown::SharedPtr unk;
  EXPECT_EQ(Com::COM_SUCCESS, str->QueryInterface(IID_IUnknown, Com::ResultPtr(unk)));
  ASSERT_TRUE(static_cast<bool>(unk));
}

TEST(String, StringReturnsString)
{
  std::shared_ptr<Com::IString> str1 = Com::SharedPtr<Com::IString>(new Com::Impl::String());
  std::shared_ptr<Com::IString> str2;
  EXPECT_EQ(Com::COM_SUCCESS, str1->QueryInterface(IID_IString, Com::ResultPtr(str2)));
  ASSERT_TRUE(static_cast<bool>(str2));
}

TEST(IString, UnknownReturnsString)
{
  std::shared_ptr<Com::IUnknown> unk = Com::SharedPtr<Com::IUnknown>(new Com::Impl::String());
  std::shared_ptr<Com::IString> str2;
  EXPECT_EQ(Com::COM_SUCCESS, unk->QueryInterface(IID_IString, Com::ResultPtr(str2)));
  ASSERT_TRUE(static_cast<bool>(str2));
}

TEST(IString, UnknownReturnsSameUnknown)
{
  std::shared_ptr<Com::IUnknown> unk1 = Com::SharedPtr<Com::IUnknown>(new Com::Impl::String());
  std::shared_ptr<Com::IUnknown> unk2;
  EXPECT_EQ(Com::COM_SUCCESS, unk1->QueryInterface(IID_IUnknown, Com::ResultPtr(unk2)));
  EXPECT_EQ(unk1, unk2);
}
TEST(IString, ValueWorks)
{
  std::shared_ptr<Com::IString> str = Com::SharedPtr<Com::IString>(new Com::Impl::String());
  const char value[] = "value";
  EXPECT_EQ(Com::COM_SUCCESS, str->Set(value));
  const char* resultValue = nullptr;
  EXPECT_EQ(Com::COM_SUCCESS, str->Get(&resultValue));
  ASSERT_TRUE(resultValue != nullptr);
  EXPECT_EQ(std::string(value), std::string(resultValue));
}

TEST(IString, ErrorIfSetParameterNull)
{
  std::shared_ptr<Com::IString> str = Com::SharedPtr<Com::IString>(new Com::Impl::String());
  EXPECT_EQ(Com::COM_INVALID_PARAMETER1, str->Set(nullptr));
}

TEST(IString, ErrorIfGetParameterNull)
{
  std::shared_ptr<Com::IString> str = Com::SharedPtr<Com::IString>(new Com::Impl::String());
  EXPECT_EQ(Com::COM_INVALID_PARAMETER1, str->Get(nullptr));
}

