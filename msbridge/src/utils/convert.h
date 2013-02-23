// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __MOZOPCBRIDGE_CONVERT_H__
#define __MOZOPCBRIDGE_CONVERT_H__

namespace MsOpcBridge
{
  // deafult convertion
  template<typename ToType> 
  class Convert
  {
  public:
    inline ToType operator()(const ToType& obj) const
    {
      return obj;
    }

    template <typename FromType>
    inline ToType operator()(const FromType& from) const
    {
      return static_cast<ToType>(from);
    }
  };
}

#endif  // __MOZOPCBRIDGE_CONVERT_H__