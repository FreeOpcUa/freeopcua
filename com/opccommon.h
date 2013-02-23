/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Thu Oct 01 14:19:30 1998
 */
/* Compiler settings for opccomn.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __opccomn_h__
#define __opccomn_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IOPCShutdown_FWD_DEFINED__
#define __IOPCShutdown_FWD_DEFINED__
typedef interface IOPCShutdown IOPCShutdown;
#endif 	/* __IOPCShutdown_FWD_DEFINED__ */


#ifndef __IOPCCommon_FWD_DEFINED__
#define __IOPCCommon_FWD_DEFINED__
typedef interface IOPCCommon IOPCCommon;
#endif 	/* __IOPCCommon_FWD_DEFINED__ */


#ifndef __IOPCServerList_FWD_DEFINED__
#define __IOPCServerList_FWD_DEFINED__
typedef interface IOPCServerList IOPCServerList;
#endif 	/* __IOPCServerList_FWD_DEFINED__ */


#ifndef __IOPCCommon_FWD_DEFINED__
#define __IOPCCommon_FWD_DEFINED__
typedef interface IOPCCommon IOPCCommon;
#endif 	/* __IOPCCommon_FWD_DEFINED__ */


#ifndef __IOPCShutdown_FWD_DEFINED__
#define __IOPCShutdown_FWD_DEFINED__
typedef interface IOPCShutdown IOPCShutdown;
#endif 	/* __IOPCShutdown_FWD_DEFINED__ */


#ifndef __IOPCServerList_FWD_DEFINED__
#define __IOPCServerList_FWD_DEFINED__
typedef interface IOPCServerList IOPCServerList;
#endif 	/* __IOPCServerList_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "comcat.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IOPCShutdown_INTERFACE_DEFINED__
#define __IOPCShutdown_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IOPCShutdown
 * at Thu Oct 01 14:19:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IOPCShutdown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("F31DFDE1-07B6-11d2-B2D8-0060083BA1FB")
    IOPCShutdown : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ShutdownRequest( 
            /* [string][in] */ LPCWSTR szReason) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOPCShutdownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOPCShutdown __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOPCShutdown __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOPCShutdown __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownRequest )( 
            IOPCShutdown __RPC_FAR * This,
            /* [string][in] */ LPCWSTR szReason);
        
        END_INTERFACE
    } IOPCShutdownVtbl;

    interface IOPCShutdown
    {
        CONST_VTBL struct IOPCShutdownVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOPCShutdown_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOPCShutdown_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOPCShutdown_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOPCShutdown_ShutdownRequest(This,szReason)	\
    (This)->lpVtbl -> ShutdownRequest(This,szReason)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOPCShutdown_ShutdownRequest_Proxy( 
    IOPCShutdown __RPC_FAR * This,
    /* [string][in] */ LPCWSTR szReason);


void __RPC_STUB IOPCShutdown_ShutdownRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOPCShutdown_INTERFACE_DEFINED__ */


#ifndef __IOPCCommon_INTERFACE_DEFINED__
#define __IOPCCommon_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IOPCCommon
 * at Thu Oct 01 14:19:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IOPCCommon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("F31DFDE2-07B6-11d2-B2D8-0060083BA1FB")
    IOPCCommon : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetLocaleID( 
            /* [in] */ LCID dwLcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocaleID( 
            /* [out] */ LCID __RPC_FAR *pdwLcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryAvailableLocaleIDs( 
            /* [out] */ DWORD __RPC_FAR *pdwCount,
            /* [size_is][size_is][out] */ LCID __RPC_FAR *__RPC_FAR *pdwLcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetErrorString( 
            /* [in] */ HRESULT dwError,
            /* [string][out] */ LPWSTR __RPC_FAR *ppString) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClientName( 
            /* [string][in] */ LPCWSTR szName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOPCCommonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOPCCommon __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOPCCommon __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOPCCommon __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLocaleID )( 
            IOPCCommon __RPC_FAR * This,
            /* [in] */ LCID dwLcid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLocaleID )( 
            IOPCCommon __RPC_FAR * This,
            /* [out] */ LCID __RPC_FAR *pdwLcid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryAvailableLocaleIDs )( 
            IOPCCommon __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwCount,
            /* [size_is][size_is][out] */ LCID __RPC_FAR *__RPC_FAR *pdwLcid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetErrorString )( 
            IOPCCommon __RPC_FAR * This,
            /* [in] */ HRESULT dwError,
            /* [string][out] */ LPWSTR __RPC_FAR *ppString);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetClientName )( 
            IOPCCommon __RPC_FAR * This,
            /* [string][in] */ LPCWSTR szName);
        
        END_INTERFACE
    } IOPCCommonVtbl;

    interface IOPCCommon
    {
        CONST_VTBL struct IOPCCommonVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOPCCommon_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOPCCommon_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOPCCommon_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOPCCommon_SetLocaleID(This,dwLcid)	\
    (This)->lpVtbl -> SetLocaleID(This,dwLcid)

#define IOPCCommon_GetLocaleID(This,pdwLcid)	\
    (This)->lpVtbl -> GetLocaleID(This,pdwLcid)

#define IOPCCommon_QueryAvailableLocaleIDs(This,pdwCount,pdwLcid)	\
    (This)->lpVtbl -> QueryAvailableLocaleIDs(This,pdwCount,pdwLcid)

#define IOPCCommon_GetErrorString(This,dwError,ppString)	\
    (This)->lpVtbl -> GetErrorString(This,dwError,ppString)

#define IOPCCommon_SetClientName(This,szName)	\
    (This)->lpVtbl -> SetClientName(This,szName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOPCCommon_SetLocaleID_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [in] */ LCID dwLcid);


void __RPC_STUB IOPCCommon_SetLocaleID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_GetLocaleID_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [out] */ LCID __RPC_FAR *pdwLcid);


void __RPC_STUB IOPCCommon_GetLocaleID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_QueryAvailableLocaleIDs_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwCount,
    /* [size_is][size_is][out] */ LCID __RPC_FAR *__RPC_FAR *pdwLcid);


void __RPC_STUB IOPCCommon_QueryAvailableLocaleIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_GetErrorString_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [in] */ HRESULT dwError,
    /* [string][out] */ LPWSTR __RPC_FAR *ppString);


