/// @author Denis Štogl 2015
/// @email denis.stogl@mailbox.org
/// @brief Method services interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_Client_METHOD_H
#define OPC_UA_Client_METHOD_H

#include <opc/common/interface.h>
#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/method.h>

#include <vector>

namespace OpcUa
{

  class MethodServices : private Common::Interface
  {
  public:
    DEFINE_CLASS_POINTERS(MethodServices);

  public:
    virtual std::vector<CallMethodResult> Call(const std::vector<CallMethodRequest>& methodsToCall) = 0;
  };

} // namespace OpcUa

#endif // OPC_UA_Client_METHOD_H

