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

#include <soapH.h>
#include <opc/ua/protocol/attribute.h>
#include <opc/ua/protocol/endpoints.h>
#include <opc/ua/protocol/view.h>

namespace OpcUa
{
  namespace Soap
  {
/*
    class SoapDeleter
    {
    public:
      SoapDeleter(soap* s)
        : Owner(s)
      {
      }

      SoapDeleter() = default;
      SoapDeleter(const SoapDeleter&) = default;
      SoapDeleter(SoapDeleter&&) = default;
      SoapDeleter& operator=(SoapDeleter&) = default;
      SoapDeleter& operator=(SoapDeleter&&) = default;

      void operator() (void* ptr)
      {
        soap_delete(Owner, ptr);
      }

    private:
      soap* Owner;
    };

    template <typename T>
    class SoapPtr : public std::unique_ptr<T, SoapDeleter>
    {
    public:
      typedef std::unique_ptr<T, SoapDeleter> Parent;

    public:
      SoapPtr(T* ptr, soap*)
        : Parent(ptr, SoapDeleter(soap))
      {
      }
    };
*/
    ns3__GetEndpointsRequest* Serialize(soap* s, const OpcUa::GetEndpointsRequest& request);
    ns3__GetEndpointsResponse* Serialize(soap* s, const OpcUa::GetEndpointsResponse& response);

    ns3__BrowseRequest* Serialize(soap* s, const OpcUa::BrowseRequest& request);
    ns3__BrowseResponse* Serialize(soap* s, const OpcUa::BrowseResponse& response);

    ns3__ReadRequest* Serialize(soap* s, const OpcUa::ReadRequest& request);
    ns3__ReadResponse* Serialize(soap* s, const OpcUa::ReadResponse& request);

    ns3__WriteRequest* Serialize(soap* s, const OpcUa::WriteRequest& request);
    ns3__WriteResponse* Serialize(soap* s, const OpcUa::WriteResponse& request);

    ns3__Variant* Serialize(soap* s, const OpcUa::Variant& var);
  }
}
