// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#ifndef __TESTOPCSERVER_CLASS_DATA_H__
#define __TESTOPCSERVER_CLASS_DATA_H__

#include <string>

namespace OPCTest
{
  class CoClassData;

  void InitializeClassData(int argc, char** argv);
  const CoClassData* GetClassData();

#ifdef _WIN32
  struct OpcRegistry
  {
    const std::string ClsId;
    const std::string ClsIdKeyName;
    const std::string ClsIdFullPathKey;
    const std::string InProcServer32Key;
    const std::string ProgId_ClsIdKey;

    OpcRegistry(const std::string& name, const std::string& guid, const std::string& progid);
  };
#endif // _WIN32

  class CoClassData
  {
  public:
   CoClassData(const std::string& filename,
               const std::string& name,
               const std::string& guid,
#ifdef _WIN32
               const std::string& progId,
               const bool disableCom,
#endif // _WIN32
               const std::string& contractId,
               const bool disableMozilla);

    virtual ~CoClassData();


    virtual std::string GetFilename() const;
    virtual std::string GetName() const;
    virtual std::string GetClsId() const;

    virtual bool IsMozillaTestsDisabled() const;
    virtual std::string GetContractId() const;
  
#ifdef _WIN32
    virtual bool IsWindowsTestsDisabled() const;
    virtual std::string GetProgId() const;
    const OpcRegistry RegistryKeys;
#endif // _WIN32

  private:
    std::string Filename;
    std::string ClassName;
    std::string ClassGuid;
    std::string ClassContractId;
#ifdef _WIN32
    std::string ClassProgId;
    const bool DisableComTests;
#endif // _WIN32
    const bool DisableMozillaTests;
  };
}

#endif // __TESTOPCSERVER_CLASS_DATA_H__