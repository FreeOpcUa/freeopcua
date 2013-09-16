/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Common errors declarations
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __errors_h__63e60e1e_bdf1_41e4_a354_0c2f71af5fec
#define __errors_h__63e60e1e_bdf1_41e4_a354_0c2f71af5fec

#include <opc/common/exception.h>

#define DEFINE_COMMON_ERROR(name) extern Common::ErrorData name;

DEFINE_COMMON_ERROR(InvalidValueType);
DEFINE_COMMON_ERROR(UnsupportedFeature);
DEFINE_COMMON_ERROR(NotImplemented);
DEFINE_COMMON_ERROR(StdException);
DEFINE_COMMON_ERROR(CannotParseUri);


#endif // __errors_h__63e60e1e_bdf1_41e4_a354_0c2f71af5fec

