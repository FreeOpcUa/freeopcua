/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IClassRegistry default implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#ifndef __iclass_registry_impl_b73ee06e_42aa_46ea_8256_823b6204fa6f
#define __iclass_registry_impl_b73ee06e_42aa_46ea_8256_823b6204fa6f

#include <com/iclass_registry.h>
#include <com/impl/iunknown_impl.h>
#include <registry.h>


namespace Com
{
  namespace Impl
  {
    class ClassRegistry
      : public IClassRegistry
      , public Unknown
    {
    public:
      ClassRegistry(std::shared_ptr<Com::ClassRegistry> registry)
        : Registry(registry)
      {
      }

      virtual HRESULT Register(const char* clsId, const char* libraryPath);
      virtual HRESULT Unregister(const char* clsId);
      virtual HRESULT GetLibraryPath(const char* clsId, IString** resultPath) const;
 
      virtual HRESULT QueryInterface(const char* iid, void** result);

    private:
      const std::shared_ptr<Com::ClassRegistry> Registry;
    };
  }
}

#endif // __iclass_registry_impl_b73ee06e_42aa_46ea_8256_823b6204fa6f

