/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Exception declarations
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef __MOZOPC_EXCEPTION__H__
#define __MOZOPC_EXCEPTION__H__


#include <boost/format.hpp>
#include <stdexcept>
#include <string>
#include <vector>


namespace Common
{

  struct ErrorData
  {
    unsigned ErrorCode;
    const char* ErrorMessage;

    ErrorData()
      : ErrorCode(0)
      , ErrorMessage(NULL)
    {
    }

    ErrorData(unsigned code, const char* msg)
      : ErrorCode(code)
      , ErrorMessage(msg)
    {
    }
  };

  class Error : public std::exception
  {
  public:
    Error();
    explicit Error(unsigned lineNum, const char* fileName, unsigned errorCode, const char* msg);
    virtual ~Error() throw();

    unsigned GetLineNum() const;
    std::string GetFileName() const;
    unsigned GetCode() const;
    std::string GetMessage() const;
    std::string GetFullMessage() const;

    Error& AddError(const Error& subError);

  public: // std::exception
    virtual const char* what() const throw();

  private:
    unsigned LineNum;
    std::string FileName;
    unsigned Code;
    std::string Message;
    mutable std::string FullMessage;
    std::vector<Error> SubErrors;
  };


  inline Error CreateError(
    unsigned lineNum,
    const char* filename,
    unsigned errorCode,
    const char* msg)
  {
    return Common::Error(lineNum, filename, errorCode, msg);
  }

  template <typename T1>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1);
    return Error(lineNum, fileName, errorCode, resultMessage.c_str());
  }

 template <typename T1>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const Common::Error& subError)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1);
    Error resultError(lineNum, fileName, errorCode, resultMessage.c_str());
    resultError.AddError(subError);
    return resultError;
  }

  template <typename T1, typename T2>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const T2& param2)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1 % param2);
    return Error(lineNum, fileName, errorCode, resultMessage.c_str());
  }


  template <typename T1, typename T2>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const T2& param2,
    const Common::Error& subError)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1 % param2);
    Error resultError(lineNum, fileName, errorCode, resultMessage.c_str());
    resultError.AddError(subError);
    return resultError;
  }

  template <typename T1, typename T2, typename T3>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const T2& param2,
    const T3& param3)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1 % param2 % param3);
    return Error(lineNum, fileName, errorCode, resultMessage.c_str());
  }

  template <typename T1, typename T2, typename T3>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const T2& param2,
    const T3& param3,
    const Common::Error& subError)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1 % param2 % param3);
    Error resultError(lineNum, fileName, errorCode, resultMessage.c_str());
    resultError.AddError(subError);
    return resultError;
  }

  template <typename T1, typename T2, typename T3, typename T4>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const T2& param2,
    const T3& param3,
    const T4& param4)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1 % param2 % param3 % param4);
    return Error(lineNum, fileName, errorCode, resultMessage.c_str());
  }

  template <typename T1, typename T2, typename T3, typename T4>
  inline Error CreateError(
    unsigned lineNum, 
    const char* fileName,
    unsigned errorCode,
    const char* msg,
    const T1& param1,
    const T2& param2,
    const T3& param3,
    const T4& param4,
    const Common::Error& subError)
  {
    const std::string& resultMessage = str(boost::format(msg) % param1 % param2 % param3 % param4);
    Error resultError(lineNum, fileName, errorCode, resultMessage.c_str());
    resultError.AddError(subError);
    return resultError;
  }


}

#define ERROR_CODE(module_id, code) (module_id << 16 | (code & 8))

#define THROW_COMMON_ERROR(code, message) throw ::Common::CreateError(__LINE__, __FILE__, code, message)
#define CREATE_COMMON_ERROR(code, message) Common::CreateError(__LINE__, __FILE__, code, message)
#define THROW_ERROR(data) throw ::Common::CreateError(__LINE__, __FILE__, data.ErrorCode, data.ErrorMessage)
#define CREATE_ERROR(data) ::Common::CreateError(__LINE__, __FILE__, data.ErrorCode, data.ErrorMessage)

/// Last parameter can be any type (is a format message parameter) or sub error.
#define CREATE_ERROR1(data, param1) (::Common::CreateError(__LINE__, __FILE__, data.ErrorCode, data.ErrorMessage, param1))
#define THROW_ERROR1(data, param1) throw CREATE_ERROR1(data, param1);

#define CREATE_ERROR2(data, param1, param2) ::Common::CreateError(__LINE__, __FILE__, data.ErrorCode, data.ErrorMessage, param1, param2)
#define THROW_ERROR2(data, param1, param2) throw CREATE_ERROR2(data, param1, param2)

#define CREATE_ERROR3(data, param1, param2, param3) ::Common::CreateError(__LINE__, __FILE__, data.ErrorCode, data.ErrorMessage, param1, param2, param3)
#define THROW_ERROR3(data, param1, param2, param3) throw CREATE_ERROR3(data, param1, param2, param3)

#define CREATE_ERROR4(data, param1, param2, param3, param4) ::Common::CreateError(__LINE__, __FILE__, data.ErrorCode, data.ErrorMessage, param1, param2, param3, param4)
#define THROW_ERROR4(data, param1, param2, param3, param4) throw CREATE_ERROR4(data, param1, param2, param3, param4)


#include "errors.h"

#define THROW_OS_ERROR(UserMsg) (throw ::Common::CreateError(__LINE__, __FILE__, errno, "%1% %2%.", UserMsg, strerror(errno)))

#define BEGIN_TRY_BLOCK try{
#define END_TRY_BLOCK(handler)\
}\
catch (const ::Common::Error& err)\
{\
  return handler(err);\
}\
catch (const std::exception& exc)\
{\
  return handler(CREATE_ERROR1(StdException, exc.what()));\
}

#endif // __MOZOPC_EXCEPTION__H__
