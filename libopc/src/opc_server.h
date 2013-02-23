/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IOPCServer implemntaion
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

#include <opccore/opc_client/server.h>

namespace Opc
{
  class Server
    : public Opc::IOPCServer
    , public Com::Impl::Unknown
  {
  public:
    Server(OpcClient::Server::UniquePtr serverImpl);

    static std::string GetClassId();

  public:
    virtual Com::HRESULT AddGroup( 
      const char* name,
      bool isActive,
      unsigned requestedUpdateRate,
      OPCHANDLE clientGroupHandle,
      const long* pTimeBias,
      const float* pPercentDeadband,
      unsigned localeId,
      Com::IUnknown** ppUnk);
          
    virtual Com::HRESULT GetErrorString( 
      Com::HRESULT error,
      unsigned locale,
      const char** message) const;
          
    virtual Com::HRESULT GetGroupByName( 
      const char* name,
      Com::IUnknown** group) const;
          
    virtual Com::HRESULT GetStatus( 
      OPCSERVERSTATUS* ppServerStatus) const;
          
    virtual Com::HRESULT RemoveGroup( 
      OPCHANDLE serverGroup,
      bool force);
          
    virtual Com::HRESULT CreateGroupEnumerator( 
      OPCENUMSCOPE dwScope,
      Com::IUnknownEnumerator** ppUnk);

  public: // IUnknown
    virtual Com::HRESULT QueryInterface(const char* iid, void** result);

  private:
    const OpcClient::Server::UniquePtr ServerImpl;
  };
}  // namespace Com

