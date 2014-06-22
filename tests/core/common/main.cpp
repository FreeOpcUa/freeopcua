/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Test main function
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
  try
  {
    CPPUNIT_NS::TestResult controller;
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener(&progress);
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);
    CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
    outputter.write();
    return result.wasSuccessful() ? 0 : -1;
  }
  catch(const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }
  return -1;
}
