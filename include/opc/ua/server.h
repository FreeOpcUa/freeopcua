/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua computer interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_SERVER_H
#define OPC_UA_SERVER_H

#include <opc/common/class_pointers.h>
#include <opc/ua/services/attributes.h>
#include <opc/ua/services/endpoints.h>
#include <opc/ua/services/node_management.h>
#include <opc/ua/services/subscriptions.h>
#include <opc/ua/services/view.h>

#include <memory>
#include <vector>

namespace OpcUa
{
  namespace Remote
  {

    struct SessionParameters
    {
      ApplicationDescription ClientDescription;
      std::vector<uint8_t> ClientCertificate;
      std::string SessionName;
      std::string ServerURI;
      std::string EndpointURL;
      Duration Timeout;
    };

    class Server : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(Server);

    public:
      virtual void CreateSession(const SessionParameters& parameters) = 0;
      virtual void ActivateSession() = 0;
      virtual void CloseSession() = 0;

      virtual EndpointServices::SharedPtr Endpoints() = 0;
      virtual ViewServices::SharedPtr Views() = 0;
      virtual NodeManagementServices::SharedPtr NodeManagement() = 0;
      virtual AttributeServices::SharedPtr Attributes() = 0;
      virtual SubscriptionServices::SharedPtr Subscriptions() = 0;
    };

  }
}

#endif //  OPC_UA_SERVER_H
