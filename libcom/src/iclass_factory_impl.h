/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief IClassFactory default implementation.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __iclass_factory_impl_11fd09e1_678a_4bb5_8dd0_7d08dbc72a4c
#define __iclass_factory_impl_11fd09e1_678a_4bb5_8dd0_7d08dbc72a4c

#include <com/iclass_factory.h>
#include <com/impl/iunknown_impl.h>
#include <registry.h>

namespace Com
{
  namespace Impl
  {
    class ClassFactory
      : public IClassFactory
      , public Unknown
    {
    public:
      ClassFactory(std::shared_ptr<Com::ClassRegistry> registry)
        : Registry(registry)
      {
      }

      virtual HRESULT CreateInstance(const char* clsId, IUnknown** result);
      virtual HRESULT QueryInterface(const char* iid, void** result);

    private:
      const std::shared_ptr<Com::ClassRegistry> Registry;
    };
  }
}

#endif // __iclass_factory_impl_11fd09e1_678a_4bb5_8dd0_7d08dbc72a4c

