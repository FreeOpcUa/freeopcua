/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Opc group implemntaion
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "opc_group.h"

#include <com/com_errors.h>

#include <opccore/opc_client/server.h>

#include <string.h>

namespace Opc
{
  Group::Group(OpcClient::Group::SharedPtr group)
    : GroupImpl(group)
  {
  }

  Com::HRESULT Group::GetState( 
      unsigned int* pUpdateRate,
      bool* pActive,
      const char**ppName,
      unsigned long**pTimeBias,
      float* pPercentDeadband,
      unsigned int* pLCID,
      OPCHANDLE *phClientGroup,
      OPCHANDLE *phServerGroup) const
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
          
  Com::HRESULT Group::SetState( 
      unsigned int* pRequestedUpdateRate,
      unsigned int* pRevisedUpdateRate,
      bool* pActive,
      long* pTimeBias,
      float* pPercentDeadband,
      int *pLCID,
      OPCHANDLE *phClientGroup)
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
          
  Com::HRESULT Group::SetName(const char* szName)
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
          
  Com::HRESULT Group::CloneGroup(const char* newGroupName, Com::IUnknown** ppUnk)
  {
    return Com::COM_NOT_IMPLEMENTED;
  }

  Com::HRESULT Group::QueryInterface(const char* iid, void** result)
  {
    void* tmp = nullptr;
    Com::HRESULT res = Unknown::QueryInterface(iid, result);
    if (res == Com::COM_SUCCESS || res != Com::COM_NOINTERFACE)
    {
      return res;
    }

    if (!strcmp(iid, Opc::IID_IOPCGroupStateMgt))
    {
      tmp = reinterpret_cast<void*>(this);
    }
    
    if (tmp != nullptr)
    {
      AddRef();
      *result = tmp;
      return Com::COM_SUCCESS;
    }

    return Com::COM_NOINTERFACE;
  }

}

