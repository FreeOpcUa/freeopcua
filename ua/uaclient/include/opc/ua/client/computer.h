/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Opc Ua computer interface.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/licenses/lgpl.html)
///

#ifndef OPC_UA_CLIENT_COMPUTER_H
#define OPC_UA_CLIENT_COMPUTER_H

#include <opc/ua/client/endpoints.h>
#include <opc/ua/client/view.h>
#include <opc/ua/protocol/data_value.h>

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

    struct AttributeValue
    {
      DataValue Value;
      DiagnosticInfo Info;
    };

    struct ReadParameters
    {
    };

    struct WriteParameters
    {
    };

    struct WriteResult
    {
    };

    class AttributeServices
    {
      virtual std::vector<AttributeValue> Read(const ReadParameters& filter) = 0;
      virtual std::vector<WriteResult> Write(const WriteParameters& filter) = 0;
    };



    class Computer
    {
    public:
      virtual void CreateSession(const SessionParameters& parameters) = 0;
      virtual void ActivateSession() = 0;
      virtual void CloseSession() = 0;

     virtual std::shared_ptr<EndpointServices> Endpoints() const = 0;
     virtual std::shared_ptr<ViewServices> Views() const = 0;
     virtual std::shared_ptr<AttributeServices> Attributes() const = 0;
    };

    /// @brief connect to remote computer
    /// @param url url of the server.
    /// @note url in the form: opc.tcp://host:port connect with opc ua binary protocol
    /// @note url in the form https://host:port
    std::unique_ptr<Computer> Connect(const std::string& url);
  }
}

#endif //  OPC_UA_CLIENT_COMPUTER_H
