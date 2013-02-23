/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCServer
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "opc_server.h"

#include "errors.h"
#include "ns_simple_enumerator.h"
#include "opc_browse.h"
#include "opc_group.h"
#include "opc_server_status.h"
#include "xp_strings.h"

#include <common/addons_core/addon_manager.h>
#include <common/addons_core/addon_ids.h>
#include <managers/error_manager/manager.h>
#include <nsXPCOM.h>
#include <opc_client/server.h>

#include <iostream>

namespace XPCOM
{
  NS_IMPL_ISUPPORTS2(
    nsOPCServer, 
    nsIOPCServer, 
    nsIOPCBrowse);

  nsOPCServer::nsOPCServer()
    : Server(OpcClient::CreateServer())
    , OpcBrowse(CreateOpcBrowse())
  {
    std::cout << "Server created" << std::endl;
  }

  nsOPCServer::~nsOPCServer()
  {
    std::cout << "Server Destroyed" << std::endl;
  }

  nsresult nsOPCServer::ProcessError(const Common::Error& err)
  {
    return NS_ERROR_FAILURE;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //////////////////////////     nsIOPCServer    /////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  // wstring getLastError ();
  NS_IMETHODIMP nsOPCServer::GetLastError(PRUnichar **_retval)
  {
    try
    {
      NS_ENSURE_ARG_POINTER(_retval);
      ErrorManager::Manager::SharedPtr manager = Common::GetAddon<ErrorManager::Manager>(Common::ADDON_ID_ERROR_MANAGER);
      const std::string& errorMessage = manager->GetLastError().GetFullMessage();
      *_retval = ToPRUnichar(errorMessage);
      return NS_OK;
    }
    catch (const Common::Error& error)
    {
      return NS_ERROR_FAILURE;
    }
  }

  // wstring getErrorString (in unsigned long error); 
  NS_IMETHODIMP nsOPCServer::GetErrorString(PRUint32 error, PRUnichar **_retval)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(_retval);
    return NS_ERROR_NOT_IMPLEMENTED;
    END_TRY_BLOCK(ProcessError);
  }

  // nsIOPCServerStatus getStatus ();
  NS_IMETHODIMP nsOPCServer::GetStatus(nsIOPCServerStatus **_retval)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(_retval);
    *_retval = NULL;

    std::unique_ptr<nsOPCServerStatus> serverStatusPtr(new nsOPCServerStatus(Server->GetStatus()));
    const nsresult result = serverStatusPtr->QueryInterface(NS_GET_IID(nsIOPCServerStatus), (void**)_retval);
    serverStatusPtr.release();
    return result;
    END_TRY_BLOCK(ProcessError);
  }

  // nsISupports addGroup (in wstring szName, in boolean active, in PRUint32 requestedUpdateRate, in PRUint32 timeBias, in float percentDeadband);
  NS_IMETHODIMP nsOPCServer::AddGroup(const PRUnichar *szName, PRBool active, PRUint32 requestedUpdateRate, PRUint32 timeBias, float percentDeadband, nsISupports **_retval)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(szName);
    NS_ENSURE_ARG_POINTER(_retval);
  
    const std::string& groupName = ToStdString(szName);
    std::shared_ptr<OpcClient::Group> group = Server->AddGroup(groupName);

    OpcClient::GroupProperties::SharedPtr groupProperties = group->GetProperties();
    groupProperties->SetIsActive(active == PR_TRUE);
    groupProperties->SetUpdateRate(static_cast<unsigned>(requestedUpdateRate));
    groupProperties->SetTimeBias(static_cast<unsigned>(timeBias));
    groupProperties->SetDeadband(percentDeadband);
    *_retval = CreateGroup(group).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }

  // nsISupports getGroupByName (in wstring name); 
  NS_IMETHODIMP nsOPCServer::GetGroupByName(const PRUnichar *name, nsISupports **_retval)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(_retval);
    NS_ENSURE_ARG_POINTER(name);
    const std::string& groupName = ToStdString(name);
    *_retval = CreateGroup(Server->GetGroupByName(groupName)).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }
} // namespace XPCOM

