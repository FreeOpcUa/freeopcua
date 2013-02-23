//==============================================================================
// TITLE: operror.h
//
// CONTENTS:
// 
// Defines error codes for the Data Access specifications.
//
// (c) Copyright 1997-2003 The OPC Foundation
// ALL RIGHTS RESERVED.
//
// DISCLAIMER:
//  This code is provided by the OPC Foundation solely to assist in 
//  understanding and use of the appropriate OPC Specification(s) and may be 
//  used as set forth in the License Grant section of the OPC Specification.
//  This code is provided as-is and without warranty or support of any sort
//  and is subject to the Warranty and Liability Disclaimers which appear
//  in the printed OPC Specification.
//
// MODIFICATION LOG:
//
// Date       By    Notes
// ---------- ---   -----
// 1997/05/12 ACC   Removed Unused messages
//                  Added OPC_S_INUSE, OPC_E_INVALIDCONFIGFILE, OPC_E_NOTFOUND
// 1997/05/12 ACC   Added OPC_E_INVALID_PID
// 2002/08/12 CRT   Added new error codes for DA3.0
// 2003/01/02 RSA   Updated formatting. Added messages to proxy/stub resource block.
// 2003/10/12 RSA   Added error codes for complex data.
//

#ifndef __OPCERROR_H
#define __OPCERROR_H


#include <com/com_types.h>

// The 'Facility' is set to the standard for COM interfaces or FACILITY_ITF (i.e. 0x004)
// The 'Code' is set in the range defined OPC Commmon for DA (i.e. 0x0400 to 0x04FF)
// Note that for backward compatibility not all existing codes use this range.

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//

//
// Define the severity codes
//

namespace Opc
{

  //
  // MessageId: OPC_E_INVALIDHANDLE
  //
  // MessageText:
  //
  //  The value of the handle is invalid.
  //
  const Com::HRESULT OPC_E_INVALIDHANDLE                    = ((const Com::HRESULT)0xC0040001L);

  //
  // MessageId: OPC_E_BADTYPE
  //
  // MessageText:
  //
  //  The server cannot convert the data between the specified format and/or requested data type and the canonical data type. 
  //
  const Com::HRESULT OPC_E_BADTYPE                    = ((const Com::HRESULT)0xC0040004L);

  //
  // MessageId: OPC_E_PUBLIC
  //
  // MessageText:
  //
  //  The requested operation cannot be done on a public group.
  //
  const Com::HRESULT OPC_E_PUBLIC                     = ((const Com::HRESULT)0xC0040005L);

  //
  // MessageId: OPC_E_BADRIGHTS
  //
  // MessageText:
  //
  //  The item's access rights do not allow the operation.
  //
  const Com::HRESULT OPC_E_BADRIGHTS                  = ((const Com::HRESULT)0xC0040006L);

  //
  // MessageId: OPC_E_UNKNOWNITEMID
  //
  // MessageText:
  //
  //  The item ID is not defined in the server address space or no longer exists in the server address space.
  //
  const Com::HRESULT OPC_E_UNKNOWNITEMID              = ((const Com::HRESULT)0xC0040007L);

  //
  // MessageId: OPC_E_INVALIDITEMID
  //
  // MessageText:
  //
  //  The item ID does not conform to the server's syntax.
  //
  const Com::HRESULT OPC_E_INVALIDITEMID              = ((const Com::HRESULT)0xC0040008L);

  //
  // MessageId: OPC_E_INVALIDFILTER
  //
  // MessageText:
  //
  //  The filter string was not valid.
  //
  const Com::HRESULT OPC_E_INVALIDFILTER              = ((const Com::HRESULT)0xC0040009L);

  //
  // MessageId: OPC_E_UNKNOWNPATH
  //
  // MessageText:
  //
  //  The item's access path is not known to the server.
  //
  const Com::HRESULT OPC_E_UNKNOWNPATH                = ((const Com::HRESULT)0xC004000AL);

  //
  // MessageId: OPC_E_RANGE
  //
  // MessageText:
  //
  //  The value was out of range.
  //
  const Com::HRESULT OPC_E_RANGE                      = ((const Com::HRESULT)0xC004000BL);

  //
  // MessageId: OPC_E_DUPLICATENAME
  //
  // MessageText:
  //
  //  Duplicate name not allowed.
  //
  const Com::HRESULT OPC_E_DUPLICATENAME              = ((const Com::HRESULT)0xC004000CL);

  //
  // MessageId: OPC_S_UNSUPPORTEDRATE
  //
  // MessageText:
  //
  //  The server does not support the requested data rate but will use the closest available rate.
  //
  const Com::HRESULT OPC_S_UNSUPPORTEDRATE            = ((const Com::HRESULT)0x0004000DL);

  //
  // MessageId: OPC_S_CLAMP
  //
  // MessageText:
  //
  //  A value passed to write was accepted but the output was clamped.
  //
  const Com::HRESULT OPC_S_CLAMP                      = ((const Com::HRESULT)0x0004000EL);

