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
#include <stdexcept>

namespace OPCTest
{

  class ComServer : public testing::Test
  {
  public:
    ComServer()
    {
    }

  protected:
    virtual void SetUp()
    {
      Dll = Com::Impl::CreateDynamicLibraryManager();
      Com::CoCreateInstanceFunc CreateInstance = Dll->Find<Com::CoCreateInstanceFunc>("CoCreateInstance", GetClassData().GetFilename());
      CreateInstance(GetClassData().GetClsId().c_str(), Com::ResultPtr<Com::IUnknown>(Unknown));
      if (!Unknown)
      {
        throw std::logic_error("Unable to create IUnknown.");
      }
    }

    virtual void TearDown()
    {
      Unknown.reset();
      Dll.reset();
    } 
    
  protected:
    std::unique_ptr<Com::Impl::DynamicLibraryManager> Dll;
    Com::IUnknown::SharedPtr Unknown;
  };

} // namespace OPCTest

