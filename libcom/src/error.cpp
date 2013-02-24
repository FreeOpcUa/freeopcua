/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Error implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <com/impl/error.h>
#include <iostream>


void Com::Impl::ProcessError(Com::HRESULT code, const char* message)
{
  std::cerr << "Error (" << code << "): " << message << std::endl;
  std::cerr.flush();
}

