/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua computer interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_COMPUTER_H
#define OPC_UA_COMPUTER_H

#include <opc/common/class_pointers.h>
#include <opc/ua/attributes.h>
#include <opc/ua/endpoints.h>
#include <opc/ua/subscriptions.h>
#include <opc/ua/view.h>

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

    // TODO rename Computer to server
    class Computer : private Common::Interface
    {
    public:
      DEFINE_CLASS_POINTERS(Computer);

    public:
      virtual void CreateSession(const SessionParameters& parameters) = 0;
      virtual void ActivateSession() = 0;
      virtual void CloseSession() = 0;

      virtual EndpointServices::SharedPtr Endpoints() const = 0;
      virtual ViewServices::SharedPtr Views() const = 0;
      virtual AttributeServices::SharedPtr Attributes() const = 0;
      virtual SubscriptionServices::SharedPtr Subscriptions() const = 0;
    };

  }
}

#endif //  OPC_UA_CLIENT_COMPUTER_H
