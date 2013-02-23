#include "errors.h"
#include <common/modules.h>

#define XPCOM_MODULE_ERROR_CODE(code) ERROR_CODE(Common::MODULE_XPCOM, code)
#define XPCOM_ERROR(name, code, message) Common::ErrorData name(XPCOM_MODULE_ERROR_CODE(code), message)

XPCOM_ERROR(InvalidBrowserFilterValue,               1, "Invalid browser filter value '%1%'.");
XPCOM_ERROR(XPCOMError,                              2, "nsresult =  %1%.");
XPCOM_ERROR(ObjectDoesntSupport_nsIOPCGroupStateMgt, 3, "Passed object doesn't support iterface nsIOPCGroupStateMgt.");
XPCOM_ERROR(VariantTypeInvalid,                      4, "Invalid variant type '%1%'.");
XPCOM_ERROR(FailedToProcessElementWithNumber,        5, "Failed to process element with number '%1%'");
XPCOM_ERROR(ArraysLengthNotEquals,                   6, "Passed arrays have different lengths.");
XPCOM_ERROR(UnableToGetArrayLength,                  7, "Error during retreiving array length.");

// nsIOPCItemMgt
XPCOM_ERROR(UnableToGetAccessPath,                   100, "Unable to get access path.");
XPCOM_ERROR(UnableToGetItemID,                       101, "Unable to get itemID.");
XPCOM_ERROR(UnableToGetIsActive,                     102, "Unable to get activity flag.");
XPCOM_ERROR(UnableToGetClientHandle,                 103, "Unable to get client handle value.");
XPCOM_ERROR(UnableToGetRequestedDataType,            104, "Unable to get requested data type.");
XPCOM_ERROR(UnableToGetServerHandle,                 105, "Unable to get server handle value.");
XPCOM_ERROR(UnableToGetValueForWriting,              106, "Unable to get value for writing.");