void __RPC_STUB IOPCCommon_GetErrorString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCCommon_SetClientName_Proxy( 
    IOPCCommon __RPC_FAR * This,
    /* [string][in] */ LPCWSTR szName);


void __RPC_STUB IOPCCommon_SetClientName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOPCCommon_INTERFACE_DEFINED__ */


#ifndef __IOPCServerList_INTERFACE_DEFINED__
#define __IOPCServerList_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IOPCServerList
 * at Thu Oct 01 14:19:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IOPCServerList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("13486D50-4821-11D2-A494-3CB306C10000")
    IOPCServerList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumClassesOfCategories( 
            /* [in] */ ULONG cImplemented,
            /* [size_is][in] */ CATID __RPC_FAR rgcatidImpl[  ],
            /* [in] */ ULONG cRequired,
            /* [size_is][in] */ CATID __RPC_FAR rgcatidReq[  ],
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClassDetails( 
            /* [in] */ REFCLSID clsid,
            /* [out] */ LPOLESTR __RPC_FAR *ppszProgID,
            /* [out] */ LPOLESTR __RPC_FAR *ppszUserType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CLSIDFromProgID( 
            /* [in] */ LPCOLESTR szProgId,
            /* [out] */ LPCLSID clsid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOPCServerListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOPCServerList __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOPCServerList __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOPCServerList __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumClassesOfCategories )( 
            IOPCServerList __RPC_FAR * This,
            /* [in] */ ULONG cImplemented,
            /* [size_is][in] */ CATID __RPC_FAR rgcatidImpl[  ],
            /* [in] */ ULONG cRequired,
            /* [size_is][in] */ CATID __RPC_FAR rgcatidReq[  ],
            /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetClassDetails )( 
            IOPCServerList __RPC_FAR * This,
            /* [in] */ REFCLSID clsid,
            /* [out] */ LPOLESTR __RPC_FAR *ppszProgID,
            /* [out] */ LPOLESTR __RPC_FAR *ppszUserType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CLSIDFromProgID )( 
            IOPCServerList __RPC_FAR * This,
            /* [in] */ LPCOLESTR szProgId,
            /* [out] */ LPCLSID clsid);
        
        END_INTERFACE
    } IOPCServerListVtbl;

    interface IOPCServerList
    {
        CONST_VTBL struct IOPCServerListVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOPCServerList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOPCServerList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOPCServerList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOPCServerList_EnumClassesOfCategories(This,cImplemented,rgcatidImpl,cRequired,rgcatidReq,ppenumClsid)	\
    (This)->lpVtbl -> EnumClassesOfCategories(This,cImplemented,rgcatidImpl,cRequired,rgcatidReq,ppenumClsid)

#define IOPCServerList_GetClassDetails(This,clsid,ppszProgID,ppszUserType)	\
    (This)->lpVtbl -> GetClassDetails(This,clsid,ppszProgID,ppszUserType)

#define IOPCServerList_CLSIDFromProgID(This,szProgId,clsid)	\
    (This)->lpVtbl -> CLSIDFromProgID(This,szProgId,clsid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOPCServerList_EnumClassesOfCategories_Proxy( 
    IOPCServerList __RPC_FAR * This,
    /* [in] */ ULONG cImplemented,
    /* [size_is][in] */ CATID __RPC_FAR rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][in] */ CATID __RPC_FAR rgcatidReq[  ],
    /* [out] */ IEnumGUID __RPC_FAR *__RPC_FAR *ppenumClsid);


void __RPC_STUB IOPCServerList_EnumClassesOfCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCServerList_GetClassDetails_Proxy( 
    IOPCServerList __RPC_FAR * This,
    /* [in] */ REFCLSID clsid,
    /* [out] */ LPOLESTR __RPC_FAR *ppszProgID,
    /* [out] */ LPOLESTR __RPC_FAR *ppszUserType);


void __RPC_STUB IOPCServerList_GetClassDetails_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOPCServerList_CLSIDFromProgID_Proxy( 
    IOPCServerList __RPC_FAR * This,
    /* [in] */ LPCOLESTR szProgId,
    /* [out] */ LPCLSID clsid);


void __RPC_STUB IOPCServerList_CLSIDFromProgID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOPCServerList_INTERFACE_DEFINED__ */



#ifndef __OPCCOMN_LIBRARY_DEFINED__
#define __OPCCOMN_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: OPCCOMN
 * at Thu Oct 01 14:19:30 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 






EXTERN_C const IID LIBID_OPCCOMN;
#endif /* __OPCCOMN_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