  //
  // MessageId: OPC_S_INUSE
  //
  // MessageText:
  //
  //  The operation cannot be performed because the object is bering referenced.
  //
  const Com::HRESULT OPC_S_INUSE                      = ((const Com::HRESULT)0x0004000FL);

  //
  // MessageId: OPC_E_INVALIDCONFIGFILE
  //
  // MessageText:
  //
  //  The server's configuration file is an invalid format.
  //
  const Com::HRESULT OPC_E_INVALIDCONFIGFILE          = ((const Com::HRESULT)0xC0040010L);

  //
  // MessageId: OPC_E_NOTFOUND
  //
  // MessageText:
  //
  //  The requested object (e.g. a public group) was not found.
  //
  const Com::HRESULT OPC_E_NOTFOUND                   = ((const Com::HRESULT)0xC0040011L);

  //
  // MessageId: OPC_E_INVALID_PID
  //
  // MessageText:
  //
  //  The specified property ID is not valid for the item.
  //
  const Com::HRESULT OPC_E_INVALID_PID                = ((const Com::HRESULT)0xC0040203L);

  //
  // MessageId: OPC_E_DEADBANDNOTSET
  //
  // MessageText:
  //
  //  The item deadband has not been set for this item.
  //
  const Com::HRESULT OPC_E_DEADBANDNOTSET             = ((const Com::HRESULT)0xC0040400L);

  //
  // MessageId: OPC_E_DEADBANDNOTSUPPORTED
  //
  // MessageText:
  //
  //  The item does not support deadband.
  //
  const Com::HRESULT OPC_E_DEADBANDNOTSUPPORTED       = ((const Com::HRESULT)0xC0040401L);

  //
  // MessageId: OPC_E_NOBUFFERING
  //
  // MessageText:
  //
  //  The server does not support buffering of data items that are collected at a faster rate than the group update rate.
  //
  const Com::HRESULT OPC_E_NOBUFFERING                = ((const Com::HRESULT)0xC0040402L);

  //
  // MessageId: OPC_E_INVALIDCONTINUATIONPOINT
  //
  // MessageText:
  //
  //  The continuation point is not valid.
  //
  const Com::HRESULT OPC_E_INVALIDCONTINUATIONPOINT   = ((const Com::HRESULT)0xC0040403L);

  //
  // MessageId: OPC_S_DATAQUEUEOVERFLOW
  //
  // MessageText:
  //
  //  Not every detected change has been returned since the server's buffer reached its limit and had to purge out the oldest data.
  //
  const Com::HRESULT OPC_S_DATAQUEUEOVERFLOW          = ((const Com::HRESULT)0x00040404L);

  //
  // MessageId: OPC_E_RATENOTSET
  //
  // MessageText:
  //
  //  There is no sampling rate set for the specified item.  
  //
  const Com::HRESULT OPC_E_RATENOTSET                 = ((const Com::HRESULT)0xC0040405L);

  //
  // MessageId: OPC_E_NOTSUPPORTED
  //
  // MessageText:
  //
  //  The server does not support writing of quality and/or timestamp.
  //
  const Com::HRESULT OPC_E_NOTSUPPORTED               = ((const Com::HRESULT)0xC0040406L);

  //
  // MessageId: OPCCPX_E_TYPE_CHANGED
  //
  // MessageText:
  //
  //  The dictionary and/or type description for the item has changed.
  //
  const Com::HRESULT OPCCPX_E_TYPE_CHANGED            = ((const Com::HRESULT)0xC0040407L);

  //
  // MessageId: OPCCPX_E_FILTER_DUPLICATE
  //
  // MessageText:
  //
  //  A data filter item with the specified name already exists. 
  //
  const Com::HRESULT OPCCPX_E_FILTER_DUPLICATE        = ((const Com::HRESULT)0xC0040408L);

  //
  // MessageId: OPCCPX_E_FILTER_INVALID
  //
  // MessageText:
  //
  //  The data filter value does not conform to the server's syntax.
  //
  const Com::HRESULT OPCCPX_E_FILTER_INVALID          = ((const Com::HRESULT)0xC0040409L);

  //
  // MessageId: OPCCPX_E_FILTER_ERROR
  //
  // MessageText:
  //
  //  An error occurred when the filter value was applied to the source data.
  //
  const Com::HRESULT OPCCPX_E_FILTER_ERROR            = ((const Com::HRESULT)0xC004040AL);

  //
  // MessageId: OPCCPX_S_FILTER_NO_DATA
  //
  // MessageText:
  //
  //  The item value is empty because the data filter has excluded all fields.
  //
  const Com::HRESULT OPCCPX_S_FILTER_NO_DATA          = ((const Com::HRESULT)0x0004040BL);

} // namespace Opc

#endif // ifndef __OPCERROR_H
