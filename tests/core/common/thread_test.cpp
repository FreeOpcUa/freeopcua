/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of OpcUa::Thread.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/thread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

namespace
{

  class TestThreadObserver : public Common::ThreadObserver
  {
  public:
    TestThreadObserver()
      : OnSuccessCallCount(0)
      , OnErrorCallCount(0)
    {
    }

    virtual void OnSuccess() override
    {
      ++OnSuccessCallCount;
    }

    virtual void OnError(const std::exception& exc) override
    {
      ++OnErrorCallCount;
    }

  public:
    volatile unsigned OnSuccessCallCount;
    volatile unsigned OnErrorCallCount;
  };

  void DoNothing()
  {
  }

  void ThrowsException()
  {
    throw std::logic_error("oppps!");
  }

}

TEST(Thread, CallsOnSuccess)
{
  TestThreadObserver observer;

  Common::Thread thread(std::function<void()>(DoNothing), &observer);
  thread.Join();
  EXPECT_EQ(observer.OnSuccessCallCount, 1);
  EXPECT_EQ(observer.OnErrorCallCount, 0);
}

TEST(Thread, CallsOnError)
{
  TestThreadObserver observer;

  Common::Thread thread(std::function<void()>(ThrowsException), &observer);
  thread.Join();
  EXPECT_EQ(observer.OnSuccessCallCount, 0);
  EXPECT_EQ(observer.OnErrorCallCount, 1);
}

