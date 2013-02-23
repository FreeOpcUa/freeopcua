/// @author Alexander Rykovanov 2010
/// @email rykovanov.as@gmail.com
/// @brief Realization of nsIOPCGroupStateMgt interface
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "opc_group_state_mgt.h"
#include "xp_strings.h"

#include <mozopc.h>

namespace
{
  nsresult ProcessError(const Common::Error& err)
  {
    return NS_ERROR_FAILURE;
  }

  class nsOPCGroupStateMgt 
    : public nsIOPCGroupStateMgt
  {
  public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIOPCGROUPSTATEMGT

    explicit nsOPCGroupStateMgt(std::shared_ptr<OpcClient::Group> group);

  private:
    virtual ~nsOPCGroupStateMgt()
    {
    }

  private:
    std::shared_ptr<OpcClient::Group> Group;
  };

  NS_IMPL_ISUPPORTS1(nsOPCGroupStateMgt, nsIOPCGroupStateMgt)

  nsOPCGroupStateMgt::nsOPCGroupStateMgt(std::shared_ptr<OpcClient::Group> group)
    : Group(group)
  {
  }

  // attribute unsigned long updateRate
  NS_IMETHODIMP nsOPCGroupStateMgt::GetUpdateRate(PRUint32 *aUpdateRate)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(aUpdateRate);
    *aUpdateRate = static_cast<PRUint32>(Group->GetProperties()->GetUpdateRate());
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetUpdateRate(PRUint32 aUpdateRate)
  {
    BEGIN_TRY_BLOCK
    Group->GetProperties()->SetUpdateRate(static_cast<unsigned>(aUpdateRate));
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }

  // attribute boolean active
  NS_IMETHODIMP nsOPCGroupStateMgt::GetActive(PRBool *aActive)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(aActive);
    *aActive = static_cast<PRBool>(Group->GetProperties()->IsActive());
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetActive(PRBool aActive)
  {
    BEGIN_TRY_BLOCK
    Group->GetProperties()->SetIsActive(static_cast<bool>(aActive));
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }

  // attribute wstring name;
  NS_IMETHODIMP nsOPCGroupStateMgt::GetName(PRUnichar **aName)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(aName);
    *aName= XPCOM::ToPRUnichar(Group->GetName());
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetName(const PRUnichar *aName)
  {
    BEGIN_TRY_BLOCK
    Group->SetName(XPCOM::ToStdString(aName));
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }

  /* attribute long timebias; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetTimebias(PRInt32 *aTimebias)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(aTimebias);
    *aTimebias = static_cast<PRInt32>(Group->GetProperties()->GetTimeBias());
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetTimebias(PRInt32 aTimebias)
  {
    BEGIN_TRY_BLOCK
    Group->GetProperties()->SetTimeBias(static_cast<unsigned>(aTimebias));
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }

  /* attribute float deadband; */
  NS_IMETHODIMP nsOPCGroupStateMgt::GetDeadband(float *aDeadband)
  {
    BEGIN_TRY_BLOCK
    NS_ENSURE_ARG_POINTER(aDeadband);
    *aDeadband = Group->GetProperties()->GetDeadband();
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }
  NS_IMETHODIMP nsOPCGroupStateMgt::SetDeadband(float aDeadband)
  {
    BEGIN_TRY_BLOCK
    Group->GetProperties()->SetDeadband(aDeadband);
    return NS_OK;
    END_TRY_BLOCK(ProcessError)
  }

  /* PRUint32 setState (in unsigned long requestedUpdateRate, in boolean active, in long timeBias, in float percentDeadband, in unsigned long locale); */
  NS_IMETHODIMP nsOPCGroupStateMgt::SetState(PRUint32 requestedUpdateRate, PRBool active, PRInt32 timeBias, float percentDeadband, PRUint32 locale, PRUint32 *_retval NS_OUTPARAM)
  {
    NS_ENSURE_ARG_POINTER(_retval);
    nsresult rv = NS_OK;

    rv = SetUpdateRate(requestedUpdateRate);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = SetActive(active);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = SetTimebias(timeBias);
    NS_ENSURE_SUCCESS(rv, rv);
    
    rv = SetDeadband(percentDeadband);
    NS_ENSURE_SUCCESS(rv, rv);

    // TODO locale
    *_retval = requestedUpdateRate;    
    return NS_OK;
  }

  /* nsISupports CloneGroup (in wstring name); */
  NS_IMETHODIMP nsOPCGroupStateMgt::CloneGroup(const PRUnichar *name, nsISupports **_retval NS_OUTPARAM)
  {
    return NS_ERROR_NOT_IMPLEMENTED;
  }
} // namespace

namespace XPCOM
{
  nsCOMPtr<nsIOPCGroupStateMgt> CreateGroupStateMgt(std::shared_ptr<OpcClient::Group> group)
  {
    return nsCOMPtr<nsIOPCGroupStateMgt>(new nsOPCGroupStateMgt(group));
  }
}

