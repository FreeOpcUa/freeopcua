// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

#include <mozilla/ModuleUtils.h>

#include "module.h"
#include "opc_server.h"

#include <iostream>

typedef XPCOM::nsOPCServer LibraryClass;
NS_GENERIC_FACTORY_CONSTRUCTOR(LibraryClass);

const mozilla::Module* GetModule()
{
  std::cout << "////////////////////////////////////////////////////////////////" << std::endl;
  std::cout << "GetModule called for server '" << GetCoClassContractId() << "'" << std::endl;
  std::cout << "////////////////////////////////////////////////////////////////" << std::endl;
  
  static nsID libraryID;
  libraryID.Parse(GetCoClassGuid());

  static mozilla::Module::CIDEntry cidEntry[2];
  memset(cidEntry, 0, sizeof(cidEntry));
  cidEntry[0].cid = &libraryID;
  cidEntry[0].service = false;
  cidEntry[0].getFactoryProc = NULL;
  cidEntry[0].constructorProc = LibraryClassConstructor;

  static mozilla::Module::ContractIDEntry libraryContractIDs[2];
  memset(libraryContractIDs, 0, sizeof(libraryContractIDs));
  libraryContractIDs[0].contractid = GetCoClassContractId();
  libraryContractIDs[0].cid = &libraryID;

  static mozilla::Module libraryModule[2];
  memset(libraryModule, 0, sizeof(libraryModule));
  libraryModule[0].mVersion = mozilla::Module::kVersion;
  libraryModule[0].mCIDs = cidEntry;
  libraryModule[0].mContractIDs = libraryContractIDs;
  return libraryModule;
}

static const mozilla::Module* module = GetModule();

NSMODULE_DEFN(LibraryClass) = module;

NS_IMPL_MOZILLA192_NSGETMODULE(module);

