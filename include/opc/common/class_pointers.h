/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Exception declarations
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __class_pointers_h__74b02386_fb4e_434a_8b54_ef3a676fac54
#define __class_pointers_h__74b02386_fb4e_434a_8b54_ef3a676fac54

#include <memory>


#define DEFINE_CLASS_POINTERS(ClassName) \
    typedef std::unique_ptr<ClassName> UniquePtr; \
    typedef std::shared_ptr<ClassName> SharedPtr; \
    typedef std::weak_ptr<ClassName> WeakPtr;

#endif // __class_pointers_h__74b02386_fb4e_434a_8b54_ef3a676fac54

