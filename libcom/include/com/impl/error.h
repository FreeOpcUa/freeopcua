/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Lib Com exception class.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include <com/com_errors.h>

#include <stdexcept>


#ifndef __error_h_79d777bd_08f5_4a12_8be9_40a9e10fb2f3
#define __error_h_79d777bd_08f5_4a12_8be9_40a9e10fb2f3

namespace Com
{
  namespace Impl
  {
    class Error : public std::exception
    {
    public:
      Error() throw()
        : Code(COM_SUCCESS)
      {
      }

      Error(Com::HRESULT code, const std::string& msg) throw()
        : Code(code)
        , Message(msg)
      {
      }

      Error(const Error& error) throw()
        : Code(error.Code)
        , Message(error.Message)
      {
      }

      virtual ~Error() throw()
      {
      }
    
      std::exception& operator=(const std::exception& exc) throw()
      {
        Code = Com::COM_INTERNAL_ERROR;
        Message = exc.what();
        return *this;
      }

      Error& operator=(const Error& error) throw()
      {
        Code = error.Code;
        Message = error.Message;
        return *this;
      }
     
      Com::HRESULT code() const throw()
      {
        return Code;
      }

      virtual const char* what() const throw()
      {
        return Message.c_str();
      }

    private:
      Com::HRESULT Code;
      std::string Message;
    };

    void ProcessError(Com::HRESULT code, const char* message);
  }
} // namespace Com


#define CATCH_BLOCK_START try {
#define CATCH_BLOCK_FINISH \
      } \
      catch (const Com::Impl::Error& err) \
      { \
        ::Com::Impl::ProcessError(err.code(), err.what()); \
        return err.code(); \
      } \
      catch (const std::exception& exc) \
      { \
        ::Com::Impl::ProcessError(Com::COM_INTERNAL_ERROR, exc.what()); \
        return ::Com::COM_INTERNAL_ERROR; \
      } \
      catch (...) \
      { \
        ::Com::Impl::ProcessError(Com::COM_UNHANDLED_EXCEPTION, ""); \
        return ::Com::COM_UNHANDLED_EXCEPTION; \
      }

#define CHECK_PTR_PARAM1(param) { if (param == nullptr) {  return Com::COM_INVALID_PARAMETER1; } }
#define CHECK_PTR_PARAM2(param) { if (param == nullptr) {  return Com::COM_INVALID_PARAMETER2; } }

#endif //  __error_h_79d777bd_08f5_4a12_8be9_40a9e10fb2f3

