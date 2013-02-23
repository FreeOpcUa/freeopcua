/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Declaration of external functions that should be implemented for every opc server. It supplies registration information of the CoClass.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __MOZOPC_NSMODULE_H__
#define __MOZOPC_NSMODULE_H__

extern "C" const char* GetCoClassName();
extern "C" const char* GetCoClassGuid();

extern "C" const char* GetCoClassContractId();
extern "C" const char* GetCoClassProgId();

#endif // __MOZOPC_NSMODULE_H__
