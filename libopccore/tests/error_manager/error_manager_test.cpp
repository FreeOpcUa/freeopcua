/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Test cases of Error manager
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/managers/error_manager/id.h>
#include <opc/managers/error_manager/manager.h>
#include <opc/managers/error_manager/factory.h>
#include <opc/common/exception.h>
#include <opc/common/addons_core/addon.h>
#include <opc/common/addons_core/addon_manager.h>

#include <cppunit/config/SourcePrefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>


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
  Common::AddonsManager::UniquePtr manager = Common::CreateAddonsManager();

  Common::AddonConfiguration errorConfig;
  errorConfig.ID = ErrorManager::ManagerID;
  errorConfig.Factory = ErrorManager::CreateAddonFactory();
  manager->Register(errorConfig);
  CPPUNIT_ASSERT_NO_THROW(manager->Start(/*addonsConfig*/));
  ErrorManager::Manager::SharedPtr errorManager;
  CPPUNIT_ASSERT_NO_THROW(errorManager = Common::GetAddon<ErrorManager::Manager>(*manager, ErrorManager::ManagerID));
  CPPUNIT_ASSERT(errorManager);
  const Common::Error& err = CREATE_COMMON_ERROR(1, "error");
  CPPUNIT_ASSERT_NO_THROW(errorManager->SetLastError(err));
  const Common::Error& lastError = errorManager->GetLastError();
  CPPUNIT_ASSERT(lastError.GetCode() == err.GetCode());
}
