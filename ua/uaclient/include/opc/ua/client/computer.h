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
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/data_value.h>

#include <memory>
#include <vector>

namespace OpcUa
{
  namespace Remote
  {

    struct SessionParameters
    {
    };


    struct IdentifyParameters
    {
    };


    struct BrowseParameters
    {
    };

    struct BrowseResult
    {
      std::vector<char> ContnuationPoint;
      std::vector<NodeID> Nodes;
    };
   
    class ViewServices
    {
    public:
      virtual BrowseResult Browse(const BrowseParameters& filter) = 0;
      virtual BrowseResult BrowseNext(const std::vector<char>& continuationPoint) = 0;
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
      virtual std::vector<AttributeValue> Browse(const ReadParameters& filter) = 0;
      virtual std::vector<WriteResult> Write(const WriteParameters& filter) = 0;
    };



    class Computer
    {
    public:
      virtual void CreateSession(const SessionParameters& parameters) = 0;
      virtual void UpdateSession(const IdentifyParameters& parameters) = 0;
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