namespace
{
  void AddGroups(std::vector<nsCOMPtr<nsISupports> >& result, const std::vector<std::shared_ptr<OpcClient::Group>>& groups)
  {
    for (std::vector<std::shared_ptr<OpcClient::Group>>::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
      result.push_back(XPCOM::CreateGroup(*groupIt));
    }
  }
} // namespace

namespace XPCOM
{
  // nsISupports CreateGroupEnumerator (in boolean bAll, in nsIDPtr riid);
  NS_IMETHODIMP nsOPCServer::CreateGroupEnumerator(PRBool bAll, const nsID * riid, nsISupports **_retval)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(_retval);
    NS_ENSURE_ARG_POINTER(riid);
    /// TODO: Add support of nsIStringEnumerator
    //NS_ENSURE_TRUE(riid->Equals(NS_GET_IID(nsIStringEnumerator)), NS_ERROR_INVALID_ARG);
    NS_ENSURE_TRUE(riid->Equals(NS_GET_IID(nsISimpleEnumerator)), NS_ERROR_INVALID_ARG);

    std::vector<nsCOMPtr<nsISupports> > result;
    AddGroups(result, Server->GetPublicGroups());
    if (bAll)
    {
      AddGroups(result, Server->GetPrivateGroups());
    }
    *_retval = XPCOM::CreateSimpleEnumerator(result).forget().get();
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }
} // namespace XPCOM

namespace
{
  std::string GetGroupName(nsISupports* ptr)
  {
    nsCOMPtr<nsIOPCGroupStateMgt> group = do_QueryInterface(ptr);
    if (!group)
    {
      THROW_ERROR(ObjectDoesntSupport_nsIOPCGroupStateMgt);
    }
    PRUnichar* name = NULL;
    nsresult err = group->GetName(&name);
    if (err != NS_OK)
    {
      THROW_ERROR1(XPCOMError, err);
    }
    const std::string& resultName = XPCOM::ToStdString(name);
    NS_Free(name);
    return resultName;
  }
} // namespace

namespace XPCOM
{
  // boolean removeGroup (in nsISupports group);
  NS_IMETHODIMP nsOPCServer::RemoveGroup(nsISupports *group, PRBool *_retval)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(_retval);
    NS_ENSURE_ARG_POINTER(group);

    const std::string& groupName = GetGroupName(group);
    if (std::shared_ptr<OpcClient::Group> group = Server->FindGroupByName(groupName))
    {
      Server->RemoveGroup(groupName);
      *_retval = PR_TRUE;
      return NS_OK;
    }
    *_retval = PR_FALSE;
    return NS_OK;
    END_TRY_BLOCK(ProcessError);
  }

  ////////////////////////////////////////////////////////////////////////////////
  /////////////////////////    nsIOPCBrowse   ////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  // nsISimpleEnumerator browse (in wstring szItemID, in unsigned short dwBrowseFilter, in wstring szElementNameFilter, in wstring szVendorFilter, in boolean bAllProperties, in boolean bPropertiesValues, in nsIArray propertiesIDs);
  NS_IMETHODIMP nsOPCServer::Browse(const PRUnichar *szItemID, PRUint16 dwBrowseFilter, const PRUnichar *szElementNameFilter, const PRUnichar *szVendorFilter, PRBool bAllProperties, PRBool bPropertiesValues, nsIArray *propertiesIDs, nsISimpleEnumerator **_retval NS_OUTPARAM)
   {
    return OpcBrowse->Browse(szItemID, dwBrowseFilter, szElementNameFilter, szVendorFilter, bAllProperties, bPropertiesValues, propertiesIDs, _retval);
  }

  // nsIArray getProperties (in wstring itemID);
  NS_IMETHODIMP nsOPCServer::GetProperties(const PRUnichar *itemID, nsIArray **_retval NS_OUTPARAM)
  {
    return OpcBrowse->GetProperties(itemID, _retval);
  }

} // namespace XPCOM
