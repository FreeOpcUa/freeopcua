/// @brief OpcUa Error codes.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/protocol/status_codes.h>

#include <stdexcept>

void OpcUa::CheckStatusCode(StatusCode code)
{
  switch (code)
  {
    case StatusCode::Good:
      return;
    case StatusCode::BadNodeIdExists:
      throw std::runtime_error("Error NodeId allready exist");
      break;
    case StatusCode::BadSourceNodeIdInvalid:
      throw std::runtime_error("Error source NodeId invalid");
      break;
    case StatusCode::BadNodeIdUnknown:
      throw std::runtime_error("Error NodeId unknown");
      break;
    case StatusCode::BadParentNodeIdInvalid:
      throw std::runtime_error("Error parent NodeId invalid");
      break;
    case StatusCode::BadAttributeIdInvalid:
      throw std::runtime_error("Error attribute id invalid");
      break;
    case StatusCode::BadSubscriptionIdInvalid:
      throw std::runtime_error("Error subscription id invalid");
      break;
    default:
      throw std::runtime_error("Error: " + std::to_string((uint32_t)code));
  }
}


