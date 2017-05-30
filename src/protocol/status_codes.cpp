/// @brief OpcUa Error codes.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///


#include <opc/ua/protocol/status_codes.h>

#include <opc/ua/protocol/string_utils.h>

#include <stdexcept>
#include <string>

void OpcUa::CheckStatusCode(StatusCode code)
{
  if (code == StatusCode::Good)
    { return; }

  throw std::runtime_error(OpcUa::ToString(code));
}


