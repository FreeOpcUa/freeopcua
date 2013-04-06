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

#include <opccore/managers/error_manager/id.h>
#include <opccore/managers/error_manager/manager.h>
#include <opccore/managers/error_manager/register.h>
#include <opccore/common/exception.h>
#include <opccore/common/addons_core/addon_ids.h>
#include <opccore/common/addons_core/addon_manager.h>


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
  Common::AddonsConfiguration addonsConfig;
  addonsConfig.StaticAddonsInitializers.push_back(std::bind(ErrorManager::RegisterErrorManagerAddon, std::placeholders::_1));

  CPPUNIT_ASSERT_NO_THROW(manager->Start(addonsConfig));
  ErrorManager::Manager::SharedPtr errorManager;
  CPPUNIT_ASSERT_NO_THROW(errorManager = Common::GetAddon<ErrorManager::Manager>(*manager, ErrorManager::ManagerID));
  CPPUNIT_ASSERT(errorManager);
  const Common::Error& err = CREATE_COMMON_ERROR(1, "error");
  CPPUNIT_ASSERT_NO_THROW(errorManager->SetLastError(err));
  const Common::Error& lastError = errorManager->GetLastError();
  CPPUNIT_ASSERT(lastError.GetCode() == err.GetCode());
}
