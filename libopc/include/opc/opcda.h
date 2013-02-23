#ifndef __opcda_h__
#define __opcda_h__

#include <opc/opcerror.h>

#include <com/istring_enumerator.h>
#include <com/iunknown_enumerator.h>
#include <com/ivariant.h>

#include <cstddef>


namespace Opc
{

  typedef unsigned int OPCHANDLE;

  typedef enum tagOPCDATASOURCE
  {
    OPC_DS_CACHE	= 1,
    OPC_DS_DEVICE = OPC_DS_CACHE + 1,
  } OPCDATASOURCE;

  typedef enum tagOPCBROWSETYPE
  {
    OPC_BRANCH = 1,
    OPC_LEAF   = OPC_BRANCH + 1,
    OPC_FLAT   = OPC_LEAF + 1
  } OPCBROWSETYPE;

  typedef enum tagOPCNAMESPACETYPE
  {
    OPC_NS_HIERARCHIAL = 1,
    OPC_NS_FLAT = OPC_NS_HIERARCHIAL + 1,
  } OPCNAMESPACETYPE;

  typedef enum tagOPCBROWSEDIRECTION
  {
    OPC_BROWSE_UP	= 1,
    OPC_BROWSE_DOWN = OPC_BROWSE_UP + 1,
    OPC_BROWSE_TO = OPC_BROWSE_DOWN + 1,
  } OPCBROWSEDIRECTION;

  typedef enum tagOPCEUTYPE
  {
    OPC_NOENUM	 = 0,
    OPC_ANALOG	 = OPC_NOENUM + 1,
    OPC_ENUMERATED = OPC_ANALOG + 1
  } OPCEUTYPE;

  typedef enum tagOPCSERVERSTATE
  {
    OPC_STATUS_RUNNING = 1,
    OPC_STATUS_FAILED  = OPC_STATUS_RUNNING + 1,
    OPC_STATUS_NOCONFIG = OPC_STATUS_FAILED + 1,
    OPC_STATUS_SUSPENDED = OPC_STATUS_NOCONFIG + 1,
    OPC_STATUS_TEST = OPC_STATUS_SUSPENDED + 1,
    OPC_STATUS_COMM_FAULT = OPC_STATUS_TEST + 1
  } OPCSERVERSTATE;

  typedef enum tagOPCENUMSCOPE
  {
    OPC_ENUM_PRIVATE_CONNECTIONS = 1,
    OPC_ENUM_PUBLIC_CONNECTIONS = OPC_ENUM_PRIVATE_CONNECTIONS + 1,
    OPC_ENUM_ALL_CONNECTIONS = OPC_ENUM_PUBLIC_CONNECTIONS + 1,
    OPC_ENUM_PRIVATE = OPC_ENUM_ALL_CONNECTIONS + 1,
    OPC_ENUM_PUBLIC = OPC_ENUM_PRIVATE + 1,
    OPC_ENUM_ALL = OPC_ENUM_PUBLIC + 1
  } OPCENUMSCOPE;

  typedef struct tagOPCGROUPHEADER
  {
    std::size_t size;
    std::size_t itemCount;
    OPCHANDLE hClientGroup;
    unsigned dwTransactionID;
    Com::HRESULT hrStatus;
  } OPCGROUPHEADER;

  typedef struct tagOPCITEMHEADER1
  {
    OPCHANDLE hClient;
    std::size_t dwValueOffset;
    unsigned short wQuality;
    unsigned short wReserved;
    Com::FILETIME ftTimeStampItem;
  } OPCITEMHEADER1;

  typedef struct tagOPCITEMHEADER2
  {
    OPCHANDLE hClient;
    unsigned int dwValueOffset;
    unsigned short wQuality;
    unsigned short wReserved;
  } OPCITEMHEADER2;

  typedef struct tagOPCGROUPHEADERWRITE
  {
    std::size_t dwItemCount;
    OPCHANDLE hClientGroup;
    unsigned int dwTransactionID;
    Com::HRESULT hrStatus;
  } OPCGROUPHEADERWRITE;

  typedef struct tagOPCITEMHEADERWRITE
  {
    OPCHANDLE hClient;
    Com::HRESULT dwError;
  } OPCITEMHEADERWRITE;

  typedef struct tagOPCITEMSTATE
  {
    OPCHANDLE hClient;
    Com::FILETIME ftTimeStamp;
    short wQuality;
    short wReserved;
    Com::IVariant* vDataValue;
  } OPCITEMSTATE;

