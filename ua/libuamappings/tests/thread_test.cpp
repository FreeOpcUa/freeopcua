/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Test of OpcUa::Thread.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#include <src/thread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

namespace
{

  class ThreadObserver : public OpcUa::ThreadObserver
  {
  public:
    typedef NiceMock<ThreadObserver> Mock;
  public:
    MOCK_METHOD0(OnSuccess, void());
    MOCK_METHOD1(OnError, void (const std::exception& exc));
  };

  void DoNothing()
  {
  }

  void ThrowsException()
  {
    throw std::logic_error("oppps!");
  }
}

TEST(OpcUaThread, CallsOnSuccess)
{
  ThreadObserver::Mock observer;
  EXPECT_CALL(observer, OnSuccess()).Times(1);
  EXPECT_CALL(observer, OnError(_)).Times(0);

  OpcUa::Thread thread(std::function<void()>(DoNothing), observer);
  thread.Join();  
}

TEST(OpcUaThread, CallsOnError)
{
  ThreadObserver::Mock observer;
  EXPECT_CALL(observer, OnSuccess()).Times(0);
  EXPECT_CALL(observer, OnError(_)).Times(1);

  OpcUa::Thread thread(std::function<void()>(ThrowsException), observer);
  thread.Join();  
}

