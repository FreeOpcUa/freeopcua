/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/gpl.html)
///

#ifndef OPC_UA_SERVER_STANDARD_NAME_SPACE_H
#define OPC_UA_SERVER_STANDARD_NAME_SPACE_H

#include <opc/ua/view.h>

#include <memory>

namespace OpcUa
{

    std::unique_ptr<Remote::ViewServices> CreateStandardNamespace();

} // namespace OpcUa

#endif // OPC_UA_SERVER_STANDARD_NAME_SPACE_H

