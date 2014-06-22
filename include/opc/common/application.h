/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///


#pragma once

#include <opc/common/addons_core/addon_manager.h>
#include <opc/common/class_pointers.h>
#include <opc/common/interface.h>

namespace OpcUa
{
  class Application : private Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(Application);

  public:
    virtual void Start(const std::vector<Common::AddonInformation>& configuration) = 0;
    virtual Common::AddonsManager& GetAddonsManager() = 0;

    virtual void Stop() = 0;
  };

  Application::UniquePtr CreateApplication();
}
