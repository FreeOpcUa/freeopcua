/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IOPCServer implementation
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "opc_server.h"

#include "opc_group.h"

#include <com/com_errors.h>

#include <opccore/opc_client/server.h>
#include <string.h>

namespace
{
  Com::HRESULT ConvertCommonError(const Common::Error)
  {
    return Com::COM_INTERNAL_ERROR;
  }  
}


namespace Opc
{
  Server::Server(OpcClient::Server::UniquePtr serverImpl)
    : ServerImpl(std::move(serverImpl))
  {
  }

  Com::HRESULT Server::AddGroup( 
    const char* name,
    bool isActive,
    unsigned requestedUpdateRate,
    OPCHANDLE clientGroupHandle,
    const long* pTimeBias,
    const float* pPercentDeadband,
    unsigned localeId,
    Com::IUnknown** ppUnk)
  {
    try
    {
      if (name == nullptr || !strlen(name))
      {
        return Com::COM_INVALID_PARAMETER1;
      }
      if (ppUnk == nullptr)
      {
        return Com::COM_INVALID_PARAMETER8;
      }
      
      OpcClient::Group::SharedPtr group = ServerImpl->AddGroup(name);
      OpcClient::GroupProperties::SharedPtr properties = group->GetProperties();
      properties->SetIsActive(isActive);
      properties->SetUpdateRate(requestedUpdateRate);
      if (pTimeBias)
      {
        properties->SetTimeBias(*pTimeBias);
      }

      if (pPercentDeadband)
      {
        properties->SetDeadband(*pPercentDeadband);
      }

      Com::IUnknown::UniquePtr unk = Com::UniquePtr<Com::IUnknown>(new Opc::Group(group));
      
      *ppUnk = unk.release();
      return Com::COM_SUCCESS;
    }
    catch(const Common::Error& error)
    {
      return ConvertCommonError(error);
    }
  }
        
  Com::HRESULT Server::GetErrorString( 
    Com::HRESULT error,
    unsigned locale,
    const char** message) const
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
        
  Com::HRESULT Server::GetGroupByName( 
    const char* name,
    Com::IUnknown** group) const
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
        
  Com::HRESULT Server::GetStatus( 
    OPCSERVERSTATUS* ppServerStatus) const
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
        
  Com::HRESULT Server::RemoveGroup( 
    OPCHANDLE serverGroup,
    bool force)
  {
    return Com::COM_NOT_IMPLEMENTED;
  }
        
  Com::HRESULT Server::CreateGroupEnumerator( 
   OPCENUMSCOPE dwScope,
   Com::IUnknownEnumerator** ppUnk)
  {
    return Com::COM_NOT_IMPLEMENTED;
  }

  Com::HRESULT Server::QueryInterface(const char* iid, void** result)
  {
    void* tmp = nullptr;
    Com::HRESULT res = Unknown::QueryInterface(iid, result);
    if (res == Com::COM_SUCCESS || res != Com::COM_NOINTERFACE)
    {
      return res;
    }

    if (!strcmp(iid, Opc::IID_IOPCServer))
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

} // namespace Opc

