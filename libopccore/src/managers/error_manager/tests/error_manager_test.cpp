/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Test cases of Error manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>

#include <opccore/managers/error_manager/manager.h>
#include <opccore/common/exception.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>
#include "../register.h"


class ErrorManagerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ErrorManagerTestCase);
  CPPUNIT_TEST(Test);
  CPPUNIT_TEST_SUITE_END();

protected:
  void Test();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ErrorManagerTestCase);


void ErrorManagerTestCase::Test()
{
  Common::AddonsManager::SharedPtr manager = Common::GetAddonsManager();
  ErrorManager::RegisterErrorManagerAddon();
  CPPUNIT_ASSERT_NO_THROW(manager->Start());
  ErrorManager::Manager::SharedPtr errorManager;
  CPPUNIT_ASSERT_NO_THROW(errorManager = Common::GetAddon<ErrorManager::Manager>(Common::ADDON_ID_ERROR_MANAGER));
  CPPUNIT_ASSERT(errorManager);
  const Common::Error& err = CREATE_COMMON_ERROR(1, "error");
  CPPUNIT_ASSERT_NO_THROW(errorManager->SetLastError(err));
  const Common::Error& lastError = errorManager->GetLastError();
  CPPUNIT_ASSERT(lastError.GetCode() == err.GetCode());
}
