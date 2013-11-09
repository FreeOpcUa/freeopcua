/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#pragma once


#include "soapH.h"
#include "soap_server.h"

#include <opc/common/thread.h>

#include <sstream>
#include <stdexcept>

namespace OpcUa
{
  namespace Soap
  {

    template <typename ServiceType>
    class ServiceImpl : public Service
    {
    public:
      ServiceImpl(Remote::Computer::SharedPtr computer, bool debug)
        : Computer(computer)
        , Debug(debug)
        , Impl(computer, debug)
      {

      }

      virtual int Dispatch(soap* s)
      {
        soap_copy_stream(&Impl, s);
        const int error = Impl.dispatch();
        if (error != SOAP_NO_METHOD && error != SOAP_OK)
        {
          Impl.soap_stream_fault(std::cerr);
          soap_send_fault(&Impl);
        }
        soap_free_stream(&Impl);
        Impl.destroy();
        return error;
      }

    private:
      Remote::Computer::SharedPtr Computer;
      bool Debug;
      ServiceType Impl;
    };

  } // namespace Impl
} // namespace OpcUa
