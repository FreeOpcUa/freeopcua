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

#include <com/impl/iunknown_impl.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace
{
  class IUnknownMock : public Com::IUnknown
  {
  public:
    MOCK_METHOD0(AddRef, unsigned());
    MOCK_METHOD0(Release, unsigned());
    MOCK_METHOD2(QueryInterface, Com::HRESULT(const char* iid, void** result));
 };
}

TEST(TestSmartPtr, SharedPtrDeletesObjectViaRelease)
{
  StrictMock<IUnknownMock> iUnknownMock;
  EXPECT_CALL(iUnknownMock, Release()).WillOnce(Return(0));
  
  {
    std::shared_ptr<Com::IUnknown> smartPtr = Com::SharedPtr<Com::IUnknown>(&iUnknownMock);
  }
}

TEST(TestSmartPtr, AddRefRelease)
{
  Com::IUnknown::SharedPtr unk = Com::SharedPtr<Com::IUnknown>(new Com::Impl::Unknown());
  EXPECT_EQ(2u, unk->AddRef());
  EXPECT_EQ(1u, unk->Release());
}

TEST(TestSmartPtr, ResultPtrReturnsValidSharedPtr)
{
  Com::IUnknown::SharedPtr unk1 = Com::SharedPtr<Com::IUnknown>(new Com::Impl::Unknown());
  Com::IUnknown::SharedPtr unk2;
  EXPECT_EQ(unk1->QueryInterface(IID_IUnknown, Com::ResultPtr(unk2)), Com::COM_SUCCESS);
  ASSERT_TRUE(static_cast<bool>(unk2));
}

namespace
{
  class UnknownMock : public Com::Impl::Unknown
  {
  public:
    virtual ~UnknownMock()
    {
      Delete();
    }
    
    MOCK_METHOD0(Delete, void());
 };
}

TEST(IUnknownSharedPtr, DestructorCallsRelease)
{
  std::shared_ptr<StrictMock<UnknownMock> > unk = Com::SharedPtr<StrictMock<UnknownMock>>(new StrictMock<UnknownMock>());
  EXPECT_CALL(*unk, Delete()).Times(1);
  unk.reset();
}

TEST(IUnknownUniquePtr, DestructorCallsRelease)
{
  std::unique_ptr<StrictMock<UnknownMock>, Com::InterfaceDeleter<StrictMock<UnknownMock>>> unk = Com::UniquePtr<StrictMock<UnknownMock>>(new StrictMock<UnknownMock>());
  EXPECT_CALL(*unk, Delete()).Times(1);
  unk.reset();
}

