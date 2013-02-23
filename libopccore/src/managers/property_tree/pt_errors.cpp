/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Property tree errors
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <opccore/managers/property_tree/errors.h>
#include <opccore/common/modules.h>

#define PROPERTY_TREE_ERROR_CODE(code) ERROR_CODE(Common::MODULE_PROPERTY_TREE, code)
#define PROPERTY_TREE_ERROR(name, code, message) Common::ErrorData name(PROPERTY_TREE_ERROR_CODE(code), message)

PROPERTY_TREE_ERROR(ObjectExistButShouldnt,     1, "Object exist, but shouldn't.");
PROPERTY_TREE_ERROR(ObjectDoesntExistButShould, 2, "Object doesn't exist, but should.");
PROPERTY_TREE_ERROR(PropertyDoesntExist,        3, "Property %1% doesn't exist.");
PROPERTY_TREE_ERROR(PropertyAlreadyExist,       4, "Property %1% already exist.");
PROPERTY_TREE_ERROR(IllegalPropertyTreePath,    5, "Illegal path to property tree element.");
PROPERTY_TREE_ERROR(PropertyTreePathShouldStartFromSlash,      6, "Property tree path should start from shash.");
PROPERTY_TREE_ERROR(UnableToRenameRootGroup,                   7, "Unable to rename property tree root group.");
PROPERTY_TREE_ERROR(UnableToRenameParentAlreadyContainElement, 8, "Unable to rename group '%2%' to '%1%': parent group already contain such element");
