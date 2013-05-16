/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OpcUa Error codes.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///


#ifndef opc_ua_errors_h
#define opc_ua_errors_h

namespace OpcUa
{
  enum class StatusCode : uint32_t
  {
    Good                 = 0,
    BadNotImplemented    = 0x80440000,
    BadWriteNotSupported = 0x80730000,
    BadNotReadable       = 0x806F0000,
  };
}

#endif // opc_ua_errors_h

