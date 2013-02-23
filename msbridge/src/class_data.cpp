// Author: Alexander Rykovanov 2009
//
// Distributed under the GNU GPL License
// (See accompanying file LICENSE or copy at 
// http://www.gnu.org/copyleft/gpl.html)
//
// $Id:  $
// $Date: $
// $Revision: $

const char* ClassName  = "OPC_Bridge";
const char* ContractId = "@treww.org/msopcbridge;1";
const char* ProgId     = "";
const char* ClassGuid  = "B6BA1A1C-2547-4439-8E4B-9FAF97EFC272";


extern "C" const char* GetCoClassName()
{
  return ClassName;
}

extern "C" const char* GetCoClassContractId()
{
  return ContractId;
}

extern "C" const char* GetCoClassProgId()
{
  return ProgId;
}

extern "C" const char* GetCoClassGuid()
{
  return ClassGuid;
}