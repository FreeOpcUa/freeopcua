#include <opc/impl/module.h>

#include <com/impl/error.h>
#include <string.h>

namespace
{
  static const char* CLSID_TestOPCServer = "ed73f3ef-abeb-4677-9b3b-49e83c122ff3";
}

extern "C"
Com::HRESULT Com::CoCreateInstance(const char* clsId, Com::IUnknown** result)
{
  CATCH_BLOCK_START
  CHECK_PTR_PARAM1(clsId);
  CHECK_PTR_PARAM2(result);
  
  if (!strcmp(clsId, CLSID_TestOPCServer))
  {
    *result = Opc::CreateInstance();
    return Com::COM_SUCCESS;
  }
  return Com::COM_UNKNOWN_CLASS;
  CATCH_BLOCK_FINISH
}

