/// @author Denis Štogl 2015
/// @email denis.stogl@mailbox.org
/// @brief Opc Ua Binary. Method service.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __OPC_UA_BINARY_METHOD_H__
#define __OPC_UA_BINARY_METHOD_H__

//#include <opc/ua/protocol/node_classes.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>

namespace OpcUa
{

  //---------------------------------------------------
  // Call
  //---------------------------------------------------

  struct CallMethodRequest
  {
    NodeId ObjectId;
    NodeId MethodId;
//    std::vector<BaseDataType> InputArguments; //TODO
    std::vector<Variant> InputArguments;

    CallMethodRequest();
  };

//  struct CallParameters
//  {
//    std::vector<CallMethodRequest> MethodsToCall;
//  };

  struct CallRequest
  {
    NodeId TypeId;
    RequestHeader Header;
//    CallParamters Parameters;
    std::vector<CallMethodRequest> MethodsToCall;
    
    CallRequest();
  };

  struct CallMethodResult
  {
    StatusCode Status;
    std::vector<StatusCode> InputArgumentResults;
    DiagnosticInfoList InputArgumentDiagnosticInfos;
//    std::vector<BaseDataType> OutputArguments; //TODO
    std::vector<Variant> OutputArguments;

    CallMethodResult();

  };

  struct CallResponse
  {
    NodeId TypeId;
    ResponseHeader Header;

    std::vector<CallMethodResult> Results;
    DiagnosticInfoList Diagnostics;

    CallResponse();
  };

} // namespace OpcUa

#endif // __OPC_UA_BINARY_METHOD_H__