  typedef struct tagOPCSERVERSTATUS
  {
    Com::FILETIME ftStartTime;
    Com::FILETIME ftCurrentTime;
    Com::FILETIME ftLastUpdateTime;
    OPCSERVERSTATE dwServerState;
    std::size_t dwGroupCount;
    unsigned int dwBandWidth;
    short wMajorVersion;
    short wMinorVersion;
    short wBuildNumber;
    short wReserved;
    const char* vendorInfo;
  } OPCSERVERSTATUS;

  typedef struct tagOPCITEMDEF
  {
    const char* szAccessPath;
    const char* szItemID;
    bool bActive;
    OPCHANDLE hClient;
    unsigned int dwBlobSize;
    const void* pBlob;
    Com::VARTYPE vtRequestedDataType;
    short wReserved;
  } OPCITEMDEF;

  typedef struct tagOPCITEMATTRIBUTES
  {
    const char* accessPath;
    const char* szItemID;
    bool bActive;
    OPCHANDLE hClient;
    OPCHANDLE hServer;
    unsigned int dwAccessRights;
    std::size_t dwBlobSize;
    const void* pBlob;
    Com::VARTYPE vtRequestedDataType;
    Com::VARTYPE vtCanonicalDataType;
    OPCEUTYPE dwEUType;
    Com::IVariant* vEUInfo;
  } OPCITEMATTRIBUTES;

  typedef struct tagOPCITEMRESULT
  {
    OPCHANDLE hServer;
    Com::VARTYPE vtCanonicalDataType;
    unsigned int wReserved;
    unsigned int dwAccessRights;
    std::size_t dwBlobSize;
    const void* pBlob;
  } OPCITEMRESULT;

  typedef struct tagOPCITEMPROPERTY
  {
    Com::VARTYPE DataType;
    unsigned int Reserved;
    unsigned int dwPropertyID;
    const char* szItemID;
    const char* szDescription;
    Com::IVariant* vValue;
    Com::HRESULT hrErrorID;
    unsigned int dwReserved;
  } OPCITEMPROPERTY;

  typedef struct tagOPCITEMPROPERTIES
  {
    Com::HRESULT hrErrorID;
    std::size_t dwNumProperties;
    OPCITEMPROPERTY *pItemProperties;
    unsigned int dwReserved;
  } OPCITEMPROPERTIES;

  typedef struct tagOPCBROWSEELEMENT
  {
    const char* szName;
    const char* szItemID;
    unsigned int dwFlagValue;
    unsigned int dwReserved;
    OPCITEMPROPERTIES ItemProperties;
  } OPCBROWSEELEMENT;

  typedef struct tagOPCITEMVQT
  {
    Com::IVariant* vDataValue;
    bool bQualitySpecified;
    short wQuality;
    short wReserved;
    bool bTimeStampSpecified;
    Com::FILETIME ftTimeStamp;
  } OPCITEMVQT;

  typedef enum tagOPCBROWSEFILTER
  {
   OPC_BROWSE_FILTER_ALL = 1,
   OPC_BROWSE_FILTER_BRANCHES = OPC_BROWSE_FILTER_ALL + 1,
   OPC_BROWSE_FILTER_ITEMS = OPC_BROWSE_FILTER_BRANCHES + 1
  } OPCBROWSEFILTER;

    
  const char IID_IOPCServer[] = "39c13a4d-011e-11d0-9675-0020afd8adb3";

  class IOPCServer : public virtual Com::IUnknown
  {
  public:
   DECLARE_INTERFACE_POINTERS(IOPCServer);

  public:
    virtual Com::HRESULT AddGroup( 
      const char* name,
      bool isActive,
      unsigned requestedUpdateRate,
      OPCHANDLE clientGroupHandle,
      const long* pTimeBias,
      const float* pPercentDeadband,
      unsigned localeId,
      Com::IUnknown** ppUnk) = 0;
          
    virtual Com::HRESULT GetErrorString( 
      Com::HRESULT error,
      unsigned locale,
      const char** message) const = 0;
          
    virtual Com::HRESULT GetGroupByName( 
      const char* name,
      Com::IUnknown** group) const = 0;
          
    virtual Com::HRESULT GetStatus( 
      OPCSERVERSTATUS* ppServerStatus) const = 0;
          
    virtual Com::HRESULT RemoveGroup( 
      OPCHANDLE serverGroup,
      bool force) = 0;
          
    virtual Com::HRESULT CreateGroupEnumerator( 
      OPCENUMSCOPE dwScope,
      Com::IUnknownEnumerator** ppUnk) = 0;
  };

