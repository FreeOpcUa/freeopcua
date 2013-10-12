/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Class which implements Discovery service over soap.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <soapBasicHttpBinding_USCOREIRegistrationEndpointService.h>

namespace OpcUa
{
  namespace Impl
  {
    class SoapRegistrationService : public BasicHttpBinding_USCOREIRegistrationEndpointService
    {
    public:
      virtual BasicHttpBinding_USCOREIRegistrationEndpointService *copy();
      virtual int RegisterServer(ns3__RegisterServerRequest *ns3__RegisterServerRequest_, ns3__RegisterServerResponse *ns3__RegisterServerResponse_);
    };
  }
}

