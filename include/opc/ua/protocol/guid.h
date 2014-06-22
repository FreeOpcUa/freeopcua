/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Guid.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef opcua_guid_h_
#define opcua_guid_h_

#include <stdint.h>
#include <string>

namespace OpcUa
{
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

    inline bool operator== (const Guid& r) const
    {
      for (unsigned i = 0; i < 8; ++i)
      {
        if (Data4[i] != r.Data4[i])
        {
          return false;
        }
      }

      return Data1 == r.Data1 && Data2 == r.Data2 && Data3 == r.Data3;
    }

    inline bool operator< (const Guid& r) const
    {
      if (Data1 > r.Data1)
      {
        return false;
      }

      if (Data1 < r.Data1)
      {
        return true;
      }

      if (Data2 > r.Data2)
      {
        return false;
      }

      if (Data2 < r.Data2)
      {
        return true;
      }

      if (Data3 > r.Data3)
      {
        return false;
      }

      if (Data3 < r.Data3)
      {
        return true;
      }

      for (unsigned i = 0; i < 8; ++i)
      {
        if (Data4[i] > r.Data4[i])
        {
          return false;
        }

        if (Data4[i] < r.Data4[i])
        {
          return true;
        }
      }

      return false;
    }

  };

} // namespace OpcUa

#endif // opcua_guid_h_

