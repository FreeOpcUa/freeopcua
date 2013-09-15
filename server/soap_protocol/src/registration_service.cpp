/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Class which implements Discovery service over soap.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "registration_service.h"

namespace OpcUa
{
  namespace Impl
  {

    BasicHttpBinding_USCOREIRegistrationEndpointService *SoapRegistrationService::copy()
    {
      return new SoapRegistrationService();
    }

    int SoapRegistrationService::RegisterServer(ns3__RegisterServerRequest *ns3__RegisterServerRequest_, ns3__RegisterServerResponse *ns3__RegisterServerResponse_)
    {
      return SOAP_OK;
    }

  } // namespace Impl
} // namespace OpcUa
