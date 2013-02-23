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

#include <opc/opcda.h>
#include <com/com_types.h>
#include <com/impl/iunknown_impl.h>

#include <opccore/opc_client/group.h>

namespace Opc
{
  class Group
    : public Opc::IOPCGroupStateMgt
    , public Com::Impl::Unknown
  {
  public:
    Group(OpcClient::Group::SharedPtr group);

  public:
    virtual Com::HRESULT  GetState( 
      unsigned int* pUpdateRate,
      bool* pActive,
      const char**ppName,
      unsigned long**pTimeBias,
      float* pPercentDeadband,
      unsigned int* pLCID,
      OPCHANDLE *phClientGroup,
      OPCHANDLE *phServerGroup) const;
          
    virtual Com::HRESULT  SetState( 
      unsigned int* pRequestedUpdateRate,
      unsigned int* pRevisedUpdateRate,
      bool* pActive,
      long* pTimeBias,
      float* pPercentDeadband,
      int *pLCID,
      OPCHANDLE *phClientGroup);
          
    virtual Com::HRESULT SetName( 
      const char* szName);
          
    virtual Com::HRESULT  CloneGroup( 
      const char* szName,
      Com::IUnknown** ppUnk);

  public: // IUnknown
    virtual Com::HRESULT QueryInterface(const char* iid, void** result);

  private:
    OpcClient::Group::SharedPtr GroupImpl;
  };
}
  