  const char IID_IOPCServerPublicGroups[] = "39c13a4e-011e-11d0-9675-0020afd8adb3";

  class IOPCServerPublicGroups : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCServerPublicGroups);

  public:
    virtual Com::HRESULT GetPublicGroupByName( 
      const char* name,
      const char* riid,
      Com::IUnknown** ppUnk) = 0;
          
    virtual Com::HRESULT RemovePublicGroup( 
      OPCHANDLE hServerGroup,
      bool bForce) = 0;
     
      };
      
  const char IID_IOPCBrowseServerAddressSpace[] = "39c13a4f-011e-11d0-9675-0020afd8adb3";

  class IOPCBrowseServerAddressSpace : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCBrowseServerAddressSpace);

  public:
    virtual Com::HRESULT QueryOrganization( 
      OPCNAMESPACETYPE* pNameSpaceType) const = 0;
          
    virtual Com::HRESULT ChangeBrowsePosition( 
      OPCBROWSEDIRECTION dwBrowseDirection,
      const char* szString) = 0;
          
    virtual Com::HRESULT BrowseOPCItemIDs( 
      OPCBROWSETYPE dwBrowseFilterType,
      const char* szFilterCriteria,
      Com::VARTYPE vtDataTypeFilter,
      unsigned int dwAccessRightsFilter,
      Com::IStringEnumerator** ppIEnumString) const = 0;
          
    virtual Com::HRESULT GetItemID( 
      const char* szItemDataID,
      const char*szItemID) const = 0;
          
    virtual Com::HRESULT  BrowseAccessPaths( 
      const char* szItemID,
      Com::IStringEnumerator** ppIEnumString) const = 0;
  };
     
  const char IID_IOPCGroupStateMgt[] = "39c13a50-011e-11d0-9675-0020afd8adb3";

  class IOPCGroupStateMgt : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCGroupStateMgt);

  public:
    virtual Com::HRESULT  GetState( 
      unsigned int* pUpdateRate,
      bool* pActive,
      const char**ppName,
      unsigned long**pTimeBias,
      float* pPercentDeadband,
      unsigned int* pLCID,
      OPCHANDLE *phClientGroup,
      OPCHANDLE *phServerGroup) const = 0;
          
    virtual Com::HRESULT  SetState( 
      unsigned int* pRequestedUpdateRate,
      unsigned int* pRevisedUpdateRate,
      bool* pActive,
      long* pTimeBias,
      float* pPercentDeadband,
      int *pLCID,
      OPCHANDLE *phClientGroup) = 0;
          
    virtual Com::HRESULT SetName( 
      const char* szName) = 0;
          
    virtual Com::HRESULT  CloneGroup( 
      const char* szName,
      Com::IUnknown** ppUnk) = 0;
  };
      
  const char IID_IOPCPublicGroupStateMgt[] = "39c13a51-011e-11d0-9675-0020afd8adb3";

  class IOPCPublicGroupStateMgt : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCPublicGroupStateMgt);

  public:
    virtual Com::HRESULT GetState( 
      bool *pPublic) const = 0;
          
    virtual Com::HRESULT MoveToPublic() = 0;
  };
      
     
  const char IID_IOPCSyncIO[] = "39c13a52-011e-11d0-9675-0020afd8adb3";

  class IOPCSyncIO : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCSyncIO);

  public:
    virtual Com::HRESULT Read( 
      OPCDATASOURCE dwSource,
      std::size_t dwCount,
      OPCHANDLE *phServer,
      OPCITEMSTATE **ppItemValues,
      Com::HRESULT **ppErrors) const = 0;
          
    virtual Com::HRESULT  Write( 
      std::size_t dwCount,
      OPCHANDLE *phServer,
      Com::IVariant* pItemValues,
      Com::HRESULT** ppErrors) = 0;
  };
      
     
  const char IID_IOPCAsyncIO[] = "39c13a53-011e-11d0-9675-0020afd8adb3";
  class IOPCAsyncIO : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCAsyncIO);

  public:
    virtual Com::HRESULT Read( 
      unsigned int dwConnection,
      OPCDATASOURCE dwSource,
      std::size_t dwCount,
      OPCHANDLE* phServer,
      unsigned int* pTransactionID,
      Com::HRESULT* ppErrors) const = 0;
          
    virtual Com::HRESULT Write( 
      unsigned int dwConnection,
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      Com::IVariant *pItemValues,
      unsigned int *pTransactionID,
      Com::HRESULT **ppErrors) = 0;
          
    virtual Com::HRESULT Refresh( 
      unsigned int dwConnection,
      OPCDATASOURCE dwSource,
      unsigned int* pTransactionID) = 0;
          
    virtual Com::HRESULT Cancel( 
       unsigned int dwTransactionID) = 0;
  };
      
  const char IID_IOPCItemMgt[] = "39c13a54-011e-11d0-9675-0020afd8adb3";
  class IOPCItemMgt : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCItemMgt);

  public:
    virtual Com::HRESULT AddItems( 
      std::size_t dwCount,
      const OPCITEMDEF* pItemArray,
      OPCITEMRESULT** ppAddResults,
      Com::HRESULT** ppErrors) = 0;
          
     virtual Com::HRESULT ValidateItems( 
       std::size_t dwCount,
       const OPCITEMDEF* pItemArray,
       bool bBlobUpdate,
       OPCITEMRESULT** ppValidationResults,
       Com::HRESULT** ppErrors) const = 0;
          
     virtual Com::HRESULT RemoveItems( 
       std::size_t dwCount,
       const OPCHANDLE *phServer,
       Com::HRESULT **ppErrors) = 0;
          
     virtual Com::HRESULT  SetActiveState( 
       std::size_t dwCount,
       const OPCHANDLE* phServer,
       bool bActive,
       Com::HRESULT** ppErrors) = 0;
          
     virtual Com::HRESULT  SetClientHandles( 
       std::size_t dwCount,
       const OPCHANDLE* phServer,
       const OPCHANDLE* phClient,
       Com::HRESULT **ppErrors) = 0;
          
     virtual Com::HRESULT  SetDatatypes( 
       std::size_t dwCount,
       const OPCHANDLE* phServer,
       const Com::VARTYPE* pRequestedDatatypes,
       Com::HRESULT** ppErrors) = 0;
          
     virtual Com::HRESULT  CreateEnumerator( 
       const char* riid,
       Com::IUnknown** ppUnk) = 0;
  };
      
     
  const char IID_IEnumOPCItemAttributes[] = "39c13a55-011e-11d0-9675-0020afd8adb3";

  class IEnumOPCItemAttributes : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IEnumOPCItemAttributes);

  public:
    virtual Com::HRESULT  Next( 
      const unsigned long celt,
      OPCITEMATTRIBUTES** ppItemArray,
      unsigned long* pceltFetched) = 0;
          
    virtual Com::HRESULT  Skip( 
      unsigned long celt) = 0;
          
    virtual Com::HRESULT  Reset() = 0;
          
    virtual Com::HRESULT  Clone( 
      IEnumOPCItemAttributes **ppEnumItemAttributes) const = 0;
  };
      
  const char IID_IOPCDataCallback[] = "39c13a70-011e-11d0-9675-0020afd8adb3";

  class IOPCDataCallback : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCDataCallback);

  public:
    virtual Com::HRESULT  OnDataChange( 
      unsigned int dwTransid,
      OPCHANDLE hGroup,
      Com::HRESULT hrMasterquality,
      Com::HRESULT hrMastererror,
      std::size_t dwCount,
      const OPCHANDLE *phClientItems,
      const Com::IVariant *pvValues,
      const short *pwQualities,
      const Com::FILETIME *pftTimeStamps,
      const Com::HRESULT *pErrors) = 0;
          
    virtual Com::HRESULT  OnReadComplete( 
      unsigned int dwTransid,
      OPCHANDLE hGroup,
      Com::HRESULT hrMasterquality,
      Com::HRESULT hrMastererror,
      std::size_t dwCount,
      const OPCHANDLE *phClientItems,
      const Com::IVariant *pvValues,
      const short *pwQualities,
      const Com::FILETIME *pftTimeStamps,
      const Com::HRESULT *pErrors) = 0;
          
    virtual Com::HRESULT  OnWriteComplete( 
      unsigned int dwTransid,
      OPCHANDLE hGroup,
      Com::HRESULT hrMastererr,
      std::size_t dwCount,
      const OPCHANDLE *pClienthandles,
      const Com::HRESULT *pErrors) = 0;
          
      virtual Com::HRESULT  OnCancelComplete( 
      unsigned int dwTransid,
      OPCHANDLE hGroup) = 0;
  };
      
  const char IOPCAsync2[] = "39c13a71-011e-11d0-9675-0020afd8adb3";

  class IOPCAsyncIO2 : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCAsyncIO2);

  public:
    virtual Com::HRESULT  Read( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      unsigned int dwTransactionID,
      unsigned int* pdwCancelID,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  Write( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      const Com::IVariant *pItemValues,
      unsigned int dwTransactionID,
      unsigned int *pdwCancelID,
      Com::HRESULT **ppErrors) = 0;
          
    virtual Com::HRESULT  Refresh2( 
      OPCDATASOURCE dwSource,
      unsigned int dwTransactionID,
      unsigned int* pdwCancelID) = 0;
          
    virtual Com::HRESULT  Cancel2( 
      unsigned int dwCancelID) = 0;
          
    virtual Com::HRESULT  SetEnable( 
      bool bEnable) = 0;
          
    virtual Com::HRESULT  GetEnable( 
      bool* pbEnable) const = 0;
  };
      
  const char IID_IOPCItemProperties[] = "39c13a72-011e-11d0-9675-0020afd8adb3";
  class IOPCItemProperties : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCItemProperties);

  public:
    virtual Com::HRESULT  QueryAvailableProperties( 
      const char* szItemID,
      std::size_t* pdwCount,
      unsigned int* ppPropertyIDs,
      const char** ppDescriptions,
      Com::VARTYPE* ppvtDataTypes) = 0;
          
    virtual Com::HRESULT  GetItemProperties( 
      const char* szItemID,
      std::size_t dwCount,
      unsigned int* pdwPropertyIDs,
      Com::IVariant* ppvData,
      Com::HRESULT* ppErrors) = 0;
          
     virtual Com::HRESULT  LookupItemIDs( 
       const char* szItemID,
       std::size_t dwCount,
       const unsigned int* pdwPropertyIDs,
       const char** ppszNewItemIDs,
       Com::HRESULT* ppErrors) = 0;
  };
      
  const char IID_IOPCItemDeadbandMgt[] = "5946DA93-8B39-4ec8-AB3D-AA73DF5BC86F";

  class IOPCItemDeadbandMgt : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCItemDeadbandMgt);

  public:
    virtual Com::HRESULT  SetItemDeadband( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      const float *pPercentDeadband,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  GetItemDeadband( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      float *ppPercentDeadband,
      Com::HRESULT *ppErrors) = 0;
          
    virtual Com::HRESULT  ClearItemDeadband( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      Com::HRESULT* ppErrors) = 0;
  };
      

  const char IID_IOPCItemSamplingMgt[] = "3E22D313-F08B-41a5-86C8-95E95CB49FFC";

  class IOPCItemSamplingMgt : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCItemSamplingMgt);

  public:
    virtual Com::HRESULT  SetItemSamplingRate( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      const unsigned int* pdwRequestedSamplingRate,
      unsigned int* ppdwRevisedSamplingRate,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  GetItemSamplingRate( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      unsigned int* ppdwSamplingRate,
      Com::HRESULT* ppErrors) const = 0;
          
    virtual Com::HRESULT  ClearItemSamplingRate( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  SetItemBufferEnable( 
      std::size_t dwCount,
      const OPCHANDLE* phServer,
      const bool *pbEnable,
      Com::HRESULT *ppErrors) = 0;
          
    virtual Com::HRESULT  GetItemBufferEnable( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      bool *ppbEnable,
      Com::HRESULT *ppErrors) const = 0;
  };
     
  const char IID_OPCBrowse[] = "39227004-A18F-4b57-8B0A-5235670F4468";
  class IOPCBrowse : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCBrowse);

  public:
    virtual Com::HRESULT  GetProperties( 
      std::size_t dwItemCount,
      const char** pszItemIDs,
      bool bReturnPropertyValues,
      std::size_t dwPropertyCount,
      const unsigned int *pdwPropertyIDs,
      OPCITEMPROPERTIES *ppItemProperties) const = 0;
          
    virtual Com::HRESULT  Browse( 
      const char* szItemID,
      const char* pszContinuationPoint,
      unsigned int dwMaxElementsReturned,
      OPCBROWSEFILTER dwBrowseFilter,
      const char* szElementNameFilter,
      const char* szVendorFilter,
      bool bReturnAllProperties,
      bool bReturnPropertyValues,
      std::size_t dwPropertyCount,
      const unsigned int *pdwPropertyIDs,
      bool *pbMoreElements,
      std::size_t *pdwCount,
      OPCBROWSEELEMENT *ppBrowseElements) = 0;
  };
      
  const char IID_IOPCItemIO[] = "85C0B427-2893-4cbc-BD78-E5FC5146F08F";
  class IOPCItemIO : public virtual Com::IUnknown
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCItemIO);

  public:
    virtual Com::HRESULT  Read( 
      std::size_t dwCount,
      const char** pszItemIDs,
      const unsigned int* pdwMaxAge,
      Com::IVariant* ppvValues,
      short* ppwQualities,
      Com::FILETIME* ppftTimeStamps,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  WriteVQT( 
      std::size_t dwCount,
      const char** pszItemIDs,
      OPCITEMVQT *pItemVQT,
      Com::HRESULT *ppErrors) = 0;
  };
      
  const char IID_IOPCSyncIO2[] = "730F5F0F-55B1-4c81-9E18-FF8A0904E1FA";

  class IOPCSyncIO2 : public IOPCSyncIO
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCSyncIO2);

  public:
    virtual Com::HRESULT  ReadMaxAge( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      const unsigned int *pdwMaxAge,
      Com::IVariant* ppvValues,
      short* ppwQualities,
      Com::FILETIME* ppftTimeStamps,
      Com::HRESULT *ppErrors) = 0;
          
    virtual Com::HRESULT  WriteVQT( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      const OPCITEMVQT *pItemVQT,
      Com::HRESULT* ppErrors) = 0;
  };
      
  const char IID_IOPCAsyncIO3[] = "0967B97B-36EF-423e-B6F8-6BFF1E40D39D";
  class IOPCAsyncIO3 : public IOPCAsyncIO2
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCAsyncIO3);

  public:
    virtual Com::HRESULT  ReadMaxAge( 
      std::size_t dwCount,
      const OPCHANDLE* phServer,
      const unsigned int* pdwMaxAge,
      unsigned int dwTransactionID,
      unsigned int* pdwCancelID,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  WriteVQT( 
      std::size_t dwCount,
      const OPCHANDLE *phServer,
      const OPCITEMVQT *pItemVQT,
      unsigned int dwTransactionID,
      unsigned int* pdwCancelID,
      Com::HRESULT* ppErrors) = 0;
          
    virtual Com::HRESULT  RefreshMaxAge( 
      unsigned int dwMaxAge,
      unsigned int dwTransactionID,
      unsigned int* pdwCancelID) = 0;
  };
      
     
  const char IID_IOPCGroupStateMgt2[] = "8E368666-D72E-4f78-87ED-647611C61C9F";
  class IOPCGroupStateMgt2 : public IOPCGroupStateMgt
  {
  public:
    DECLARE_INTERFACE_POINTERS(IOPCGroupStateMgt2);

  public:
    virtual Com::HRESULT  SetKeepAlive( 
      unsigned int dwKeepAliveTime,
      unsigned int* pdwRevisedKeepAliveTime) = 0;
          
    virtual Com::HRESULT  GetKeepAlive( 
      unsigned int* pdwKeepAliveTime) const = 0;
  };
      
  const char OPC_CATEGORY_DESCRIPTION_DA10[] = "OPC Data Access Servers Version 1.0";

  const char OPC_CATEGORY_DESCRIPTION_DA20[] = "OPC Data Access Servers Version 2.0";

  const char OPC_CATEGORY_DESCRIPTION_DA30[] = "OPC Data Access Servers Version 3.0";

  const char OPC_CATEGORY_DESCRIPTION_XMLDA10[] = "OPC XML Data Access Servers Version 1.0";

  const unsigned int OPC_READABLE = 0x1;

  const unsigned int OPC_WRITEABLE = 0x2;

  const unsigned int OPC_BROWSE_HASCHILDREN = 0x1;

  const unsigned int OPC_BROWSE_ISITEM = 0x2;

  const char OPC_TYPE_SYSTEM_OPCBINARY[] = "OPCBinary";

  const char OPC_TYPE_SYSTEM_XMLSCHEMA[] = "XMLSchema";

  const char OPC_CONSISTENCY_WINDOW_UNKNOWN[] = "Unknown";

  const char OPC_CONSISTENCY_WINDOW_NOT_CONSISTENT[] = "Not Consistent";

  const char OPC_WRITE_BEHAVIOR_BEST_EFFORT[] = "Best Effort";

  const char OPC_WRITE_BEHAVIOR_ALL_OR_NOTHING[] = "All or Nothing";


  // module OPCDA_Qualities


  const short OPC_QUALITY_MASK	=	0xc0;

  const short OPC_STATUS_MASK	=	0xfc;

  const short OPC_LIMIT_MASK	=	0x3;

  const short OPC_QUALITY_BAD	=	0;

  const short OPC_QUALITY_UNCERTAIN	=	0x40;

  const short OPC_QUALITY_GOOD	=	0xc0;

  const short OPC_QUALITY_CONFIG_ERROR	=	0x4;

  const short OPC_QUALITY_NOT_CONNECTED	=	0x8;

  const short OPC_QUALITY_DEVICE_FAILURE	=	0xc;

  const short OPC_QUALITY_SENSOR_FAILURE	=	0x10;

  const short OPC_QUALITY_LAST_KNOWN	=	0x14;

  const short OPC_QUALITY_COMM_FAILURE	=	0x18;

  const short OPC_QUALITY_OUT_OF_SERVICE	=	0x1c;

  const short OPC_QUALITY_WAITING_FOR_INITIAL_DATA	=	0x20;

  const short OPC_QUALITY_LAST_USABLE	=	0x44;

  const short OPC_QUALITY_SENSOR_CAL	=	0x50;

  const short OPC_QUALITY_EGU_EXCEEDED	=	0x54;

  const short OPC_QUALITY_SUB_NORMAL	=	0x58;

  const short OPC_QUALITY_LOCAL_OVERRIDE	=	0xd8;

  const short OPC_LIMIT_OK	=	0;

  const short OPC_LIMIT_LOW	=	0x1;

  const short OPC_LIMIT_HIGH	=	0x2;

  const short OPC_LIMIT_CONST	=	0x3;


  // module OPCDA_Properties 

  const unsigned int OPC_PROPERTY_DATATYPE	=	1;

  const unsigned int OPC_PROPERTY_VALUE	=	2;

  const unsigned int OPC_PROPERTY_QUALITY	=	3;

  const unsigned int OPC_PROPERTY_TIMESTAMP	=	4;

  const unsigned int OPC_PROPERTY_ACCESS_RIGHTS	=	5;

  const unsigned int OPC_PROPERTY_SCAN_RATE	=	6;

  const unsigned int OPC_PROPERTY_EU_TYPE	=	7;

  const unsigned int OPC_PROPERTY_EU_INFO	=	8;

  const unsigned int OPC_PROPERTY_EU_UNITS	=	100;

  const unsigned int OPC_PROPERTY_DESCRIPTION	=	101;

  const unsigned int OPC_PROPERTY_HIGH_EU	=	102;

  const unsigned int OPC_PROPERTY_LOW_EU	=	103;

  const unsigned int OPC_PROPERTY_HIGH_IR	=	104;

  const unsigned int OPC_PROPERTY_LOW_IR	=	105;

  const unsigned int OPC_PROPERTY_CLOSE_LABEL	=	106;

  const unsigned int OPC_PROPERTY_OPEN_LABEL	=	107;

  const unsigned int OPC_PROPERTY_TIMEZONE	=	108;

  const unsigned int OPC_PROPERTY_CONDITION_STATUS	=	300;

  const unsigned int OPC_PROPERTY_ALARM_QUICK_HELP	=	301;

  const unsigned int OPC_PROPERTY_ALARM_AREA_LIST	=	302;

  const unsigned int OPC_PROPERTY_PRIMARY_ALARM_AREA	=	303;

  const unsigned int OPC_PROPERTY_CONDITION_LOGIC	=	304;

  const unsigned int OPC_PROPERTY_LIMIT_EXCEEDED	=	305;

  const unsigned int OPC_PROPERTY_DEADBAND	=	306;

  const unsigned int OPC_PROPERTY_HIHI_LIMIT	=	307;

  const unsigned int OPC_PROPERTY_HI_LIMIT	=	308;

  const unsigned int OPC_PROPERTY_LO_LIMIT	=	309;

  const unsigned int OPC_PROPERTY_LOLO_LIMIT	=	310;

  const unsigned int OPC_PROPERTY_CHANGE_RATE_LIMIT	=	311;

  const unsigned int OPC_PROPERTY_DEVIATION_LIMIT	=	312;

  const unsigned int OPC_PROPERTY_SOUND_FILE	=	313;

  const unsigned int OPC_PROPERTY_TYPE_SYSTEM_ID	=	600;

  const unsigned int OPC_PROPERTY_DICTIONARY_ID	=	601;

  const unsigned int OPC_PROPERTY_TYPE_ID	=	602;

  const unsigned int OPC_PROPERTY_DICTIONARY	=	603;

  const unsigned int OPC_PROPERTY_TYPE_DESCRIPTION	=	604;

  const unsigned int OPC_PROPERTY_CONSISTENCY_WINDOW	=	605;

  const unsigned int OPC_PROPERTY_WRITE_BEHAVIOR	=	606;

  const unsigned int OPC_PROPERTY_UNCONVERTED_ITEM_ID	=	607;

  const unsigned int OPC_PROPERTY_UNFILTERED_ITEM_ID	=	608;

  const unsigned int OPC_PROPERTY_DATA_FILTER_VALUE	=	609;

  const char OPC_PROPERTY_DESC_DATATYPE[]	=	"Item Canonical Data Type";

  const char OPC_PROPERTY_DESC_VALUE[]		=	"Item Value";

  const char OPC_PROPERTY_DESC_QUALITY[]		=	"Item Quality";

  const char OPC_PROPERTY_DESC_TIMESTAMP[]		=	"Item Timestamp";

  const char OPC_PROPERTY_DESC_ACCESS_RIGHTS[]		=	"Item Access Rights";

  const char OPC_PROPERTY_DESC_SCAN_RATE[]		=	"Server Scan Rate";

  const char OPC_PROPERTY_DESC_EU_TYPE[]		=	"Item EU Type";

  const char OPC_PROPERTY_DESC_EU_INFO[]		=	"Item EU Info";

  const char OPC_PROPERTY_DESC_EU_UNITS[]		=	"EU Units";

  const char OPC_PROPERTY_DESC_DESCRIPTION[]		=	"Item Description";

  const char OPC_PROPERTY_DESC_HIGH_EU[]		=	"High EU";

  const char OPC_PROPERTY_DESC_LOW_EU[]		=	"Low EU";

  const char OPC_PROPERTY_DESC_HIGH_IR[]		=	"High Instrument Range";

  const char OPC_PROPERTY_DESC_LOW_IR[]		=	"Low Instrument Range";

  const char OPC_PROPERTY_DESC_CLOSE_LABEL[]		=	"Contact Close Label";

  const char OPC_PROPERTY_DESC_OPEN_LABEL[]		=	"Contact Open Label";

  const char OPC_PROPERTY_DESC_TIMEZONE[]		=	"Item Timezone";

  const char OPC_PROPERTY_DESC_CONDITION_STATUS[]		=	"Condition Status";

  const char OPC_PROPERTY_DESC_ALARM_QUICK_HELP[]		=	"Alarm Quick Help";

  const char OPC_PROPERTY_DESC_ALARM_AREA_LIST[]		=	"Alarm Area List";

  const char OPC_PROPERTY_DESC_PRIMARY_ALARM_AREA[]		=	"Primary Alarm Area";

  const char OPC_PROPERTY_DESC_CONDITION_LOGIC[]		=	"Condition Logic";

  const char OPC_PROPERTY_DESC_LIMIT_EXCEEDED[]		=	"Limit Exceeded";

  const char OPC_PROPERTY_DESC_DEADBAND[]		=	"Deadband";

  const char OPC_PROPERTY_DESC_HIHI_LIMIT[]		=	"HiHi Limit";

  const char OPC_PROPERTY_DESC_HI_LIMIT[]		=	"Hi Limit";

  const char OPC_PROPERTY_DESC_LO_LIMIT[]		=	"Lo Limit";

  const char OPC_PROPERTY_DESC_LOLO_LIMIT[]		=	"LoLo Limit";

  const char OPC_PROPERTY_DESC_CHANGE_RATE_LIMIT[]		=	"Rate of Change Limit";

  const char OPC_PROPERTY_DESC_DEVIATION_LIMIT[]		=	"Deviation Limit";

  const char OPC_PROPERTY_DESC_SOUND_FILE[]		=	"Sound File";

  const char OPC_PROPERTY_DESC_TYPE_SYSTEM_ID[]		=	"Type System ID";

  const char OPC_PROPERTY_DESC_DICTIONARY_ID[]		=	"Dictionary ID";

  const char OPC_PROPERTY_DESC_TYPE_ID[]		=	"Type ID";

  const char OPC_PROPERTY_DESC_DICTIONARY[]		=	"Dictionary";

  const char OPC_PROPERTY_DESC_TYPE_DESCRIPTION[]		=	"Type Description";

  const char OPC_PROPERTY_DESC_CONSISTENCY_WINDOW[]		=	"Consistency Window";

  const char OPC_PROPERTY_DESC_WRITE_BEHAVIOR[]		=	"Write Behavior";

  const char OPC_PROPERTY_DESC_UNCONVERTED_ITEM_ID[]		=	"Unconverted Item ID";

  const char OPC_PROPERTY_DESC_UNFILTERED_ITEM_ID[]		=	"Unfiltered Item ID";

  const char OPC_PROPERTY_DESC_DATA_FILTER_VALUE[]		=	"Data Filter Value";

} // namespace Opc

#endif // __opcda_h__

