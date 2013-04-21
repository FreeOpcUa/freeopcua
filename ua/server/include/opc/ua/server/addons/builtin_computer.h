/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Addon for accessing opcua server inside process.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opc_ua_builtin_addon_addon_h
#define opc_ua_builtin_addon_addon_h

#include <opc/ua/computer.h>
#include <opccore/common/addons_core/addon.h>

namespace OpcUa
{
  namespace Server
  {

    class BuiltinComputerAddon : public Common::Addon
    {
    public:
      virtual std::shared_ptr<Remote::Computer> GetComputer() const = 0;
    };

    const char BuiltinComputerAddonID[] = "builtin_computer";

  } // namespace Server
} // namespace OpcUa

#endif // opc_ua_builtin_addon_addon_h

