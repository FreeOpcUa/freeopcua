/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of Opc group object
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

#include "opc_group_interfaces_factory.h"
#include "opc_group_state_mgt.h"
#include "opc_item_mgt.h"
#include "opc_sync_io.h"

#include <opc_client/sync_io.h>

namespace
{
  class nsOPCGroup 
    : public nsIOPCGroupStateMgt
    , public nsIOPCItemMgt
    , public nsIOPCGroupInterfacesFactory
    , public nsIOPCSyncIO
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCGROUPSTATEMGT
    NS_DECL_NSIOPCITEMMGT
    NS_DECL_NSIOPCGROUPINTERFACESFACTORY
    NS_DECL_NSIOPCSYNCIO

  public:
    explicit nsOPCGroup(std::shared_ptr<OpcClient::Group> groupImpl);

  private:
    virtual ~nsOPCGroup()
    {
    }

  private:
    nsCOMPtr<nsIOPCGroupStateMgt> GroupStateMgt;
    nsCOMPtr<nsIOPCItemMgt> ItemMgt;
    nsCOMPtr<nsIOPCGroupInterfacesFactory> InterfacesFactory;
    nsCOMPtr<nsIOPCSyncIO> SyncIO;
  };

  // Implementation file
  NS_IMPL_ISUPPORTS4(nsOPCGroup, 
                     nsIOPCGroupStateMgt,
                     nsIOPCItemMgt,
                     nsIOPCGroupInterfacesFactory,
                     nsIOPCSyncIO)

  nsOPCGroup::nsOPCGroup(std::shared_ptr<OpcClient::Group> groupImpl)
    : GroupStateMgt(XPCOM::CreateGroupStateMgt(groupImpl))
    , ItemMgt(XPCOM::CreateItemMgt(groupImpl))
    , InterfacesFactory(XPCOM::CreateGroupInterfacesFactory())
    , SyncIO(XPCOM::CreateSyncIO(OpcClient::CreateSyncIO(groupImpl)))
  {
  }

  //////////////////////////////////////////////////////////////////////////////
  // nsIOPCGroupStateMgt

  // attribute unsigned long updateRate
  NS_IMETHODIMP nsOPCGroup::GetUpdateRate(PRUint32 *aUpdateRate)
  {
    return GroupStateMgt->GetUpdateRate(aUpdateRate);
  }
  NS_IMETHODIMP nsOPCGroup::SetUpdateRate(PRUint32 aUpdateRate)
  {
    return GroupStateMgt->SetUpdateRate(aUpdateRate);
  }

  // attribute boolean active
  NS_IMETHODIMP nsOPCGroup::GetActive(PRBool *aActive)
  {
    return GroupStateMgt->GetActive(aActive);
  }
  NS_IMETHODIMP nsOPCGroup::SetActive(PRBool aActive)
  {
    return GroupStateMgt->SetActive(aActive);
  }

  // attribute wstring name
  NS_IMETHODIMP nsOPCGroup::GetName(PRUnichar **aName)
  {
    return GroupStateMgt->GetName(aName);
  }
  NS_IMETHODIMP nsOPCGroup::SetName(const PRUnichar *aName)
  {
    return GroupStateMgt->SetName(aName);
  }

  // attribute long timebias
  NS_IMETHODIMP nsOPCGroup::GetTimebias(PRInt32 *aTimebias)
  {
    return GroupStateMgt->GetTimebias(aTimebias);
  }
  NS_IMETHODIMP nsOPCGroup::SetTimebias(PRInt32 aTimebias)
  {
    return GroupStateMgt->SetTimebias(aTimebias);
  }

  // attribute float deadband
  NS_IMETHODIMP nsOPCGroup::GetDeadband(float *aDeadband)
  {
    return GroupStateMgt->GetDeadband(aDeadband);
  }
  NS_IMETHODIMP nsOPCGroup::SetDeadband(float aDeadband)
  {
    return GroupStateMgt->SetDeadband(aDeadband);
  }

  // PRUint32 setState (in unsigned long requestedUpdateRate, in boolean active, in long timeBias, in float percentDeadband, in unsigned long locale)
  NS_IMETHODIMP nsOPCGroup::SetState(PRUint32 requestedUpdateRate, PRBool active, PRInt32 timeBias, float percentDeadband, PRUint32 locale, PRUint32 *_retval NS_OUTPARAM)
  {
    return GroupStateMgt->SetState(requestedUpdateRate, active, timeBias, percentDeadband, locale, _retval);
  }

  // nsISupports CloneGroup (in wstring name)
  NS_IMETHODIMP nsOPCGroup::CloneGroup(const PRUnichar *name, nsISupports **_retval NS_OUTPARAM)
  {
    return GroupStateMgt->CloneGroup(name, _retval);
  }

  //////////////////////////////////////////////////////////////////////////////
  // nsIOPCItemMgt 

  // nsIArray addItems (in nsIArray itemDefArray); 
  NS_IMETHODIMP nsOPCGroup::AddItems(nsIArray *itemDefArray, nsIArray **_retval NS_OUTPARAM)
  {
    return ItemMgt->AddItems(itemDefArray, _retval); 
  }

  // nsIArray validateItems (in nsIArray itemDefArray);
  NS_IMETHODIMP nsOPCGroup::ValidateItems(nsIArray *itemDefArray, nsIArray **_retval NS_OUTPARAM)
  {
    return ItemMgt->ValidateItems(itemDefArray, _retval);
  }

  // nsIArray removeItems (in nsIArray serverHandles);
  NS_IMETHODIMP nsOPCGroup::RemoveItems(nsIArray *serverHandles, nsIArray **_retval NS_OUTPARAM)
  {
    return ItemMgt->RemoveItems(serverHandles, _retval);
  }

  // nsIArray setActiveState (in nsIArray serverHandles, in boolean isActive);
  NS_IMETHODIMP nsOPCGroup::SetActiveState(nsIArray *serverHandles, PRBool isActive, nsIArray **_retval NS_OUTPARAM)
  {
    return ItemMgt->SetActiveState(serverHandles, isActive, _retval);
  }

  // nsIArray setClientHandles (in nsIArray serverHandles, in nsIArray clientHandles); 
  NS_IMETHODIMP nsOPCGroup::SetClientHandles(nsIArray *serverHandles, nsIArray *clientHandles, nsIArray **_retval NS_OUTPARAM)
  {
    return ItemMgt->SetClientHandles(serverHandles, clientHandles, _retval);
  }

  // nsIArray setDatatypes (in nsIArray serverHandles, in nsIArray requestedDatatypes);
  NS_IMETHODIMP nsOPCGroup::SetDataTypes(nsIArray *serverHandles, nsIArray *requestedDataTypes, nsIArray **_retval NS_OUTPARAM)
  {
    return ItemMgt->SetDataTypes(serverHandles, requestedDataTypes, _retval);
  }

  // nsISimpleEnumerator createEnumerator ();
  NS_IMETHODIMP nsOPCGroup::CreateEnumerator(nsISimpleEnumerator **_retval NS_OUTPARAM)
  {
    return ItemMgt->CreateEnumerator(_retval);
  }

  //////////////////////////////////////////////////////////////////////////////
  // nsIOPCGroupInterfacesFactory

  /// nsIOPCItemDef CreateItemDef ()
  NS_IMETHODIMP nsOPCGroup::CreateItemDef(nsIOPCItemDef **_retval NS_OUTPARAM)
  {
    return InterfacesFactory->CreateItemDef(_retval);
  }

  // nsIOPCItemVQT CreateItemVQT ()
  NS_IMETHODIMP nsOPCGroup::CreateItemVQT(nsIOPCItemVQT **_retval NS_OUTPARAM)
  {
    return InterfacesFactory->CreateItemVQT(_retval);
  }  

  //////////////////////////////////////////////////////////////////////////////
  /// nsIOPCSyncIO
  // unsigned long Read (in boolean bChache, in nsIArray phServer, out nsISupportsArray ppItemValues, out nsIArray ppErrors);
  
  // nsIArray Read (in boolean fromCache, in nsIArray serverHandles);
  NS_IMETHODIMP nsOPCGroup::Read(PRBool fromCache, nsIArray *serverHandles, nsIArray **_retval NS_OUTPARAM)
  {
    return SyncIO->Read(fromCache, serverHandles, _retval);
  }

  // nsIArray Write (in nsIArray serverHandles, in nsIArray itemValues);
  NS_IMETHODIMP nsOPCGroup::Write(nsIArray *serverHandles, nsIArray *itemValues, nsIArray **_retval NS_OUTPARAM)
  {
    return SyncIO->Write(serverHandles, itemValues, _retval);
  }
}


namespace XPCOM
{
  nsCOMPtr<nsISupports> CreateGroup(std::shared_ptr<OpcClient::Group> group)
  {
    return nsCOMPtr<nsISupports>(static_cast<nsIOPCGroupStateMgt*>(new nsOPCGroup(group)));
  }
}

