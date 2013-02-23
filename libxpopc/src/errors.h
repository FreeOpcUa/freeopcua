/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCBrowse
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __errors_h__0d958525_e383_4cff_922b_a8f6e54d52eb
#define __errors_h__0d958525_e383_4cff_922b_a8f6e54d52eb

#include <common/exception.h>

#define DEFINE_XPCOM_ERROR(name) extern Common::ErrorData name;

DEFINE_XPCOM_ERROR(InvalidBrowserFilterValue);
DEFINE_XPCOM_ERROR(XPCOMError);
DEFINE_XPCOM_ERROR(ObjectDoesntSupport_nsIOPCGroupStateMgt);
DEFINE_XPCOM_ERROR(VariantTypeInvalid);
DEFINE_XPCOM_ERROR(FailedToProcessElementWithNumber);
DEFINE_XPCOM_ERROR(ArraysLengthNotEquals);
DEFINE_XPCOM_ERROR(UnableToGetArrayLength);

// nsIOPCItemMgt
DEFINE_XPCOM_ERROR(UnableToGetAccessPath);
DEFINE_XPCOM_ERROR(UnableToGetItemID);
DEFINE_XPCOM_ERROR(UnableToGetIsActive);
DEFINE_XPCOM_ERROR(UnableToGetClientHandle);
DEFINE_XPCOM_ERROR(UnableToGetRequestedDataType);
DEFINE_XPCOM_ERROR(UnableToGetServerHandle);
DEFINE_XPCOM_ERROR(UnableToGetValueForWriting);



#endif // __errors_h__0d958525_e383_4cff_922b_a8f6e54d52eb
