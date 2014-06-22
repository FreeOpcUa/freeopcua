/// @author Alexander Rykovanov 2011
/// @email rykovanov.as@gmail.com
/// @brief Exception class realization
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/common/exception.h>

Common::Error::Error()
  : LineNum(0)
  , FileName()
  , Code(0)
  , Message()
{
}

Common::Error::Error(unsigned lineNum, const char* fileName, unsigned errorCode, const char* msg)
  : LineNum(lineNum)
  , FileName(fileName)
  , Code(errorCode)
  , Message(msg)
{
}

Common::Error::~Error() throw()
{
}

unsigned Common::Error::GetLineNum() const
{
  return LineNum;
}

std::string Common::Error::GetFileName() const
{
  return FileName;
}

unsigned Common::Error::GetCode() const
{
  return Code;
}

std::string Common::Error::GetMessage() const
{
  return Message;
}

Common::Error& Common::Error::AddError(const Error& subError)
{
  SubErrors.push_back(subError);
  return *this;
}

std::string Common::Error::GetFullMessage() const
{
  std::string msg = FileName;
  msg += "(" + std::to_string(LineNum) + "):\n";
  msg += Message;
  std::vector<Common::Error>::const_iterator it = SubErrors.begin();

  for (; it != SubErrors.end(); ++ it)
  {
    msg += "\n";
    msg += it->GetMessage();
  }
  return msg;
}

const char* Common::Error::what() const throw()
{
  return  Message.c_str();
}
