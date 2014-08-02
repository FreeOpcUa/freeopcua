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
      throw std::runtime_error("NodeId allready exist");
      break;
    case StatusCode::BadSourceNodeIdInvalid:
      throw std::runtime_error("Source NodeId invalid");
      break;
    case StatusCode::BadNodeIdUnknown:
      throw std::runtime_error("NodeId unknown");
      break;
    case StatusCode::BadParentNodeIdInvalid:
      throw std::runtime_error("Parent NodeId invalid");
      break;
    case StatusCode::BadAttributeIdInvalid:
      throw std::runtime_error("Attribute id invalid");
      break;
    case StatusCode::BadSubscriptionIdInvalid:
      throw std::runtime_error("Subscription id invalid");
      break;
    case StatusCode::BadNotReadable:
      throw std::runtime_error("Attribute could not be read, it does not exist or you are not allowed to read it");
      break;
    case StatusCode::BadNotWritable:
      throw std::runtime_error("Attribute not writable, it does not exist or you are not allowed");
      break;
    case StatusCode::BadNoMatch:
      throw std::runtime_error("No match for request");
      break;
    default:
      throw std::runtime_error("Please report, CheckStatusCode does not have yet handler implemented for error: " + std::to_string((uint32_t)code));
  }
}


