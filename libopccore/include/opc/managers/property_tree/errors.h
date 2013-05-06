/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Property tree errors definitions
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __errors_h__f8b7e021_ca73_48aa_95dd_3476586286a2
#define __errors_h__f8b7e021_ca73_48aa_95dd_3476586286a2

#include <opc/common/exception.h>

#define DEFINE_PROPERTY_TREE_ERROR(name) extern Common::ErrorData name;

DEFINE_PROPERTY_TREE_ERROR(ObjectExistButShouldnt);
DEFINE_PROPERTY_TREE_ERROR(ObjectDoesntExistButShould);
DEFINE_PROPERTY_TREE_ERROR(PropertyDoesntExist);
DEFINE_PROPERTY_TREE_ERROR(PropertyAlreadyExist);
DEFINE_PROPERTY_TREE_ERROR(IllegalPropertyTreePath);
DEFINE_PROPERTY_TREE_ERROR(PropertyTreePathShouldStartFromSlash);
DEFINE_PROPERTY_TREE_ERROR(UnableToRenameRootGroup);
DEFINE_PROPERTY_TREE_ERROR(UnableToRenameParentAlreadyContainElement);

#endif // __errors_h__f8b7e021_ca73_48aa_95dd_3476586286a2

