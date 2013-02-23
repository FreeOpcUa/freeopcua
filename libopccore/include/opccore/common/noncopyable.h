/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief NonCopyable class has virtual destructor against boost::noncopyable;
/// Modules are parts of project not addons.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __noncopyable_h_8e0910e4_285f_4fde_8388_be66638f9950
#define __noncopyable_h_8e0910e4_285f_4fde_8388_be66638f9950

namespace Common
{
  class NonCopyable
  {
  protected:
    NonCopyable(){}
    virtual ~NonCopyable(){}

  private:
    NonCopyable(const NonCopyable&);
    const NonCopyable& operator=(const NonCopyable&);
  };
}


#endif // __noncopyable_h_8e0910e4_285f_4fde_8388_be66638f9950
