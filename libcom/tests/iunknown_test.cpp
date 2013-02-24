/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Tests of Com::Impl::String.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <com/impl/iunknown_impl.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

TEST(TestIUnknown, CorrectIIDIUnknown)
{
  EXPECT_EQ(0, strcmp(IID_IUnknown, "00000000-0000-0000-C000-000000000046"));
}

TEST(TestIUnknown, AddRefRelease)
{
  Com::IUnknown* unk = new Com::Impl::Unknown();
  EXPECT_EQ(2u, unk->AddRef());
  EXPECT_EQ(1u, unk->Release());
  EXPECT_EQ(0u, unk->Release());
}

TEST(TestIUnknown, TestQueryInterfaceReturnsIUnknown)
{
  std::shared_ptr<Com::IUnknown> unk = Com::SharedPtr<Com::IUnknown>(new Com::Impl::Unknown());
  Com::IUnknown::SharedPtr unk1;
  EXPECT_EQ(Com::COM_SUCCESS, unk->QueryInterface(IID_IUnknown, Com::ResultPtr(unk1)));
  ASSERT_TRUE(static_cast<bool>(unk1));
}

TEST(TestIUnknown, QueryInterfaceIncreasesReferenceCount)
{
  Com::IUnknown* unk1 = new Com::Impl::Unknown();
  Com::IUnknown* unk2 = NULL;
  EXPECT_EQ(Com::COM_SUCCESS, unk1->QueryInterface(IID_IUnknown, (void**)&unk2));
  EXPECT_EQ(1, unk2->Release());
  EXPECT_EQ(0, unk1->Release());
}


TEST(TestIUnknown, TestQueryInterfaceReturnsErrorIfInterfaceUnknown)
{
  std::shared_ptr<Com::IUnknown> unk = Com::SharedPtr<Com::IUnknown>(new Com::Impl::Unknown());
  Com::IUnknown* unk1 = NULL;
  EXPECT_EQ(Com::COM_NOINTERFACE, unk->QueryInterface("iid_wrong", (void**)&unk1));
  EXPECT_EQ(NULL, unk1);
}

TEST(TestIUnknown, TestTemplateQueryInterfaceReturnsValidPtrForIUnknown)
{
  std::shared_ptr<Com::IUnknown> input = Com::SharedPtr<Com::IUnknown>(new Com::Impl::Unknown());
  std::shared_ptr<Com::IUnknown> output;
  ASSERT_TRUE(input->QueryInterface(IID_IUnknown, Com::ResultPtr(output)) == Com::COM_SUCCESS);
  ASSERT_TRUE(static_cast<bool>(output));
}


