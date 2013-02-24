/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Bindings com interfaces to smart pointers.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __com_smart_ptr_h_6dfdcb7d_5642_4af8_acd6_765d4dd411ea
#define __com_smart_ptr_h_6dfdcb7d_5642_4af8_acd6_765d4dd411ea

#include <memory>
#include <com/com_errors.h>

namespace Com
{

  template <typename Interface>
  class InterfaceDeleter
  {
  public:
    void operator()(Interface* itf)
    {
      itf->Release();
    }
  };

  template <typename Interface, typename ClassPtr>
  std::shared_ptr<Interface> SharedPtr(ClassPtr* cls)
  {
    return std::shared_ptr<Interface>(cls, InterfaceDeleter<Interface>());
  }

  template <typename Interface, typename ClassPtr>
  std::unique_ptr<Interface, InterfaceDeleter<Interface>> UniquePtr(ClassPtr* cls)
  {
    return std::unique_ptr<Interface, InterfaceDeleter<Interface>>(cls);
  }

  template <typename Interface>
  class ResultPtrHelper
  {
  public:
    explicit ResultPtrHelper(std::shared_ptr<Interface>& result)
      : Result(result)
      , Ptr(nullptr)
    {
    }

    ~ResultPtrHelper()
    {
      Assign();
    }

    operator Interface**()
    {
      return &Ptr;
    }

    operator void**()
    {
      return reinterpret_cast<void**>(&Ptr);
    }
  
  private:
    void Assign()
    {
      if (Ptr != nullptr)
      {
        Result.reset(Ptr, InterfaceDeleter<Interface>());
        Ptr = nullptr;
      }
    }

  private:
    std::shared_ptr<Interface>& Result;
    Interface* Ptr;
  };

  template <typename Interface>
  ResultPtrHelper<Interface> ResultPtr(std::shared_ptr<Interface>& ptr)
  {
    return ResultPtrHelper<Interface>(ptr);
  }


#define DECLARE_INTERFACE_POINTERS(itf) \
  typedef std::shared_ptr<itf> SharedPtr; \
  typedef std::unique_ptr<itf, Com::InterfaceDeleter<itf>> UniquePtr

} // namespace Com

#endif // __com_smart_ptr_h_6dfdcb7d_5642_4af8_acd6_765d4dd411ea

