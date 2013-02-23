/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief C++ types of binary protocol.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $


#ifndef __OPC_UA_MAPPING_TYPES_H__
#define __OPC_UA_MAPPING_TYPES_H__

#include <opc/ua/binary/datetime.h>

#include <stdint.h>
#include <string>
#include <vector>

namespace OpcUa
{
  namespace Binary
  {

    typedef uint32_t StatusCode;

    struct Guid
    {
      uint32_t Data1;
      uint16_t Data2;
      uint16_t Data3;
      uint8_t  Data4[8];

      Guid()
        : Data1(0)
        , Data2(0)
        , Data3(0)
      {
        for (unsigned i = 0; i < 8; ++i)
        {
          Data4[i] = 0;
        }
      }
    };

    inline bool operator== (const Guid& l, const Guid&r)
    {
      for (unsigned i = 0; i < 8; ++i)
      {
        if (l.Data4[i] != r.Data4[i])
        {
          return false;
        }
      }

      return l.Data1 == r.Data1 && l.Data2 == r.Data2 && l.Data3 == r.Data3;
    }

    struct QualifiedName
    {
      uint16_t NamespaceIndex;
      std::string Name;

      QualifiedName()
        : NamespaceIndex(0)
      {
      }
    };

    // LocalizedText encoding mask
    const uint8_t HAS_LOCALE = 1;
    const uint8_t HAS_TEXT = 2;

    struct LocalizedText
    {
      uint8_t Encoding;
      std::string Locale;
      std::string Text;
    };
  } // namespace Binary
} // namespace OpcUa

#endif // __OPC_UA_MAPPING_TYPES_H__
