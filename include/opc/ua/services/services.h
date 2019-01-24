/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua computer interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/common/class_pointers.h>
#include <opc/ua/protocol/session.h>
#include <opc/ua/protocol/secure_channel.h>
#include <opc/ua/services/attributes.h>
#include <opc/ua/services/endpoints.h>
#include <opc/ua/services/method.h>
#include <opc/ua/services/node_management.h>
#include <opc/ua/services/subscriptions.h>
#include <opc/ua/services/view.h>

#include <opc/ua/protocol/protocol.h>

#include <memory>
#include <vector>

#include <boost/version.hpp>


namespace boost
{
namespace asio
{
#if BOOST_VERSION < 106600
  class io_service;
#else
  class io_context;
  typedef io_context io_service;
#endif
}
}

namespace OpcUa
{

struct RemoteSessionParameters
{
  ApplicationDescription ClientDescription;
  std::vector<uint8_t> ClientCertificate;
  std::string SessionName;
  std::string ServerURI;
  std::string EndpointUrl;
  Duration Timeout;
};

class Services : private Common::Interface
{
public:
  DEFINE_CLASS_POINTERS(Services)

public:
  virtual OpenSecureChannelResponse OpenSecureChannel(const OpenSecureChannelParameters & parameters) = 0;
  virtual void CloseSecureChannel(uint32_t channelId) = 0;
  virtual CreateSessionResponse CreateSession(const RemoteSessionParameters & parameters) = 0;
  virtual ActivateSessionResponse ActivateSession(const ActivateSessionParameters & session_parameters) = 0;
  virtual CloseSessionResponse CloseSession() = 0;
  virtual void AbortSession() = 0;
  virtual DeleteNodesResponse DeleteNodes(const std::vector<OpcUa::DeleteNodesItem> & nodesToDelete) = 0;

  virtual AttributeServices::SharedPtr Attributes() = 0;
  virtual EndpointServices::SharedPtr Endpoints() = 0;
  virtual MethodServices::SharedPtr Method() = 0;
  virtual NodeManagementServices::SharedPtr NodeManagement() = 0;
  virtual SubscriptionServices::SharedPtr Subscriptions() = 0;
  virtual ViewServices::SharedPtr Views() = 0;
};

}
