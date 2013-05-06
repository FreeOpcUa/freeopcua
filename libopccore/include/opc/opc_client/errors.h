/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Group interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __errors_h_5eb5e373_9dc9_4cec_b2a7_1ddcfe345ae8
#define __errors_h_5eb5e373_9dc9_4cec_b2a7_1ddcfe345ae8

#include <opc/common/exception.h>

#define DEFINE_OPCCLIENT_ERROR(name) extern Common::ErrorData name;

DEFINE_OPCCLIENT_ERROR(GroupAlreadyExist);
DEFINE_OPCCLIENT_ERROR(GroupNotFound);
DEFINE_OPCCLIENT_ERROR(InvalidServerID);
DEFINE_OPCCLIENT_ERROR(ItemPathNotFound);
DEFINE_OPCCLIENT_ERROR(InvalidAccessPath);
DEFINE_OPCCLIENT_ERROR(BlobsUnsupported);
DEFINE_OPCCLIENT_ERROR(NothingToRead);
DEFINE_OPCCLIENT_ERROR(NothingToWrite);

#endif // __errors_h_5eb5e373_9dc9_4cec_b2a7_1ddcfe345ae8

